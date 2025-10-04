#ifndef MATRIX_SETTING_H
#define MATRIX_SETTING_H

#include <stdint.h>

// Language enum
typedef enum {
    LANG_CHINESE = 0,
    LANG_ENGLISH = 1,
    LANG_COUNT = 2
} Language;

// Maximum language count constant
#define MAX_LANG LANG_COUNT

// Common word indices
typedef enum {
    WORD_SETTING = 0,
    WORD_BRIGHTNESS = 1,
    WORD_BRIGHTNESS_AUTO = 2,
    WORD_BRIGHTNESS_ON = 3,
    WORD_BRIGHTNESS_OFF = 4,
    WORD_LANGUGE = 5,
    WORD_CURRENT_LANGUGE = 6,
    WORD_TIMER = 7,
    WORD_DATE = 8,
    WORD_VOLUME = 9,
    WORD_COUNT = 10
} CommonWordIndex;

class MatrixSettings {
public:
    // Constructor
    MatrixSettings();
    
    // Language management
    void setLanguage(Language lang);
    Language getCurrentLanguage() const;
    void nextLanguage();
    void previousLanguage();
    
    // Get common word by index
    static const char* getCommonWord(Language lang, CommonWordIndex index);
    
    // Instance methods using current language
    const char* getCommonWord(CommonWordIndex index);

private:
    // Current language setting
    Language currentLanguage ;
    
    // Common words arrays
    static const char* commonWords_CN[WORD_COUNT];
    static const char* commonWords_EN[WORD_COUNT];
};

// Global extern object for external access
extern MatrixSettings matrixSettings;

#endif // MATRIX_SETTING_H