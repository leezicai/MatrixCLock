#include "matrixCore.h"
#include <algorithm>
#include <stdexcept>

MatrixCore::MatrixCore(float x, float y, int16_t fontGroupIndex, int16_t fontIndex,
                       int16_t colorIndex1, int16_t colorIndex2, int16_t displayGroup,
                       int16_t displayIndex, int16_t animationType)
    : x(x), y(y), fontGroupIndex(fontGroupIndex), fontIndex(fontIndex),
      colorIndex1(colorIndex1), colorIndex2(colorIndex2), displayGroup(displayGroup),
      displayIndex(displayIndex), animationType(animationType) {}

MatrixCoreManager::MatrixCoreManager() 
    : pageIndex(0), secondaryIndex(0), elementGroupIndex(0) {
    initializeMatrixCores();
}

void MatrixCoreManager::initializeMatrixCores() {
    // Create MatrixCore elements for different pages
    // Primary Page 0
    PrimaryPage primaryPage0;
    
    // 24 getHour24_Minute_Second 
    SecondaryPage secondaryPage0_0 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 1, 0, 0, 1, 1)  // 24 getHour24_Minute_Second
    };
    SecondaryPage secondaryPage0_1 = { // 24 getHour24_Minute
        MatrixCore(0.495f, 0.45f, 0, 0, 1, 0, 0, 4, 2 )  // PAGE_0_1_0
    };
    SecondaryPage secondaryPage0_2 = {  // 12 getHour12_MinuteAmpm
        MatrixCore(0.495f, 0.45f, 2, 0, 0, 0, 0, 6, 0)  // PAGE_0_0_0
    };
    SecondaryPage secondaryPage0_3 = {  // 12 getHour12_Minute_Second
        MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 2, 0)  // PAGE_0_0_0
    };
    SecondaryPage secondaryPage0_4 = { // 12 getHour12_Minute
        MatrixCore(0.495f, 0.43f, 0, 0, 0, 0, 0, 5, 0)  // PAGE_0_0_0
    };
    primaryPage0.push_back(secondaryPage0_0);
    primaryPage0.push_back(secondaryPage0_1);
    primaryPage0.push_back(secondaryPage0_2);
    primaryPage0.push_back(secondaryPage0_3);
    primaryPage0.push_back(secondaryPage0_4);
    
    interface.push_back(primaryPage0);
    
    // Primary Page 1
    PrimaryPage primaryPage1;
    
    // Primary Page 1 - Secondary Page 0
    SecondaryPage secondaryPage1_0 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 1, 0, 0, 1, 0),  
        MatrixCore(0.495f, 0.8f, 3, 0, 1, 0, 0, 0, 0)
    };
    SecondaryPage secondaryPage1_1 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 1, 0, 0, 6, 0), 
        MatrixCore(0.495f, 0.8f, 4, 0, 1, 0, 0, 0, 0)
    };
    SecondaryPage secondaryPage1_2 = {
        MatrixCore(0.495f, 0.45f, 2, 0, 0, 0, 0, 6, 0),  
        MatrixCore(0.495f, 0.8f, 5, 0, 1, 0, 0, 0, 0)
    };
    SecondaryPage secondaryPage1_3 = {
        MatrixCore(0.5f, 0.3f, 6, 0, 1, 0, 0, 0, 0),
        MatrixCore(0.5f, 0.6f, 1, 0, 0, 0, 0, 1, 0)
    };
    SecondaryPage secondaryPage1_4 = {
        MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 0, 0, 0),
        MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};
    SecondaryPage secondaryPage1_5 = {
        MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
        MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};
    SecondaryPage secondaryPage1_6 = {
        MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 4, 0),
        MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};
    SecondaryPage secondaryPage1_7 = {
        MatrixCore(0.495f, 0.45f, 10, 40, 0, 0, 1, 5, 0),
        MatrixCore(0.495f, 0.8f, 10, 0, 1, 0, 0, 0, 0)};
    SecondaryPage secondaryPage1_8 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 1, 0),
        MatrixCore(0.495f, 0.8f, 11, 0, 1, 0, 0, 0, 0)};

    SecondaryPage secondaryPage1_9 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 1, 0),
        MatrixCore(0.495f, 0.8f, 12, 0, 1, 0, 0, 0, 0)};
    SecondaryPage secondaryPage1_10 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 1, 0),
        MatrixCore(0.495f, 0.8f, 13, 0, 1, 0, 0, 0, 0)};
    SecondaryPage secondaryPage1_11 = {
        MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 1, 0),
        MatrixCore(0.495f, 0.8f, 14, 0, 1, 0, 0, 0, 0)};
    
    primaryPage1.push_back(secondaryPage1_0);
    primaryPage1.push_back(secondaryPage1_1);
    primaryPage1.push_back(secondaryPage1_2);
    primaryPage1.push_back(secondaryPage1_3);
    primaryPage1.push_back(secondaryPage1_4);
    primaryPage1.push_back(secondaryPage1_5);
    primaryPage1.push_back(secondaryPage1_6);
    primaryPage1.push_back(secondaryPage1_7);
    primaryPage1.push_back(secondaryPage1_8);
    primaryPage1.push_back(secondaryPage1_9);
    primaryPage1.push_back(secondaryPage1_10);
    primaryPage1.push_back(secondaryPage1_11);
    
    interface.push_back(primaryPage1);
    
    // Initialize history array with same size as primary pages, all starting at 0
    history.resize(interface.size(), 0);
}

