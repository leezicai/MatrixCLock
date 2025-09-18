#include "matrixFonts.h"
#include "string.h"
#include <U8g2_for_Adafruit_GFX.h>

// FontInfo constructor implementation
FontInfo::FontInfo(const uint8_t* font_data, float offsetSepX, float offsetSepY, float offsetFontX, float offsetFontY)
    : font_data(font_data), offsetSepX(offsetSepX), offsetSepY(offsetSepY), offsetFontX(offsetFontX), offsetFontY(offsetFontY) {
}

const FontInfo SMALL_FONT_1(u8g2_font_ncenB18_tn, 0.2, 0.3, 0, 0);
const FontInfo SMALL_FONT_2(u8g2_font_6x12_tf, 0.2, 0.3, 0, 0);
const FontInfo SMALL_FONT_3(u8g2_font_7x13_tf,0.2, 0.3, 0, 0);

const FontGroup FONTS_SMALL = {
    SMALL_FONT_1,
    SMALL_FONT_2, 
    SMALL_FONT_3
};

const FontInfo MEDIUM_FONT_1(u8g2_font_10x20_tf, 0.2, 0.3, 0, 0);
const FontInfo MEDIUM_FONT_2(u8g2_font_7x13_tf, 0.2, 0.3, 0, 0);

const FontGroup FONTS_MEDIUM = {
    MEDIUM_FONT_1,
    MEDIUM_FONT_2
};

const FontInfo LARGE_FONT_1(u8g2_font_fur20_tf, 0.2, 0.3, 0, 0);
const FontInfo LARGE_FONT_2(u8g2_font_fur25_tf, 0.2, 0.3, 0, 0);

const FontGroup FONTS_LARGE = {
    LARGE_FONT_1,
    LARGE_FONT_2
};

const FontGroup* SimpleFontManager::font_groups[] = {
    &FONTS_SMALL,
    &FONTS_MEDIUM,
    &FONTS_LARGE
};

const uint8_t SimpleFontManager::font_groups_count = 3;

// Constructor implementation
SimpleFontManager::SimpleFontManager() {
    // Constructor can be empty since we don't need to store state
}

// Global SimpleFontManager object instance
SimpleFontManager simpleFontManager;

// Get current font info based on group index and font index within group
const FontInfo* SimpleFontManager::getCurrentFont(uint8_t groupIndex, uint8_t index) {
    // Validate group index
    if (groupIndex >= font_groups_count) {
        return nullptr;  // Invalid group index
    }
    
    const FontGroup* currentGroup = font_groups[groupIndex];
    
    // Validate font index within the group
    if (index >= currentGroup->size()) {
        return nullptr;  // Invalid font index
    }
    
    // Return pointer to the font info
    return &((*currentGroup)[index]);
}

// Switch to next font index within the same group (circular)
int SimpleFontManager::switchToNextFontIndex(uint8_t groupIndex, uint8_t index) {
    // Validate group index
    if (groupIndex >= font_groups_count) {
        return -1;  // Invalid group index
    }
    
    const FontGroup* currentGroup = font_groups[groupIndex];
    
    // Validate current font index
    if (index >= currentGroup->size()) {
        return -1;  // Invalid current font index
    }
    
    // Calculate next index (circular)
    uint8_t nextIndex = (index + 1) % currentGroup->size();
    
    return nextIndex;
}

// Switch to previous font index within the same group (circular)
int SimpleFontManager::switchToPreviousFontIndex(uint8_t groupIndex, uint8_t index) {
    // Validate group index
    if (groupIndex >= font_groups_count) {
        return -1;  // Invalid group index
    }
    
    const FontGroup* currentGroup = font_groups[groupIndex];
    
    // Validate current font index
    if (index >= currentGroup->size()) {
        return -1;  // Invalid current font index
    }
    
    // Calculate previous index (circular)
    uint8_t previousIndex;
    if (index == 0) {
        previousIndex = currentGroup->size() - 1;  // Wrap to last font
    } else {
        previousIndex = index - 1;
    }
    
    return previousIndex;
}