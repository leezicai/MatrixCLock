#include "matrixFonts.h"
#include "string.h"
#include <U8g2_for_Adafruit_GFX.h>

// FontInfo constructor implementation
FontInfo::FontInfo(const uint8_t* fontName, float offsetSepX, float offsetSepY, float offsetFontX, float offsetFontY)
    : fontName(fontName), offsetSepX(offsetSepX), offsetSepY(offsetSepY), offsetFontX(offsetFontX), offsetFontY(offsetFontY) {
}

// Constructor implementation
MatrixFontManager::MatrixFontManager() {
    // Constructor can be empty since initialization is done in init()
    init();
}

// Global SimpleFontManager object instance
MatrixFontManager matrixFontManager;

// Initialize all font groups and add them to the two-level structure
void MatrixFontManager::init() {
    // Clear existing data
    fontGroups.clear();
    
    // Create small fonts group
    FontGroup smallFonts;
    smallFonts.push_back(FontInfo(u8g2_font_crox5hb_tf, 0.3, 0.1, 0, 0));
    smallFonts.push_back(FontInfo(u8g2_font_fub17_tf, 0.2, 0.3, 0, 0));
    smallFonts.push_back(FontInfo(u8g2_font_ncenB18_tf, 0.2, 0.3, 0, 0));
    
    // Create medium fonts group
    FontGroup mediumFonts;
    mediumFonts.push_back(FontInfo(u8g2_font_10x20_tf, 0.3, 0.09, 0, 0));
    mediumFonts.push_back(FontInfo(u8g2_font_7x13_tf, 0.2, 0.3, 0, 0));
    
    // Create large fonts group
    FontGroup largeFonts;
    largeFonts.push_back(FontInfo(u8g2_font_6x12_tf, 0.2, 0.3, 0, 0));
    largeFonts.push_back(FontInfo(u8g2_font_6x12_tf, 0.2, 0.3, 0, 0));
    
    // Add all groups to the main FontGroups container
    fontGroups.push_back(smallFonts);
    fontGroups.push_back(mediumFonts);
    fontGroups.push_back(largeFonts);
}

// Get current font info based on group index and font index within group
const FontInfo* MatrixFontManager::getCurrentFont(int16_t groupIndex, int16_t index) {
    // Validate group index
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return nullptr;  // Invalid group index
    }
    
    const FontGroup& currentGroup = fontGroups[groupIndex];
    
    // Validate font index within the group
    if (index < 0 || index >= static_cast<int16_t>(currentGroup.size())) {
        return nullptr;  // Invalid font index
    }
    
    // Return pointer to the font info
    return &(currentGroup[index]);
}

// Switch to next font index within the same group (circular)
int16_t MatrixFontManager::switchToNextFontIndex(int16_t groupIndex, int16_t index) {
    // Validate group index
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return -1;  // Invalid group index
    }
    
    const FontGroup& currentGroup = fontGroups[groupIndex];
    
    // Validate current font index
    if (index < 0 || index >= static_cast<int16_t>(currentGroup.size())) {
        return -1;  // Invalid current font index
    }
    
    // Calculate next index (circular)
    int16_t nextIndex = (index + 1) % static_cast<int16_t>(currentGroup.size());
    
    return nextIndex;
}

// Switch to previous font index within the same group (circular)
int16_t MatrixFontManager::switchToPreFontIndex(int16_t groupIndex, int16_t index) {
    // Validate group index
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return -1;  // Invalid group index
    }
    
    const FontGroup& currentGroup = fontGroups[groupIndex];
    
    // Validate current font index
    if (index < 0 || index >= static_cast<int16_t>(currentGroup.size())) {
        return -1;  // Invalid current font index
    }
    
    // Calculate previous index (circular)
    int16_t groupSize = static_cast<int16_t>(currentGroup.size());
    int16_t previousIndex = (index - 1 + groupSize) % groupSize;
    
    return previousIndex;
}

// Get total group count
size_t MatrixFontManager::getGroupCount() const {
    return fontGroups.size();
}

// Get font count in specific group
size_t MatrixFontManager::getFontCount(int16_t groupIndex) const {
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return 0;  // Invalid group index
    }
    
    return fontGroups[groupIndex].size();
}