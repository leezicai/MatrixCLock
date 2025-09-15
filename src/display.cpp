#include "display.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

// 定义全局唯一实例
Display display;

Display::Display() {}

uint16_t Display::scaleColorRGB565Custom(uint16_t colorRGB565,
                                         float animationSpeed,
                                         bool colorFlag ,
                                         float minBrightness) {
  // If colorFlag is false, return original color (100% brightness)
  if (!colorFlag) {
    return colorRGB565;
  }

  // Clamp animationSpeed to valid range (0.0 to 1.0)
  if (animationSpeed < 0.0f)
    animationSpeed = 0.0f;
  if (animationSpeed > 1.0f)
    animationSpeed = 1.0f;
  // Clamp minBrightness to valid range
  if (minBrightness < 0.0f)
    minBrightness = 0.0f;
  if (minBrightness > 1.0f)
    minBrightness = 1.0f;
  // Calculate fade factor:
  // animationSpeed = 0 -> fadeFactor = 1.0 (100% brightness)
  // animationSpeed = 1 -> fadeFactor = minBrightness (configurable minimum)
  float fadeFactor = 1.0f - (animationSpeed * (1.0f - minBrightness));
  // Extract and scale RGB components
  uint8_t r = (colorRGB565 >> 11) & 0x1F;
  uint8_t g = (colorRGB565 >> 5) & 0x3F;
  uint8_t b = colorRGB565 & 0x1F;
  r = (uint8_t)(r * fadeFactor);
  g = (uint8_t)(g * fadeFactor);
  b = (uint8_t)(b * fadeFactor);
  return (r << 11) | (g << 5) | b;
}

uint16_t Display::scaleColorRGB565Forward(uint16_t colorRGB565,
                                          float animationSpeed,
                                          bool colorFlag,
                                          float minBrightness ) {
  // If colorFlag is false, return original color (100% brightness)
  if (!colorFlag) {
    return colorRGB565;
  }

  // Clamp animationSpeed to valid range (0.0 to 1.0)
  if (animationSpeed < 0.0f)
    animationSpeed = 0.0f;
  if (animationSpeed > 1.0f)
    animationSpeed = 1.0f;
  // Clamp minBrightness to valid range
  if (minBrightness < 0.0f)
    minBrightness = 0.0f;
  if (minBrightness > 1.0f)
    minBrightness = 1.0f;
  // Calculate fade factor:
  // animationSpeed = 0 -> fadeFactor = minBrightness (60% brightness)
  // animationSpeed = 1 -> fadeFactor = 1.0 (100% brightness)
  float fadeFactor = minBrightness + (animationSpeed * (1.0f - minBrightness));
  // Extract and scale RGB components
  uint8_t r = (colorRGB565 >> 11) & 0x1F; // Extract 5-bit red (0-31)
  uint8_t g = (colorRGB565 >> 5) & 0x3F;  // Extract 6-bit green (0-63)
  uint8_t b = colorRGB565 & 0x1F;         // Extract 5-bit blue (0-31)
  r = (uint8_t)(r * fadeFactor);
  g = (uint8_t)(g * fadeFactor);
  b = (uint8_t)(b * fadeFactor);
  return (r << 11) | (g << 5) | b;
}

