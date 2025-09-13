#pragma once

#include <U8g2_for_Adafruit_GFX.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *dma_display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
#define time_page 0


// 定义一个结构体，保存所有需要的字符串
struct TimeStrings {
    std::string year;        // "2025"
    std::string month;       // "09"
    std::string day;         // "09"
    std::string hour24;      // "23"
    std::string minute;      // "59"
    std::string second;      // "58"
    std::string hour12;      // "11"
    std::string ampm;        // "AM" or "PM"

     // 方法：返回 "YYYY-MM-DD"
    std::string getDateString() const {
        return year + "-" + month + "-" + day;
    }

    // 方法：返回 "HH:MM:SS"
    std::string getTimeString24() const {
        return hour24 + ":" + minute + ":" + second;
    }

    // 方法：返回 "HH:MM:SS AM/PM"
    std::string getTimeString12() const {
        return hour12 + ":" + minute + ":" + second + " " + ampm;
    }
};
class Display {
public:
    Display();
    // RGB to RGB565
    uint16_t rgbToRgb565(uint8_t red, uint8_t green, uint8_t blue);

    // 显示文字（RGB565）
    void displayText(uint16_t colorRGB565,
                     int x,
                     int y,
                     const uint8_t* fontName,
                     const char* text);

    // 显示文字（RGB888）
    void displayTextRGB(uint8_t red,
                        uint8_t green,
                        uint8_t blue,
                        int x,
                        int y,
                        const uint8_t* fontName,
                        const char* text);
    // 时间字符串结构体
    TimeStrings getTimeStrings(time_t now);
};

// 全局实例声明
extern Display display;