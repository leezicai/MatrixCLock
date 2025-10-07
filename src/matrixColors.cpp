#include "matrixColors.h"

// Global vector with 8 colors total (RGB565 format)
std::vector<uint16_t> Matrix_COLORS = {
    0xF800, // [0] Red
    0xFAA0, // [1] Orange-Red
    0xFD40, // [2] Orange
    0xFFE0, // [3] Yellow-Orange
    0xAFE0, // [4] Yellow
    0x57E0, // [5] Yellow-Green
    0x07E0, // [6] Green
    0x07EA, // [7] Cyan-Green
    0x07F5, // [8] Cyan
    0x07FF, // [9] Cyan-Blue
    0x055F, // [10] Blue
    0x02BF, // [11] Blue-Purple
    0x001F, // [12] Purple
    0x501F, // [13] Magenta-Purple
    0xA81F, // [14] Magenta
    0xF81F, // [15] Magenta-Red
    0xF815, // [16] Red-Magenta
    0xF80A, // [17] Red-Orange
    0xFFFF, // [18] White
    0xBDF7, // [19] Light Gray
    0x8410  // [20] Gray
};

// Global instance
MatrixColorManager matrixColorManager;

int16_t MatrixColorManager::previousIndex(int16_t index) {
    // Handle boundary checks
     if ( index < 0 || index >= COLOR_COUNT) {
        return 0; // Invalid index or empty vector
    }
    
    // Use modular arithmetic for wrapping: (index - 1 + size) % size
    return (index - 1 + COLOR_COUNT % COLOR_COUNT);
}

int16_t MatrixColorManager::nextIndex(int16_t index) {
    // Handle boundary checks
    if ( index < 0 || index >= COLOR_COUNT) {
        return 0; // Invalid index or empty vector
    }
    
    // Use modular arithmetic for wrapping: (index + 1) % size
    return (index + 1) % COLOR_COUNT;
}

uint16_t MatrixColorManager::getColor(int16_t index) {
    // Handle boundary checks
    if (index < 0 || index >= COLOR_COUNT) {
        return Matrix_COLORS[0]; // Return black for invalid index or empty vector
    }
    
    // Return the color at the specified index
    return Matrix_COLORS[index];
}

size_t MatrixColorManager::getColorCount() const {
    return Matrix_COLORS.size();
}