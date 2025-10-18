#include "matrixSetting.h"
#include <stdio.h>
#include <string.h>

// Global object definition
MatrixSettings matrixSettings;

// Common words
const char* MatrixSettings::commonWords_CN[WORD_COUNT] = {
    "设置-时间",  // WORD_SETTING
    "亮度",      // WORD_BRIGHTNESS
    "自动",      // WORD_TIME
    "开",      // WORD_TEMPERATURE
    "关",      // WORD_HUMIDITY
    "语言",      // WORD_WEATHER
    "中文",      // WORD_ALARM
    "连接WiFi并访问",    // WORD_TIMER
    "MatrixClock",      // WORD_BRIGHTNESS
    "http://10.0.0.1",       // WORD_VOLUME
    "连接WiFi错误",       // WORD_VOLUME
    "同步时间网络错误",       // WORD_VOLUME
    "MatrixClock",       // WORD_VOLUME
    "",
    "启动...",
    "闹钟",
    "时区"
};

const char* MatrixSettings::commonWords_EN[WORD_COUNT] = {
    "Set-Time",  // WORD_SETTING
    "Bright",     // 亮度
    "Auto",          // WORD_TIME
    "On",   // WORD_TEMPERATURE
    "Off",      // WORD_HUMIDITY
    "Languge",       // WORD_WEATHER
    "En",         // WORD_ALARM
    "ConnWiFi&Access",         // WORD_TIMER
    "MatrixClock",    // WORD_BRIGHTNESS
    "http://10.0.0.1",         // WORD_VOLUME
    "WiFi Connect Failed",       // WORD_VOLUME
    "SyncTime Net Failed",       // WORD_VOLUME
    "MatrixClock",       // WORD_VOLUME
    "",       // WORD_VOLUME
    "Start...",
    "Alarm",
    "TZ"
};
// #define LOADING_ERR_MSG_FAIL_WIFI "WiFi Connect Failed"
// #define LOADING_ERR_MSG_FAIL_NETWORK "Net SyncTime Failed"

// Constructor
MatrixSettings::MatrixSettings() {
    init();
}

void MatrixSettings::init(){
    setLanguage(static_cast<Language>(matrixDataManager.getLanguage()) );
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
    matrixDataManager.setLanguage(currentLanguage);
}

void MatrixSettings::previousLanguage() {
    currentLanguage = (Language)((currentLanguage - 1 + MAX_LANG) % MAX_LANG);
    matrixDataManager.setLanguage(currentLanguage);
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