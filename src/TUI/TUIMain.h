#pragma once

#include "Common/TUICommon.h"
#include "Wallet/WalletTUI.h"
#include "Daemon/DaemonTUI.h"
#include "Mining/MiningTUI.h"
#include <memory>
#include <string>

namespace FuegoTUI {

enum class TUIMode {
    WALLET,
    DAEMON,
    MINING,
    FULL
};

class TUIMain {
public:
    static TUIMain& getInstance();
    
    void initialize();
    void run();
    void cleanup();
    
    void setMode(TUIMode mode);
    void setWalletFile(const std::string& walletFile);
    void setPassword(const std::string& password);
    void setDaemonAddress(const std::string& address, uint16_t port);
    void setMiningAddress(const std::string& address);
    void setMiningThreads(uint32_t threads);
    
    void showWallet();
    void showDaemon();
    void showMining();
    void showFullInterface();
    
    void refreshAll();
    void saveAll();
    void loadAll();

private:
    TUIMain() = default;
    ~TUIMain() = default;
    TUIMain(const TUIMain&) = delete;
    TUIMain& operator=(const TUIMain&) = delete;
    
    TUIMode currentMode = TUIMode::FULL;
    bool initialized = false;
    
    std::unique_ptr<WalletTUIManager> walletManager;
    std::unique_ptr<DaemonTUIManager> daemonManager;
    std::unique_ptr<MiningTUIManager> miningManager;
    
    void setupManagers();
    void configureWallet();
    void configureDaemon();
    void configureMining();
};

// Command line interface
class TUICLI {
public:
    static int run(int argc, char* argv[]);
    
private:
    static void printUsage(const char* programName);
    static void printVersion();
    static void printHelp();
    static TUIMode parseMode(const std::string& mode);
    static bool parseArguments(int argc, char* argv[], TUIMain& tuiMain);
};

} // namespace FuegoTUI