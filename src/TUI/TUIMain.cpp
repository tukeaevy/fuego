#include "TUIMain.h"
#include <iostream>
#include <string>
#include <cstring>

namespace FuegoTUI {

TUIMain& TUIMain::getInstance() {
    static TUIMain instance;
    return instance;
}

void TUIMain::initialize() {
    if (initialized) return;
    
    setupManagers();
    initialized = true;
}

void TUIMain::run() {
    if (!initialized) {
        initialize();
    }
    
    switch (currentMode) {
        case TUIMode::WALLET:
            if (walletManager) {
                walletManager->run();
            }
            break;
        case TUIMode::DAEMON:
            if (daemonManager) {
                daemonManager->run();
            }
            break;
        case TUIMode::MINING:
            if (miningManager) {
                miningManager->run();
            }
            break;
        case TUIMode::FULL:
            showFullInterface();
            break;
    }
}

void TUIMain::cleanup() {
    if (walletManager) {
        walletManager->cleanup();
        walletManager.reset();
    }
    
    if (daemonManager) {
        daemonManager->cleanup();
        daemonManager.reset();
    }
    
    if (miningManager) {
        miningManager->cleanup();
        miningManager.reset();
    }
    
    initialized = false;
}

void TUIMain::setMode(TUIMode mode) {
    currentMode = mode;
}

void TUIMain::setWalletFile(const std::string& walletFile) {
    if (walletManager) {
        walletManager->setWalletFile(walletFile);
    }
}

void TUIMain::setPassword(const std::string& password) {
    if (walletManager) {
        walletManager->setPassword(password);
    }
}

void TUIMain::setDaemonAddress(const std::string& address, uint16_t port) {
    if (daemonManager) {
        daemonManager->setDaemonAddress(address, port);
    }
    if (miningManager) {
        miningManager->setPoolAddress(address, port);
    }
}

void TUIMain::setMiningAddress(const std::string& address) {
    if (miningManager) {
        miningManager->setMiningAddress(address);
    }
}

void TUIMain::setMiningThreads(uint32_t threads) {
    if (miningManager) {
        miningManager->setThreads(threads);
    }
}

void TUIMain::showWallet() {
    if (walletManager) {
        walletManager->run();
    }
}

void TUIMain::showDaemon() {
    if (daemonManager) {
        daemonManager->run();
    }
}

void TUIMain::showMining() {
    if (miningManager) {
        miningManager->run();
    }
}

void TUIMain::showFullInterface() {
    auto& tuiManager = TUIManager::getInstance();
    tuiManager.initialize();
    tuiManager.setHeader("Fuego TUI - XFG Privacy Blockchain");
    
    // Create main menu
    auto mainMenu = std::make_shared<TUIList>(Position(1, 1), Size(30, 10));
    mainMenu->addItem("Wallet Management");
    mainMenu->addItem("Daemon Monitor");
    mainMenu->addItem("Mining Interface");
    mainMenu->addItem("Exit");
    
    // Create status window
    auto statusWindow = std::make_shared<TUIWindow>(Position(32, 1), Size(68, 15), "System Status");
    
    // Create info window
    auto infoWindow = std::make_shared<TUIWindow>(Position(1, 12), Size(98, 12), "Information");
    
    // Add components
    tuiManager.addComponent(mainMenu);
    tuiManager.addComponent(statusWindow);
    tuiManager.addComponent(infoWindow);
    
    // Handle menu selection
    auto menuHandler = std::make_shared<TUIComponent>();
    menuHandler->handleInput = [&](int key) {
        if (key == '\n' || key == '\r') {
            int selected = mainMenu->getSelectedIndex();
            switch (selected) {
                case 0: // Wallet Management
                    tuiManager.stop();
                    showWallet();
                    break;
                case 1: // Daemon Monitor
                    tuiManager.stop();
                    showDaemon();
                    break;
                case 2: // Mining Interface
                    tuiManager.stop();
                    showMining();
                    break;
                case 3: // Exit
                    tuiManager.stop();
                    break;
            }
        }
    };
    
    tuiManager.addComponent(menuHandler);
    tuiManager.run();
    tuiManager.cleanup();
}

void TUIMain::refreshAll() {
    if (walletManager) {
        walletManager->refreshWalletData();
    }
    if (daemonManager) {
        daemonManager->refreshData();
    }
    if (miningManager) {
        miningManager->refreshData();
    }
}

void TUIMain::saveAll() {
    if (walletManager) {
        walletManager->saveWallet();
    }
    if (miningManager) {
        miningManager->saveConfig();
    }
}

void TUIMain::loadAll() {
    if (walletManager) {
        walletManager->loadWallet();
    }
    if (miningManager) {
        miningManager->loadConfig();
    }
}

void TUIMain::setupManagers() {
    walletManager = std::make_unique<WalletTUIManager>();
    daemonManager = std::make_unique<DaemonTUIManager>();
    miningManager = std::make_unique<MiningTUIManager>();
    
    walletManager->initialize();
    daemonManager->initialize();
    miningManager->initialize();
    
    configureWallet();
    configureDaemon();
    configureMining();
}

void TUIMain::configureWallet() {
    // Default wallet configuration
    walletManager->setWalletFile("fuego_wallet.wallet");
    walletManager->setPassword("");
    walletManager->setDaemonAddress("localhost", 8080);
}

void TUIMain::configureDaemon() {
    // Default daemon configuration
    daemonManager->setDaemonAddress("localhost", 8080);
}

void TUIMain::configureMining() {
    // Default mining configuration
    miningManager->setMiningAddress("");
    miningManager->setPoolAddress("localhost", 8080);
    miningManager->setThreads(1);
    miningManager->setIntensity(1);
}

// TUICLI implementation
int TUICLI::run(int argc, char* argv[]) {
    TUIMain& tuiMain = TUIMain::getInstance();
    
    if (!parseArguments(argc, argv, tuiMain)) {
        return 1;
    }
    
    tuiMain.initialize();
    tuiMain.run();
    tuiMain.cleanup();
    
    return 0;
}

void TUICLI::printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -m, --mode MODE        TUI mode (wallet|daemon|mining|full)\n";
    std::cout << "  -w, --wallet FILE      Wallet file path\n";
    std::cout << "  -p, --password PASS    Wallet password\n";
    std::cout << "  -d, --daemon ADDR      Daemon address\n";
    std::cout << "  --daemon-port PORT     Daemon port (default: 8080)\n";
    std::cout << "  --mining-addr ADDR     Mining address\n";
    std::cout << "  --mining-threads N     Number of mining threads\n";
    std::cout << "  -h, --help             Show this help message\n";
    std::cout << "  -v, --version          Show version information\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " --mode wallet --wallet my_wallet.wallet\n";
    std::cout << "  " << programName << " --mode daemon --daemon 192.168.1.100\n";
    std::cout << "  " << programName << " --mode mining --mining-addr XFG...\n";
}

