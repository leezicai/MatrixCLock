#ifndef MATRIX_TIME_UTILS_H
#define MATRIX_TIME_UTILS_H

#include "matrixLanguage.h"
#include "matrixTimeData.h"
#include "matrixSetting.h"


class MatrixTimeUtils {
public:
    // Get long weekday name (Monday/星期一)
    static const char* getLongWeekday(Language lang, const TimeData& timeData);
    
    // Get short weekday name (Mon/周一)
    static const char* getShortWeekday(Language lang, const TimeData& timeData);
    // Get short weekday name (Mon/周一)
    static const char* getShortSWeekday(Language lang, int index);
    
    // Get formatted date string
    static const char* getDateString(Language lang, const TimeData& timeData);
    
    // Get date + short weekday string (Oct 20 Mon / 10月20日 周一)
    static const char* getDateShortWeekday(Language lang, const TimeData& timeData);
    
    // Get month + date string (Oct 20 / 10月20日)
    static const char* getMonthDate(Language lang, const TimeData& timeData);
    
    // Get month + date + weekday string (Oct 20 Monday / 10月20日 星期一)
    static const char* getMonthDateWeekday(Language lang, const TimeData& timeData);
    
    // Universal string getter method for time-related strings
    static const char* getStr(Language lang, const TimeData& timeData, int16_t index);
    
    // Instance methods using current language from matrixSettings
    const char* getLongWeekday(const TimeData& timeData);
    const char* getShortWeekday(const TimeData& timeData);
    const char* getShortShortWeekday(int index);
    const char* getDateString(const TimeData& timeData);
    const char* getDateShortWeekday(const TimeData& timeData);
    const char* getMonthDate(const TimeData& timeData);
    const char* getMonthDateWeekday(const TimeData& timeData);
    const char* getStr(const TimeData& timeData, int16_t index);

private:
    // Long weekday names arrays
    static const char* longWeekdays_CN[7];
    static const char* longWeekdays_EN[7];
    
    // Short weekday names arrays
    static const char* shortWeekdays_CN[7];
    static const char* shortWeekdays_EN[7];
    
    // Short Short weekday names arrays
    static const char* shortSWeekdays_CN[7];
    static const char* shortSWeekdays_EN[7];

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

// String type indices for time-related strings
typedef enum {
    STR_LONG_WEEKDAY = 0,
    STR_SHORT_WEEKDAY = 1,
    STR_DATE_STRING = 2,
    STR_DATE_SHORT_WEEKDAY = 3,
    STR_MONTH_DATE = 4,
    STR_MONTH_DATE_WEEKDAY = 5
} TimeStringTypeIndex;

// Global extern object for external access
extern MatrixTimeUtils matrixTimeUtils;

#endif // MATRIX_TIME_UTILS_H