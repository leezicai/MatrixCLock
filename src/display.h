#pragma once

#include <U8g2_for_Adafruit_GFX.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <string>
#include <iomanip>
#include <sstream>
#include "page.h"
#include "animation1.h"
#include "animation2.h"

extern MatrixPanel_I2S_DMA *dma_display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;


// 定义一个结构体，保存所有需要的字符串
struct TimeStrings {
    int16_t year;        // 2025
    int16_t year_1;      // 2 (first digit)
    int16_t year_2;      // 0 (second digit) 
    int16_t year_3;      // 2 (third digit)
    int16_t year_4;      // 5 (fourth digit)
    int16_t month;       // 9
    int16_t month_1;     // 0 (first digit)
    int16_t month_2;     // 9 (second digit)
    int16_t day;         // 14
    int16_t day_1;       // 1 (first digit)
    int16_t day_2;       // 4 (second digit)
    int16_t mday;
    int16_t hour24;      // 23
    int16_t hour24_1;    // 2 (first digit)
    int16_t hour24_2;    // 3 (second digit)
    int16_t minute;      // 59
    int16_t minute_1;    // 5 (first digit)
    int16_t minute_2;    // 9 (second digit)
    int16_t second;      // 58
    int16_t second_1;    // 5 (first digit)
    int16_t second_2;    // 8 (second digit)
    int16_t hour12;      // 11
    int16_t hour12_1;    // 1 (first digit)
    int16_t hour12_2;    // 1 (second digit)
    std::string ampm;        // 0 for AM, 1 for PM
    std::string ampm_1;        // 0 for AM, 1 for PM
    std::string ampm_2;        // 0 for AM, 1 for PM
    
    // Method: return "YYYY-MM-DD"
    std::string getDateString() const {
        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << year << "-"
            << std::setw(2) << std::setfill('0') << month << "-"
            << std::setw(2) << std::setfill('0') << day;
        return oss.str();
    }
    
    // Method: return "HH:MM:SS"
    std::string getTimeString24() const {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << hour24 << ":"
            << std::setw(2) << std::setfill('0') << minute << ":"
            << std::setw(2) << std::setfill('0') << second;
        return oss.str();
    }
    
    // Method: return "HH:MM:SS AM/PM"
    std::string getTimeString12() const {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << hour12 << ":"
            << std::setw(2) << std::setfill('0') << minute << ":"
            << std::setw(2) << std::setfill('0') << second << " " 
            << ampm;
        return oss.str();
    }
};

struct DiffTimeStrings {
    int16_t year;        // 2025
    int16_t year_1;      // 2 (first digit)
    int16_t year_2;      // 0 (second digit) 
    int16_t year_3;      // 2 (third digit)
    int16_t year_4;      // 5 (fourth digit)
    int16_t month;       // 9
    int16_t month_1;     // 0 (first digit)
    int16_t month_2;     // 9 (second digit)
    int16_t day;         // 14
    int16_t day_1;       // 1 (first digit)
    int16_t day_2;       // 4 (second digit)
    int16_t mday;
    int16_t hour24;      // 23
    int16_t hour24_1;    // 2 (first digit)
    int16_t hour24_2;    // 3 (second digit)
    int16_t minute;      // 59
    int16_t minute_1;    // 5 (first digit)
    int16_t minute_2;    // 9 (second digit)
    int16_t second;      // 58
    int16_t second_1;    // 5 (first digit)
    int16_t second_2;    // 8 (second digit)
    int16_t hour12;      // 11
    int16_t hour12_1;    // 1 (first digit)
    int16_t hour12_2;    // 1 (second digit)
    int16_t ampm;        // 0 for AM, 1 for PM
    int16_t ampm_1;        // 0 for AM, 1 for PM
    int16_t ampm_2;        // 0 for AM, 1 for PM
};

struct FontMetrics {
  int height;
  int ascent;
  int descent;
  int charWidth;
};

