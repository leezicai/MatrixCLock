#ifndef MATRIXCORE_H
#define MATRIXCORE_H
#include <vector>
#include <cstdint>

struct MatrixCore {
    float x;                    // 4 bytes - coordinate precision needed
    float y;                    // 4 bytes - coordinate precision needed
    int16_t fontGroupIndex;     // 2 bytes - range 0-32767 usually enough
    int16_t fontIndex;          // 2 bytes - element index
    int16_t colorIndex1;        // 2 bytes - color palette index
    int16_t colorIndex2;        // 2 bytes - color palette index
    int16_t displayGroup;       // display group
    int16_t displayIndex;       // 2 bytes - display position
    int16_t animationType;      // animation type
    
    MatrixCore(float x, float y, int16_t fontGroupIndex, int16_t fontIndex, 
               int16_t colorIndex1, int16_t colorIndex2, int16_t displayGroup, 
               int16_t displayIndex, int16_t animationType);
};

// 3-layer vector structure
typedef std::vector<MatrixCore> SecondaryPage;        // Layer 3: Multiple MatrixCore elements
typedef std::vector<SecondaryPage> PrimaryPage;       // Layer 2: Multiple secondary pages form a primary page  
typedef std::vector<PrimaryPage> Interface;           // Layer 1: Multiple primary pages form the interface

class MatrixCoreManager {
private:
    Interface interface;        // The complete 3-layer structure
    
    // Current position indices
    int16_t pageIndex;          // Primary page index
    int16_t secondaryIndex;     // Secondary page index
    int16_t elementGroupIndex;  // Element group index (now refers to individual MatrixCore elements)

    int16_t button2LongPressFlag = 0;
    
    // History array to remember last secondary page position for each primary page
    std::vector<int16_t> history;
    
    // Initialize all MatrixCore data
    void initializeMatrixCores();
    
    // Update history when secondary page changes
    void updateHistory();

public:
    MatrixCoreManager();
    
    // Navigation methods for primary pages
    void nextPrimaryPage();
    void prevPrimaryPage();
    void resetPrimaryPage();
    
    // Navigation methods for secondary pages
    void nextSecondaryPage();
    void prevSecondaryPage();
    void resetSecondaryPage();
    
    // Navigation methods for element groups (now individual elements)
    void nextElementGroup();
    void prevElementGroup();
    void resetElementGroup();
    
    void button2LongPressStart();
    void button2LongPressEnd();
    int16_t getButtton2LongPressFlag();
    // Modification methods
    void modifyElement(int16_t pageIndex, int16_t secondaryIndex, int16_t elementGroupIndex, 
                      const MatrixCore& newElement);
    void modifyCurrentElement(const MatrixCore &newElement);

    // Getter methods
    int16_t getCurrentPageIndex() const { return pageIndex; }
    int16_t getCurrentSecondaryIndex() const { return secondaryIndex; }
    int16_t getCurrentElementGroupIndex() const { return elementGroupIndex; }
    
    // Get current active elements
    const MatrixCore* getCurrentElementGroup() const;
    const SecondaryPage* getCurrentSecondaryPage() const;
    const PrimaryPage* getCurrentPrimaryPage() const;
    
    // New method: Get current MatrixCore element
    const MatrixCore& getCurrentMatrixCore() const;
    
    // Get total counts
    size_t getPrimaryPageCount() const { return interface.size(); }
    size_t getSecondaryPageCount() const;
    size_t getElementGroupCount() const;
    
    // History related methods
    int16_t getHistoryForPage(int16_t primaryPageIdx) const;
    void setHistoryForPage(int16_t primaryPageIdx, int16_t secondaryPageIdx);
};

extern MatrixCoreManager matrixCoreManager;

#endif // MATRIXCORE_H