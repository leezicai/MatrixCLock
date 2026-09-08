#pragma once
#include <cstdint>
#include <vector>
#define COLOR_IDX(e) static_cast<int16_t>(e)
// Global vector for matrix colors
extern std::vector<uint16_t> Matrix_COLORS;

// Color indices enum for easy access
typedef enum {
    COLOR_RED = 0,             // 0xF800  Red
    COLOR_ORANGE_RED,          // 0xFAA0  Orange-Red
    COLOR_ORANGE,              // 0xFD40  Orange
    COLOR_YELLOW_ORANGE,       // 0xFFE0  Yellow-Orange
    COLOR_YELLOW,              // 0xAFE0  Yellow
    COLOR_YELLOW_GREEN,        // 0x57E0  Yellow-Green
    COLOR_GREEN,               // 0x07E0  Green
    COLOR_CYAN_GREEN,          // 0x07EA  Cyan-Green
    COLOR_CYAN,                // 0x07F5  Cyan
    COLOR_CYAN_BLUE,           // 0x07FF  Cyan-Blue
    COLOR_BLUE,                // 0x055F  Blue
    COLOR_BLUE_PURPLE,         // 0x02BF  Blue-Purple
    COLOR_PURPLE,              // 0x001F  Purple
    COLOR_MAGENTA_PURPLE,      // 0x501F  Magenta-Purple
    COLOR_MAGENTA,             // 0xA81F  Magenta
    COLOR_MAGENTA_RED,         // 0xF81F  Magenta-Red
    COLOR_RED_MAGENTA,         // 0xF815  Red-Magenta
    COLOR_RED_ORANGE,          // 0xF80A  Red-Orange
    COLOR_WHITE,               // 0xFFFF  White
    COLOR_LIGHT_GRAY,          // 0xBDF7  Light Gray
    COLOR_GRAY,                // 0x8410  Gray
    COLOR_COUNT
} ColorIndex;

class MatrixColorManager {
public:
    int16_t previousIndex(int16_t index);
    int16_t nextIndex(int16_t index);
    uint16_t getColor(int16_t index); 
    
    // Get total number of colors
    size_t getColorCount() const;
};

extern MatrixColorManager matrixColorManager;