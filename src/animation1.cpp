#include "animation1.h"
#include "display.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

// 定义全局唯一实例
Animation1 animation1;

Animation1::Animation1() {}

// Animation methods for current time (fading out)
void Animation1::displayTimeNowHour(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.hour24_1),
      x + (1 + fontWidth) * offsetNum,
      y - animationSpeed * fontHeight * diffTimeStrings.hour24_1,
      timeNow.hour24_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.hour24_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y - animationSpeed * fontHeight * diffTimeStrings.hour24_2,
      timeNow.hour24_2);
}

void Animation1::displayTimeNowMinute(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.minute_1),
      x + (1 + fontWidth) * offsetNum,
      y - animationSpeed * fontHeight * diffTimeStrings.minute_1,
      timeNow.minute_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.minute_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y - animationSpeed * fontHeight * diffTimeStrings.minute_2,
      timeNow.minute_2);
}

void Animation1::displayTimeNowSecond(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.second_1),
      x + (1 + fontWidth) * offsetNum,
      y - animationSpeed * fontHeight * diffTimeStrings.second_1,
      timeNow.second_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.second_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y - animationSpeed * fontHeight * diffTimeStrings.second_2,
      timeNow.second_2);
}

void Animation1::displayTimeNowYear(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_1),
      x + (1 + fontWidth) * offsetNum,
      y - animationSpeed * fontHeight * diffTimeStrings.year_1, timeNow.year_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y - animationSpeed * fontHeight * diffTimeStrings.year_2, timeNow.year_2);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_3),
      x + (1 + fontWidth) * (offsetNum + 2),
      y - animationSpeed * fontHeight * diffTimeStrings.year_3, timeNow.year_3);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.year_4),
      x + (1 + fontWidth) * (offsetNum + 3),
      y - animationSpeed * fontHeight * diffTimeStrings.year_4, timeNow.year_4);
}

void Animation1::displayTimeNowMonth(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(display.scaleColorRGB565Custom(colorRGB565,
                                                     (1 - animationSpeed),
                                                     diffTimeStrings.month_1),
                      x + (1 + fontWidth) * offsetNum,
                      y - animationSpeed * fontHeight * diffTimeStrings.month_1,
                      timeNow.month_1);

  display.displayU8g2(display.scaleColorRGB565Custom(colorRGB565,
                                                     (1 - animationSpeed),
                                                     diffTimeStrings.month_2),
                      x + (1 + fontWidth) * (offsetNum + 1),
                      y - animationSpeed * fontHeight * diffTimeStrings.month_2,
                      timeNow.month_2);
}

void Animation1::displayTimeNowDay(
    float animationSpeed, TimeStrings timeNow, DiffTimeStrings diffTimeStrings,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.day_1),
      x + (1 + fontWidth) * offsetNum,
      y - animationSpeed * fontHeight * diffTimeStrings.day_1, timeNow.day_1);

  display.displayU8g2(
      display.scaleColorRGB565Custom(colorRGB565, (1 - animationSpeed),
                                     diffTimeStrings.day_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y - animationSpeed * fontHeight * diffTimeStrings.day_2, timeNow.day_2);
}

// Animation methods for next time (fading in)
void Animation1::displayTimeNextHour(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.hour24_1),
      x + (1 + fontWidth) * offsetNum,
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.hour24_1,
      timeNowNextSec.hour24_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.hour24_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.hour24_2,
      timeNowNextSec.hour24_2);
}

void Animation1::displayTimeNextMinute(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.minute_1),
      x + (1 + fontWidth) * offsetNum,
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.minute_1,
      timeNowNextSec.minute_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.minute_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.minute_2,
      timeNowNextSec.minute_2);
}

void Animation1::displayTimeNextSecond(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.second_1),
      x + (1 + fontWidth) * offsetNum,
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.second_1,
      timeNowNextSec.second_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.second_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.second_2,
      timeNowNextSec.second_2);
}

void Animation1::displayTimeNextYear(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_1),
      x + (1 + fontWidth) * offsetNum,
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.year_1,
      timeNowNextSec.year_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.year_2,
      timeNowNextSec.year_2);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_3),
      x + (1 + fontWidth) * (offsetNum + 2),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.year_3,
      timeNowNextSec.year_3);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.year_4),
      x + (1 + fontWidth) * (offsetNum + 3),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.year_4,
      timeNowNextSec.year_4);
}

void Animation1::displayTimeNextMonth(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.month_1),
      x + (1 + fontWidth) * offsetNum,
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.month_1,
      timeNowNextSec.month_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.month_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.month_2,
      timeNowNextSec.month_2);
}

void Animation1::displayTimeNextDay(
    float animationSpeed, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, uint16_t colorRGB565, int x, int y,
    int fontWidth, int fontHeight, int offsetNum, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.day_1),
      x + (1 + fontWidth) * offsetNum,
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.day_1,
      timeNowNextSec.day_1);

  display.displayU8g2(
      display.scaleColorRGB565Forward(colorRGB565, animationSpeed,
                                      diffTimeStrings.day_2),
      x + (1 + fontWidth) * (offsetNum + 1),
      y + (1 - animationSpeed) * fontHeight * diffTimeStrings.day_2,
      timeNowNextSec.day_2);
}
