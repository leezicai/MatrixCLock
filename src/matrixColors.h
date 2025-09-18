// matrixColors.h
#pragma once
#define MAX_COLORS_NUM 56
#include <cstdint>

// Simple C-style array - more efficient for embedded systems
const uint16_t Matrix_COLORS[MAX_COLORS_NUM] = {
    // Red series (7 colors) - Dark Red to Pure Red
    0x4000,  // Dark Red
    0x6000,  // Medium Dark Red
    0x8000,  // Medium Red
    0xA000,  // Medium Light Red
    0xC000,  // Light Red
    0xE000,  // Very Light Red
    0xF800,  // Pure Red
    
    // Green series (7 colors) - Dark Green to Pure Green
    0x0100,  // Dark Green
    0x0200,  // Medium Dark Green
    0x0300,  // Medium Green
    0x0400,  // Medium Light Green
    0x0500,  // Light Green
    0x0600,  // Very Light Green
    0x07E0,  // Pure Green
    
    // Blue series (7 colors) - Dark Blue to Pure Blue
    0x0004,  // Dark Blue
    0x0008,  // Medium Dark Blue
    0x000C,  // Medium Blue
    0x0010,  // Medium Light Blue
    0x0014,  // Light Blue
    0x0018,  // Very Light Blue
    0x001F,  // Pure Blue
    
    // White series (7 colors) - Dark Gray to Pure White
    0x2104,  // Dark Gray
    0x4208,  // Medium Dark Gray
    0x630C,  // Medium Gray
    0x8410,  // Medium Light Gray
    0xA514,  // Light Gray
    0xC618,  // Very Light Gray
    0xFFFF,  // Pure White
    
    // Purple series (7 colors) - Dark Purple to Pure Purple
    0x4004,  // Dark Purple
    0x6008,  // Medium Dark Purple
    0x800C,  // Medium Purple
    0xA010,  // Medium Light Purple
    0xC014,  // Light Purple
    0xE018,  // Very Light Purple
    0xF81F,  // Pure Purple (Magenta)
    
    // Cyan series (7 colors) - Dark Cyan to Pure Cyan
    0x0110,  // Dark Cyan
    0x0220,  // Medium Dark Cyan
    0x0330,  // Medium Cyan
    0x0440,  // Medium Light Cyan
    0x0550,  // Light Cyan
    0x0660,  // Very Light Cyan
    0x07FF,  // Pure Cyan
    
    // Yellow series (7 colors) - Dark Yellow to Pure Yellow
    0x4120,  // Dark Yellow
    0x6240,  // Medium Dark Yellow
    0x8360,  // Medium Yellow
    0xA480,  // Medium Light Yellow
    0xC5A0,  // Light Yellow
    0xE6C0,  // Very Light Yellow
    0xFFE0,  // Pure Yellow
    
    // Orange series (7 colors) - Dark Orange to Pure Orange
    0x4100,  // Dark Orange
    0x6200,  // Medium Dark Orange
    0x8300,  // Medium Orange
    0xA400,  // Medium Light Orange
    0xC500,  // Light Orange
    0xE600,  // Very Light Orange
    0xFD20   // Pure Orange
};

// Color group indices for easy access (ordered by HUB75 LED preference)
enum ColorGroups {
    RED_START = 0,      // indices 0-6   (Most preferred)
    GREEN_START = 7,    // indices 7-13
    BLUE_START = 14,    // indices 14-20
    WHITE_START = 21,   // indices 21-27
    PURPLE_START = 28,  // indices 28-34
    CYAN_START = 35,    // indices 35-41
    YELLOW_START = 42,  // indices 42-48
    ORANGE_START = 49   // This would be beyond array bounds, used for size calculation
};

// Helper functions for color access
inline uint16_t getRedColor(int level) { return Matrix_COLORS[RED_START + level]; }
inline uint16_t getGreenColor(int level) { return Matrix_COLORS[GREEN_START + level]; }
inline uint16_t getBlueColor(int level) { return Matrix_COLORS[BLUE_START + level]; }
inline uint16_t getWhiteColor(int level) { return Matrix_COLORS[WHITE_START + level]; }
inline uint16_t getPurpleColor(int level) { return Matrix_COLORS[PURPLE_START + level]; }
inline uint16_t getCyanColor(int level) { return Matrix_COLORS[CYAN_START + level]; }
inline uint16_t getYellowColor(int level) { return Matrix_COLORS[YELLOW_START + level]; }
inline uint16_t getOrangeColor(int level) { return Matrix_COLORS[ORANGE_START + level]; }

class MatrixColors{
private:
public:
    int previousIndex(int index);
    int nextIndex(int index);
};

extern MatrixColors matrixColors;