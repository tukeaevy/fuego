#include "DaemonTUI.h"
#include <CryptoNoteCore/Currency.h>
#include <Logging/ConsoleLogger.h>
#include <Common/ConsoleTools.h>
#include <thread>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace FuegoTUI {

DaemonTUI::DaemonTUI() {
    // Initialize UI components
    mainWindow = std::make_unique<TUIWindow>(Position(1, 1), Size(100, 30), "Fuego Daemon TUI");
    statusWindow = std::make_unique<TUIWindow>(Position(1, 1), Size(50, 12), "Status");
    infoWindow = std::make_unique<TUIWindow>(Position(52, 1), Size(48, 12), "Blockchain Info");
    
    // Create menu
    menuList = std::make_unique<TUIList>(Position(1, 14), Size(30, 10));
    menuList->addItem("Blockchain Info");
    menuList->addItem("Peer List");
    menuList->addItem("Mining Status");
    menuList->addItem("Network Stats");
    menuList->addItem("Logs");
    menuList->addItem("Settings");
    menuList->addItem("Refresh Data");
    menuList->addItem("Exit");
    
    // Create peer list
    peerList = std::make_unique<TUIList>(Position(32, 14), Size(68, 10));
    
    // Create log list
    logList = std::make_unique<TUIList>(Position(1, 14), Size(98, 10));
    
    // Initialize data
    blockchainInfo.height = 0;
    blockchainInfo.difficulty = 0;
    blockchainInfo.hashrate = 0;
    blockchainInfo.timestamp = 0;
    blockchainInfo.topBlockHash = "";
    blockchainInfo.totalTransactions = 0;
    blockchainInfo.totalCoins = 0;
    blockchainInfo.networkHeight = 0;
    blockchainInfo.isSynchronized = false;
    blockchainInfo.peerCount = 0;
    blockchainInfo.version = "1.0.0";
    
    miningInfo.isMining = false;
    miningInfo.threads = 0;
    miningInfo.hashrate = 0;
    miningInfo.blocksFound = 0;
    miningInfo.address = "";
    miningInfo.difficulty = 0;
    miningInfo.lastBlockHash = "";
    
    networkStats.bytesReceived = 0;
    networkStats.bytesSent = 0;
    networkStats.packetsReceived = 0;
    networkStats.packetsSent = 0;
    networkStats.activeConnections = 0;
    networkStats.maxConnections = 100;
    networkStats.uptime = 0.0;
    
    lastUpdate = std::chrono::steady_clock::now();
}

void DaemonTUI::render() {
    renderMainInterface();
    renderStatus();
    renderMenu();
    
    if (showBlockchainFlag) {
        renderBlockchainInfo();
    } else if (showPeersFlag) {
        renderPeerList();
    } else if (showMiningFlag) {
        renderMiningStatus();
    } else if (showNetworkFlag) {
        renderNetworkStats();
    } else if (showLogsFlag) {
        renderLogs();
    } else if (showSettingsFlag) {
        renderSettings();
    } else {
        renderBlockchainInfo();
    }
}

void DaemonTUI::handleInput(int key) {
    if (showPeersFlag) {
        handlePeerInput(key);
    } else if (showLogsFlag) {
        handleLogInput(key);
    } else {
        handleMenuInput(key);
    }
}

void DaemonTUI::setNode(std::shared_ptr<CryptoNote::INode> node) {
    this->node = node;
}

void DaemonTUI::updateBlockchainInfo(const BlockchainInfo& info) {
    blockchainInfo = info;
}

void DaemonTUI::updatePeerInfo(const std::vector<PeerInfo>& peers) {
    this->peers = peers;
}

void DaemonTUI::updateMiningInfo(const MiningInfo& info) {
    miningInfo = info;
}

void DaemonTUI::updateNetworkStats(const NetworkStats& stats) {
    networkStats = stats;
}

