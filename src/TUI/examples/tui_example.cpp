#include "../TUIMain.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace FuegoTUI;

int main() {
    std::cout << "Fuego TUI Example\n";
    std::cout << "================\n\n";
    
    try {
        // Initialize TUI
        TUIMain& tuiMain = TUIMain::getInstance();
        tuiMain.initialize();
        
        // Configure TUI
        tuiMain.setMode(TUIMode::FULL);
        tuiMain.setWalletFile("example_wallet.wallet");
        tuiMain.setDaemonAddress("localhost", 8080);
        tuiMain.setMiningAddress("XFGexample123...");
        tuiMain.setMiningThreads(2);
        
        std::cout << "Starting Fuego TUI...\n";
        std::cout << "Press 'q' or ESC to exit\n";
        std::cout << "Use arrow keys to navigate\n";
        std::cout << "Press Enter to select\n\n";
        
        // Small delay to show the message
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Run TUI
        tuiMain.run();
        
        // Cleanup
        tuiMain.cleanup();
        
        std::cout << "\nTUI session ended.\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}