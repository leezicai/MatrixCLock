#include "matrixCore.h"
#include <algorithm>
#include <stdexcept>
#include "matrixData.h"
#include "brightnessManager.h"
#include "matrixDma.h"
#include "matrixColors.h"

extern uint8_t g_panelType;

MatrixCore::MatrixCore(float x, float y, int16_t fontGroupIndex, int16_t fontIndex,
                       int16_t colorIndex1, int16_t colorIndex2, int16_t displayGroup,
                       int16_t displayIndex, int16_t animationType)
    : x(x), y(y), fontGroupIndex(fontGroupIndex), fontIndex(fontIndex),
      colorIndex1(colorIndex1), colorIndex2(colorIndex2), displayGroup(displayGroup),
      displayIndex(displayIndex), animationType(animationType) {}

MatrixCoreManager::MatrixCoreManager() {
    // initializeMatrixCores();
}

SecondaryPage MatrixCoreManager::getSecondaryPage2_1(){
    return secondaryPage2_1;
}
SecondaryPage MatrixCoreManager::getSecondaryPage2_1_(){
    return secondaryPage2_1_;
}

void MatrixCoreManager::initializeMatrixCores() {
    // Create MatrixCore elements for different pages
    // Primary Page 0
    PrimaryPage primaryPage0;
    PrimaryPage primaryPage1;
    PrimaryPage primaryPage2;

    switch (g_panelType) {
    case 0: {
      // 24 getHour24_Minute_Second
      SecondaryPage secondaryPage0_0 = {MatrixCore(0.495f, 0.45f, 1, 0, 1, 0, 0, 1, 1)};
      // 24 getHour24_Minute
      SecondaryPage secondaryPage0_1 = { MatrixCore(0.495f, 0.45f, 0, 0, 1, 0, 0, 4, 2) };
      // 12 getHour12_MinuteAmpm
      SecondaryPage secondaryPage0_2 = { MatrixCore(0.495f, 0.45f, 2, 0, 0, 0, 0, 6, 0) };
      // 12 getHour12_Minute_Second
      SecondaryPage secondaryPage0_3 = { MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 2, 0) };
      // 12 getHour12_Minute
      SecondaryPage secondaryPage0_4 = { MatrixCore(0.495f, 0.43f, 0, 0, 0, 0, 0, 5, 0) };

      secondaryPage0_0 = matrixDataManager.loadPage(0, 0, secondaryPage0_0);
      secondaryPage0_1 = matrixDataManager.loadPage(0, 1, secondaryPage0_1);
      secondaryPage0_2 = matrixDataManager.loadPage(0, 2, secondaryPage0_2);
      secondaryPage0_3 = matrixDataManager.loadPage(0, 3, secondaryPage0_3);
      secondaryPage0_4 = matrixDataManager.loadPage(0, 4, secondaryPage0_4);

      primaryPage0.push_back(secondaryPage0_0);
      primaryPage0.push_back(secondaryPage0_1);
      primaryPage0.push_back(secondaryPage0_2);
      primaryPage0.push_back(secondaryPage0_3);
      primaryPage0.push_back(secondaryPage0_4);

      interface.push_back(primaryPage0);

      // Primary Page 1 - Secondary Page 0
      SecondaryPage secondaryPage1_0 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};
      SecondaryPage secondaryPage1_1 = {MatrixCore(0.5f, 0.15f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.495f, 0.55f, 0, 0, 1, 0, 0, 4, 2)};
      SecondaryPage secondaryPage1_2 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 2, 0)};
      SecondaryPage secondaryPage1_3 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 2, 0, 0, 0, 0, 6, 0)};
      SecondaryPage secondaryPage1_4 = {MatrixCore(0.5f, 0.15f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.495f, 0.55f, 0, 0, 1, 0, 0, 5, 2)};
      SecondaryPage secondaryPage1_5 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};

      secondaryPage1_0 = matrixDataManager.loadPage(1, 0, secondaryPage1_0);
      secondaryPage1_1 = matrixDataManager.loadPage(1, 1, secondaryPage1_1);
      secondaryPage1_2 = matrixDataManager.loadPage(1, 2, secondaryPage1_2);
      secondaryPage1_3 = matrixDataManager.loadPage(1, 3, secondaryPage1_3);
      secondaryPage1_4 = matrixDataManager.loadPage(1, 4, secondaryPage1_4);
      secondaryPage1_5 = matrixDataManager.loadPage(1, 5, secondaryPage1_5);

      primaryPage1.push_back(secondaryPage1_0);
      primaryPage1.push_back(secondaryPage1_1);
      primaryPage1.push_back(secondaryPage1_2);
      primaryPage1.push_back(secondaryPage1_3);
      primaryPage1.push_back(secondaryPage1_4);
      primaryPage1.push_back(secondaryPage1_5);

      interface.push_back(primaryPage1);

      SecondaryPage secondaryPage2_0 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 0, 0),

          MatrixCore(0.03f, 0.47f, 8, 1, 3, 0, 4, 8, 0),
          MatrixCore(0.36f, 0.47f, 8, 1, 3, 0, 4, 9, 0),
          MatrixCore(0.68f, 0.47f, 8, 1, 3, 0, 4, 10, 0),

          MatrixCore(0.03f, 0.79f, 8, 1, 3, 0, 4, 11, 0),
          MatrixCore(0.36f, 0.79f, 8, 1, 3, 0, 4, 12, 0),
          MatrixCore(0.68f, 0.79f, 8, 1, 3, 0, 4, 13, 0)};

      // 特殊处理,使用已有变量
      secondaryPage2_1 = {MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 1, 1),

                          MatrixCore(0.03f, 0.47f, 10, 40, 5, 0, 2, 2, 1),
                          MatrixCore(0.52f, 0.47f, 10, 40, 3, 0, 3, 0, 1),

                          MatrixCore(0.03f, 0.79f, 8, 1, 3, 0, 3, 2, 1),
                          MatrixCore(0.52f, 0.79f, 8, 1, 3, 0, 3, 3, 1)};

      // 特殊处理,使用已有变量
      secondaryPage2_1_ = {MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 1, 1),

                           MatrixCore(0.03f, 0.47f, 10, 40, 3, 0, 2, 2, 1),
                           MatrixCore(0.52f, 0.47f, 10, 40, 3, 0, 3, 0, 1),

                           MatrixCore(0.52f, 0.79f, 8, 1, 3, 0, 3, 1, 1)};

      SecondaryPage secondaryPage2_2 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 5, 1),

          MatrixCore(0.03f, 0.47f, 10, 40, 3, 0, 3, 4, 1),
      };

       SecondaryPage secondaryPage2_3 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 5, 7, 0),
          MatrixCore(0.40f, 0.16f, 10, 40, 0, 0, 5, 8, 0),
          MatrixCore(0.68, 0.16f, 10, 40, 5, 0, 5, 12, 0),

          MatrixCore(0.03f, 0.38f, 10, 40, 3, 0, 5, 1, 0),
          MatrixCore(0.36f, 0.38f, 10, 40, 3, 0, 5, 2, 0),
          MatrixCore(0.68f, 0.38f, 10, 40, 3, 0, 5, 3, 0),

          MatrixCore(0.03f, 0.6f, 10, 40, 3, 0, 5, 4, 0),
          MatrixCore(0.25f, 0.6f, 10, 40, 3, 0, 5, 5, 0),
          MatrixCore(0.47f, 0.6f, 10, 40, 3, 0, 5, 6, 0),
          MatrixCore(0.69f, 0.6f, 10, 40, 3, 0, 5, 0, 0),

          MatrixCore(0.03f, 0.82f, 8, 1, 3, 0, 5, 9, 0),
          MatrixCore(0.15f, 0.82f, 8, 1, 3, 0, 5, 10, 0),
          MatrixCore(0.22f, 0.82f, 8, 1, 3, 0, 5, 11, 0),
          
      };

      secondaryPage2_0 = matrixDataManager.loadPage(2, 0, secondaryPage2_0);
      primaryPage2.push_back(secondaryPage2_0);
      if (brightnessManager.getAutoMode()) {
        primaryPage2.push_back(secondaryPage2_1);
      } else {
        primaryPage2.push_back(secondaryPage2_1_);
      }
      primaryPage2.push_back(secondaryPage2_2);
      primaryPage2.push_back(secondaryPage2_3);
      interface.push_back(primaryPage2);
      break;
    }
    case 1: {
      break;

    }
    case 2: {

      break;
    }
    default: {

      break;
    }
    }

    // Initialize history array with same size as primary pages, all starting at 0
    history.resize(interface.size(), 0);
    for (size_t i = 0; i < history.size(); i++) {
      history[i] = matrixDataManager.loadPageIndex(i);
    }

    pageIndex = matrixDataManager.loadPrimaryPage();
    secondaryIndex = history[pageIndex];

}