void DaemonTUI::showBlockchainInfo() {
    showBlockchainFlag = true;
    showPeersFlag = false;
    showMiningFlag = false;
    showNetworkFlag = false;
    showLogsFlag = false;
    showSettingsFlag = false;
}

void DaemonTUI::showPeerList() {
    showPeersFlag = true;
    showBlockchainFlag = false;
    showMiningFlag = false;
    showNetworkFlag = false;
    showLogsFlag = false;
    showSettingsFlag = false;
}

void DaemonTUI::showMiningStatus() {
    showMiningFlag = true;
    showBlockchainFlag = false;
    showPeersFlag = false;
    showNetworkFlag = false;
    showLogsFlag = false;
    showSettingsFlag = false;
}

void DaemonTUI::showNetworkStats() {
    showNetworkFlag = true;
    showBlockchainFlag = false;
    showPeersFlag = false;
    showMiningFlag = false;
    showLogsFlag = false;
    showSettingsFlag = false;
}

void DaemonTUI::showLogs() {
    showLogsFlag = true;
    showBlockchainFlag = false;
    showPeersFlag = false;
    showMiningFlag = false;
    showNetworkFlag = false;
    showSettingsFlag = false;
}

void DaemonTUI::showSettings() {
    showSettingsFlag = true;
    showBlockchainFlag = false;
    showPeersFlag = false;
    showMiningFlag = false;
    showNetworkFlag = false;
    showLogsFlag = false;
}

void DaemonTUI::refreshData() {
    updateBlockchainData();
    updatePeerData();
    updateMiningData();
    updateNetworkData();
    updateLogs();
    lastUpdate = std::chrono::steady_clock::now();
}

void DaemonTUI::startMining() {
    miningInfo.isMining = true;
    // Implementation would start mining
}

void DaemonTUI::stopMining() {
    miningInfo.isMining = false;
    // Implementation would stop mining
}

void DaemonTUI::restartDaemon() {
    // Implementation would restart daemon
}

void DaemonTUI::renderMainInterface() {
    if (mainWindow) {
        mainWindow->render();
    }
}

void DaemonTUI::renderStatus() {
    if (!statusWindow) return;
    
    statusWindow->clear();
    statusWindow->addText("Daemon Status", 1, 0, ColorPair::HEADER);
    
    statusWindow->addText("Sync Status:", 1, 2, ColorPair::INFO);
    statusWindow->addText(getSyncStatus(), 15, 2, 
                         blockchainInfo.isSynchronized ? ColorPair::SUCCESS : ColorPair::WARNING);
    
    statusWindow->addText("Mining Status:", 1, 3, ColorPair::INFO);
    statusWindow->addText(getMiningStatus(), 15, 3, 
                         miningInfo.isMining ? ColorPair::SUCCESS : ColorPair::DEFAULT);
    
    statusWindow->addText("Peer Count:", 1, 4, ColorPair::INFO);
    statusWindow->addText(std::to_string(blockchainInfo.peerCount), 15, 4, ColorPair::DEFAULT);
    
    statusWindow->addText("Uptime:", 1, 5, ColorPair::INFO);
    statusWindow->addText(formatUptime(networkStats.uptime), 15, 5, ColorPair::DEFAULT);
    
    statusWindow->addText("Version:", 1, 6, ColorPair::INFO);
    statusWindow->addText(blockchainInfo.version, 15, 6, ColorPair::DEFAULT);
    
    statusWindow->refresh();
}