// RGB to RGB565
uint16_t Display::rgbToRgb565(uint8_t red, uint8_t green, uint8_t blue) {
	// Red: 8 bits -> 5 bits (keep upper 5 bits)
	// Green: 8 bits -> 6 bits (keep upper 6 bits)
	// Blue: 8 bits -> 5 bits (keep upper 5 bits)
	return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

// 显示文字（RGB565）
void Display::displayText(uint16_t colorRGB565, int x, int y,
													const uint8_t *fontName, const char *text) {
	u8g2_for_adafruit_gfx.setFont(fontName);
	u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
	u8g2_for_adafruit_gfx.setCursor(x, y);
	u8g2_for_adafruit_gfx.print(text);
}
void Display::displayText(uint16_t colorRGB565, int x, int y,
													const uint8_t *fontName, const int16_t time_int) {
	u8g2_for_adafruit_gfx.setFont(fontName);
	u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
	u8g2_for_adafruit_gfx.setCursor(x, y);
	u8g2_for_adafruit_gfx.print(time_int);
}

// 显示文字（RGB888）
void Display::displayTextRGB(uint8_t red, uint8_t green, uint8_t blue, int x,
															int y, const uint8_t *fontName, const char *text) {
	uint16_t colorRGB565 = rgbToRgb565(red, green, blue);

	displayText(colorRGB565, x, y, fontName, text);
}

void Display::displayTextRGB(uint8_t red, uint8_t green, uint8_t blue, int x,
															int y, const uint8_t *fontName,
															const int16_t time_int) {
	uint16_t colorRGB565 = rgbToRgb565(red, green, blue);

	displayText(colorRGB565, x, y, fontName, time_int);
}

// 获取当前时间并转换成字符串结构体
TimeStrings Display::getTimeStrings(time_t now) {
	// Get system time
	struct tm timeinfo;
	localtime_r(&now, &timeinfo);

	TimeStrings ts;

	// Year processing
	ts.year = timeinfo.tm_year + 1900;
	ts.year_1 = ts.year / 1000;       // 2025 -> 2
	ts.year_2 = (ts.year / 100) % 10; // 2025 -> 0
	ts.year_3 = (ts.year / 10) % 10;  // 2025 -> 2
	ts.year_4 = ts.year % 10;         // 2025 -> 5

	// Month processing
	ts.month = timeinfo.tm_mon + 1;
	ts.month_1 = ts.month / 10; // 09 -> 0
	ts.month_2 = ts.month % 10; // 09 -> 9

	// Day processing
	ts.day = timeinfo.tm_mday;
	ts.day_1 = ts.day / 10; // 14 -> 1
	ts.day_2 = ts.day % 10; // 14 -> 4

	// Hour 24 processing
	ts.hour24 = timeinfo.tm_hour;
	ts.hour24_1 = ts.hour24 / 10; // 23 -> 2
	ts.hour24_2 = ts.hour24 % 10; // 23 -> 3

	// Minute processing
	ts.minute = timeinfo.tm_min;
	ts.minute_1 = ts.minute / 10; // 59 -> 5
	ts.minute_2 = ts.minute % 10; // 59 -> 9

	// Second processing
	ts.second = timeinfo.tm_sec;
	ts.second_1 = ts.second / 10; // 58 -> 5
	ts.second_2 = ts.second % 10; // 58 -> 8

	// Hour 12 processing
	int h12 = timeinfo.tm_hour % 12;
	if (h12 == 0)
		h12 = 12;
	ts.hour12 = h12;
	ts.hour12_1 = ts.hour12 / 10; // 11 -> 1
	ts.hour12_2 = ts.hour12 % 10; // 11 -> 1

	// AM/PM processing
	ts.ampm = (timeinfo.tm_hour < 12) ? "AM" : "PM";
	ts.ampm_1 = ts.ampm.substr(0, 1); // "A" or "P"
	ts.ampm_2 = ts.ampm.substr(1, 1); // "M"

	return ts;
}

DiffTimeStrings Display::compareTimeStrings(const TimeStrings& ts1, const TimeStrings& ts2) {
		DiffTimeStrings diff;
		
		// Compare all integer fields
		diff.year = (ts1.year != ts2.year) ? 1 : 0;
		diff.year_1 = (ts1.year_1 != ts2.year_1) ? 0 : 0;
		diff.year_2 = (ts1.year_2 != ts2.year_2) ? 1 : 0;
		diff.year_3 = (ts1.year_3 != ts2.year_3) ? 1 : 0;
		diff.year_4 = (ts1.year_4 != ts2.year_4) ? 1 : 0;
		
		diff.month = (ts1.month != ts2.month) ? 1 : 0;
		diff.month_1 = (ts1.month_1 != ts2.month_1) ? 1 : 0;
		diff.month_2 = (ts1.month_2 != ts2.month_2) ? 1 : 0;
		
		diff.day = (ts1.day != ts2.day) ? 1 : 0;
		diff.day_1 = (ts1.day_1 != ts2.day_1) ? 1 : 0;
		diff.day_2 = (ts1.day_2 != ts2.day_2) ? 1 : 0;
		
		diff.hour24 = (ts1.hour24 != ts2.hour24) ? 1 : 0;
		diff.hour24_1 = (ts1.hour24_1 != ts2.hour24_1) ? 1 : 0;
		diff.hour24_2 = (ts1.hour24_2 != ts2.hour24_2) ? 1 : 0;
		
		diff.minute = (ts1.minute != ts2.minute) ? 1 : 0;
		diff.minute_1 = (ts1.minute_1 != ts2.minute_1) ? 1 : 0;
		diff.minute_2 = (ts1.minute_2 != ts2.minute_2) ? 1 : 0;
		
		diff.second = (ts1.second == ts2.second) ? 1 : 0;
		diff.second_1 = (ts1.second_1 != ts2.second_1) ? 1 : 0;
		diff.second_2 = (ts1.second_2 != ts2.second_2) ? 1 : 0;
		
		diff.hour12 = (ts1.hour12 != ts2.hour12) ? 1 : 0;
		diff.hour12_1 = (ts1.hour12_1 != ts2.hour12_1) ? 1 : 0;
		diff.hour12_2 = (ts1.hour12_2 != ts2.hour12_2) ? 1 : 0;
		
		// Compare string fields
		diff.ampm = (ts1.ampm != ts2.ampm) ? 1 : 0;
		diff.ampm_1 = (ts1.ampm_1 != ts2.ampm_1) ? 1 : 0;
		diff.ampm_2 = (ts1.ampm_2 != ts2.ampm_2) ? 1 : 0;
		
		return diff;
}

void Display::flipDMABuffer() { dma_display->flipDMABuffer(); }

void Display::clearScreen() { dma_display->clearScreen(); }

void Display::displayU8g2(uint16_t colorRGB565, int x, int y, int printInt) {
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(x, y);
  u8g2_for_adafruit_gfx.print(printInt);
}
void Display::displayU8g2(uint16_t colorRGB565, int x, int y, String printInt) {
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(x, y);
  u8g2_for_adafruit_gfx.print(printInt);
}
void Display::displayU8g2(uint16_t colorRGB565, int x, int y,
                          const uint8_t *fontName, int printInt) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(x, y);
  u8g2_for_adafruit_gfx.print(printInt);
}
void Display::displayU8g2(uint16_t colorRGB565, int x, int y,
                          const uint8_t *fontName, String printInt) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(x, y);
  u8g2_for_adafruit_gfx.print(printInt);
}

