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

  ts.mday = timeinfo.tm_mday;

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

    diff.mday = (ts1.mday != ts2.mday) ? 1 : 0;
		
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
void Display::setupDisplayContext(uint16_t colorRGB565, int x, int y,
                                  int fontWidth, int fontHeight,
                                  int separatorWidth, int offSetNumFont,
                                  int offSetNumSep, int offSetFont,
                                  int offSetSepX,int offSetSepY,const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(
      x + (offSetFont + fontWidth) * offSetNumFont +
          (offSetFont + separatorWidth) * offSetNumSep - offSetSepX,
      y - offSetSepY);
}

FontMetrics Display::getFontMetrics(const uint8_t *font, const char *character) {
  u8g2_for_adafruit_gfx.setFont(font);

  FontMetrics metrics;
  metrics.ascent = u8g2_for_adafruit_gfx.getFontAscent();
  metrics.descent = u8g2_for_adafruit_gfx.getFontDescent();
  metrics.height = metrics.ascent - metrics.descent;
  metrics.charWidth = u8g2_for_adafruit_gfx.getUTF8Width(character);

  return metrics;
}

// 使用示例：优化后的displayAnimationHourMinuteSecond方法
void Display::displayHourMinuteSecond(
    unsigned long elapsed, TimeStrings timeNow, TimeStrings timeNowNextSec,
    DiffTimeStrings diffTimeStrings, int16_t animationType,
    uint16_t colorRGB565, int x, int y, int fontWidth, int fontHeight,
    const uint8_t *fontName, const char* fontM, const char* sepM) {

  FontMetrics fontMetrics = getFontMetrics(fontName, fontM);
  FontMetrics sepMetrics = getFontMetrics(fontName, sepM);
  
  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods
    // displayStaticOneTemplate(timeNow.hour24_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
    // displayStaticOneTemplate(timeNow.hour24_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);

    displayStaticOneTemplate(timeNow.hour24, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);

    displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 0, 1, 0, 0, fontName);
    displayStaticOneTemplate(timeNow.minute_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName);
    displayStaticOneTemplate(timeNow.minute_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
    displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 1, 1, 0, 0, fontName);
    displayStaticOneTemplate(timeNow.second_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
    displayStaticOneTemplate(timeNow.second_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);

    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
      // Static display during non-animation periods
      displayStaticOneTemplate(timeNow.hour24_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.hour24_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.minute_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.minute_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.second_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.second_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

      // Current time fading out
      displayStaticOneTemplate( timeNow.hour24_1, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.hour24_1),
          x, y - animationSpeed * fontMetrics.height * diffTimeStrings.hour24_1, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.hour24_2, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.hour24_2),
          x, y - animationSpeed * fontMetrics.height * diffTimeStrings.hour24_2, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.minute_1, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.minute_1),
          x, y - animationSpeed * fontMetrics.height * diffTimeStrings.minute_1, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.minute_2, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.minute_2),
          x, y - animationSpeed * fontMetrics.height * diffTimeStrings.minute_2, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.second_1, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.second_1),
          x, y - animationSpeed * fontMetrics.height * diffTimeStrings.second_1, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.second_2, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.second_2),
          x, y - animationSpeed * fontMetrics.height * diffTimeStrings.second_2, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);

      // Static colons
      displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 1, 1, 0, 0, fontName);

      // Next time fading in

      displayStaticOneTemplate( timeNowNextSec.hour24_1, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.hour24_1),
        x, y + (1 - animationSpeed) * fontMetrics.height * diffTimeStrings.hour24_1, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.hour24_2, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.hour24_2),
        x, y + (1 - animationSpeed) * fontMetrics.height * diffTimeStrings.hour24_2, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.minute_1, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.minute_1), 
        x, y + (1 - animationSpeed) * fontMetrics.height * diffTimeStrings.minute_1, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName); 
      displayStaticOneTemplate( timeNowNextSec.minute_2, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.minute_2),
        x, y + (1 - animationSpeed) * fontMetrics.height * diffTimeStrings.minute_2, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.second_1, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.second_1),
        x, y + (1 - animationSpeed) * fontMetrics.height * diffTimeStrings.second_1, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.second_2, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.second_2),
        x, y + (1 - animationSpeed) * fontMetrics.height * diffTimeStrings.second_2, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);
    }
    break;
  case ANIMATION_TYPE_2:
    if (elapsed < 400) {
      animationSpeed = elapsed / 400.0;
      displayStaticOneTemplate( timeNow.hour24_1, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.hour24_1, 0),
          x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.hour24_2, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.hour24_2, 0),
          x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.minute_1, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.minute_1, 0),
          x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.minute_2, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.minute_2, 0),
          x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.second_1, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.second_1, 0),
          x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNow.second_2, scaleColorRGB565Custom(colorRGB565, animationSpeed, diffTimeStrings.second_2, 0),
          x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);

    } else if (elapsed < 800) {
      // Animation display using new methods
      animationSpeed = (elapsed - 400) / (800.0 - 400.0);

      displayStaticOneTemplate( timeNowNextSec.hour24_1, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.hour24_1, 0.1),
        x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.hour24_2, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.hour24_2, 0.1),
        x, y , fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.minute_1, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.minute_1, 0.1), 
        x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName); 
      displayStaticOneTemplate( timeNowNextSec.minute_2, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.minute_2, 0.1),
        x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.second_1, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.second_1, 0.1),
        x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate( timeNowNextSec.second_2, scaleColorRGB565Forward(colorRGB565, animationSpeed, diffTimeStrings.second_2, 0.10),
        x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);

    } else if (elapsed >= 1000) {
      displayStaticOneTemplate(timeNow.hour24_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.hour24_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.minute_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.minute_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.second_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNow.second_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);

    } else {
      displayStaticOneTemplate(timeNowNextSec.hour24_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 0, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNowNextSec.hour24_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 1, 0, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNowNextSec.minute_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNowNextSec.minute_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 3, 1, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNowNextSec.second_1, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 2, 1, 0, 0, fontName);
      displayStaticOneTemplate(timeNowNextSec.second_2, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 5, 2, 1, 0, 0, fontName);
    }
    displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 2, 0, 1, 0, 0, fontName);
    displayStaticOneTemplate(sepM, colorRGB565, x, y, fontMetrics.charWidth, fontMetrics.height, sepMetrics.charWidth, 4, 1, 1, 0, 0, fontName);
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

  FontMetrics fontMetrics = getFontMetrics(fontName, "2");
  FontMetrics sepMetrics = getFontMetrics(fontName, ":");
  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods

   
    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
     
    } else {
      
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
  FontMetrics fontMetrics = getFontMetrics(fontName, "2");
  FontMetrics sepMetrics = getFontMetrics(fontName, ":");

  switch (animationType) {
  case ANIMATION_TYPE_0:
    
    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

      // Current time fading out
     

      // Static colons
      

      // Next time fading in
     
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

  FontMetrics fontMetrics = getFontMetrics(fontName, "2");
  FontMetrics sepMetrics = getFontMetrics(fontName, ":");
  switch (animationType) {
  case ANIMATION_TYPE_0:
    // Static display using new methods

    break;

  case ANIMATION_TYPE_1:
    if (elapsed < 699 || elapsed > 999) {
      // Static display during non-animation periods
      
    } else {
      // Animation display using new methods
      animationSpeed = (elapsed - 699) / 300.0;

     

      // Static colons
    
      
   
    }
    break;

  default:
    break;
  }
}