#include "display.h"
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

// 定义全局唯一实例
Display display;

Display::Display() {}

// RGB to RGB565
uint16_t Display::rgbToRgb565(uint8_t red, uint8_t green, uint8_t blue) {
    // Red: 8 bits -> 5 bits (keep upper 5 bits)
    // Green: 8 bits -> 6 bits (keep upper 6 bits)  
    // Blue: 8 bits -> 5 bits (keep upper 5 bits)
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

// 显示文字（RGB565）
void Display::displayText(uint16_t colorRGB565,
                          int x,
                          int y,
                          const uint8_t* fontName,
                          const char* text) {
    u8g2_for_adafruit_gfx.setFont(fontName);
    u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
    u8g2_for_adafruit_gfx.setCursor(x, y);
    u8g2_for_adafruit_gfx.print(text);
}

// 显示文字（RGB888）
void Display::displayTextRGB(uint8_t red,
                             uint8_t green,
                             uint8_t blue,
                             int x,
                             int y,
                             const uint8_t* fontName,
                             const char* text) {
    uint16_t colorRGB565 = rgbToRgb565(red, green, blue);

    displayText(colorRGB565, x, y, fontName, text);
}

// 获取当前时间并转换成字符串结构体
TimeStrings Display::getTimeStrings(time_t now) {
    // 获取系统时间
    // time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    TimeStrings ts;

    // 转换成年月日等字符串
    std::ostringstream oss;

    oss << (timeinfo.tm_year + 1900);
    ts.year = oss.str(); oss.str(""); oss.clear();

    oss << std::setw(2) << std::setfill('0') << (timeinfo.tm_mon + 1);
    ts.month = oss.str(); oss.str(""); oss.clear();

    oss << std::setw(2) << std::setfill('0') << timeinfo.tm_mday;
    ts.day = oss.str(); oss.str(""); oss.clear();

    oss << std::setw(2) << std::setfill('0') << timeinfo.tm_hour;
    ts.hour24 = oss.str(); oss.str(""); oss.clear();

    oss << std::setw(2) << std::setfill('0') << timeinfo.tm_min;
    ts.minute = oss.str(); oss.str(""); oss.clear();

    oss << std::setw(2) << std::setfill('0') << timeinfo.tm_sec;
    ts.second = oss.str(); oss.str(""); oss.clear();

    int h12 = timeinfo.tm_hour % 12;
    if (h12 == 0) h12 = 12;
    oss << std::setw(2) << std::setfill('0') << h12;
    ts.hour12 = oss.str(); oss.str(""); oss.clear();

    ts.ampm = (timeinfo.tm_hour < 12) ? "AM" : "PM";

    return ts;
}
