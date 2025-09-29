#ifndef SETTING_H
#define SETTING_H
#include <stdint.h>
#include "matrixTimeData.h"
struct TimeData;

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
    WORD_DATE = 1,
    WORD_TIME = 2,
    WORD_TEMPERATURE = 3,
    WORD_HUMIDITY = 4,
    WORD_WEATHER = 5,
    WORD_ALARM = 6,
    WORD_TIMER = 7,
    WORD_BRIGHTNESS = 8,
    WORD_VOLUME = 9,
    WORD_COUNT = 10
} CommonWordIndex;

// String type indices for getStr method
typedef enum {
    STR_LONG_WEEKDAY = 0,
    STR_SHORT_WEEKDAY = 1,
    STR_DATE_STRING = 2,
    STR_DATE_SHORT_WEEKDAY = 3,      // New: Date + Short Weekday
    STR_MONTH_DATE = 4,              // New: Month + Date
    STR_MONTH_DATE_WEEKDAY = 5,      // New: Month + Date + Weekday
    STR_COMMON_WORD_START = 100      // Common words start from index 100
} StringTypeIndex;

class MatrixLanguageSettings {
public:
    // Constructor
    MatrixLanguageSettings();
    
    // Language management
    void setLanguage(Language lang);
    Language getCurrentLanguage() const;
    void nextLanguage();
    void previousLanguage();
    
    // Get long weekday name (Monday/星期一)
    static const char* getLongWeekday(Language lang, const TimeData& timeData);
    
    // Get short weekday name (Mon/周一)
    static const char* getShortWeekday(Language lang, const TimeData& timeData);
    
    // Get formatted date string
    static const char* getDateString(Language lang, const TimeData& timeData);
    
    // NEW: Get date + short weekday string (Oct 20 Mon / 10月20日 周一)
    static const char* getDateShortWeekday(Language lang, const TimeData& timeData);
    
    // NEW: Get month + date string (Oct 20 / 10月20日)
    static const char* getMonthDate(Language lang, const TimeData& timeData);
    
    // NEW: Get month + date + weekday string (Oct 20 Monday / 10月20日 星期一)
    static const char* getMonthDateWeekday(Language lang, const TimeData& timeData);
    
    // Get common word by index
    static const char* getCommonWord(Language lang, CommonWordIndex index);
    
    // Universal string getter method
    static const char* getStr(Language lang, const TimeData& timeData, int16_t index);
    
    // Instance methods using current language
    const char* getLongWeekday(const TimeData& timeData);
    const char* getShortWeekday(const TimeData& timeData);
    const char* getDateString(const TimeData& timeData);
    const char* getDateShortWeekday(const TimeData& timeData);      // NEW
    const char* getMonthDate(const TimeData& timeData);            // NEW
    const char* getMonthDateWeekday(const TimeData& timeData);     // NEW
    const char* getCommonWord(CommonWordIndex index);
    const char* getStr(const TimeData& timeData, int16_t index);

private:
    // Current language setting
    Language currentLanguage;
    
    // Long weekday names arrays
    static const char* longWeekdays_CN[7];
    static const char* longWeekdays_EN[7];
    
    // Short weekday names arrays
    static const char* shortWeekdays_CN[7];
    static const char* shortWeekdays_EN[7];
    
    // Common words arrays
    static const char* commonWords_CN[WORD_COUNT];
    static const char* commonWords_EN[WORD_COUNT];
    
    // Chinese number conversion
    static const char* chineseNumbers[10];
    static const char* chineseTens[4];
    static const char* chineseMonths[12];
    
    // English month abbreviations
    static const char* englishMonthsShort[12];
    
    // Static buffers for different format strings
    static char dateBuffer[64];
    static char dateWeekdayBuffer[64];
    static char monthDateBuffer[64];
    static char monthDateWeekdayBuffer[64];
};

// Global extern object for external access
extern MatrixLanguageSettings matrixSettings;

#endif // SETTING_H