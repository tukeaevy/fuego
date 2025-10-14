#include "include/DynamicRingSize.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Testing Dynamic Ring Size Calculator..." << std::endl;
    
    // Test data
    std::vector<CryptoNote::OutputInfo> outputs = {
        {1000000000, 20, "High availability"},
        {5000000000, 15, "Medium availability"},
        {10000000000, 8, "Low availability"}
    };
    
    uint8_t blockMajorVersion = 10;
    size_t minRingSize = 8;
    size_t maxRingSize = 20;
    
    // Test calculation
    size_t optimalRingSize = CryptoNote::DynamicRingSizeCalculator::calculateOptimalRingSize(
        0, outputs, blockMajorVersion, minRingSize, maxRingSize
    );
    
    std::cout << "Optimal ring size: " << optimalRingSize << std::endl;
    
    // Test privacy level description
    std::string description = CryptoNote::DynamicRingSizeCalculator::getPrivacyLevelDescription(optimalRingSize);
    std::cout << "Privacy level: " << description << std::endl;
    
    // Test target ring sizes
    auto targetSizes = CryptoNote::DynamicRingSizeCalculator::getTargetRingSizes();
    std::cout << "Target ring sizes: ";
    for (size_t size : targetSizes) {
        std::cout << size << " ";
    }
    std::cout << std::endl;
    
    // Test achievability
    bool achievable = CryptoNote::DynamicRingSizeCalculator::isRingSizeAchievable(18, outputs);
    std::cout << "Ring size 18 achievable: " << (achievable ? "Yes" : "No") << std::endl;
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}