// matrixFonts.h
#pragma once
#include <string>
#include <vector>

struct FontInfo{
    const uint8_t * fontName;   
    float offsetSepX;
    float offsetSepY;
    float offsetFontX;
    float offsetFontY;
    FontInfo(const uint8_t* fontName, float offsetSepX, float offsetSepY, float offsetFontX,float offsetFontY);
};

typedef std::vector<FontInfo> FontGroup;
typedef std::vector<FontGroup> FontGroups;

class MatrixFontManager {
private:
    // Two-level font groups structure
    FontGroups fontGroups;
    
public:
    // Constructor
    MatrixFontManager();
    
    // Initialization method
    void init();
    
    // Font retrieval method
    const FontInfo* getCurrentFont(int16_t groupIndex, int16_t index);
    
    // Button control methods
    int16_t switchToNextFontIndex(int16_t groupIndex, int16_t index);            
    int16_t switchToPreFontIndex(int16_t groupIndex, int16_t index);
    
    // Get total group count
    size_t getGroupCount() const;
    
    // Get font count in specific group
    size_t getFontCount(int16_t groupIndex) const;
};

// Global SimpleFontManager object
extern MatrixFontManager matrixFontManager;