#include "../TUIMain.h"
#include <iostream>
#include <cassert>

using namespace FuegoTUI;

int main() {
    std::cout << "Fuego TUI Test Suite\n";
    std::cout << "===================\n\n";
    
    try {
        // Test TUI initialization
        std::cout << "Testing TUI initialization...\n";
        TUIMain& tuiMain = TUIMain::getInstance();
        tuiMain.initialize();
        std::cout << "✓ TUI initialization successful\n";
        
        // Test mode setting
        std::cout << "Testing mode setting...\n";
        tuiMain.setMode(TUIMode::WALLET);
        tuiMain.setMode(TUIMode::DAEMON);
        tuiMain.setMode(TUIMode::MINING);
        tuiMain.setMode(TUIMode::FULL);
        std::cout << "✓ Mode setting successful\n";
        
        // Test configuration
        std::cout << "Testing configuration...\n";
        tuiMain.setWalletFile("test_wallet.wallet");
        tuiMain.setPassword("test_password");
        tuiMain.setDaemonAddress("localhost", 8080);
        tuiMain.setMiningAddress("XFGtest123...");
        tuiMain.setMiningThreads(4);
        std::cout << "✓ Configuration successful\n";
        
        // Test cleanup
        std::cout << "Testing cleanup...\n";
        tuiMain.cleanup();
        std::cout << "✓ Cleanup successful\n";
        
        std::cout << "\nAll tests passed! ✓\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}