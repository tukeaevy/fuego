#include "WalletTUI.h"
#include <CryptoNoteCore/Currency.h>
#include <Wallet/WalletGreen.h>
#include <WalletLegacy/WalletLegacy.h>
#include <Logging/ConsoleLogger.h>
#include <Common/ConsoleTools.h>
#include <sstream>
#include <iomanip>

namespace FuegoTUI {

WalletTUI::WalletTUI() {
    // Initialize UI components
    mainWindow = std::make_unique<TUIWindow>(Position(1, 1), Size(80, 24), "Fuego Wallet TUI");
    balanceWindow = std::make_unique<TUIWindow>(Position(1, 1), Size(40, 8), "Balance");
    transactionWindow = std::make_unique<TUIWindow>(Position(42, 1), Size(38, 15), "Recent Transactions");
    
    // Create menu
    menuList = std::make_unique<TUIList>(Position(1, 10), Size(40, 8));
    menuList->addItem("Send XFG");
    menuList->addItem("Receive XFG");
    menuList->addItem("Transaction History");
    menuList->addItem("Wallet Settings");
    menuList->addItem("Refresh Data");
    menuList->addItem("Exit");
    
    // Create transaction list
    transactionList = std::make_unique<TUIList>(Position(1, 1), Size(36, 13));
    
    // Initialize wallet info
    walletInfo.isLoaded = false;
    walletInfo.balance = 0;
    walletInfo.unlockedBalance = 0;
    walletInfo.pendingBalance = 0;
}

void WalletTUI::render() {
    if (!walletInfo.isLoaded) {
        renderMainInterface();
        return;
    }
    
    renderMainInterface();
    renderBalance();
    renderTransactions();
    renderMenu();
    
    if (showSendDialogFlag) {
        renderSendDialog();
    } else if (showReceiveDialogFlag) {
        renderReceiveDialog();
    } else if (showHistoryFlag) {
        renderTransactionHistory();
    } else if (showSettingsFlag) {
        renderWalletSettings();
    }
}

void WalletTUI::handleInput(int key) {
    if (showSendDialogFlag) {
        handleSendDialogInput(key);
    } else if (showReceiveDialogFlag) {
        handleReceiveDialogInput(key);
    } else if (showHistoryFlag) {
        handleTransactionInput(key);
    } else if (showSettingsFlag) {
        handleMenuInput(key);
    } else {
        handleMenuInput(key);
        handleTransactionInput(key);
    }
}

void WalletTUI::setWallet(std::shared_ptr<CryptoNote::IWallet> wallet) {
    this->wallet = wallet;
    walletInfo.isLoaded = (wallet != nullptr);
}

void WalletTUI::setWalletInfo(const WalletInfo& info) {
    walletInfo = info;
}

void WalletTUI::addTransaction(const TransactionInfo& transaction) {
    transactions.insert(transactions.begin(), transaction);
    
    // Keep only last 50 transactions
    if (transactions.size() > 50) {
        transactions.resize(50);
    }
    
    updateTransactionList();
}

void WalletTUI::updateBalance(uint64_t balance, uint64_t unlockedBalance) {
    walletInfo.balance = balance;
    walletInfo.unlockedBalance = unlockedBalance;
    walletInfo.pendingBalance = balance - unlockedBalance;
    updateBalanceDisplay();
}

void WalletTUI::showSendDialog() {
    showSendDialogFlag = true;
    showReceiveDialogFlag = false;
    showHistoryFlag = false;
    showSettingsFlag = false;
    
    // Create send dialog components
    sendDialog = std::make_unique<TUIWindow>(Position(10, 5), Size(60, 15), "Send XFG");
    addressInput = std::make_unique<TUIInput>(Position(12, 8), Size(56, 3), "Recipient Address:");
    amountInput = std::make_unique<TUIInput>(Position(12, 10), Size(56, 3), "Amount (XFG):");
    feeInput = std::make_unique<TUIInput>(Position(12, 12), Size(56, 3), "Fee (XFG):");
    confirmButton = std::make_unique<TUIButton>("Confirm", Position(15, 14), Size(10, 3));
    cancelButton = std::make_unique<TUIButton>("Cancel", Position(35, 14), Size(10, 3));
    
    // Set default fee
    feeInput->setText("0.0001");
    feeInput->setFocused(true);
}

void WalletTUI::showReceiveDialog() {
    showReceiveDialogFlag = true;
    showSendDialogFlag = false;
    showHistoryFlag = false;
    showSettingsFlag = false;
    
    // Create receive dialog components
    receiveDialog = std::make_unique<TUIWindow>(Position(10, 5), Size(60, 15), "Receive XFG");
}

void WalletTUI::showTransactionHistory() {
    showHistoryFlag = true;
    showSendDialogFlag = false;
    showReceiveDialogFlag = false;
    showSettingsFlag = false;
}

void WalletTUI::showWalletSettings() {
    showSettingsFlag = true;
    showSendDialogFlag = false;
    showReceiveDialogFlag = false;
    showHistoryFlag = false;
}

void WalletTUI::refreshData() {
    if (wallet) {
        // Refresh wallet data from daemon
        // This would typically involve calling wallet->save() and wallet->load()
        updateTransactionList();
        updateBalanceDisplay();
    }
}

void WalletTUI::renderMainInterface() {
    if (mainWindow) {
        mainWindow->render();
    }
}

void WalletTUI::renderBalance() {
    if (!balanceWindow) return;
    
    balanceWindow->clear();
    balanceWindow->addText("Total Balance:", 1, 1, ColorPair::INFO);
    balanceWindow->addText(formatXFGAmount(walletInfo.balance), 15, 1, ColorPair::SUCCESS);
    
    balanceWindow->addText("Unlocked Balance:", 1, 2, ColorPair::INFO);
    balanceWindow->addText(formatXFGAmount(walletInfo.unlockedBalance), 15, 2, ColorPair::SUCCESS);
    
    if (walletInfo.pendingBalance > 0) {
        balanceWindow->addText("Pending Balance:", 1, 3, ColorPair::WARNING);
        balanceWindow->addText(formatXFGAmount(walletInfo.pendingBalance), 15, 3, ColorPair::WARNING);
    }
    
    balanceWindow->addText("Address:", 1, 5, ColorPair::INFO);
    balanceWindow->addText(formatHash(walletInfo.address), 1, 6, ColorPair::DEFAULT);
    
    balanceWindow->refresh();
}

void WalletTUI::renderTransactions() {
    if (!transactionWindow) return;
    
    transactionWindow->clear();
    transactionWindow->addText("Recent Transactions", 1, 0, ColorPair::HEADER);
    
    if (transactionList) {
        transactionList->render();
    }
    
    transactionWindow->refresh();
}

void WalletTUI::renderMenu() {
    if (menuList) {
        menuList->render();
    }
}

void WalletTUI::renderSendDialog() {
    if (!sendDialog) return;
    
    sendDialog->render();
    
    if (addressInput) {
        addressInput->render();
    }
    if (amountInput) {
        amountInput->render();
    }
    if (feeInput) {
        feeInput->render();
    }
    if (confirmButton) {
        confirmButton->render();
    }
    if (cancelButton) {
        cancelButton->render();
    }
}

void WalletTUI::renderReceiveDialog() {
    if (!receiveDialog) return;
    
    receiveDialog->render();
    receiveDialog->addText("Your XFG Address:", 1, 1, ColorPair::INFO);
    receiveDialog->addText(walletInfo.address, 1, 2, ColorPair::HIGHLIGHT);
    receiveDialog->addText("", 1, 3);
    receiveDialog->addText("Share this address to receive XFG payments.", 1, 4, ColorPair::INFO);
    receiveDialog->addText("", 1, 5);
    receiveDialog->addText("Press any key to close...", 1, 6, ColorPair::DEFAULT);
}

void WalletTUI::renderTransactionHistory() {
    // Full transaction history view
    if (transactionWindow) {
        transactionWindow->setTitle("Transaction History");
        transactionWindow->render();
    }
    
    if (transactionList) {
        transactionList->render();
    }
}

void WalletTUI::renderWalletSettings() {
    // Wallet settings view
    if (mainWindow) {
        mainWindow->setTitle("Wallet Settings");
        mainWindow->render();
    }
}

void WalletTUI::handleMenuInput(int key) {
    if (menuList) {
        menuList->handleInput(key);
        
        if (key == '\n' || key == '\r') {
            int selected = menuList->getSelectedIndex();
            switch (selected) {
                case 0: // Send XFG
                    showSendDialog();
                    break;
                case 1: // Receive XFG
                    showReceiveDialog();
                    break;
                case 2: // Transaction History
                    showTransactionHistory();
                    break;
                case 3: // Wallet Settings
                    showWalletSettings();
                    break;
                case 4: // Refresh Data
                    refreshData();
                    break;
                case 5: // Exit
                    // Exit handled by main TUI manager
                    break;
            }
        }
    }
}

void WalletTUI::handleTransactionInput(int key) {
    if (transactionList) {
        transactionList->handleInput(key);
    }
}

void WalletTUI::handleSendDialogInput(int key) {
    if (key == '\t') {
        // Tab between input fields
        if (addressInput && addressInput->isFocused()) {
            addressInput->setFocused(false);
            if (amountInput) amountInput->setFocused(true);
        } else if (amountInput && amountInput->isFocused()) {
            amountInput->setFocused(false);
            if (feeInput) feeInput->setFocused(true);
        } else if (feeInput && feeInput->isFocused()) {
            feeInput->setFocused(false);
            if (addressInput) addressInput->setFocused(true);
        }
    } else if (key == '\n' || key == '\r') {
        sendTransaction();
    } else if (key == 27) { // ESC
        showSendDialogFlag = false;
        sendDialog.reset();
        addressInput.reset();
        amountInput.reset();
        feeInput.reset();
        confirmButton.reset();
        cancelButton.reset();
    } else {
        if (addressInput && addressInput->isFocused()) {
            addressInput->handleInput(key);
        } else if (amountInput && amountInput->isFocused()) {
            amountInput->handleInput(key);
        } else if (feeInput && feeInput->isFocused()) {
            feeInput->handleInput(key);
        }
    }
}

void WalletTUI::handleReceiveDialogInput(int key) {
    if (key == 27 || key == 'q' || key == 'Q') { // ESC or 'q'
        showReceiveDialogFlag = false;
        receiveDialog.reset();
    }
}

void WalletTUI::sendTransaction() {
    if (!addressInput || !amountInput || !feeInput) return;
    
    std::string address = addressInput->getText();
    std::string amountStr = amountInput->getText();
    std::string feeStr = feeInput->getText();
    
    if (address.empty() || amountStr.empty()) {
        // Show error message
        return;
    }
    
    try {
        // Parse amount and fee
        double amount = std::stod(amountStr);
        double fee = std::stod(feeStr);
        
        uint64_t amountAtomic = static_cast<uint64_t>(amount * 100000000); // Convert to atomic units
        uint64_t feeAtomic = static_cast<uint64_t>(fee * 100000000);
        
        if (wallet && amountAtomic <= walletInfo.unlockedBalance) {
            // Create transaction
            // This would involve calling wallet->sendTransaction()
            // For now, just show success message
            
            showSendDialogFlag = false;
            sendDialog.reset();
            addressInput.reset();
            amountInput.reset();
            feeInput.reset();
            confirmButton.reset();
            cancelButton.reset();
            
            // Add to transaction list
            TransactionInfo tx;
            tx.hash = "pending_" + std::to_string(time(nullptr));
            tx.timestamp = formatTime(time(nullptr));
            tx.type = "outgoing";
            tx.amount = amountAtomic;
            tx.fee = feeAtomic;
            tx.address = address;
            tx.confirmations = 0;
            tx.isConfirmed = false;
            
            addTransaction(tx);
            updateBalance(walletInfo.balance - amountAtomic - feeAtomic, walletInfo.unlockedBalance - amountAtomic - feeAtomic);
        }
    } catch (const std::exception& e) {
        // Show error message
    }
}

void WalletTUI::generateNewAddress() {
    if (wallet) {
        // Generate new address
        // This would involve calling wallet->createAddress()
    }
}

void WalletTUI::exportKeys() {
    // Export private keys
    if (wallet) {
        // This would involve calling wallet->exportKeys()
    }
}

void WalletTUI::importKeys() {
    // Import private keys
}

void WalletTUI::changePassword() {
    // Change wallet password
}

void WalletTUI::updateTransactionList() {
    if (!transactionList) return;
    
    transactionList->clear();
    
    for (const auto& tx : transactions) {
        std::stringstream ss;
        ss << formatTransactionType(tx.type) << " " 
           << formatTransactionAmount(tx.amount, tx.type) 
           << " " << formatHash(tx.hash);
        
        if (tx.isConfirmed) {
            ss << " ✓";
        }
        
        transactionList->addItem(ss.str());
    }
}

void WalletTUI::updateBalanceDisplay() {
    // Balance display is updated in renderBalance()
}

std::string WalletTUI::formatTransactionType(const std::string& type) const {
    if (type == "incoming") return "IN";
    if (type == "outgoing") return "OUT";
    if (type == "pending") return "PEND";
    return "UNK";
}

std::string WalletTUI::formatTransactionAmount(uint64_t amount, const std::string& type) const {
    std::string formatted = formatXFGAmount(amount);
    if (type == "outgoing") {
        return "-" + formatted;
    }
    return "+" + formatted;
}

// WalletTUIManager implementation
WalletTUIManager& WalletTUIManager::getInstance() {
    static WalletTUIManager instance;
    return instance;
}

void WalletTUIManager::initialize() {
    if (initialized) return;
    
    walletTUI = std::make_unique<WalletTUI>();
    initialized = true;
}

void WalletTUIManager::run() {
    if (!initialized) return;
    
    auto& tuiManager = TUIManager::getInstance();
    tuiManager.initialize();
    tuiManager.setHeader("Fuego Wallet TUI - XFG Privacy Blockchain");
    tuiManager.addComponent(walletTUI);
    tuiManager.run();
    tuiManager.cleanup();
}

void WalletTUIManager::cleanup() {
    if (walletTUI) {
        walletTUI.reset();
    }
    initialized = false;
}

void WalletTUIManager::setWalletFile(const std::string& walletFile) {
    this->walletFile = walletFile;
}

void WalletTUIManager::setPassword(const std::string& password) {
    this->password = password;
}

void WalletTUIManager::setDaemonAddress(const std::string& address, uint16_t port) {
    this->daemonAddress = address;
    this->daemonPort = port;
}

bool WalletTUIManager::loadWallet() {
    if (!initialized || walletFile.empty()) return false;
    
    try {
        // Load wallet implementation
        // This would involve creating a wallet instance and loading from file
        walletLoaded = true;
        return true;
    } catch (const std::exception& e) {
        walletLoaded = false;
        return false;
    }
}

void WalletTUIManager::createWallet() {
    if (!initialized) return;
    
    try {
        // Create new wallet implementation
        // This would involve creating a new wallet instance
        walletLoaded = true;
    } catch (const std::exception& e) {
        walletLoaded = false;
    }
}

void WalletTUIManager::saveWallet() {
    if (!walletLoaded || !walletTUI) return;
    
    // Save wallet implementation
    // This would involve calling wallet->save()
}

void WalletTUIManager::refreshWalletData() {
    if (!walletLoaded || !walletTUI) return;
    
    walletTUI->refreshData();
}

void WalletTUIManager::syncWallet() {
    if (!walletLoaded || !walletTUI) return;
    
    // Sync wallet with daemon
    // This would involve calling wallet->sync()
}

} // namespace FuegoTUI