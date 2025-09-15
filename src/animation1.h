#pragma once

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <iomanip>
#include <sstream>
#include <string>

extern MatrixPanel_I2S_DMA *dma_display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

struct TimeStrings;
struct DiffTimeStrings;

class Animation1{
	public:
	Animation1();
	// Animation methods for current time (fading out)
      void displayTimeNowHour(float animationSpeed,
                                        TimeStrings timeNow,
                                        DiffTimeStrings diffTimeStrings,
                                        uint16_t colorRGB565, int x, int y,
                                        int fontWidth, int fontHeight,
                                        int offsetNum, const uint8_t *fontName);

      void displayTimeNowMinute(
          float animationSpeed, TimeStrings timeNow,
          DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
          int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName);

      void displayTimeNowSecond(
          float animationSpeed, TimeStrings timeNow,
          DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
          int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName);

      void displayTimeNowYear(float animationSpeed,
                                        TimeStrings timeNow,
                                        DiffTimeStrings diffTimeStrings,
                                        uint16_t colorRGB565, int x, int y,
                                        int fontWidth, int fontHeight,
                                        int offsetNum, const uint8_t *fontName);

      void displayTimeNowMonth(float animationSpeed,
                                         TimeStrings timeNow,
                                         DiffTimeStrings diffTimeStrings,
                                         uint16_t colorRGB565, int x, int y,
                                         int fontWidth, int fontHeight,
                                         int offsetNum, const uint8_t *fontName);

      void displayTimeNowDay(float animationSpeed,
                                       TimeStrings timeNow,
                                       DiffTimeStrings diffTimeStrings,
                                       uint16_t colorRGB565, int x, int y,
                                       int fontWidth, int fontHeight,
                                       int offsetNum, const uint8_t *fontName);

      // Animation methods for next time (fading in)
      void displayTimeNextHour(float animationSpeed,
                                         TimeStrings timeNowNextSec,
                                         DiffTimeStrings diffTimeStrings,
                                         uint16_t colorRGB565, int x, int y,
                                         int fontWidth, int fontHeight,
                                         int offsetNum, const uint8_t *fontName);

      void displayTimeNextMinute(
          float animationSpeed, TimeStrings timeNowNextSec,
          DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
          int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName);

      void displayTimeNextSecond(float animationSpeed,
                                          TimeStrings timeNowNextSec,
                                          DiffTimeStrings diffTimeStrings,
                                          uint16_t colorRGB565, int x, int y,
                                          int fontWidth, int fontHeight,
                                          int offsetNum,
                                          const uint8_t *fontName);

      void displayTimeNextYear(float animationSpeed,
                                        TimeStrings timeNowNextSec,
                                        DiffTimeStrings diffTimeStrings,
                                        uint16_t colorRGB565, int x, int y,
                                        int fontWidth, int fontHeight,
                                        int offsetNum, const uint8_t *fontName);

      void displayTimeNextMonth(float animationSpeed,
                                         TimeStrings timeNowNextSec,
                                         DiffTimeStrings diffTimeStrings,
                                         uint16_t colorRGB565, int x, int y,
                                         int fontWidth, int fontHeight,
                                         int offsetNum,
                                         const uint8_t *fontName);

      void displayTimeNextDay(float animationSpeed,
                                       TimeStrings timeNowNextSec,
                                       DiffTimeStrings diffTimeStrings,
                                       uint16_t colorRGB565, int x, int y,
                                       int fontWidth, int fontHeight,
                                       int offsetNum, const uint8_t *fontName);
};
extern Animation1 animation1;