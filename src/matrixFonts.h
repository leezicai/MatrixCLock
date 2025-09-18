// matrixFonts.h
#pragma once
#include <string>
#include <vector>

struct FontInfo{
    const uint8_t * font_data;   
    float offsetSepX;
    float offsetSepY;
    float offsetFontX;
    float offsetFontY;
    FontInfo(const uint8_t* font_data, float offsetSepX, float offsetSepY, float offsetFontX,float offsetFontY);
};

typedef std::vector<FontInfo> FontGroup;

class SimpleFontManager {
private:
    // Static font groups (defined as constants)
    static const FontGroup* font_groups[];
    static const uint8_t font_groups_count;
    
public:
    // Constructor
    SimpleFontManager();
    
    // Font retrieval method
    const FontInfo* getCurrentFont(uint8_t groupIndex, uint8_t index);
    
    // Button control methods
    int switchToNextFontIndex(uint8_t groupIndex, uint8_t index);            
    int switchToPreviousFontIndex(uint8_t groupIndex, uint8_t index);
    
};

// Static font groups definition (to be defined in .cpp)
extern const FontGroup SMALL_FONTS;
extern const FontGroup MEDIUM_FONTS;  
extern const FontGroup LARGE_FONTS;

// Global SimpleFontManager object
extern SimpleFontManager simpleFontManager;