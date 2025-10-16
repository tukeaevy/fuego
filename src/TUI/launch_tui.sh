#!/bin/bash

# Fuego TUI Launcher Script
# This script provides an easy way to launch the Fuego TUI with different configurations

set -e

# Default values
MODE="full"
WALLET_FILE=""
PASSWORD=""
DAEMON_ADDRESS="localhost"
DAEMON_PORT="8080"
MINING_ADDRESS=""
MINING_THREADS="1"
VERBOSE=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print usage information
print_usage() {
    echo "Fuego TUI Launcher"
    echo "=================="
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -m, --mode MODE        TUI mode (wallet|daemon|mining|full)"
    echo "  -w, --wallet FILE      Wallet file path"
    echo "  -p, --password PASS    Wallet password"
    echo "  -d, --daemon ADDR      Daemon address (default: localhost)"
    echo "  --daemon-port PORT     Daemon port (default: 8080)"
    echo "  --mining-addr ADDR     Mining address"
    echo "  --mining-threads N     Number of mining threads (default: 1)"
    echo "  -v, --verbose          Verbose output"
    echo "  -h, --help             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --mode wallet --wallet my_wallet.wallet"
    echo "  $0 --mode daemon --daemon 192.168.1.100"
    echo "  $0 --mode mining --mining-addr XFG... --mining-threads 4"
    echo "  $0 --mode full --wallet wallet.wallet --daemon pool.fuego.org --daemon-port 8080"
}

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if fuego-tui exists
check_tui_executable() {
    if [ ! -f "./fuego-tui" ]; then
        print_error "fuego-tui executable not found!"
        print_info "Please build the project first:"
        print_info "  mkdir build && cd build"
        print_info "  cmake .. && make fuego-tui"
        exit 1
    fi
}

# Check system requirements
check_requirements() {
    print_info "Checking system requirements..."
    
    # Check terminal size
    if [ -t 1 ]; then
        LINES=$(tput lines)
        COLS=$(tput cols)
        if [ $LINES -lt 24 ] || [ $COLS -lt 80 ]; then
            print_warning "Terminal size is ${COLS}x${LINES}. Recommended minimum is 80x24."
            print_warning "Some UI elements may not display correctly."
        fi
    fi
    
    # Check ncurses support
    if ! command -v tput >/dev/null 2>&1; then
        print_warning "tput command not found. ncurses support may be limited."
    fi
    
    print_success "System requirements check completed."
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -m|--mode)
                MODE="$2"
                shift 2
                ;;
            -w|--wallet)
                WALLET_FILE="$2"
                shift 2
                ;;
            -p|--password)
                PASSWORD="$2"
                shift 2
                ;;
            -d|--daemon)
                DAEMON_ADDRESS="$2"
                shift 2
                ;;
            --daemon-port)
                DAEMON_PORT="$2"
                shift 2
                ;;
            --mining-addr)
                MINING_ADDRESS="$2"
                shift 2
                ;;
            --mining-threads)
                MINING_THREADS="$2"
                shift 2
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -h|--help)
                print_usage
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
}

# Validate configuration
validate_config() {
    print_info "Validating configuration..."
    
    # Validate mode
    case $MODE in
        wallet|daemon|mining|full)
            ;;
        *)
            print_error "Invalid mode: $MODE. Must be one of: wallet, daemon, mining, full"
            exit 1
            ;;
    esac
    
    # Validate wallet file if provided
    if [ -n "$WALLET_FILE" ] && [ ! -f "$WALLET_FILE" ]; then
        print_warning "Wallet file '$WALLET_FILE' does not exist. A new wallet will be created."
    fi
    
    # Validate daemon port
    if ! [[ "$DAEMON_PORT" =~ ^[0-9]+$ ]] || [ "$DAEMON_PORT" -lt 1 ] || [ "$DAEMON_PORT" -gt 65535 ]; then
        print_error "Invalid daemon port: $DAEMON_PORT. Must be a number between 1 and 65535."
        exit 1
    fi
    
    # Validate mining threads
    if ! [[ "$MINING_THREADS" =~ ^[0-9]+$ ]] || [ "$MINING_THREADS" -lt 1 ]; then
        print_error "Invalid mining threads: $MINING_THREADS. Must be a positive number."
        exit 1
    fi
    
    print_success "Configuration validation completed."
}

# Build command line arguments for fuego-tui
build_tui_args() {
    local args=""
    
    args="$args --mode $MODE"
    
    if [ -n "$WALLET_FILE" ]; then
        args="$args --wallet $WALLET_FILE"
    fi
    
    if [ -n "$PASSWORD" ]; then
        args="$args --password $PASSWORD"
    fi
    
    args="$args --daemon $DAEMON_ADDRESS"
    args="$args --daemon-port $DAEMON_PORT"
    
    if [ -n "$MINING_ADDRESS" ]; then
        args="$args --mining-addr $MINING_ADDRESS"
    fi
    
    args="$args --mining-threads $MINING_THREADS"
    
    echo "$args"
}

# Main function
main() {
    print_info "Fuego TUI Launcher starting..."
    
    # Parse arguments
    parse_arguments "$@"
    
    # Check requirements
    check_requirements
    
    # Validate configuration
    validate_config
    
    # Check if executable exists
    check_tui_executable
    
    # Build command
    local tui_args=$(build_tui_args)
    
    if [ "$VERBOSE" = true ]; then
        print_info "Launching fuego-tui with arguments: $tui_args"
    fi
    
    # Launch TUI
    print_success "Launching Fuego TUI..."
    print_info "Mode: $MODE"
    print_info "Daemon: $DAEMON_ADDRESS:$DAEMON_PORT"
    if [ -n "$WALLET_FILE" ]; then
        print_info "Wallet: $WALLET_FILE"
    fi
    if [ -n "$MINING_ADDRESS" ]; then
        print_info "Mining Address: $MINING_ADDRESS"
        print_info "Mining Threads: $MINING_THREADS"
    fi
    echo ""
    
    # Execute fuego-tui
    exec ./fuego-tui $tui_args
}

# Run main function with all arguments
main "$@"