class Display {
    private:
      float animationSpeed;
    public:
      Display();
      // Function declarations
      uint16_t scaleColorRGB565Custom(uint16_t colorRGB565,
                                      float animationSpeed,
                                      bool colorFlag = true,
                                      float minBrightness = 0.2f);
      uint16_t scaleColorRGB565Forward(uint16_t colorRGB565,
                                       float animationSpeed,
                                       bool colorFlag = true,
                                       float minBrightness = 0.8f);
      // RGB to RGB565
      uint16_t rgbToRgb565(uint8_t red, uint8_t green, uint8_t blue);

      // 显示文字（RGB565）
      void displayText(uint16_t colorRGB565, int x, int y,
                       const uint8_t *fontName, const char *text);
      // 显示文字（RGB565）
      void displayText(uint16_t colorRGB565, int x, int y,
                       const uint8_t *fontName, const int16_t time_int);
      // 显示文字（RGB888）
      void displayTextRGB(uint8_t red, uint8_t green, uint8_t blue, int x,
                          int y, const uint8_t *fontName, const char *text);
      // 显示文字（RGB888）
      void displayTextRGB(uint8_t red, uint8_t green, uint8_t blue, int x,
                          int y, const uint8_t *fontName,
                          const int16_t time_int);
      // 时间字符串结构体
      TimeStrings getTimeStrings(time_t now);
      DiffTimeStrings compareTimeStrings(const TimeStrings &ts1,
                                         const TimeStrings &ts2);
      void flipDMABuffer();
      void clearScreen();
      void displayU8g2(uint16_t colorRGB565, int x, int y, int printInt);
      void displayU8g2(uint16_t colorRGB565, int x, int y, String printStr);
      void displayU8g2(uint16_t colorRGB565, int x, int y,
                       const uint8_t *fontName, int printInt);
      void displayU8g2(uint16_t colorRGB565, int x, int y,
                       const uint8_t *fontName, String printStr);
      // Static display methods
      void setupDisplayContext(uint16_t colorRGB565, int x, int y,
                               int fontWidth, int fontHeight,
                               int separatorWidth, int offSetNumFont,
                               int offSetNumSep, int offSetFont, int offSetSepX,int offSetSepY,
                               const uint8_t *fontName);
      template <typename T>
      void displayStaticOneTemplate(T one, uint16_t colorRGB565, int x, int y,
                                    int fontWidth, int fontHeight,
                                    int separatorWidth, int offSetNumFont,
                                    int offSetNumSep, int offSetFont,
                                    int offSetSepX, int offSetSepY, const uint8_t *fontName) {
        setupDisplayContext(colorRGB565, x, y, fontWidth, fontHeight,
                            separatorWidth, offSetNumFont, offSetNumSep,
                            offSetFont, offSetSepX, offSetSepY, fontName);
        u8g2_for_adafruit_gfx.print(one);
      }

      FontMetrics getFontMetrics(const uint8_t *font, const char *character);

      void displayHourMinuteSecond(unsigned long elapsed, TimeStrings timeNow,
                                   TimeStrings timeNowNextSec,
                                   DiffTimeStrings diffTimeStrings,
                                   int16_t animationType, uint16_t colorRGB565,
                                   int x, int y, int fontWidth, int fontHeight,
                                   const uint8_t *fontName, const char* fontM, const char* sepM);
      void displayHourMinute(
          unsigned long elapsed, TimeStrings timeNow,
          TimeStrings timeNowNextSec, DiffTimeStrings diffTimeStrings,
          int16_t animationType, uint16_t colorRGB565, int x, int y,
          int fontWidth, int fontHeight, const uint8_t *fontName);
      void displayYearMonthDay(
          unsigned long elapsed, TimeStrings timeNow,
          TimeStrings timeNowNextSec, DiffTimeStrings diffTimeStrings,
          int16_t animationType, uint16_t colorRGB565, int x, int y,
          int fontWidth, int fontHeight, const uint8_t *fontName);
      void displayMonthDay(
          unsigned long elapsed, TimeStrings timeNow,
          TimeStrings timeNowNextSec, DiffTimeStrings diffTimeStrings,
          int16_t animationType, uint16_t colorRGB565, int x, int y,
          int fontWidth, int fontHeight, const uint8_t *fontName);
};

// 全局实例声明
extern Display display;