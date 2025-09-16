#include "animation2.h"
#include "display.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

// 定义全局唯一实例
Animation2 animation2;

Animation2::Animation2() {}

// Animation methods for current time (fading out)
void Animation2::displayTimeNowHour(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.hour24_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y ,
      timeNow.hour24_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.hour24_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNow.hour24_2);
}

void Animation2::displayTimeNowMinute(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.minute_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNow.minute_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.minute_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y ,
      timeNow.minute_2);
}

void Animation2::displayTimeNowSecond(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.second_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNow.second_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.second_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y ,
      timeNow.second_2);
}

void Animation2::displayTimeNowYear(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y, timeNow.year_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y, timeNow.year_2);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_3, 0),
      x + (1 + fontWidth) * (offsetNum + 2),
      y, timeNow.year_3);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_4, 0),
      x + (1 + fontWidth) * (offsetNum + 3),
      y, timeNow.year_4);
}

void Animation2::displayTimeNowMonth(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.month_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNow.month_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.month_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNow.month_2);
}

void Animation2::displayTimeNowDay(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.day_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y, timeNow.day_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.day_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y, timeNow.day_2);
}

// Animation methods for next time (fading in)
void Animation2::displayTimeNextHour(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.hour24_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNowNextSec.hour24_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.hour24_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNowNextSec.hour24_2);
}

void Animation2::displayTimeNextMinute(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.minute_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNowNextSec.minute_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.minute_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y ,
      timeNowNextSec.minute_2);
}

void Animation2::displayTimeNextSecond(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.second_1, 0.1),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNowNextSec.second_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.second_2, 0.1),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNowNextSec.second_2);
}

void Animation2::displayTimeNextYear(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNowNextSec.year_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNowNextSec.year_2);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_3, 0),
      x + (1 + fontWidth) * (offsetNum + 2),
      y,
      timeNowNextSec.year_3);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_4, 0),
      x + (1 + fontWidth) * (offsetNum + 3),
      y,
      timeNowNextSec.year_4);
}

void Animation2::displayTimeNextMonth(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.month_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNowNextSec.month_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.month_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNowNextSec.month_2);
}

void Animation2::displayTimeNextDay(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.day_1, 0),
      x + (1 + fontWidth) * offsetNum,
      y,
      timeNowNextSec.day_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.day_2, 0),
      x + (1 + fontWidth) * (offsetNum + 1),
      y,
      timeNowNextSec.day_2);
}