void DaemonTUI::renderBlockchainInfo() {
    if (!infoWindow) return;
    
    infoWindow->clear();
    infoWindow->setTitle("Blockchain Information");
    infoWindow->render();
    
    infoWindow->addText("Height:", 1, 1, ColorPair::INFO);
    infoWindow->addText(std::to_string(blockchainInfo.height), 15, 1, ColorPair::DEFAULT);
    
    infoWindow->addText("Network Height:", 1, 2, ColorPair::INFO);
    infoWindow->addText(std::to_string(blockchainInfo.networkHeight), 15, 2, ColorPair::DEFAULT);
    
    infoWindow->addText("Difficulty:", 1, 3, ColorPair::INFO);
    infoWindow->addText(formatDifficulty(blockchainInfo.difficulty), 15, 3, ColorPair::DEFAULT);
    
    infoWindow->addText("Hashrate:", 1, 4, ColorPair::INFO);
    infoWindow->addText(formatHashrate(blockchainInfo.hashrate), 15, 4, ColorPair::DEFAULT);
    
    infoWindow->addText("Total Coins:", 1, 5, ColorPair::INFO);
    infoWindow->addText(formatXFGAmount(blockchainInfo.totalCoins), 15, 5, ColorPair::SUCCESS);
    
    infoWindow->addText("Transactions:", 1, 6, ColorPair::INFO);
    infoWindow->addText(std::to_string(blockchainInfo.totalTransactions), 15, 6, ColorPair::DEFAULT);
    
    infoWindow->addText("Top Block:", 1, 7, ColorPair::INFO);
    infoWindow->addText(formatHash(blockchainInfo.topBlockHash), 1, 8, ColorPair::DEFAULT);
    
    infoWindow->refresh();
}

void DaemonTUI::renderPeerList() {
    if (!peerList) return;
    
    peerList->clear();
    
    for (const auto& peer : peers) {
        std::stringstream ss;
        ss << peer.address << ":" << peer.port;
        if (peer.isOnline) {
            ss << " [ONLINE]";
        } else {
            ss << " [OFFLINE]";
        }
        ss << " H:" << peer.height;
        if (peer.latency > 0) {
            ss << " L:" << peer.latency << "ms";
        }
        
        peerList->addItem(ss.str());
    }
    
    peerList->render();
}

void DaemonTUI::renderMiningStatus() {
    if (!infoWindow) return;
    
    infoWindow->clear();
    infoWindow->setTitle("Mining Status");
    infoWindow->render();
    
    infoWindow->addText("Mining:", 1, 1, ColorPair::INFO);
    infoWindow->addText(miningInfo.isMining ? "Active" : "Inactive", 15, 1, 
                       miningInfo.isMining ? ColorPair::SUCCESS : ColorPair::DEFAULT);
    
    infoWindow->addText("Threads:", 1, 2, ColorPair::INFO);
    infoWindow->addText(std::to_string(miningInfo.threads), 15, 2, ColorPair::DEFAULT);
    
    infoWindow->addText("Hashrate:", 1, 3, ColorPair::INFO);
    infoWindow->addText(formatHashrate(miningInfo.hashrate), 15, 3, ColorPair::DEFAULT);
    
    infoWindow->addText("Blocks Found:", 1, 4, ColorPair::INFO);
    infoWindow->addText(std::to_string(miningInfo.blocksFound), 15, 4, ColorPair::SUCCESS);
    
    infoWindow->addText("Address:", 1, 5, ColorPair::INFO);
    infoWindow->addText(formatHash(miningInfo.address), 1, 6, ColorPair::DEFAULT);
    
    infoWindow->addText("Difficulty:", 1, 7, ColorPair::INFO);
    infoWindow->addText(formatDifficulty(miningInfo.difficulty), 15, 7, ColorPair::DEFAULT);
    
    infoWindow->refresh();
}