// Static display methods for different time components
void Display::displayStaticHour(TimeStrings timeNow,
                                DiffTimeStrings diffTimeStrings,
                                uint16_t colorRGB565, int x, int y,
                                int fontWidth, int fontHeight, int offsetNum,
                                const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);

  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y,
              timeNow.hour24_1);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 1), y,
              timeNow.hour24_2);
}

void Display::displayStaticMinute(TimeStrings timeNow,
                                  DiffTimeStrings diffTimeStrings,
                                  uint16_t colorRGB565, int x, int y,
                                  int fontWidth, int fontHeight, int offsetNum,
                                  const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);

  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y,
              timeNow.minute_1);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 1), y,
              timeNow.minute_2);
}

void Display::displayStaticSecond(TimeStrings timeNow,
                                  DiffTimeStrings diffTimeStrings,
                                  uint16_t colorRGB565, int x, int y,
                                  int fontWidth, int fontHeight, int offsetNum,
                                  const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);

  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y,
              timeNow.second_1);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 1), y,
              timeNow.second_2);
}

void Display::displayStaticYear(TimeStrings timeNow,
                                DiffTimeStrings diffTimeStrings,
                                uint16_t colorRGB565, int x, int y,
                                int fontWidth, int fontHeight, int offsetNum,
                                const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);

  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y, timeNow.year_1);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 1), y,
              timeNow.year_2);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 2), y,
              timeNow.year_3);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 3), y,
              timeNow.year_4);
}

