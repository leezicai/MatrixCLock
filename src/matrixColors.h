#pragma once
#include <cstdint>
#include <vector>

// Global vector for matrix colors
extern std::vector<uint16_t> Matrix_COLORS;

// Color indices enum for easy access
enum ColorIndex {
    RED = 0,     // index 0
    ORANGE = 1,  // index 1
    YELLOW = 2,  // index 2
    GREEN = 3,   // index 3
    CYAN = 4,    // index 4
    BLUE = 5,    // index 5
    VIOLET = 6,  // index 6
    WHITE = 7    // index 7
};

class MatrixColorManager {
public:
    int16_t previousIndex(int16_t index);
    int16_t nextIndex(int16_t index);
    uint16_t getColor(int16_t index); 
    
    // Get total number of colors
    size_t getColorCount() const;
};

extern MatrixColorManager matrixColorManager;