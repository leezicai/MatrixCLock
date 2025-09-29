#include "setting.h"
#include <stdio.h>
#include <string.h>

// Global object definition
MatrixLanguageSettings matrixSettings;

// Static member definitions
struct TimeData;

// Long weekday names (Sunday=0, Monday=1, ..., Saturday=6)
const char* MatrixLanguageSettings::longWeekdays_CN[7] = {
    "星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
};

const char* MatrixLanguageSettings::longWeekdays_EN[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

// Short weekday names
const char* MatrixLanguageSettings::shortWeekdays_CN[7] = {
    "周日", "周一", "周二", "周三", "周四", "周五", "周六"
};

const char* MatrixLanguageSettings::shortWeekdays_EN[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

// Common words
const char* MatrixLanguageSettings::commonWords_CN[WORD_COUNT] = {
    "设置-时间",      // WORD_SETTING
    "日期",      // WORD_DATE
    "时间",      // WORD_TIME
    "温度",      // WORD_TEMPERATURE
    "湿度",      // WORD_HUMIDITY
    "天气",      // WORD_WEATHER
    "闹钟",      // WORD_ALARM
    "定时器",    // WORD_TIMER
    "亮度",      // WORD_BRIGHTNESS
    "音量"       // WORD_VOLUME
};

const char* MatrixLanguageSettings::commonWords_EN[WORD_COUNT] = {
    "Settings-Time",     // WORD_SETTING
    "Date",         // WORD_DATE
    "Time",         // WORD_TIME
    "Temperature",  // WORD_TEMPERATURE
    "Humidity",     // WORD_HUMIDITY
    "Weather",      // WORD_WEATHER
    "Alarm",        // WORD_ALARM
    "Timer",        // WORD_TIMER
    "Brightness",   // WORD_BRIGHTNESS
    "Volume"       // WORD_VOLUME
};

// Chinese numbers for date formatting
const char* MatrixLanguageSettings::chineseNumbers[10] = {
    "零", "一", "二", "三", "四", "五", "六", "七", "八", "九"
};

const char* MatrixLanguageSettings::chineseMonths[12] = {
    "一月", "二月", "三月", "四月", "五月", "六月",
    "七月", "八月", "九月", "十月", "十一月", "十二月"
};

const char* MatrixLanguageSettings::chineseTens[4] = {
    "十", "二十", "三十"
};

// English month abbreviations
const char* MatrixLanguageSettings::englishMonthsShort[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// Static buffers for different format strings
char MatrixLanguageSettings::dateBuffer[64];
char MatrixLanguageSettings::dateWeekdayBuffer[64];
char MatrixLanguageSettings::monthDateBuffer[64];
char MatrixLanguageSettings::monthDateWeekdayBuffer[64];

// Constructor
MatrixLanguageSettings::MatrixLanguageSettings() : currentLanguage(LANG_CHINESE) {
}

// Language management methods
void MatrixLanguageSettings::setLanguage(Language lang) {
    if (lang >= 0 && lang < MAX_LANG) {
        currentLanguage = lang;
    }
}

Language MatrixLanguageSettings::getCurrentLanguage() const {
    return currentLanguage;
}

void MatrixLanguageSettings::nextLanguage() {
    currentLanguage = (Language)((currentLanguage + 1) % MAX_LANG);
}

void MatrixLanguageSettings::previousLanguage() {
    currentLanguage = (Language)((currentLanguage - 1 + MAX_LANG) % MAX_LANG);
}

// Helper function to convert Chinese day number
static void formatChineseDay(int day, char* dayStr, size_t size) {
    const char* chineseNumbers[10] = {
        "零", "一", "二", "三", "四", "五", "六", "七", "八", "九"
    };
    
    if (day < 10) {
        snprintf(dayStr, size, "%s", chineseNumbers[day]);
    } else if (day == 10) {
        snprintf(dayStr, size, "十");
    } else if (day < 20) {
        snprintf(dayStr, size, "十%s", chineseNumbers[day % 10]);
    } else if (day % 10 == 0) {
        snprintf(dayStr, size, "%s十", chineseNumbers[day / 10]);
    } else {
        snprintf(dayStr, size, "%s十%s", chineseNumbers[day / 10], chineseNumbers[day % 10]);
    }
}

// Static method implementations
const char* MatrixLanguageSettings::getLongWeekday(Language lang, const TimeData& timeData) {
    if (timeData.mDay < 0 || timeData.mDay > 6) {
        return "";
    }
    
    switch (lang) {
        case LANG_CHINESE:
            return longWeekdays_CN[timeData.mDay];
        case LANG_ENGLISH:
            return longWeekdays_EN[timeData.mDay];
        default:
            return "";
    }
}

const char* MatrixLanguageSettings::getShortWeekday(Language lang, const TimeData& timeData) {
    if (timeData.mDay < 0 || timeData.mDay > 6) {
        return "";
    }
    
    switch (lang) {
        case LANG_CHINESE:
            return shortWeekdays_CN[timeData.mDay];
        case LANG_ENGLISH:
            return shortWeekdays_EN[timeData.mDay];
        default:
            return "";
    }
}

const char* MatrixLanguageSettings::getDateString(Language lang, const TimeData& timeData) {
    memset(dateBuffer, 0, sizeof(dateBuffer));
    
    if (lang == LANG_CHINESE) {
        // Chinese format: 二零二五年二月二十日
        char yearStr[32] = "";
        char dayStr[16] = "";
        
        // Convert year to Chinese
        int year = timeData.year;
        int thousands = year / 1000;
        int hundreds = (year % 1000) / 100;
        int tens = (year % 100) / 10;
        int ones = year % 10;
        
        snprintf(yearStr, sizeof(yearStr), "%s%s%s%s年", 
                chineseNumbers[thousands],
                chineseNumbers[hundreds],
                chineseNumbers[tens],
                chineseNumbers[ones]);
        
        // Convert day to Chinese
        formatChineseDay(timeData.day, dayStr, sizeof(dayStr));
        
        snprintf(dateBuffer, sizeof(dateBuffer), "%s%s%s日", 
                yearStr, chineseMonths[timeData.month - 1], dayStr);
        
    } else if (lang == LANG_ENGLISH) {
        // English format: Oct 20, 2025
        snprintf(dateBuffer, sizeof(dateBuffer), "%s %d, %d", 
                englishMonthsShort[timeData.month - 1], 
                timeData.day, 
                timeData.year);
    }
    
    return dateBuffer;
}

// NEW: Get date + short weekday string
const char* MatrixLanguageSettings::getDateShortWeekday(Language lang, const TimeData& timeData) {
    memset(dateWeekdayBuffer, 0, sizeof(dateWeekdayBuffer));
    
    if (lang == LANG_CHINESE) {
        // Chinese format: 10月20日 周一
        char dayStr[16] = "";
        formatChineseDay(timeData.day, dayStr, sizeof(dayStr));
        
        snprintf(dateWeekdayBuffer, sizeof(dateWeekdayBuffer), "%s%s日 %s", 
                chineseMonths[timeData.month - 1], 
                dayStr,
                shortWeekdays_CN[timeData.mDay]);
        
    } else if (lang == LANG_ENGLISH) {
        // English format: Oct 20 Mon
        snprintf(dateWeekdayBuffer, sizeof(dateWeekdayBuffer), "%s, %d %s", 
                englishMonthsShort[timeData.month - 1], 
                timeData.day,
                shortWeekdays_EN[timeData.mDay]);
    }
    
    return dateWeekdayBuffer;
}

// NEW: Get month + date string
const char* MatrixLanguageSettings::getMonthDate(Language lang, const TimeData& timeData) {
    memset(monthDateBuffer, 0, sizeof(monthDateBuffer));
    
    if (lang == LANG_CHINESE) {
        // Chinese format: 10月20日
        char dayStr[16] = "";
        formatChineseDay(timeData.day, dayStr, sizeof(dayStr));
        
        snprintf(monthDateBuffer, sizeof(monthDateBuffer), "%s%s日", 
                chineseMonths[timeData.month - 1], dayStr);
        
    } else if (lang == LANG_ENGLISH) {
        // English format: Oct 20
        snprintf(monthDateBuffer, sizeof(monthDateBuffer), "%s, %d", 
                englishMonthsShort[timeData.month - 1], 
                timeData.day);
    }
    
    return monthDateBuffer;
}

// NEW: Get month + date + weekday string
const char* MatrixLanguageSettings::getMonthDateWeekday(Language lang, const TimeData& timeData) {
    memset(monthDateWeekdayBuffer, 0, sizeof(monthDateWeekdayBuffer));
    
    if (lang == LANG_CHINESE) {
        // Chinese format: 10月20日 星期一
        char dayStr[16] = "";
        formatChineseDay(timeData.day, dayStr, sizeof(dayStr));
        
        snprintf(monthDateWeekdayBuffer, sizeof(monthDateWeekdayBuffer), "%s%s日 %s", 
                chineseMonths[timeData.month - 1], 
                dayStr,
                longWeekdays_CN[timeData.mDay]);
        
    } else if (lang == LANG_ENGLISH) {
        // English format: Oct 20 Monday
        snprintf(monthDateWeekdayBuffer, sizeof(monthDateWeekdayBuffer), "%s %d %s", 
                englishMonthsShort[timeData.month - 1], 
                timeData.day,
                longWeekdays_EN[timeData.mDay]);
    }
    
    return monthDateWeekdayBuffer;
}

const char* MatrixLanguageSettings::getCommonWord(Language lang, CommonWordIndex index) {
    if (index < 0 || index >= WORD_COUNT) {
        return "";
    }
    
    switch (lang) {
        case LANG_CHINESE:
            return commonWords_CN[index];
        case LANG_ENGLISH:
            return commonWords_EN[index];
        default:
            return "";
    }
}

// Universal string getter method (static)
const char* MatrixLanguageSettings::getStr(Language lang, const TimeData& timeData, int16_t index) {
    switch (index) {
        case STR_LONG_WEEKDAY:
            return getLongWeekday(lang, timeData);
        case STR_SHORT_WEEKDAY:
            return getShortWeekday(lang, timeData);
        case STR_DATE_STRING:
            return getDateString(lang, timeData);
        case STR_DATE_SHORT_WEEKDAY:
            return getDateShortWeekday(lang, timeData);
        case STR_MONTH_DATE:
            return getMonthDate(lang, timeData);
        case STR_MONTH_DATE_WEEKDAY:
            return getMonthDateWeekday(lang, timeData);
        default:
            // Check if it's a common word index
            if (index >= STR_COMMON_WORD_START && index < STR_COMMON_WORD_START + WORD_COUNT) {
                CommonWordIndex wordIndex = (CommonWordIndex)(index - STR_COMMON_WORD_START);
                return getCommonWord(lang, wordIndex);
            }
            return "";
    }
}

// Instance methods using current language
const char* MatrixLanguageSettings::getLongWeekday(const TimeData& timeData) {
    return getLongWeekday(currentLanguage, timeData);
}

const char* MatrixLanguageSettings::getShortWeekday(const TimeData& timeData) {
    return getShortWeekday(currentLanguage, timeData);
}

const char* MatrixLanguageSettings::getDateString(const TimeData& timeData) {
    return getDateString(currentLanguage, timeData);
}

const char* MatrixLanguageSettings::getDateShortWeekday(const TimeData& timeData) {
    return getDateShortWeekday(currentLanguage, timeData);
}

const char* MatrixLanguageSettings::getMonthDate(const TimeData& timeData) {
    return getMonthDate(currentLanguage, timeData);
}

const char* MatrixLanguageSettings::getMonthDateWeekday(const TimeData& timeData) {
    return getMonthDateWeekday(currentLanguage, timeData);
}

const char* MatrixLanguageSettings::getCommonWord(CommonWordIndex index) {
    return getCommonWord(currentLanguage, index);
}

const char* MatrixLanguageSettings::getStr(const TimeData& timeData, int16_t index) {
    return getStr(currentLanguage, timeData, index);
}