void Display::displayStaticMonth(TimeStrings timeNow,
                                 DiffTimeStrings diffTimeStrings,
                                 uint16_t colorRGB565, int x, int y,
                                 int fontWidth, int fontHeight, int offsetNum,
                                 const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);

  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y, timeNow.month_1);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 1), y,
              timeNow.month_2);
}

void Display::displayStaticDay(TimeStrings timeNow,
                               DiffTimeStrings diffTimeStrings,
                               uint16_t colorRGB565, int x, int y,
                               int fontWidth, int fontHeight, int offsetNum,
                               const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);

  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y, timeNow.day_1);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * (offsetNum + 1), y,
              timeNow.day_2);
}
void Display::displaySeparator(const char *separator, uint16_t colorRGB565,
                               int x, int y, int offsetNum, int fontWidth,
                               const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  displayU8g2(colorRGB565, x + (1 + fontWidth) * offsetNum, y, separator);
}


// 使用示例：优化后的displayAnimationHourMinuteSecond方法
void Display::displayHourMinuteSecond(
    unsigned long elapsed, TimeStrings timeNow, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, int16_t animationType,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    const uint8_t *fontName) {

  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods
    displayStaticHour(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                      fontHeight, 0, fontName);
    displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
    displayStaticMinute(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 3, fontName);
    displaySeparator(":", colorRGB565, x, y, 5, fontWidth, fontName);

    displayStaticSecond(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 6, fontName);
    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
      // Static display during non-animation periods
      displayStaticHour(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 0, fontName);
      displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
      displayStaticMinute(timeNow, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 3, fontName);
      displaySeparator(":", colorRGB565, x, y, 5, fontWidth, fontName);
      displayStaticSecond(timeNow, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 6, fontName);
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

      // Current time fading out
      animation1.displayTimeNowHour(animationSpeed, timeNow, diffTimeStrings,
                                  colorRGB565, x, y, fontWidth, fontHeight, 0,
                                  fontName);
      animation1.displayTimeNowMinute(animationSpeed, timeNow, diffTimeStrings,
                                      colorRGB565, x, y, fontWidth, fontHeight,
                                      3, fontName);
      animation1.displayTimeNowSecond(animationSpeed, timeNow, diffTimeStrings,
                                      colorRGB565, x, y, fontWidth, fontHeight,
                                      6, fontName);

      // Static colons
      displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
      displaySeparator(":", colorRGB565, x, y, 5, fontWidth, fontName);

      // Next time fading in
      animation1.displayTimeNextHour(animationSpeed, timeNowNextSec,
                                     diffTimeStrings, colorRGB565, x, y,
                                     fontWidth, fontHeight, 0, fontName);
      animation1.displayTimeNextMinute(animationSpeed, timeNowNextSec,
                                       diffTimeStrings, colorRGB565, x, y,
                                       fontWidth, fontHeight, 3, fontName);
      animation1.displayTimeNextSecond(animationSpeed, timeNowNextSec,
                                       diffTimeStrings, colorRGB565, x, y,
                                       fontWidth, fontHeight, 6, fontName);
    }
    break;
  case ANIMATION_TYPE_2:
    if (elapsed < 400 ) {
      animationSpeed = elapsed / 400.0;
      Serial.print("animationSpeed fade out: ");
      Serial.print(animationSpeed);
      Serial.print(": ");
      Serial.print(elapsed);
      Serial.print(": ");
      Serial.print(timeNow.second_2);
      Serial.println("-----------");
      animation2.displayTimeNowHour(1 - animationSpeed, timeNow,
                                    diffTimeStrings, colorRGB565, x, y,
                                    fontWidth, fontHeight, 0, fontName);
      animation2.displayTimeNowMinute(1 - animationSpeed, timeNow,
                                      diffTimeStrings, colorRGB565, x, y,
                                      fontWidth, fontHeight, 3, fontName);
      animation2.displayTimeNowSecond(1 - animationSpeed, timeNow,
                                      diffTimeStrings, colorRGB565, x, y,
                                      fontWidth, fontHeight, 6, fontName);
      // Static display during non-animation periods
      
    } else if(elapsed < 800){
      // Animation display using new methods
      animationSpeed = (elapsed - 400) / (800.0 - 400.0);
      Serial.print("animationSpeed: fade in: ");
      Serial.print(animationSpeed);
      Serial.print(": ");
      Serial.print(elapsed);
      Serial.print(": ");
      Serial.print(timeNowNextSec.second_2);
      Serial.println("-----------");

      animation2.displayTimeNextHour(animationSpeed, timeNowNextSec,
                                     diffTimeStrings, colorRGB565, x, y,
                                     fontWidth, fontHeight, 0, fontName);
      animation2.displayTimeNextMinute(animationSpeed, timeNowNextSec,
                                       diffTimeStrings, colorRGB565, x, y,
                                       fontWidth, fontHeight, 3, fontName);
      animation2.displayTimeNextSecond(animationSpeed, timeNowNextSec,
                                       diffTimeStrings, colorRGB565, x, y,
                                       fontWidth, fontHeight, 6, fontName);
      // Static colons

    } else if (elapsed >= 1000) {

      displayStaticHour(timeNow, diffTimeStrings, colorRGB565, x, y,
                        fontWidth, fontHeight, 0, fontName);
      displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
      displayStaticMinute(timeNow, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 3, fontName);
      displaySeparator(":", colorRGB565, x, y, 5, fontWidth, fontName);
      displayStaticSecond(timeNow, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 6, fontName);
    } else {
      displayStaticHour(timeNowNextSec, diffTimeStrings, colorRGB565, x, y,
                        fontWidth, fontHeight, 0, fontName);
      displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
      displayStaticMinute(timeNowNextSec, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 3, fontName);
      displaySeparator(":", colorRGB565, x, y, 5, fontWidth, fontName);
      displayStaticSecond(timeNowNextSec, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 6, fontName);
    }

      displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
      displaySeparator(":", colorRGB565, x, y, 5, fontWidth, fontName);
      break;

  default:
    break;
  }
}

