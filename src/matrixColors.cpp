#include "matrixColors.h"

// Global vector with 8 colors total (RGB565 format)
std::vector<uint16_t> Matrix_COLORS = {
    0xF800,  // RED (index 0) - Pure red
    0xFD20,  // ORANGE (index 1) - Pure orange
    0xFFE0,  // YELLOW (index 2) - Pure yellow
    0x07E0,  // GREEN (index 3) - Pure green
    0x07FF,  // CYAN (index 4) - Pure cyan
    0x001F,  // BLUE (index 5) - Pure blue
    0xF81F,  // VIOLET (index 6) - Pure violet/magenta
    0xFFFF   // WHITE (index 7) - Pure white
};

// Global instance
MatrixColorManager matrixColorManager;

int16_t MatrixColorManager::previousIndex(int16_t index) {
    // Handle boundary checks
    if (Matrix_COLORS.empty() || index < 0 || index >= static_cast<int16_t>(Matrix_COLORS.size())) {
        return -1; // Invalid index or empty vector
    }
    
    // Use modular arithmetic for wrapping: (index - 1 + size) % size
    return (index - 1 + static_cast<int16_t>(Matrix_COLORS.size())) % static_cast<int16_t>(Matrix_COLORS.size());
}

int16_t MatrixColorManager::nextIndex(int16_t index) {
    // Handle boundary checks
    if (Matrix_COLORS.empty() || index < 0 || index >= static_cast<int16_t>(Matrix_COLORS.size())) {
        return -1; // Invalid index or empty vector
    }
    
    // Use modular arithmetic for wrapping: (index + 1) % size
    return (index + 1) % static_cast<int16_t>(Matrix_COLORS.size());
}

uint16_t MatrixColorManager::getColor(int16_t index) {
    // Handle boundary checks
    if (Matrix_COLORS.empty() || index < 0 || index >= static_cast<int16_t>(Matrix_COLORS.size())) {
        return 0x0000; // Return black for invalid index or empty vector
    }
    
    // Return the color at the specified index
    return Matrix_COLORS[index];
}

size_t MatrixColorManager::getColorCount() const {
    return Matrix_COLORS.size();
}