// Update history when secondary page changes
void MatrixCoreManager::updateHistory() {
    if (pageIndex < static_cast<int16_t>(history.size())) {
        history[pageIndex] = secondaryIndex;
    }
}

// Primary page navigation - Circular switching
void MatrixCoreManager::nextPrimaryPage() {
    updateHistory();  // Save current secondary page position before switching
    
    if (!interface.empty()) {
        pageIndex = (pageIndex + 1) % static_cast<int16_t>(interface.size());
        // Restore last secondary page position for the new primary page
        if (pageIndex < static_cast<int16_t>(history.size())) {
            secondaryIndex = history[pageIndex];
        }
    }
    resetElementGroup();  // Reset to first element when switching primary page
}

void MatrixCoreManager::prevPrimaryPage() {
    updateHistory();  // Save current secondary page position before switching
    
    if (!interface.empty()) {
        pageIndex = (pageIndex - 1 + static_cast<int16_t>(interface.size())) % static_cast<int16_t>(interface.size());
        // Restore last secondary page position for the new primary page
        if (pageIndex < static_cast<int16_t>(history.size())) {
            secondaryIndex = history[pageIndex];
        }
    }
    resetElementGroup();  // Reset to first element when switching primary page
}

void MatrixCoreManager::resetPrimaryPage() {
    updateHistory();  // Save current position before reset
    pageIndex = 0;
    if (!history.empty()) {
        secondaryIndex = history[0];
    }
    resetElementGroup();
}

// Secondary page navigation - Circular switching
void MatrixCoreManager::nextSecondaryPage() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        !interface[pageIndex].empty()) {
        secondaryIndex = (secondaryIndex + 1) % static_cast<int16_t>(interface[pageIndex].size());
        updateHistory();  // Update history when secondary page changes
    }
    resetElementGroup();  // Reset to first element when switching secondary page
}

void MatrixCoreManager::prevSecondaryPage() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        !interface[pageIndex].empty()) {
        secondaryIndex = (secondaryIndex - 1 + static_cast<int16_t>(interface[pageIndex].size())) % static_cast<int16_t>(interface[pageIndex].size());
        updateHistory();  // Update history when secondary page changes
    }
    resetElementGroup();  // Reset to first element when switching secondary page
}

void MatrixCoreManager::resetSecondaryPage() {
    secondaryIndex = 0;
    updateHistory();  // Update history when secondary page changes
    resetElementGroup();
}

