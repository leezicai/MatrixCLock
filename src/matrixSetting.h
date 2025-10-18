#ifndef MATRIX_SETTING_H
#define MATRIX_SETTING_H

#include <stdint.h>
#include "matrixData.h"
#include "matrixLanguage.h"

// Language enum
// typedef enum {
//     LANG_CHINESE = 0,
//     LANG_ENGLISH = 1,
//     LANG_COUNT = 2
// } Language;

// Maximum language count constant
#define MAX_LANG LANG_COUNT

// Common word indices
typedef enum {
    WORD_SETTING = 0,
    WORD_BRIGHTNESS = 1,
    WORD_BRIGHTNESS_AUTO = 2,
    WORD_ON = 3,
    WORD_OFF = 4,
    WORD_LANGUGE = 5,
    WORD_CURRENT_LANGUGE = 6,
    WORD_SETUP = 7,
    WORD_SETUP_WIFI_TITLE = 8,
    WORD_SETUP_WIFI_ADDRESS = 9,
    WORD_SETUP_WIFI_FAILD = 10,
    WORD_SETUP_WIFI_NETWORK_FAILD = 11,
    WORD_START_TITLE = 12,
    WORD_BLANK = 13,
    WORD_SETUP_WIFI_STARTING = 14,
    WORD_ALARM = 15,
    WORD_TIMEZONE = 16,
    WORD_COUNT = 17
} CommonWordIndex;

class MatrixSettings {
public:
    // Constructor
    MatrixSettings();
    void init();
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