void DaemonTUI::renderNetworkStats() {
    if (!infoWindow) return;
    
    infoWindow->clear();
    infoWindow->setTitle("Network Statistics");
    infoWindow->render();
    
    infoWindow->addText("Bytes Received:", 1, 1, ColorPair::INFO);
    infoWindow->addText(formatBytes(networkStats.bytesReceived), 15, 1, ColorPair::DEFAULT);
    
    infoWindow->addText("Bytes Sent:", 1, 2, ColorPair::INFO);
    infoWindow->addText(formatBytes(networkStats.bytesSent), 15, 2, ColorPair::DEFAULT);
    
    infoWindow->addText("Packets Received:", 1, 3, ColorPair::INFO);
    infoWindow->addText(std::to_string(networkStats.packetsReceived), 15, 3, ColorPair::DEFAULT);
    
    infoWindow->addText("Packets Sent:", 1, 4, ColorPair::INFO);
    infoWindow->addText(std::to_string(networkStats.packetsSent), 15, 4, ColorPair::DEFAULT);
    
    infoWindow->addText("Active Connections:", 1, 5, ColorPair::INFO);
    infoWindow->addText(std::to_string(networkStats.activeConnections), 15, 5, ColorPair::DEFAULT);
    
    infoWindow->addText("Max Connections:", 1, 6, ColorPair::INFO);
    infoWindow->addText(std::to_string(networkStats.maxConnections), 15, 6, ColorPair::DEFAULT);
    
    infoWindow->refresh();
}

void DaemonTUI::renderLogs() {
    if (!logList) return;
    
    logList->clear();
    
    for (const auto& log : logMessages) {
        logList->addItem(log);
    }
    
    logList->render();
}

void DaemonTUI::renderSettings() {
    if (!infoWindow) return;
    
    infoWindow->clear();
    infoWindow->setTitle("Daemon Settings");
    infoWindow->render();
    
    infoWindow->addText("Settings not implemented yet", 1, 1, ColorPair::WARNING);
    infoWindow->refresh();
}

void DaemonTUI::renderMenu() {
    if (menuList) {
        menuList->render();
    }
}

void DaemonTUI::handleMenuInput(int key) {
    if (menuList) {
        menuList->handleInput(key);
        
        if (key == '\n' || key == '\r') {
            int selected = menuList->getSelectedIndex();
            switch (selected) {
                case 0: // Blockchain Info
                    showBlockchainInfo();
                    break;
                case 1: // Peer List
                    showPeerList();
                    break;
                case 2: // Mining Status
                    showMiningStatus();
                    break;
                case 3: // Network Stats
                    showNetworkStats();
                    break;
                case 4: // Logs
                    showLogs();
                    break;
                case 5: // Settings
                    showSettings();
                    break;
                case 6: // Refresh Data
                    refreshData();
                    break;
                case 7: // Exit
                    // Exit handled by main TUI manager
                    break;
            }
        }
    }
}

void DaemonTUI::handlePeerInput(int key) {
    if (peerList) {
        peerList->handleInput(key);
    }
}

void DaemonTUI::handleLogInput(int key) {
    if (logList) {
        logList->handleInput(key);
    }
}

void DaemonTUI::updateStatus() {
    // Update status information
}

void DaemonTUI::updateBlockchainData() {
    if (node) {
        // Update blockchain data from node
        // This would involve calling node->getLastLocalBlockHeight(), etc.
    }
}

void DaemonTUI::updatePeerData() {
    if (node) {
        // Update peer data from node
        // This would involve calling node->getPeerCount(), etc.
    }
}

void DaemonTUI::updateMiningData() {
    // Update mining data
}

void DaemonTUI::updateNetworkData() {
    // Update network statistics
}

void DaemonTUI::updateLogs() {
    // Add new log messages
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << formatTime(time_t) << " [INFO] Data refreshed";
    logMessages.push_back(ss.str());
    
    // Keep only last 100 log messages
    if (logMessages.size() > 100) {
        logMessages.erase(logMessages.begin());
    }
}

std::string DaemonTUI::formatUptime(double seconds) const {
    int hours = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>((seconds - hours * 3600) / 60);
    int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
    
    std::stringstream ss;
    ss << hours << "h " << minutes << "m " << secs << "s";
    return ss.str();
}

std::string DaemonTUI::formatBytes(uint64_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        unit++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return ss.str();
}

