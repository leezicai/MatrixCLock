#pragma once

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <iomanip>
#include <sstream>
#include <string>
#include "display.h"

extern MatrixPanel_I2S_DMA *dma_display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;


struct TimeStrings;
struct DiffTimeStrings;
struct FontMetrics;

class Animation1{
public:
  Animation1();
  // Animation methods for current time (fading out)
  void displayStaticHourMinuteSecond(TimeStrings ts, uint16_t colorRGB565,
                                     int x, int y, FontMetrics fontMetrics,
                                     FontMetrics sepMetrics, int offSetNumFont,
                                     int offSetNumSep, int offSetFont,
                                     int offSetSepX, int offSetSepY,
                                     const uint8_t *fontName);

};
extern Animation1 animation1;