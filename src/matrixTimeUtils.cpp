#include "matrixTimeUtils.h"
#include "matrixSetting.h"
#include <stdio.h>
#include <string.h>

// Global object definition
MatrixTimeUtils matrixTimeUtils;

// Long weekday names (Sunday=0, Monday=1, ..., Saturday=6)
const char* MatrixTimeUtils::longWeekdays_CN[7] = {
    "星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
};

const char* MatrixTimeUtils::longWeekdays_EN[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

// Short weekday names
const char* MatrixTimeUtils::shortWeekdays_CN[7] = {
    "周日", "周一", "周二", "周三", "周四", "周五", "周六"
};

const char* MatrixTimeUtils::shortSWeekdays_CN[7] = {
    "日", "一", "二", "三", "四", "五", "六"
};

const char* MatrixTimeUtils::shortWeekdays_EN[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char* MatrixTimeUtils::shortSWeekdays_EN[7] = {
    "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"
};
// Chinese numbers for date formatting
const char* MatrixTimeUtils::chineseNumbers[10] = {
    "零", "一", "二", "三", "四", "五", "六", "七", "八", "九"
};

const char* MatrixTimeUtils::chineseMonths[12] = {
    "一月", "二月", "三月", "四月", "五月", "六月",
    "七月", "八月", "九月", "十月", "十一月", "十二月"
};

const char* MatrixTimeUtils::chineseTens[4] = {
    "十", "廿", "卅"
};

// English month abbreviations
const char* MatrixTimeUtils::englishMonthsShort[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// Static buffers definition
char MatrixTimeUtils::dateBuffer[64];
char MatrixTimeUtils::dateWeekdayBuffer[64];
char MatrixTimeUtils::monthDateBuffer[64];
char MatrixTimeUtils::monthDateWeekdayBuffer[64];

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

const char* MatrixTimeUtils::getLongWeekday(Language lang, const TimeData& timeData) {
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

// Instance methods using current language from matrixSettings
const char* MatrixTimeUtils::getLongWeekday(const TimeData& timeData) {
    return getLongWeekday(matrixSettings.getCurrentLanguage(), timeData);
}

const char* MatrixTimeUtils::getShortWeekday(const TimeData& timeData) {
    return getShortWeekday(matrixSettings.getCurrentLanguage(), timeData);
}
const char* MatrixTimeUtils::getShortShortWeekday(int index) {
    return getShortSWeekday(matrixSettings.getCurrentLanguage(), index);
}

const char* MatrixTimeUtils::getDateString(const TimeData& timeData) {
    return getDateString(matrixSettings.getCurrentLanguage(), timeData);
}

const char* MatrixTimeUtils::getDateShortWeekday(const TimeData& timeData) {
    return getDateShortWeekday(matrixSettings.getCurrentLanguage(), timeData);
}

const char* MatrixTimeUtils::getMonthDate(const TimeData& timeData) {
    return getMonthDate(matrixSettings.getCurrentLanguage(), timeData);
}

const char* MatrixTimeUtils::getMonthDateWeekday(const TimeData& timeData) {
    return getMonthDateWeekday(matrixSettings.getCurrentLanguage(), timeData);
}

const char* MatrixTimeUtils::getStr(const TimeData& timeData, int16_t index) {
    return getStr(matrixSettings.getCurrentLanguage(), timeData, index);
}

const char* MatrixTimeUtils::getShortWeekday(Language lang, const TimeData& timeData) {
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

const char* MatrixTimeUtils::getShortSWeekday(Language lang, int index) {
    if (index < 0 || index > 6) {
        return "";
    }
    
    switch (lang) {
        case LANG_CHINESE:
            return shortSWeekdays_CN[index];
        case LANG_ENGLISH:
            return shortSWeekdays_EN[index];
        default:
            return "";
    }
}

const char* MatrixTimeUtils::getDateString(Language lang, const TimeData& timeData) {
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

const char* MatrixTimeUtils::getDateShortWeekday(Language lang, const TimeData& timeData) {
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

const char* MatrixTimeUtils::getMonthDate(Language lang, const TimeData& timeData) {
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

const char* MatrixTimeUtils::getMonthDateWeekday(Language lang, const TimeData& timeData) {
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

const char* MatrixTimeUtils::getStr(Language lang, const TimeData& timeData, int16_t index) {
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
            return "";
    }
}