std::string DaemonTUI::formatHashrate(uint64_t hashrate) const {
    const char* units[] = {"H/s", "KH/s", "MH/s", "GH/s", "TH/s"};
    int unit = 0;
    double rate = static_cast<double>(hashrate);
    
    while (rate >= 1000.0 && unit < 4) {
        rate /= 1000.0;
        unit++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << rate << " " << units[unit];
    return ss.str();
}

std::string DaemonTUI::formatDifficulty(uint64_t difficulty) const {
    const char* units[] = {"", "K", "M", "B", "T"};
    int unit = 0;
    double diff = static_cast<double>(difficulty);
    
    while (diff >= 1000.0 && unit < 4) {
        diff /= 1000.0;
        unit++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << diff << units[unit];
    return ss.str();
}

std::string DaemonTUI::getSyncStatus() const {
    if (blockchainInfo.isSynchronized) {
        return "Synchronized";
    } else if (blockchainInfo.height == 0) {
        return "Not Started";
    } else {
        return "Synchronizing";
    }
}

std::string DaemonTUI::getMiningStatus() const {
    return miningInfo.isMining ? "Active" : "Inactive";
}

// DaemonTUIManager implementation
DaemonTUIManager& DaemonTUIManager::getInstance() {
    static DaemonTUIManager instance;
    return instance;
}

void DaemonTUIManager::initialize() {
    if (initialized) return;
    
    daemonTUI = std::make_unique<DaemonTUI>();
    initialized = true;
}

void DaemonTUIManager::run() {
    if (!initialized) return;
    
    auto& tuiManager = TUIManager::getInstance();
    tuiManager.initialize();
    tuiManager.setHeader("Fuego Daemon TUI - XFG Privacy Blockchain");
    tuiManager.addComponent(daemonTUI);
    
    startDataRefreshThread();
    tuiManager.run();
    stopDataRefreshThread();
    tuiManager.cleanup();
}

void DaemonTUIManager::cleanup() {
    if (daemonTUI) {
        daemonTUI.reset();
    }
    initialized = false;
}

void DaemonTUIManager::setNode(std::shared_ptr<CryptoNote::INode> node) {
    this->node = node;
    if (daemonTUI) {
        daemonTUI->setNode(node);
    }
}

void DaemonTUIManager::setDaemonAddress(const std::string& address, uint16_t port) {
    this->daemonAddress = address;
    this->daemonPort = port;
}

void DaemonTUIManager::setMiningAddress(const std::string& address) {
    this->miningAddress = address;
}

void DaemonTUIManager::setMiningThreads(uint32_t threads) {
    this->miningThreads = threads;
}

void DaemonTUIManager::startDaemon() {
    running = true;
    setupDaemon();
}

void DaemonTUIManager::stopDaemon() {
    running = false;
    stopMining();
}

void DaemonTUIManager::restartDaemon() {
    stopDaemon();
    startDaemon();
}

void DaemonTUIManager::refreshData() {
    if (daemonTUI) {
        daemonTUI->refreshData();
    }
}

void DaemonTUIManager::startMining() {
    mining = true;
    if (daemonTUI) {
        daemonTUI->startMining();
    }
}

void DaemonTUIManager::stopMining() {
    mining = false;
    if (daemonTUI) {
        daemonTUI->stopMining();
    }
}

void DaemonTUIManager::setupDaemon() {
    // Setup daemon configuration
}

void DaemonTUIManager::updateData() {
    if (daemonTUI) {
        daemonTUI->refreshData();
    }
}

void DaemonTUIManager::startDataRefreshThread() {
    stopRefresh = false;
    dataRefreshThread = std::thread([this]() {
        while (!stopRefresh) {
            updateData();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void DaemonTUIManager::stopDataRefreshThread() {
    stopRefresh = true;
    if (dataRefreshThread.joinable()) {
        dataRefreshThread.join();
    }
}

} // namespace FuegoTUI