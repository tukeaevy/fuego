# Fuego TUI (Terminal User Interface)

A comprehensive ncurses-based terminal user interface for the Fuego XFG privacy blockchain network.

## Features

### Wallet Management
- **Send XFG**: Send XFG to other addresses with customizable fees
- **Receive XFG**: Display your XFG address for receiving payments
- **Transaction History**: View recent transactions with detailed information
- **Balance Display**: Real-time balance monitoring (total, unlocked, pending)
- **Wallet Settings**: Configure wallet options and export/import keys

### Daemon Monitoring
- **Blockchain Status**: Monitor blockchain height, difficulty, and synchronization
- **Peer Management**: View connected peers and their status
- **Network Statistics**: Monitor network traffic and connection statistics
- **Mining Status**: Track mining performance and statistics
- **System Logs**: View daemon logs in real-time

### Mining Interface
- **Mining Control**: Start, stop, pause, and resume mining
- **Performance Stats**: Monitor hashrate, efficiency, and shares
- **Hash Results**: View recent hash results and validation status
- **Pool Configuration**: Configure mining pool settings
- **Thread Management**: Adjust mining threads and intensity

## Building

### Prerequisites

- CMake 3.10 or higher
- GCC 7.0 or higher (or equivalent C++17 compiler)
- ncurses development libraries
- pthread support

### Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install libncurses5-dev libncursesw5-dev

# CentOS/RHEL/Fedora
sudo yum install ncurses-devel

# macOS
brew install ncurses

# Arch Linux
sudo pacman -S ncurses
```

### Compilation

```bash
# From the project root
mkdir build
cd build
cmake ..
make fuego-tui

# Or build with tests
cmake -DBUILD_TUI_TESTS=ON ..
make tui_test
```

## Usage

### Command Line Interface

```bash
# Full interface (default)
./fuego-tui

# Wallet only
./fuego-tui --mode wallet --wallet my_wallet.wallet

# Daemon monitoring
./fuego-tui --mode daemon --daemon 192.168.1.100 --daemon-port 8080

# Mining interface
./fuego-tui --mode mining --mining-addr XFG... --mining-threads 4

# Help
./fuego-tui --help
```

### Command Line Options

- `-m, --mode MODE`: TUI mode (wallet|daemon|mining|full)
- `-w, --wallet FILE`: Wallet file path
- `-p, --password PASS`: Wallet password
- `-d, --daemon ADDR`: Daemon address
- `--daemon-port PORT`: Daemon port (default: 8080)
- `--mining-addr ADDR`: Mining address
- `--mining-threads N`: Number of mining threads
- `-h, --help`: Show help message
- `-v, --version`: Show version information

### Keyboard Shortcuts

#### Global Shortcuts
- `q` or `ESC`: Exit application
- `Tab`: Switch between input fields
- `Enter`: Confirm selection
- `Arrow Keys`: Navigate menus and lists

#### Wallet Interface
- `s`: Send XFG
- `r`: Receive XFG
- `h`: Transaction history
- `c`: Wallet configuration

#### Daemon Interface
- `r`: Refresh data
- `s`: Start/stop daemon
- `m`: Start/stop mining

#### Mining Interface
- `s`: Start/stop mining
- `p`: Pause/resume mining
- `c`: Configuration
- `r`: Refresh statistics

## Architecture

### Core Components

- **TUICommon**: Base TUI framework with common UI components
- **WalletTUI**: Wallet management interface
- **DaemonTUI**: Daemon monitoring interface
- **MiningTUI**: Mining control interface
- **TUIMain**: Main application coordinator

### UI Components

- **TUIWindow**: Window container with borders and titles
- **TUIButton**: Interactive buttons with callbacks
- **TUIList**: Scrollable lists for data display
- **TUIInput**: Text input fields with validation
- **TUIManager**: Main TUI coordinator and event handler

### Design Patterns

- **Singleton**: TUIManager and specialized managers
- **Observer**: Event-driven UI updates
- **Factory**: Component creation and management
- **Strategy**: Different TUI modes and configurations

## Configuration

### Wallet Configuration

```cpp
// Set wallet file and password
tuiMain.setWalletFile("fuego_wallet.wallet");
tuiMain.setPassword("your_password");

// Set daemon connection
tuiMain.setDaemonAddress("localhost", 8080);
```

### Mining Configuration

```cpp
// Set mining parameters
tuiMain.setMiningAddress("XFG...");
tuiMain.setMiningThreads(4);

// Set pool connection
tuiMain.setDaemonAddress("pool.fuego.org", 8080);
```

## Development

### Adding New Components

1. Create new component class inheriting from `TUIComponent`
2. Implement `render()` and `handleInput()` methods
3. Add to appropriate manager class
4. Update CMakeLists.txt if needed

### Testing

```bash
# Run TUI tests
./tui_test

# Run with valgrind (if available)
valgrind --leak-check=full ./tui_test
```

### Code Style

- Use consistent indentation (4 spaces)
- Follow C++17 standards
- Use meaningful variable and function names
- Add comments for complex logic
- Use const correctness

## Troubleshooting

### Common Issues

1. **ncurses not found**: Install ncurses development packages
2. **Compilation errors**: Ensure C++17 support
3. **Runtime crashes**: Check terminal compatibility
4. **Display issues**: Verify terminal size and ncurses support

### Debug Mode

```bash
# Enable debug output
export FUEGO_TUI_DEBUG=1
./fuego-tui
```

### Logging

The TUI uses the standard Fuego logging system. Logs are written to:
- Console output (if available)
- Log files (if configured)
- System logs (platform dependent)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This TUI module is part of the Fuego project and follows the same license terms.

## Support

For issues and questions:
- GitHub Issues: [Fuego Repository](https://github.com/usexfg/fuego)
- Website: [https://usexfg.org](https://usexfg.org)
- Documentation: [Fuego Docs](https://docs.usexfg.org)