// Element group navigation - Now navigates individual MatrixCore elements
void MatrixCoreManager::nextElementGroup() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        !interface[pageIndex][secondaryIndex].empty()) {
        elementGroupIndex = (elementGroupIndex + 1) % static_cast<int16_t>(interface[pageIndex][secondaryIndex].size());
    }
}

void MatrixCoreManager::prevElementGroup() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        !interface[pageIndex][secondaryIndex].empty()) {
        elementGroupIndex = (elementGroupIndex - 1 + static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) % static_cast<int16_t>(interface[pageIndex][secondaryIndex].size());
    }
}

void MatrixCoreManager::resetElementGroup() {
    elementGroupIndex = 0;
}

void MatrixCoreManager::button2LongPressStart() { button2LongPressFlag = 1; }
void MatrixCoreManager::button2LongPressEnd() { button2LongPressFlag = 0; }
int16_t MatrixCoreManager::getButtton2LongPressFlag() { return button2LongPressFlag ; }

int16_t MatrixCoreManager::getLineFlagTime() { return lineFlagTime;}
void MatrixCoreManager::setLineFlagTime(int16_t lineFlagTime) { 
    this->lineFlagTime = lineFlagTime; 
}

int16_t MatrixCoreManager::getPageFlagTime() { return pageFlagTime;}
void MatrixCoreManager::setPageFlagTime(int16_t pageFlagTime) { 
    this->pageFlagTime = pageFlagTime; 
}

// Modification methods (removed the first overload as requested)
void MatrixCoreManager::modifyElement(int16_t pageIndex, int16_t secondaryIndex, 
                                   int16_t elementGroupIndex, const MatrixCore& newElement) {
    // In 3-layer structure, elementGroupIdx is the element index in secondary page
    // elementIdx parameter is now redundant but kept for API compatibility

    if (secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        interface[pageIndex][secondaryIndex][elementGroupIndex] = newElement;
    }
}
void MatrixCoreManager::modifyCurrentElement(const MatrixCore& newElement) {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        interface[pageIndex][secondaryIndex][elementGroupIndex] = newElement;
    }
}

// Getter methods
const MatrixCore* MatrixCoreManager::getCurrentElementGroup() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        return &interface[pageIndex][secondaryIndex][elementGroupIndex];
    }
    return nullptr;
}

const SecondaryPage* MatrixCoreManager::getCurrentSecondaryPage() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size())) {
        
        return &interface[pageIndex][secondaryIndex];
    }
    return nullptr;
}

const PrimaryPage* MatrixCoreManager::getCurrentPrimaryPage() const {
    if (pageIndex < static_cast<int16_t>(interface.size())) {
        return &interface[pageIndex];
    }
    return nullptr;
}

// New method: Get current MatrixCore element
const MatrixCore& MatrixCoreManager::getCurrentMatrixCore() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        return interface[pageIndex][secondaryIndex][elementGroupIndex];
    }
    
    // Throw exception if indices are invalid
    throw std::out_of_range("Invalid indices for getCurrentMatrixCore()");
}

size_t MatrixCoreManager::getSecondaryPageCount() const {
    if (pageIndex < static_cast<int16_t>(interface.size())) {
        return interface[pageIndex].size();
    }
    return 0;
}

size_t MatrixCoreManager::getElementGroupCount() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size())) {
        return interface[pageIndex][secondaryIndex].size();
    }
    return 0;
}

// History related methods
int16_t MatrixCoreManager::getHistoryForPage(int16_t primaryPageIdx) const {
    if (primaryPageIdx >= 0 && primaryPageIdx < static_cast<int16_t>(history.size())) {
        return history[primaryPageIdx];
    }
    return 0;
}

void MatrixCoreManager::setHistoryForPage(int16_t primaryPageIdx, int16_t secondaryPageIdx) {
    if (primaryPageIdx >= 0 && primaryPageIdx < static_cast<int16_t>(history.size())) {
        history[primaryPageIdx] = secondaryPageIdx;
    }
}

// Global instance
MatrixCoreManager matrixCoreManager;