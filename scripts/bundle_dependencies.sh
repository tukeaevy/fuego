#!/bin/bash
# Bundle shared library dependencies for Fuego executables
# This script collects all required shared libraries and creates a self-contained bundle

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default values
BUILD_DIR="${PROJECT_ROOT}/build"
OUTPUT_DIR=""
BUNDLE_NAME=""
PLATFORM="$(uname -s | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"

# Function to print usage
usage() {
    echo "Usage: $0 -o OUTPUT_DIR -n BUNDLE_NAME [-b BUILD_DIR]"
    echo "  -o OUTPUT_DIR   Directory to create the bundle"
    echo "  -n BUNDLE_NAME  Name of the bundle directory"
    echo "  -b BUILD_DIR    Build directory containing executables (default: $BUILD_DIR)"
    echo "  -h              Show this help"
    exit 1
}

# Parse command line arguments
while getopts "o:n:b:h" opt; do
    case $opt in
        o) OUTPUT_DIR="$OPTARG" ;;
        n) BUNDLE_NAME="$OPTARG" ;;
        b) BUILD_DIR="$OPTARG" ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Validate required arguments
if [[ -z "$OUTPUT_DIR" || -z "$BUNDLE_NAME" ]]; then
    echo "Error: OUTPUT_DIR and BUNDLE_NAME are required"
    usage
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"
BUNDLE_PATH="$OUTPUT_DIR/$BUNDLE_NAME"
mkdir -p "$BUNDLE_PATH"

echo "Creating bundle: $BUNDLE_PATH"
echo "Platform: $PLATFORM-$ARCH"

# Find executables
EXECUTABLES=(
    "$BUILD_DIR/src/fuegod"
    "$BUILD_DIR/src/fuego-wallet-cli"
    "$BUILD_DIR/src/walletd"
    "$BUILD_DIR/src/optimizer"
)

# Create bundle structure
mkdir -p "$BUNDLE_PATH/bin"
mkdir -p "$BUNDLE_PATH/lib"
mkdir -p "$BUNDLE_PATH/share"

# Function to get shared library dependencies
get_dependencies() {
    local executable="$1"
    if [[ ! -f "$executable" ]]; then
        echo "Warning: Executable not found: $executable" >&2
        return
    fi
    
    # Get all shared library dependencies, including important system libraries
    ldd "$executable" 2>/dev/null | grep -E "\.so" | awk '{
        if ($3 ~ /^\//) {
            # Format: libname.so => /path/to/lib (address)
            print $3
        } else if ($1 ~ /^\//) {
            # Format: /path/to/lib (address)
            print $1
        }
    }' | grep -E "(boost|icu|ssl|crypto|miniupnpc|qrencode)" | sort -u
}

# Function to copy library and its dependencies recursively
copy_library_recursive() {
    local lib_path="$1"
    local dest_dir="$2"
    local processed_libs="$3"
    
    if [[ ! -f "$lib_path" ]]; then
        return
    fi
    
    local lib_name="$(basename "$lib_path")"
    
    # Skip if already processed
    if echo "$processed_libs" | grep -q "$lib_name"; then
        return
    fi
    
    echo "  Copying library: $lib_name"
    cp "$lib_path" "$dest_dir/"
    
    # Mark as processed
    processed_libs="$processed_libs $lib_name"
    
    # Get dependencies of this library
    local lib_deps
    lib_deps=$(get_dependencies "$lib_path")
    
    # Recursively copy dependencies
    while IFS= read -r dep_path; do
        if [[ -n "$dep_path" && -f "$dep_path" ]]; then
            copy_library_recursive "$dep_path" "$dest_dir" "$processed_libs"
        fi
    done <<< "$lib_deps"
}

# Collect all dependencies
echo "Collecting shared library dependencies..."
ALL_DEPS=""
PROCESSED_LIBS=""

for exe in "${EXECUTABLES[@]}"; do
    if [[ -f "$exe" ]]; then
        echo "Analyzing: $(basename "$exe")"
        
        # Copy executable
        cp "$exe" "$BUNDLE_PATH/bin/"
        
        # Get dependencies
        deps=$(get_dependencies "$exe")
        while IFS= read -r dep_path; do
            if [[ -n "$dep_path" && -f "$dep_path" ]]; then
                copy_library_recursive "$dep_path" "$BUNDLE_PATH/lib" "$PROCESSED_LIBS"
            fi
        done <<< "$deps"
    else
        echo "Warning: Executable not found: $exe"
    fi
done

