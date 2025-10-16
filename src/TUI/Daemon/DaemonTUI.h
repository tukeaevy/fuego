#pragma once

#include "../Common/TUICommon.h"
#include <CryptoNote.h>
#include <INode.h>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

namespace FuegoTUI {

struct BlockchainInfo {
    uint64_t height;
    uint64_t difficulty;
    uint64_t hashrate;
    uint64_t timestamp;
    std::string topBlockHash;
    uint64_t totalTransactions;
    uint64_t totalCoins;
    uint64_t networkHeight;
    bool isSynchronized;
    uint32_t peerCount;
    std::string version;
};

struct PeerInfo {
    std::string address;
    uint32_t port;
    uint64_t lastSeen;
    bool isOnline;
    uint64_t height;
    std::string version;
    uint64_t latency;
};

struct MiningInfo {
    bool isMining;
    uint32_t threads;
    uint64_t hashrate;
    uint64_t blocksFound;
    std::string address;
    uint64_t difficulty;
    std::string lastBlockHash;
};

struct NetworkStats {
    uint64_t bytesReceived;
    uint64_t bytesSent;
    uint64_t packetsReceived;
    uint64_t packetsSent;
    uint32_t activeConnections;
    uint32_t maxConnections;
    double uptime;
};

class DaemonTUI : public TUIComponent {
public:
    DaemonTUI();
    ~DaemonTUI() override = default;
    
    void render() override;
    void handleInput(int key) override;
    
    void setNode(std::shared_ptr<CryptoNote::INode> node);
    void updateBlockchainInfo(const BlockchainInfo& info);
    void updatePeerInfo(const std::vector<PeerInfo>& peers);
    void updateMiningInfo(const MiningInfo& info);
    void updateNetworkStats(const NetworkStats& stats);
    
    void showBlockchainInfo();
    void showPeerList();
    void showMiningStatus();
    void showNetworkStats();
    void showLogs();
    void showSettings();
    
    void refreshData();
    void startMining();
    void stopMining();
    void restartDaemon();

private:
    std::shared_ptr<CryptoNote::INode> node;
    BlockchainInfo blockchainInfo;
    std::vector<PeerInfo> peers;
    MiningInfo miningInfo;
    NetworkStats networkStats;
    
    // UI Components
    std::unique_ptr<TUIWindow> mainWindow;
    std::unique_ptr<TUIWindow> statusWindow;
    std::unique_ptr<TUIWindow> infoWindow;
    std::unique_ptr<TUIList> menuList;
    std::unique_ptr<TUIList> peerList;
    std::unique_ptr<TUIList> logList;
    
    // Status indicators
    std::unique_ptr<TUIButton> syncButton;
    std::unique_ptr<TUIButton> miningButton;
    std::unique_ptr<TUIButton> restartButton;
    
    bool showBlockchainFlag = false;
    bool showPeersFlag = false;
    bool showMiningFlag = false;
    bool showNetworkFlag = false;
    bool showLogsFlag = false;
    bool showSettingsFlag = false;
    
    int selectedMenuIndex = 0;
    int selectedPeerIndex = 0;
    int selectedLogIndex = 0;
    
    std::vector<std::string> logMessages;
    std::chrono::steady_clock::time_point lastUpdate;
    
    void renderMainInterface();
    void renderStatus();
    void renderBlockchainInfo();
    void renderPeerList();
    void renderMiningStatus();
    void renderNetworkStats();
    void renderLogs();
    void renderSettings();
    void renderMenu();
    
    void handleMenuInput(int key);
    void handlePeerInput(int key);
    void handleLogInput(int key);
    
    void updateStatus();
    void updateBlockchainData();
    void updatePeerData();
    void updateMiningData();
    void updateNetworkData();
    void updateLogs();
    
    std::string formatUptime(double seconds) const;
    std::string formatBytes(uint64_t bytes) const;
    std::string formatHashrate(uint64_t hashrate) const;
    std::string formatDifficulty(uint64_t difficulty) const;
    std::string getSyncStatus() const;
    std::string getMiningStatus() const;
};

class DaemonTUIManager {
public:
    static DaemonTUIManager& getInstance();
    
    void initialize();
    void run();
    void cleanup();
    
    void setNode(std::shared_ptr<CryptoNote::INode> node);
    void setDaemonAddress(const std::string& address, uint16_t port);
    void setMiningAddress(const std::string& address);
    void setMiningThreads(uint32_t threads);
    
    void startDaemon();
    void stopDaemon();
    void restartDaemon();
    
    void refreshData();
    void startMining();
    void stopMining();
    
    bool isRunning() const { return running; }
    bool isMining() const { return mining; }

private:
    DaemonTUIManager() = default;
    ~DaemonTUIManager() = default;
    DaemonTUIManager(const DaemonTUIManager&) = delete;
    DaemonTUIManager& operator=(const DaemonTUIManager&) = delete;
    
    std::unique_ptr<DaemonTUI> daemonTUI;
    std::shared_ptr<CryptoNote::INode> node;
    std::string daemonAddress;
    uint16_t daemonPort = 0;
    std::string miningAddress;
    uint32_t miningThreads = 1;
    
    bool initialized = false;
    bool running = false;
    bool mining = false;
    
    void setupDaemon();
    void updateData();
    void startDataRefreshThread();
    void stopDataRefreshThread();
    
    std::thread dataRefreshThread;
    std::atomic<bool> stopRefresh{false};
};

} // namespace FuegoTUI