void TUICLI::printVersion() {
    std::cout << "Fuego TUI v1.0.0\n";
    std::cout << "XFG Privacy Blockchain Terminal User Interface\n";
    std::cout << "Copyright (c) 2024 Fuego Project\n";
}

void TUICLI::printHelp() {
    printVersion();
    std::cout << "\n";
    printUsage("fuego-tui");
    std::cout << "\nTUI Modes:\n";
    std::cout << "  wallet    - Wallet management interface\n";
    std::cout << "  daemon    - Daemon monitoring interface\n";
    std::cout << "  mining    - Mining interface\n";
    std::cout << "  full      - Full interface with all components (default)\n";
}

TUIMode TUICLI::parseMode(const std::string& mode) {
    if (mode == "wallet") return TUIMode::WALLET;
    if (mode == "daemon") return TUIMode::DAEMON;
    if (mode == "mining") return TUIMode::MINING;
    if (mode == "full") return TUIMode::FULL;
    return TUIMode::FULL; // Default
}

bool TUICLI::parseArguments(int argc, char* argv[], TUIMain& tuiMain) {
    std::string mode = "full";
    std::string walletFile;
    std::string password;
    std::string daemonAddress = "localhost";
    uint16_t daemonPort = 8080;
    std::string miningAddress;
    uint32_t miningThreads = 1;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return false;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return false;
        } else if (arg == "-m" || arg == "--mode") {
            if (i + 1 < argc) {
                mode = argv[++i];
            } else {
                std::cerr << "Error: --mode requires an argument\n";
                return false;
            }
        } else if (arg == "-w" || arg == "--wallet") {
            if (i + 1 < argc) {
                walletFile = argv[++i];
            } else {
                std::cerr << "Error: --wallet requires an argument\n";
                return false;
            }
        } else if (arg == "-p" || arg == "--password") {
            if (i + 1 < argc) {
                password = argv[++i];
            } else {
                std::cerr << "Error: --password requires an argument\n";
                return false;
            }
        } else if (arg == "-d" || arg == "--daemon") {
            if (i + 1 < argc) {
                daemonAddress = argv[++i];
            } else {
                std::cerr << "Error: --daemon requires an argument\n";
                return false;
            }
        } else if (arg == "--daemon-port") {
            if (i + 1 < argc) {
                daemonPort = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Error: --daemon-port requires an argument\n";
                return false;
            }
        } else if (arg == "--mining-addr") {
            if (i + 1 < argc) {
                miningAddress = argv[++i];
            } else {
                std::cerr << "Error: --mining-addr requires an argument\n";
                return false;
            }
        } else if (arg == "--mining-threads") {
            if (i + 1 < argc) {
                miningThreads = static_cast<uint32_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Error: --mining-threads requires an argument\n";
                return false;
            }
        } else {
            std::cerr << "Error: Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            return false;
        }
    }
    
    // Configure TUI
    tuiMain.setMode(parseMode(mode));
    tuiMain.setWalletFile(walletFile);
    tuiMain.setPassword(password);
    tuiMain.setDaemonAddress(daemonAddress, daemonPort);
    tuiMain.setMiningAddress(miningAddress);
    tuiMain.setMiningThreads(miningThreads);
    
    return true;
}

} // namespace FuegoTUI