# Create launcher script
cat > "$BUNDLE_PATH/fuego-launcher.sh" << 'EOF'
#!/bin/bash
# Fuego Bundle Launcher
# This script sets up the environment and launches Fuego executables

BUNDLE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$BUNDLE_DIR/lib:$LD_LIBRARY_PATH"

# Function to show available commands
show_help() {
    echo "Fuego CLI Suite - Available commands:"
    echo "  fuegod           - Start Fuego daemon"
    echo "  fuego-wallet-cli - Start wallet CLI"
    echo "  walletd          - Start wallet daemon"
    echo "  optimizer        - Start optimizer"
    echo ""
    echo "Usage: $0 [command] [arguments...]"
    echo "   or: $0 --help"
}

# Check if any arguments provided
if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

# Handle help
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    show_help
    exit 0
fi

# Get command name
COMMAND="$1"
shift

# Check if command exists
if [ ! -f "$BUNDLE_DIR/bin/$COMMAND" ]; then
    echo "Error: Command '$COMMAND' not found"
    echo ""
    show_help
    exit 1
fi

# Execute command
exec "$BUNDLE_DIR/bin/$COMMAND" "$@"
EOF

chmod +x "$BUNDLE_PATH/fuego-launcher.sh"

# Create individual launcher scripts for each executable
for exe in "${EXECUTABLES[@]}"; do
    if [[ -f "$exe" ]]; then
        exe_name="$(basename "$exe")"
        cat > "$BUNDLE_PATH/$exe_name" << EOF
#!/bin/bash
# Launcher for $exe_name
BUNDLE_DIR="\$(cd "\$(dirname "\${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="\$BUNDLE_DIR/lib:\$LD_LIBRARY_PATH"
exec "\$BUNDLE_DIR/bin/$exe_name" "\$@"
EOF
        chmod +x "$BUNDLE_PATH/$exe_name"
    fi
done

# Create README
cat > "$BUNDLE_PATH/README.md" << EOF
# Fuego CLI Suite Bundle

This is a self-contained bundle of the Fuego cryptocurrency suite with all required shared libraries included.

## Contents

- **bin/**: Executable binaries
- **lib/**: Shared libraries
- **fuego-launcher.sh**: Main launcher script
- Individual launcher scripts for each executable

## Usage

### Using the main launcher:
\`\`\`bash
./fuego-launcher.sh fuegod --help
./fuego-launcher.sh fuego-wallet-cli
./fuego-launcher.sh walletd
./fuego-launcher.sh optimizer
\`\`\`

### Using individual launchers:
\`\`\`bash
./fuegod --help
./fuego-wallet-cli
./walletd
./optimizer
\`\`\`

## System Requirements

- Linux x86_64
- glibc 2.31 or later (compatible with Ubuntu 20.04+)

## Troubleshooting

If you encounter library loading errors:
1. Ensure you're on a compatible Linux distribution
2. Check that the bundle directory is not on a filesystem mounted with noexec
3. Verify that all files have correct permissions

## Platform Information

- Built on: $(uname -a)
- Bundle created: $(date)
- Architecture: $PLATFORM-$ARCH
EOF

# Create version info
if [[ -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
    VERSION=$(grep -E "^set\(VERSION" "$PROJECT_ROOT/CMakeLists.txt" | sed 's/.*"\([^"]*\)".*/\1/' || echo "unknown")
    COMMIT=$(grep -E "^set\(COMMIT" "$PROJECT_ROOT/CMakeLists.txt" | sed 's/.*"\([^"]*\)".*/\1/' || echo "unknown")
    
    cat > "$BUNDLE_PATH/VERSION" << EOF
VERSION=$VERSION
COMMIT=$COMMIT
PLATFORM=$PLATFORM
ARCH=$ARCH
BUILD_DATE=$(date -u +"%Y-%m-%d %H:%M:%S UTC")
EOF
fi

# Set permissions
chmod -R 755 "$BUNDLE_PATH"

# Calculate bundle size
BUNDLE_SIZE=$(du -sh "$BUNDLE_PATH" | cut -f1)

echo ""
echo "Bundle created successfully!"
echo "Location: $BUNDLE_PATH"
echo "Size: $BUNDLE_SIZE"
echo ""
echo "Libraries included:"
ls -la "$BUNDLE_PATH/lib/" | grep -E "\.so" | wc -l | xargs echo "  Count:"
echo ""
echo "To test the bundle:"
echo "  cd $BUNDLE_PATH"
echo "  ./fuego-launcher.sh --help"