#pragma once

#include "../Common/TUICommon.h"
#include <CryptoNote.h>
#include <IWallet.h>
#include <memory>
#include <vector>
#include <string>

namespace FuegoTUI {

struct TransactionInfo {
    std::string hash;
    std::string timestamp;
    std::string type; // "incoming", "outgoing", "pending"
    uint64_t amount;
    uint64_t fee;
    std::string address;
    uint32_t confirmations;
    bool isConfirmed;
};

struct WalletInfo {
    uint64_t balance;
    uint64_t unlockedBalance;
    uint64_t pendingBalance;
    std::string address;
    std::string viewKey;
    std::string spendKey;
    bool isLoaded;
    std::string walletFile;
};

class WalletTUI : public TUIComponent {
public:
    WalletTUI();
    ~WalletTUI() override = default;
    
    void render() override;
    void handleInput(int key) override;
    
    void setWallet(std::shared_ptr<CryptoNote::IWallet> wallet);
    void setWalletInfo(const WalletInfo& info);
    void addTransaction(const TransactionInfo& transaction);
    void updateBalance(uint64_t balance, uint64_t unlockedBalance);
    
    void showSendDialog();
    void showReceiveDialog();
    void showTransactionHistory();
    void showWalletSettings();
    
    void refreshData();

private:
    std::shared_ptr<CryptoNote::IWallet> wallet;
    WalletInfo walletInfo;
    std::vector<TransactionInfo> transactions;
    
    // UI Components
    std::unique_ptr<TUIWindow> mainWindow;
    std::unique_ptr<TUIWindow> balanceWindow;
    std::unique_ptr<TUIWindow> transactionWindow;
    std::unique_ptr<TUIList> menuList;
    std::unique_ptr<TUIList> transactionList;
    std::unique_ptr<TUIButton> sendButton;
    std::unique_ptr<TUIButton> receiveButton;
    std::unique_ptr<TUIButton> historyButton;
    std::unique_ptr<TUIButton> settingsButton;
    
    // Dialog components
    std::unique_ptr<TUIWindow> sendDialog;
    std::unique_ptr<TUIWindow> receiveDialog;
    std::unique_ptr<TUIInput> addressInput;
    std::unique_ptr<TUIInput> amountInput;
    std::unique_ptr<TUIInput> feeInput;
    std::unique_ptr<TUIButton> confirmButton;
    std::unique_ptr<TUIButton> cancelButton;
    
    bool showSendDialogFlag = false;
    bool showReceiveDialogFlag = false;
    bool showHistoryFlag = false;
    bool showSettingsFlag = false;
    
    int selectedMenuIndex = 0;
    int selectedTransactionIndex = 0;
    
    void renderMainInterface();
    void renderBalance();
    void renderTransactions();
    void renderMenu();
    void renderSendDialog();
    void renderReceiveDialog();
    void renderTransactionHistory();
    void renderWalletSettings();
    
    void handleMenuInput(int key);
    void handleTransactionInput(int key);
    void handleSendDialogInput(int key);
    void handleReceiveDialogInput(int key);
    
    void sendTransaction();
    void generateNewAddress();
    void exportKeys();
    void importKeys();
    void changePassword();
    
    void updateTransactionList();
    void updateBalanceDisplay();
    
    std::string formatTransactionType(const std::string& type) const;
    std::string formatTransactionAmount(uint64_t amount, const std::string& type) const;
};

class WalletTUIManager {
public:
    static WalletTUIManager& getInstance();
    
    void initialize();
    void run();
    void cleanup();
    
    void setWalletFile(const std::string& walletFile);
    void setPassword(const std::string& password);
    void setDaemonAddress(const std::string& address, uint16_t port);
    
    bool loadWallet();
    void createWallet();
    void saveWallet();
    
    void refreshWalletData();
    void syncWallet();
    
private:
    WalletTUIManager() = default;
    ~WalletTUIManager() = default;
    WalletTUIManager(const WalletTUIManager&) = delete;
    WalletTUIManager& operator=(const WalletTUIManager&) = delete;
    
    std::unique_ptr<WalletTUI> walletTUI;
    std::string walletFile;
    std::string password;
    std::string daemonAddress;
    uint16_t daemonPort = 0;
    
    bool initialized = false;
    bool walletLoaded = false;
    
    void setupWallet();
    void loadWalletData();
    void saveWalletData();
};

} // namespace FuegoTUI