// Update history when secondary page changes
void MatrixCoreManager::updateHistory() {
    if (pageIndex < static_cast<int16_t>(history.size())) {
        matrixDataManager.savePageIndex(pageIndex, secondaryIndex);
        history[pageIndex] = secondaryIndex;
    }
}
void MatrixCoreManager::updatePage() { matrixDataManager.savePrimaryPage(pageIndex); }

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

    updatePage();

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
    updatePage();
    resetElementGroup();  // Reset to first element when switching primary page
}

void MatrixCoreManager::resetPrimaryPage() {
    updateHistory();  // Save current position before reset
    pageIndex = 0;
    if (!history.empty()) {
        secondaryIndex = history[0];
    }
    updatePage();
    resetElementGroup();
}

// Secondary page navigation - Circular switching
void MatrixCoreManager::nextSecondaryPage() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        !interface[pageIndex].empty()) {
        secondaryIndex = (secondaryIndex + 1) % static_cast<int16_t>(interface[pageIndex].size());
        updateHistory();  // Update history when secondary page changes
    }
    updatePage();
    resetElementGroup();  // Reset to first element when switching secondary page
}

void MatrixCoreManager::prevSecondaryPage() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        !interface[pageIndex].empty()) {
        secondaryIndex = (secondaryIndex - 1 + static_cast<int16_t>(interface[pageIndex].size())) % static_cast<int16_t>(interface[pageIndex].size());
        updateHistory();  // Update history when secondary page changes
    }
    updatePage();
    resetElementGroup();  // Reset to first element when switching secondary page
}

