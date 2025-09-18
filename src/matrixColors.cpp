// matrixColors.cpp
#include "matrixColors.h"

// Global instance
MatrixColors matrixColors;

int MatrixColors::previousIndex(int index) {
    // Handle boundary checks
    if (index < 0 || index >= MAX_COLORS_NUM) {
        return -1; // Invalid index
    }
    
    // If at the first color (index 0), wrap to the last color
    if (index == 0) {
        return MAX_COLORS_NUM - 1;
    }
    
    // Otherwise, return previous index in the entire color array
    return index - 1;
}

int MatrixColors::nextIndex(int index) {
    // Handle boundary checks
    if (index < 0 || index >= MAX_COLORS_NUM) {
        return -1; // Invalid index
    }
    
    // If at the last color, wrap to the first color (index 0)
    if (index == MAX_COLORS_NUM - 1) {
        return 0;
    }
    
    // Otherwise, return next index in the entire color array
    return index + 1;
}