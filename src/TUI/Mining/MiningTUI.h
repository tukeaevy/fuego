#pragma once

#include "../Common/TUICommon.h"
#include <CryptoNote.h>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

namespace FuegoTUI {

struct MiningStats {
    bool isMining;
    uint32_t threads;
    uint64_t hashrate;
    uint64_t totalHashes;
    uint64_t blocksFound;
    uint64_t sharesSubmitted;
    uint64_t sharesAccepted;
    uint64_t sharesRejected;
    double efficiency;
    std::string currentBlockHash;
    uint64_t currentDifficulty;
    uint64_t targetDifficulty;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastShareTime;
    std::string miningAddress;
    std::string poolAddress;
    uint16_t poolPort;
    bool connectedToPool;
};

struct HashResult {
    std::string hash;
    uint64_t nonce;
    uint64_t timestamp;
    bool isValid;
    uint64_t difficulty;
};

struct MiningConfig {
    std::string address;
    std::string poolAddress;
    uint16_t poolPort;
    uint32_t threads;
    bool useCPU;
    bool useGPU;
    uint32_t gpuDeviceId;
    uint64_t intensity;
    bool autoStart;
    bool backgroundMining;
};

class MiningTUI : public TUIComponent {
public:
    MiningTUI();
    ~MiningTUI() override = default;
    
    void render() override;
    void handleInput(int key) override;
    
    void setMiningStats(const MiningStats& stats);
    void addHashResult(const HashResult& result);
    void setMiningConfig(const MiningConfig& config);
    
    void startMining();
    void stopMining();
    void pauseMining();
    void resumeMining();
    void restartMining();
    
    void showMiningStatus();
    void showHashResults();
    void showMiningConfig();
    void showPerformanceStats();
    void showPoolStatus();
    
    void refreshData();

private:
    MiningStats miningStats;
    MiningConfig miningConfig;
    std::vector<HashResult> hashResults;
    
    // UI Components
    std::unique_ptr<TUIWindow> mainWindow;
    std::unique_ptr<TUIWindow> statusWindow;
    std::unique_ptr<TUIWindow> statsWindow;
    std::unique_ptr<TUIWindow> configWindow;
    std::unique_ptr<TUIList> menuList;
    std::unique_ptr<TUIList> hashList;
    std::unique_ptr<TUIList> performanceList;
    
    // Control buttons
    std::unique_ptr<TUIButton> startButton;
    std::unique_ptr<TUIButton> stopButton;
    std::unique_ptr<TUIButton> pauseButton;
    std::unique_ptr<TUIButton> configButton;
    
    // Configuration inputs
    std::unique_ptr<TUIInput> addressInput;
    std::unique_ptr<TUIInput> poolInput;
    std::unique_ptr<TUIInput> portInput;
    std::unique_ptr<TUIInput> threadsInput;
    
    bool showStatusFlag = true;
    bool showHashResultsFlag = false;
    bool showConfigFlag = false;
    bool showPerformanceFlag = false;
    bool showPoolFlag = false;
    
    int selectedMenuIndex = 0;
    int selectedHashIndex = 0;
    int selectedPerformanceIndex = 0;
    
    std::chrono::steady_clock::time_point lastUpdate;
    std::atomic<bool> miningPaused{false};
    
    void renderMainInterface();
    void renderMiningStatus();
    void renderHashResults();
    void renderMiningConfig();
    void renderPerformanceStats();
    void renderPoolStatus();
    void renderMenu();
    void renderControls();
    
    void handleMenuInput(int key);
    void handleHashInput(int key);
    void handlePerformanceInput(int key);
    void handleConfigInput(int key);
    
    void updateMiningStats();
    void updateHashResults();
    void updatePerformanceStats();
    void updatePoolStatus();
    
    void saveConfig();
    void loadConfig();
    void resetConfig();
    
    std::string formatHashrate(uint64_t hashrate) const;
    std::string formatDifficulty(uint64_t difficulty) const;
    std::string formatEfficiency(double efficiency) const;
    std::string formatUptime(const std::chrono::steady_clock::time_point& startTime) const;
    std::string formatHash(const std::string& hash) const;
    std::string getMiningStatus() const;
    std::string getPoolStatus() const;
    
    void calculateEfficiency();
    void updateHashrate();
};

class MiningTUIManager {
public:
    static MiningTUIManager& getInstance();
    
    void initialize();
    void run();
    void cleanup();
    
    void setMiningAddress(const std::string& address);
    void setPoolAddress(const std::string& address, uint16_t port);
    void setThreads(uint32_t threads);
    void setIntensity(uint64_t intensity);
    
    void startMining();
    void stopMining();
    void pauseMining();
    void resumeMining();
    
    void refreshData();
    void saveConfig();
    void loadConfig();
    
    bool isMining() const { return miningStats.isMining; }
    bool isPaused() const { return miningPaused; }
    MiningStats getMiningStats() const { return miningStats; }

private:
    MiningTUIManager() = default;
    ~MiningTUIManager() = default;
    MiningTUIManager(const MiningTUIManager&) = delete;
    MiningTUIManager& operator=(const MiningTUIManager&) = delete;
    
    std::unique_ptr<MiningTUI> miningTUI;
    MiningStats miningStats;
    MiningConfig miningConfig;
    
    bool initialized = false;
    std::atomic<bool> miningPaused{false};
    std::thread miningThread;
    std::atomic<bool> stopMiningFlag{false};
    
    void setupMining();
    void startMiningThread();
    void stopMiningThread();
    void miningWorker();
    void updateMiningData();
    void startDataRefreshThread();
    void stopDataRefreshThread();
    
    std::thread dataRefreshThread;
    std::atomic<bool> stopRefresh{false};
};

} // namespace FuegoTUI