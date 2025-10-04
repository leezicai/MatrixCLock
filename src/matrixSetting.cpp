#include "matrixSetting.h"
#include <stdio.h>
#include <string.h>

// Global object definition
MatrixSettings matrixSettings;

// Common words
const char* MatrixSettings::commonWords_CN[WORD_COUNT] = {
    "设置-时间",  // WORD_SETTING
    "设置-亮度",      // WORD_BRIGHTNESS
    "自动",      // WORD_TIME
    "开",      // WORD_TEMPERATURE
    "关",      // WORD_HUMIDITY
    "语言",      // WORD_WEATHER
    "中文",      // WORD_ALARM
    "定时器",    // WORD_TIMER
    "亮度",      // WORD_BRIGHTNESS
    "音量"       // WORD_VOLUME
};

const char* MatrixSettings::commonWords_EN[WORD_COUNT] = {
    "Set-Time",  // WORD_SETTING
    "Set-Bright",     // 亮度
    "Auto",          // WORD_TIME
    "On",   // WORD_TEMPERATURE
    "Off",      // WORD_HUMIDITY
    "Languge",       // WORD_WEATHER
    "En",         // WORD_ALARM
    "Timer",         // WORD_TIMER
    "Brightness",    // WORD_BRIGHTNESS
    "Volume"         // WORD_VOLUME
};

// Constructor
MatrixSettings::MatrixSettings() : currentLanguage(LANG_CHINESE) {
}

// Language management methods
void MatrixSettings::setLanguage(Language lang) {
    if (lang >= 0 && lang < MAX_LANG) {
        currentLanguage = lang;
    }
}

Language MatrixSettings::getCurrentLanguage() const {
    return currentLanguage;
}

void MatrixSettings::nextLanguage() {
    currentLanguage = (Language)((currentLanguage + 1) % MAX_LANG);
}

void MatrixSettings::previousLanguage() {
    currentLanguage = (Language)((currentLanguage - 1 + MAX_LANG) % MAX_LANG);
}

// Common word methods
const char* MatrixSettings::getCommonWord(Language lang, CommonWordIndex index) {
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

// Instance methods using current language
const char* MatrixSettings::getCommonWord(CommonWordIndex index) {
    return getCommonWord(currentLanguage, index);
}