void MatrixCoreManager::resetSecondaryPage() {
    secondaryIndex = 0;
    updateHistory();  // Update history when secondary page changes
    updatePage();
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

    matrixDataManager.savePage(getCurrentPageIndex(), getCurrentSecondaryIndex(), interface[pageIndex][secondaryIndex]);
}

// Swap/Replace a secondary page with a new one
void MatrixCoreManager::swapSecondaryPage(int16_t pageIdx, int16_t secondaryIdx, 
                                          const SecondaryPage& newSecondaryPage) {
    // Validate pageIdx is within bounds
    if (pageIdx < 0 || pageIdx >= static_cast<int16_t>(interface.size())) {
        return; // Invalid primary page index
    }
    
    // Validate secondaryIdx is within bounds
    if (secondaryIdx < 0 || secondaryIdx >= static_cast<int16_t>(interface[pageIdx].size())) {
        return; // Invalid secondary page index
    }
    
    // Replace the secondary page
    interface[pageIdx][secondaryIdx] = newSecondaryPage;
    
    // If we're currently on this page, check if elementGroupIndex is still valid
    if (pageIndex == pageIdx && secondaryIndex == secondaryIdx) {
        // If current elementGroupIndex exceeds new page size, reset to 0
        if (elementGroupIndex >= static_cast<int16_t>(newSecondaryPage.size())) {
            resetElementGroup();
        }
        // Otherwise keep the current elementGroupIndex unchanged
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