void Display::displayHourMinute(
    unsigned long elapsed, TimeStrings timeNow, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, int16_t animationType,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    const uint8_t *fontName) {

  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods
    displayStaticHour(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                      fontHeight, 0, fontName);
    displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
    displayStaticMinute(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 3, fontName);
    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
      // Static display during non-animation periods
      displayStaticHour(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 0, fontName);
      displaySeparator(":", colorRGB565, x, y, 2, fontWidth, fontName);
      displayStaticMinute(timeNow, diffTimeStrings, colorRGB565, x, y,
                          fontWidth, fontHeight, 3, fontName);
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

      // Current time fading out
      animation1.displayTimeNowHour(animationSpeed, timeNow, diffTimeStrings,
                                    colorRGB565, x, y, fontWidth, fontHeight, 0,
                                    fontName);
      animation1.displayTimeNowMinute(animationSpeed, timeNow, diffTimeStrings,
                                      colorRGB565, x, y, fontWidth, fontHeight,
                                      3, fontName);

      // Static colons
      displaySeparator(":", colorRGB565, x, y, 2, fontWidth,
                                  fontName);

      // Next time fading in
      animation1.displayTimeNextHour(animationSpeed, timeNowNextSec,
                                     diffTimeStrings, colorRGB565, x, y,
                                     fontWidth, fontHeight, 0, fontName);
      animation1.displayTimeNextMinute(animationSpeed, timeNowNextSec,
                                       diffTimeStrings, colorRGB565, x, y,
                                       fontWidth, fontHeight, 3, fontName);
    }
    break;

  default:
    break;
  }
}

void Display::displayYearMonthDay(
    unsigned long elapsed, TimeStrings timeNow, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, int16_t animationType,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    const uint8_t *fontName) {

  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods
    displayStaticYear(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                      fontHeight, 0, fontName);
    displaySeparator("-", colorRGB565, x, y, 4, fontWidth, fontName);
    displayStaticMonth(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 5, fontName);
    displaySeparator("-", colorRGB565, x, y, 7, fontWidth, fontName);
    displayStaticDay(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                       fontHeight, 8, fontName);
    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
      // Static display during non-animation periods
      displayStaticYear(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                        fontHeight, 0, fontName);
      displaySeparator("-", colorRGB565, x, y, 4, fontWidth, fontName);
      displayStaticMonth(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                         fontHeight, 5, fontName);
      displaySeparator("-", colorRGB565, x, y, 7, fontWidth, fontName);
      displayStaticDay(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                       fontHeight, 8, fontName);
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

      // Current time fading out
      animation1.displayTimeNowYear(animationSpeed, timeNow, diffTimeStrings,
                                    colorRGB565, x, y, fontWidth, fontHeight, 0,
                                    fontName);
      animation1.displayTimeNowMonth(animationSpeed, timeNow, diffTimeStrings,
                                     colorRGB565, x, y, fontWidth, fontHeight,
                                     5, fontName);
      animation1.displayTimeNowDay(animationSpeed, timeNow, diffTimeStrings,
                                   colorRGB565, x, y, fontWidth, fontHeight, 8,
                                   fontName);

      // Static colons
      displaySeparator("-", colorRGB565, x, y, 4, fontWidth, fontName);
      displaySeparator("-", colorRGB565, x, y, 7, fontWidth, fontName);

      // Next time fading in
      animation1.displayTimeNextYear(animationSpeed, timeNowNextSec,

                                     diffTimeStrings, colorRGB565, x, y,
                                     fontWidth, fontHeight, 0, fontName);
      animation1.displayTimeNextMonth(animationSpeed, timeNowNextSec,
                                      diffTimeStrings, colorRGB565, x, y,
                                      fontWidth, fontHeight, 5, fontName);
      animation1.displayTimeNextDay(animationSpeed, timeNowNextSec,
                                    diffTimeStrings, colorRGB565, x, y,
                                    fontWidth, fontHeight, 8, fontName);
    }
    break;

  default:
    break;
  }
}

void Display::displayMonthDay(
    unsigned long elapsed, TimeStrings timeNow, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, int16_t animationType,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    const uint8_t *fontName) {

  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods

    displayStaticMonth(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                       fontHeight, 0, fontName);
    displaySeparator("-", colorRGB565, x, y, 2, fontWidth, fontName);
    displayStaticDay(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                     fontHeight, 3, fontName);
    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
      // Static display during non-animation periods
      displayStaticMonth(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                         fontHeight, 0, fontName);
      displaySeparator("-", colorRGB565, x, y, 2, fontWidth, fontName);
      displayStaticDay(timeNow, diffTimeStrings, colorRGB565, x, y, fontWidth,
                       fontHeight, 3, fontName);
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

      animation1.displayTimeNowMonth(animationSpeed, timeNow, diffTimeStrings,
                                     colorRGB565, x, y, fontWidth, fontHeight,
                                     0, fontName);
      animation1.displayTimeNowDay(animationSpeed, timeNow, diffTimeStrings,
                                   colorRGB565, x, y, fontWidth, fontHeight, 3,
                                   fontName);

      // Static colons
      displaySeparator("-", colorRGB565, x, y, 2, fontWidth, fontName);

      // Next time fading in
      animation1.displayTimeNextMonth(animationSpeed, timeNowNextSec,
                                      diffTimeStrings, colorRGB565, x, y,
                                      fontWidth, fontHeight, 0, fontName);
      animation1.displayTimeNextDay(animationSpeed, timeNowNextSec,
                                    diffTimeStrings, colorRGB565, x, y,
                                    fontWidth, fontHeight, 3, fontName);
    }
    break;

  default:
    break;
  }
}