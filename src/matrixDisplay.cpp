#include "matrixDisplay.h"
#include "stock.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

// Default constructor - initialize all counts to zero
CharCount::CharCount() {
  countNum = 0;
  countSpace = 0;
  countABC = 0;
  countHyphen = 0;
}

// Parameterized constructor
CharCount::CharCount(int16_t num, int16_t space, int16_t abc, int16_t hyphen) {
  countNum = num;
  countSpace = space;
  countABC = abc;
  countHyphen = hyphen;
}

// Reset all counts to zero
void CharCount::reset() {
  countNum = 0;
  countSpace = 0;
  countABC = 0;
  countHyphen = 0;
}

// Initialize with specific values
void CharCount::initialize(int16_t num, int16_t space, int16_t abc,
                           int16_t hyphen) {
  countNum = num;
  countSpace = space;
  countABC = abc;
  countHyphen = hyphen;
}

int Display::charType[256];
bool Display::tableInitialized = false;

// Initialize lookup table
void Display::initializeTable() {
  if (tableInitialized)
    return;

  // Initialize all to default type (abc)
  for (int i = 0; i < 256; i++) {
    charType[i] = 3; // default: abc
  }

  // Set specific types
  for (int i = '0'; i <= '9'; i++) {
    charType[i] = 0; // digit
  }

  charType['-'] = 1;
  charType['+'] = 1;
  charType['/'] = 1; // hyphen
  charType['.'] = 2;
  charType[','] = 2; // space
  charType[':'] = 2;
  charType[' '] = 2;

  tableInitialized = true;
}
// 定义全局唯一实例
Display display;

Display::Display() { initializeTable(); }

uint16_t Display::scaleColorRGB565Custom(uint16_t colorRGB565,
                                         float animationSpeed, bool colorFlag,
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
                                          float animationSpeed, bool colorFlag,
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
                          const uint8_t *fontName, int16_t intP) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(x, y);
  u8g2_for_adafruit_gfx.print(intP);
}


// 显示文字（RGB888）
void Display::displayTextRGB(uint8_t red, uint8_t green, uint8_t blue, int x,
                             int y, const uint8_t *fontName, const char *text) {
  uint16_t colorRGB565 = rgbToRgb565(red, green, blue);

  displayText(colorRGB565, x, y, fontName, text);
}

void Display::displayTextRGB(uint8_t red, uint8_t green, uint8_t blue, int x,
                             int y, const uint8_t *fontName, int16_t pageInfo) {
  uint16_t colorRGB565 = rgbToRgb565(red, green, blue);

  displayText(colorRGB565, x, y, fontName, pageInfo);
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

DiffTimeStrings Display::compareTimeStrings(const TimeStrings &ts1,
                                            const TimeStrings &ts2) {
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


// Static display methods for different time components
void Display::setupDisplayContext(uint16_t colorRGB565, int x, int y,
                                  int fontWidth, int fontHeight,
                                  int separatorWidth, int offSetNumFont,
                                  int offSetNumSep, int offSetFont,
                                  int offSetSepX, int offSetSepY,
                                  const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(
      x + (offSetFont + fontWidth) * offSetNumFont +
          (offSetFont + separatorWidth) * offSetNumSep - offSetSepX,
      y - offSetSepY);
}
void Display::setupDisplayContext(uint16_t colorRGB565, int x, int y,
                                  int fontWidth, int fontHeight,
                                  int separatorWidth, int offSetNumFont,
                                  int offSetNumSep, int offSetFont,
                                  float offSetSepX, float offSetSepY,
                                  const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(x + (offSetFont + fontWidth) * offSetNumFont +
                                      (offSetFont + separatorWidth) *
                                          offSetNumSep -
                                      offSetSepX * fontHeight,
                                  y - offSetSepY * fontHeight);
}

void Display::setupDisplayContext(
    uint16_t colorRGB565, int16_t x, int16_t y, int16_t fontWidth,
    int16_t fontHeight, int16_t numberWidth, int16_t spaceWidth,
    int16_t hyphenWidth, int16_t offsetFontCountABC, int16_t offsetCountNum,
    int16_t offsetCountSpace, int16_t offsetCountHyphen, int16_t offsetPreFont,
    int16_t offsetSpaceX, int16_t offsetSpaceY, const uint8_t *fontName) {
  u8g2_for_adafruit_gfx.setFont(fontName);
  u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
  u8g2_for_adafruit_gfx.setCursor(
      x + (offsetPreFont + fontWidth) * offsetFontCountABC +
          (offsetPreFont + numberWidth) * offsetCountNum +
          (offsetPreFont + spaceWidth) * offsetCountSpace +
          (offsetPreFont + hyphenWidth) * offsetCountHyphen + offsetSpaceX,
      y + offsetSpaceY);
}

FontMetrics Display::getFontMetrics(const uint8_t *font,
                                    const char *character) {
  u8g2_for_adafruit_gfx.setFont(font);

  FontMetrics metrics;
  metrics.ascent = u8g2_for_adafruit_gfx.getFontAscent();
  metrics.descent = u8g2_for_adafruit_gfx.getFontDescent();
  metrics.height = metrics.ascent - metrics.descent;

  char firstChar[2] = {character[0], '\0'}; // 提取第一个字符并添加结束符
  metrics.charWidth = u8g2_for_adafruit_gfx.getUTF8Width(firstChar);

  return metrics;
}
int16_t Display::getStrWidth(int16_t fontWidth, int16_t sepWidth,
                             const char *str) {
  int16_t totalWidth = 0;

  // Check if string pointer is null
  if (str == NULL) {
    return 0;
  }

  // Iterate through each character in the string
  while (*str != '\0') {
    if (*str == ':') {
      // If character is ':', add separator width
      totalWidth += sepWidth;
    } else {
      // For all other characters, add font width
      totalWidth += fontWidth;
    }
    str++; // Move to next character
  }

  return totalWidth;
}

CharCount Display::analyzeCharInStr(const char *str) {
  CharCount result = {0, 0, 0, 0};

  // Check if string pointer is null
  if (str == NULL) {
    return result;
  }

  // Iterate through each character in the string
  for (int i = 0; str[i] != '\0'; i++) {
    switch (charType[(unsigned char)str[i]]) {
    case 0:
      result.countNum++;
      break;
    case 1:
      result.countHyphen++;
      break;
    case 2:
      result.countSpace++;
      break;
    case 3:
      result.countABC++;
      break;
    }
  }

  return result;
}

std::vector<int8_t> Display::compare_with_vector(const char *str1,
                                                 const char *str2) {
  size_t length = strlen(str1);        // Get length of first string
  std::vector<int8_t> results(length); // Initialize with calculated size
  for (size_t i = 0; i < length; ++i) {
    results[i] =
        (str1[i] != str2[i]) ? 1 : 0; // Return 1 if different, 0 if same
  }
  return results;
}

// ---- Per-pixel transitions (ANIMATION_3 / 4 / 5) 逐像素特效 ----

// Small integer avalanche hash. Gives every glyph pixel a value that stays
// the same for the whole transition, which is what makes the dissolve read as
// a dissolve instead of flickering static.
// 逐像素稳定哈希：同一个点在整段动画里取值不变，否则噪点每帧重洗就成雪花了。
static inline uint32_t glyphHash32(uint32_t v) {
  v ^= v >> 16;
  v *= 0x7feb352dU;
  v ^= v >> 15;
  v *= 0x846ca68bU;
  v ^= v >> 16;
  return v;
}

float Display::animProgress(unsigned long elapsed, uint16_t startMs,
                            uint16_t endMs) {
  if (endMs <= startMs) {
    return 0.0f;
  }
  if (elapsed <= startMs) {
    return 0.0f;
  }
  if (elapsed >= endMs) {
    return 1.0f;
  }
  return (float)(elapsed - startMs) / (float)(endMs - startMs);
}

float Display::pixelAnimProgress(int16_t animationType, unsigned long elapsed) {
  switch (animationType) {
  case ANIMATION_3:
    return animProgress(elapsed, SCAN_START_MS, SCAN_END_MS);
  case ANIMATION_4:
    return animProgress(elapsed, DISSOLVE_START_MS, DISSOLVE_END_MS);
  case ANIMATION_5:
    return animProgress(elapsed, PARTICLE_START_MS, PARTICLE_END_MS);
  default:
    return 0.0f;
  }
}

bool Display::isAsciiString(const char *str) {
  if (str == NULL) {
    return false;
  }
  for (int i = 0; str[i] != '\0'; i++) {
    if ((unsigned char)str[i] >= 0x80) {
      return false;
    }
  }
  return true;
}

// Blend a colour toward white. Used for the highlights that sell the beam and
// the dissolve front - both need to read brighter than the base colour, which
// scaleColorRGB565Forward() cannot do since it only ever darkens.
// 向白色混合。扫描线和溶解锋面都要比底色更亮，而现有的两个 scale 函数只会变暗。
uint16_t Display::mixWhiteRGB565(uint16_t colorRGB565, float f) {
  if (f < 0.0f) {
    f = 0.0f;
  }
  if (f > 1.0f) {
    f = 1.0f;
  }
  uint8_t r = (colorRGB565 >> 11) & 0x1F;
  uint8_t g = (colorRGB565 >> 5) & 0x3F;
  uint8_t b = colorRGB565 & 0x1F;
  r = (uint8_t)(r + (0x1F - r) * f);
  g = (uint8_t)(g + (0x3F - g) * f);
  b = (uint8_t)(b + (0x1F - b) * f);
  return (uint16_t)((r << 11) | (g << 5) | b);
}

// ANIMATION_3 - CRT scanline wipe.
// A beam sweeps top to bottom: everything above it is already the new glyph,
// everything below is still the old one. The rows just under the beam are
// sheared sideways by a pixel, the way a CRT tears when the sync slips.
// 扫描线：光束自上而下扫过，上方已是新字、下方仍是旧字，光束下方几行做 1px 撕裂。
void Display::drawScanChar(float t, uint16_t colorRGB565, int16_t charX,
                           int16_t topY) {
  const int16_t w =
      glyphOld.width() > glyphNew.width() ? glyphOld.width() : glyphNew.width();
  const int16_t h = glyphOld.height();

  // Run the beam slightly past the bottom row so the last row is fully swept.
  const int16_t scanY = (int16_t)(t * (float)(h + SCAN_TEAR_ROWS));

  for (int16_t gy = 0; gy < h; ++gy) {
    const GlyphBitmap &src = (gy < scanY) ? glyphNew : glyphOld;

    int16_t tear = 0;
    const int16_t below = gy - scanY;
    if (SCAN_TEAR_ROWS > 0 && below >= 0 && below < SCAN_TEAR_ROWS) {
      tear = (gy & 1) ? 1 : -1;
    }

    for (int16_t gx = 0; gx < w; ++gx) {
      if (!src.pixelAt(gx, gy)) {
        continue;
      }
      // Keep the shear inside the glyph box. Without this clamp the outermost
      // column of a torn row lands one pixel outside the character and reads
      // as a stray dot next to the digit rather than as a tear.
      // 错位必须限制在字框内，否则最外侧一列会甩到数字外面，看起来是杂点而不是撕裂。
      const int16_t dx = gx + tear;
      if (dx < 0 || dx >= w) {
        continue;
      }
      dma_display->drawPixel(charX + dx, topY + gy, colorRGB565);
    }
  }

  // The beam itself, drawn across the whole glyph box.
  if (scanY >= 0 && scanY < h) {
    dma_display->drawFastHLine(charX, topY + scanY, w,
                               mixWhiteRGB565(colorRGB565, SCAN_LINE_MIX));
  }
}

// ANIMATION_4 - dissolve / noise.
// Every pixel gets a fixed random threshold. The old glyph keeps the pixels
// whose threshold is still ahead of the progress, the new glyph takes the ones
// it has already passed, and the narrow band around the threshold is drawn
// brighter so a sparkling front sweeps through the glyph.
// 溶解：每点一个固定随机阈值，进度扫过阈值就从旧字换成新字，阈值附近一小段画亮，
// 形成一道游走的噪点锋面。
void Display::drawDissolveChar(float t, uint16_t colorRGB565, int16_t charX,
                               int16_t topY) {
  const int16_t w =
      glyphOld.width() > glyphNew.width() ? glyphOld.width() : glyphNew.width();
  const int16_t h = glyphOld.height();

  // Seeded by the character's x position so neighbouring digits do not
  // dissolve with an identical pattern.
  const uint32_t seed = (uint32_t)charX * 2654435761u;
  const uint16_t tInt = (uint16_t)(t * 65535.0f);
  const uint16_t band = (uint16_t)(DISSOLVE_FRONT_BAND * 65535.0f);
  const uint16_t frontColor = mixWhiteRGB565(colorRGB565, DISSOLVE_FRONT_MIX);

  for (int16_t gy = 0; gy < h; ++gy) {
    for (int16_t gx = 0; gx < w; ++gx) {
      const bool oldLit = glyphOld.pixelAt(gx, gy);
      const bool newLit = glyphNew.pixelAt(gx, gy);
      if (!oldLit && !newLit) {
        continue;
      }

      const uint16_t threshold =
          (uint16_t)(glyphHash32(seed ^ ((uint32_t)gx << 16) ^ (uint32_t)gy) &
                     0xFFFF);
      const uint16_t distance =
          (threshold > tInt) ? (threshold - tInt) : (tInt - threshold);
      const uint16_t pixelColor =
          (distance < band) ? frontColor : colorRGB565;

      if (threshold < tInt) {
        if (newLit) {
          dma_display->drawPixel(charX + gx, topY + gy, pixelColor);
        }
      } else {
        if (oldLit) {
          dma_display->drawPixel(charX + gx, topY + gy, pixelColor);
        }
      }
    }
  }
}

// ANIMATION_5 - particle burst.
// Stateless on purpose: a pixel's position is a pure function of its glyph
// coordinate and the progress, so there is no particle pool to allocate, no
// lifetime to manage and nothing to leak when the page changes mid-flight.
// The old glyph explodes outward, the new one converges from outside; the two
// phases overlap so the slot is never blank.
// 无状态粒子：位置由"字形坐标 + 进度"直接算出，不需要粒子池，页面中途切换也不会
// 有残留。旧字向外炸开，新字从外向内聚拢，两段重叠所以不会出现空档。
void Display::drawParticleChar(float t, uint16_t colorRGB565, int16_t charX,
                               int16_t topY) {
  const int16_t w =
      glyphOld.width() > glyphNew.width() ? glyphOld.width() : glyphNew.width();
  const int16_t h = glyphOld.height();
  const float cx = w * 0.5f;
  const float cy = h * 0.5f;
  const uint32_t seed = (uint32_t)charX * 2654435761u;

  float tOut = t / PARTICLE_PHASE;
  if (tOut > 1.0f) {
    tOut = 1.0f;
  }
  float tIn = (t - (1.0f - PARTICLE_PHASE)) / PARTICLE_PHASE;
  if (tIn < 0.0f) {
    tIn = 0.0f;
  }
  if (tIn > 1.0f) {
    tIn = 1.0f;
  }

  const bool drawOut = (tOut < 1.0f);
  const bool drawIn = (tIn > 0.0f);
  if (!drawOut && !drawIn) {
    return;
  }

  // Outgoing particles fade to black, incoming ones fade up from black.
  const uint16_t outColor =
      scaleColorRGB565Custom(colorRGB565, tOut, true, 0.0f);
  const uint16_t inColor =
      scaleColorRGB565Forward(colorRGB565, tIn, true, 0.0f);

  for (int16_t gy = 0; gy < h; ++gy) {
    for (int16_t gx = 0; gx < w; ++gx) {
      const bool oldLit = drawOut && glyphOld.pixelAt(gx, gy);
      const bool newLit = drawIn && glyphNew.pixelAt(gx, gy);
      if (!oldLit && !newLit) {
        continue;
      }

      const uint32_t hv =
          glyphHash32(seed ^ ((uint32_t)gx << 16) ^ (uint32_t)gy);
      // Two independent scatter values in [-1, 1].
      const float jx =
          ((float)(hv & 0xFF) / 127.5f - 1.0f) * PARTICLE_JITTER;
      const float jy =
          ((float)((hv >> 8) & 0xFF) / 127.5f - 1.0f) * PARTICLE_JITTER;

      // Radial offset from the glyph centre - outer pixels travel further,
      // which reads as an explosion rather than a uniform slide.
      const float rx = (float)gx - cx;
      const float ry = (float)gy - cy;

      if (oldLit) {
        const float px = gx + rx * PARTICLE_SPREAD * tOut + jx * tOut;
        const float py = gy + ry * PARTICLE_SPREAD * tOut + jy * tOut +
                         PARTICLE_GRAVITY * tOut * tOut;
        dma_display->drawPixel(charX + (int16_t)px, topY + (int16_t)py,
                               outColor);
      }
      if (newLit) {
        const float u = 1.0f - tIn; // 1 -> 0, particles land on the glyph
        const float px = gx + rx * PARTICLE_SPREAD * u + jx * u;
        const float py = gy + ry * PARTICLE_SPREAD * u + jy * u -
                         PARTICLE_GRAVITY * u * u;
        dma_display->drawPixel(charX + (int16_t)px, topY + (int16_t)py,
                               inColor);
      }
    }
  }
}

// Shared entry point for every per-pixel transition.
void Display::drawPixelAnimChar(int16_t animationType, char chNow, char chNext,
                                float t, uint16_t colorRGB565, int16_t charX,
                                int16_t baselineY, const uint8_t *fontName) {
  if (t < 0.0f) {
    t = 0.0f;
  }
  if (t > 1.0f) {
    t = 1.0f;
  }

  // Cheap path: nothing is changing, the animation has not started / has
  // finished, or the glyph does not fit the off-screen buffer. This is what
  // keeps the per-pixel cost limited to the handful of digits that actually
  // move in any given second.
  // 便宜路径：字符没变、动画未开始或已结束、字形放不下离屏缓冲。
  if (chNow == chNext || t <= 0.0f || t >= 1.0f ||
      !glyphOld.render(fontName, chNow) || !glyphNew.render(fontName, chNext)) {
    u8g2_for_adafruit_gfx.setFont(fontName);
    u8g2_for_adafruit_gfx.setForegroundColor(colorRGB565);
    u8g2_for_adafruit_gfx.setCursor(charX, baselineY);
    u8g2_for_adafruit_gfx.print(t < 0.5f ? chNow : chNext);
    return;
  }

  const int16_t topY = baselineY - glyphOld.ascent(); // glyph row 0 on panel

  switch (animationType) {
  case ANIMATION_3:
    drawScanChar(t, colorRGB565, charX, topY);
    break;
  case ANIMATION_4:
    drawDissolveChar(t, colorRGB565, charX, topY);
    break;
  case ANIMATION_5:
    drawParticleChar(t, colorRGB565, charX, topY);
    break;
  default:
    break;
  }
}

void Display::display(unsigned long elapsed, const char *nowStr,
                      const char *nextStr, std::vector<int8_t> results,
                      MatrixCore matrixCore) {
  const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
      matrixCore.fontGroupIndex, matrixCore.fontIndex);

  charCountForCalWidth = analyzeCharInStr(nowStr);

  FontMetrics fontABCMetrics; // = getFontMetrics(fontInfo->fontName, "M");
  if(matrixCore.fontGroupIndex == 10 && matrixCore.fontIndex == 40){
    fontABCMetrics.charWidth = 13;
    fontABCMetrics.height = 13;
    charCountForCalWidth.reset();
    charCountForCalWidth.countABC =  strlen(nowStr) / 3;
  } else {
    fontABCMetrics = getFontMetrics(fontInfo->fontName, "M");
  }
  FontMetrics fontNumMetrics = getFontMetrics(fontInfo->fontName, "5");
  FontMetrics fontHyphenMetrics = getFontMetrics(fontInfo->fontName, "-");
  FontMetrics spaceMetrics = getFontMetrics(fontInfo->fontName, ":");

  int16_t strSingleWidth =
      (spaceMetrics.charWidth + 1) * charCountForCalWidth.countSpace +
      (fontNumMetrics.charWidth + 1) * charCountForCalWidth.countNum +
      (fontABCMetrics.charWidth + 1) * charCountForCalWidth.countABC +
      (fontHyphenMetrics.charWidth + 1) * charCountForCalWidth.countHyphen;
  int16_t x = g_panelWidthChain * matrixCore.x - strSingleWidth / 2 ;
  int16_t y =
      g_panelHeightChain * matrixCore.y + fontNumMetrics.height / 2;
  uint16_t colorRGB565 = matrixColorManager.getColor(matrixCore.colorIndex1);
  int16_t offsetSpaceX = fontInfo->offsetSepX * spaceMetrics.charWidth;
  int16_t offsetSpaceY = fontInfo->offsetSepY * spaceMetrics.height;

  switch (matrixCore.animationType) {
  case ANIMATION_0:
    charCountForString.reset();
    char chNow;
    char chNowNext;
    for (int i = 0; nowStr[i] != '\0'; i++) {
      chNow = nowStr[i];
      switch (charType[(unsigned char)chNow]) {
      case 0:
        displayStaticOneTemplate(
            chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
            fontABCMetrics.height, fontNumMetrics.charWidth,
            spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
            charCountForString.countABC, charCountForString.countNum,
            charCountForString.countSpace, charCountForString.countHyphen, 1, 0,
            0, fontInfo->fontName);
        charCountForString.countNum++;
        break;
      case 1:
        displayStaticOneTemplate(
            chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
            fontABCMetrics.height, fontNumMetrics.charWidth,
            spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
            charCountForString.countABC, charCountForString.countNum,
            charCountForString.countSpace, charCountForString.countHyphen, 1, 0,
            0, fontInfo->fontName);
        charCountForString.countHyphen++;
        break;
      case 2:
        displayStaticOneTemplate(
            chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
            fontABCMetrics.height, fontNumMetrics.charWidth,
            spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
            charCountForString.countABC, charCountForString.countNum,
            charCountForString.countSpace, charCountForString.countHyphen, 1,
            offsetSpaceX, offsetSpaceY, fontInfo->fontName);
        charCountForString.countSpace++;
        break;
      case 3:
        displayStaticOneTemplate(
            chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
            fontABCMetrics.height, fontNumMetrics.charWidth,
            spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
            charCountForString.countABC, charCountForString.countNum,
            charCountForString.countSpace, charCountForString.countHyphen, 1, 0,
            0, fontInfo->fontName);
        charCountForString.countABC++;
        break;
      }
    }

    break;
  case ANIMATION_1:
    if (elapsed < ROLL_START_MS || elapsed > ROLL_END_MS) {
      charCountForString.reset();
      for (int i = 0; nowStr[i] != '\0'; i++) {
        chNow = nowStr[i];
        switch (charType[(unsigned char)chNow]) {
        case 0:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              0, 0, fontInfo->fontName);
          charCountForString.countNum++;
          break;
        case 1:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              0, 0, fontInfo->fontName);
          charCountForString.countHyphen++;
          break;
        case 2:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              offsetSpaceX, offsetSpaceY, fontInfo->fontName);
          charCountForString.countSpace++;
          break;
        case 3:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              0, 0, fontInfo->fontName);
          charCountForString.countABC++;
          break;
        }
      }
    } else {
      charCountForString.reset();
      animationSpeed = (elapsed - ROLL_START_MS) /
                       (float)(ROLL_END_MS - ROLL_START_MS);
      for (int i = 0; nowStr[i] != '\0'; i++) {
        chNow = nowStr[i];
        chNowNext = nextStr[i];
        switch (charType[(unsigned char)chNow]) {
        case 0:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y - animationSpeed * fontNumMetrics.height * results[i],
              fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          displayStaticOneTemplate(
              chNowNext,
              scaleColorRGB565Forward(colorRGB565, animationSpeed, results[i]),
              x, y + (1 - animationSpeed) * fontNumMetrics.height * results[i],
              fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);
          charCountForString.countNum++;
          break;
        case 1:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          charCountForString.countHyphen++;
          break;
        case 2:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, offsetSpaceX, offsetSpaceY,
              fontInfo->fontName);

          charCountForString.countSpace++;
          break;
        case 3:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y - animationSpeed * fontABCMetrics.height * results[i],
              fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          displayStaticOneTemplate(
              chNowNext,
              scaleColorRGB565Forward(colorRGB565, animationSpeed, results[i]),
              x, y + (1 - animationSpeed) * fontABCMetrics.height * results[i],
              fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          charCountForString.countNum++;
          break;
        }
      }
    }

    break;
  case ANIMATION_2:
    if (elapsed < FADE_START_MS || elapsed > FADE_END_MS) {
      // 如果 1->2 时  <350是当前时间 1 , >1000 就是下一个元素2, >1000ms时
      // 但是nowStr就已经切换2
      charCountForString.reset();
      for (int i = 0; nowStr[i] != '\0'; i++) {
        chNow = nowStr[i];
        switch (charType[(unsigned char)chNow]) {
        case 0:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              0, 0, fontInfo->fontName);
          charCountForString.countNum++;
          break;
        case 1:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              0, 0, fontInfo->fontName);
          charCountForString.countHyphen++;
          break;
        case 2:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              offsetSpaceX, offsetSpaceY, fontInfo->fontName);
          charCountForString.countSpace++;
          break;
        case 3:
          displayStaticOneTemplate(
              chNow, colorRGB565, x, y, fontABCMetrics.charWidth,
              fontABCMetrics.height, fontNumMetrics.charWidth,
              spaceMetrics.charWidth, fontHyphenMetrics.charWidth,
              charCountForString.countABC, charCountForString.countNum,
              charCountForString.countSpace, charCountForString.countHyphen, 1,
              0, 0, fontInfo->fontName);
          charCountForString.countABC++;
          break;
        }
      }
    } else if (elapsed < FADE_MID_MS) {
      charCountForString.reset();
      animationSpeed = (elapsed - FADE_START_MS) /
                       (float)(FADE_MID_MS - FADE_START_MS);
      for (int i = 0; nowStr[i] != '\0'; i++) {
        chNow = nowStr[i];
        chNowNext = nextStr[i];
        switch (charType[(unsigned char)chNow]) {
        case 0:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          charCountForString.countNum++;
          break;
        case 1:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          charCountForString.countHyphen++;
          break;
        case 2:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, offsetSpaceX, offsetSpaceY,
              fontInfo->fontName);

          charCountForString.countSpace++;
          break;
        case 3:
          displayStaticOneTemplate(
              chNow,
              scaleColorRGB565Custom(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);

          charCountForString.countNum++;
          break;
        }
      }
    } else {
      animationSpeed = (elapsed - FADE_MID_MS) /
                       (float)(FADE_END_MS - FADE_MID_MS);
      charCountForString.reset();
      for (int i = 0; nowStr[i] != '\0'; i++) {
        chNow = nowStr[i];
        chNowNext = nextStr[i];
        switch (charType[(unsigned char)chNow]) {
        case 0:
          displayStaticOneTemplate(
              chNowNext,
              scaleColorRGB565Forward(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);
          charCountForString.countNum++;
          break;
        case 1:
          displayStaticOneTemplate(
              chNowNext,
              scaleColorRGB565Forward(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);
          charCountForString.countHyphen++;
          break;
        case 2:
          displayStaticOneTemplate(
              chNowNext,
              scaleColorRGB565Forward(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, offsetSpaceX, offsetSpaceY,
              fontInfo->fontName);
          charCountForString.countSpace++;
          break;
        case 3:
          displayStaticOneTemplate(
              chNowNext,
              scaleColorRGB565Forward(colorRGB565, animationSpeed, results[i]),
              x, y, fontABCMetrics.charWidth, fontABCMetrics.height,
              fontNumMetrics.charWidth, spaceMetrics.charWidth,
              fontHyphenMetrics.charWidth, charCountForString.countABC,
              charCountForString.countNum, charCountForString.countSpace,
              charCountForString.countHyphen, 1, 0, 0, fontInfo->fontName);
          charCountForString.countABC++;
          break;
        }
      }
    }
    break;
  case ANIMATION_3:
  case ANIMATION_4:
  case ANIMATION_5: {
    // Per-pixel transitions. Each character animates on its own; only the
    // characters that actually change take the per-pixel path, the rest are
    // printed normally.
    // 逐像素特效。每个字符独立动画，只有真正变化的字符走逐像素路径。
    const float animT = pixelAnimProgress(matrixCore.animationType, elapsed);
    const int nextLen = (nextStr != NULL) ? (int)strlen(nextStr) : 0;
    charCountForString.reset();
    for (int i = 0; nowStr[i] != '\0'; i++) {
      chNow = nowStr[i];
      chNowNext = (i < nextLen) ? nextStr[i] : chNow;

      // Same cursor arithmetic as setupDisplayContext(), so a flipping
      // character stays aligned with its static neighbours.
      // 与 setupDisplayContext 完全一致的定位公式，保证对齐。
      int16_t charX =
          x + (1 + fontABCMetrics.charWidth) * charCountForString.countABC +
          (1 + fontNumMetrics.charWidth) * charCountForString.countNum +
          (1 + spaceMetrics.charWidth) * charCountForString.countSpace +
          (1 + fontHyphenMetrics.charWidth) * charCountForString.countHyphen;
      int16_t charY = y;

      switch (charType[(unsigned char)chNow]) {
      case 0:
        charCountForString.countNum++;
        break;
      case 1:
        charCountForString.countHyphen++;
        break;
      case 2:
        charX += offsetSpaceX;
        charY += offsetSpaceY;
        charCountForString.countSpace++;
        break;
      case 3:
        charCountForString.countABC++;
        break;
      }

      drawPixelAnimChar(matrixCore.animationType, chNow, chNowNext, animT,
                        colorRGB565, charX, charY, fontInfo->fontName);
    }
    break;
  }
  default:
    break;
  }
}
void Display::displayString(unsigned long elapsed, TimeData timeNow,
                            TimeData timeNowNextSec, MatrixCore matrixCore, boolean isUnderLine) {
  const char *nowStr;
  const char *nowNextStr;
  std::vector<int8_t> results;
  switch (matrixCore.displayGroup) {
  case 0:
    nowStr = matrixTimeData.getStrStaff(timeNow, matrixCore.displayIndex);
    nowNextStr =
        matrixTimeData.getStrStaff(timeNowNextSec, matrixCore.displayIndex);
    results = compare_with_vector(nowStr, nowNextStr);
    display(elapsed, nowStr, nowNextStr, results, matrixCore);
    if(isUnderLine){displayUnderline(false, nowStr, matrixCore);}
    break;
  case 1:
    if(matrixSettings.getCurrentLanguage() == LANG_CHINESE){
      matrixCore.fontGroupIndex = 10;
      matrixCore.fontIndex = 40;
    }
    nowStr = matrixTimeUtils.getStr(timeNow, matrixCore.displayIndex);
    nowNextStr = matrixTimeUtils.getStr(timeNowNextSec, matrixCore.displayIndex);
    results = compare_with_vector(nowStr, nowNextStr);
    // display(elapsed, nowStr, nowNextStr, results, matrixCore);
    displayString(elapsed, nowStr, nowStr, 1, matrixCore);
    if(isUnderLine){displayUnderline(false, nowStr, matrixCore);}
    // displayString(nowStr, matrixCore);
    break;
  case 2:
    nowStr = matrixSettings.getCommonWord(static_cast<CommonWordIndex>(matrixCore.displayIndex));
    displayString(elapsed, nowStr, nowStr, 0, matrixCore);
    if(isUnderLine){displayUnderline(true, nowStr, matrixCore);}
    break;
  case 3:
    nowStr = matrixStatusManager.getSysStatus(matrixCore.displayIndex);
    displayString(elapsed, nowStr, nowStr, 0, matrixCore);
    if(isUnderLine){displayUnderline(true, nowStr, matrixCore);}
    break;
  case 4:
    nowStr = matrixTimeData.getStrStaff(timeNow, matrixCore.displayIndex);
    nowNextStr = matrixTimeData.getStrStaff(timeNowNextSec, matrixCore.displayIndex);
    displayString(elapsed, nowStr, nowNextStr, 0, matrixCore);
    if(isUnderLine){displayUnderline(true, nowStr, matrixCore);}
    break;
  case 5:
    nowStr = alarmManager.getAlarmStr(matrixCore.displayIndex);
    displayString(elapsed, nowStr, nowStr, 0, matrixCore);
    if(isUnderLine){displayUnderline(true, nowStr, matrixCore);}
    break;
  case 6:
    nowStr = matrixStatusManager.getSysStatus(matrixCore.displayIndex);
    displayString(elapsed, nowStr, nowStr, 2, matrixCore);
    if(isUnderLine){displayUnderline(true, nowStr, matrixCore);}
    break;
  case 7: {
    // Stock index text. matrixCore is taken by value, so overriding the colour
    // here only affects this draw and leaves the stored page untouched.
    // Every value field is tinted by the gain / loss scheme; only the label
    // keeps the colour the user configured.
    if (STOCK_FIELD_OF(matrixCore.displayIndex) != STOCK_FIELD_LABEL) {
      matrixCore.colorIndex1 = stockManager.getChangeColorIndex(matrixCore.displayIndex);
    }
    nowStr = stockManager.getStockStr(matrixCore.displayIndex);
    // Left aligned: matrixCore.x is the left edge, so the label column lines up
    // between pages even as the value's width changes through the day.
    displayString(elapsed, nowStr, nowStr, 0, matrixCore);
    if(isUnderLine){displayUnderline(true, nowStr, matrixCore);}
    break;
  }
  case 8:
    // Gain / loss bar, drawn directly - there is no string to lay out.
    drawStockBar(matrixCore);
    break;
  default:
    break;
  }
}

void Display::displayUnderline(boolean isStartLeft,const char* nowStr,MatrixCore matrixCore){

  int16_t x;
  int16_t leftX;
  int16_t centerX;

  const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
      matrixCore.fontGroupIndex, matrixCore.fontIndex);

  charCountForCalWidth.reset();
  charCountForCalWidth = analyzeCharInStr(nowStr);

  FontMetrics fontABCMetrics; // = getFontMetrics(fontInfo->fontName, "M");
  if(matrixCore.fontGroupIndex == 10 && matrixCore.fontIndex == 40){
    if (matrixSettings.getCurrentLanguage() == Language::LANG_CHINESE) {
      fontABCMetrics.charWidth = 13;
      fontABCMetrics.height = 13;
      charCountForCalWidth.countABC = strlen(nowStr) / 3;
    }else {
      fontABCMetrics = getFontMetrics(fontInfo->fontName, "S");
    } 
  } else {
    fontABCMetrics = getFontMetrics(fontInfo->fontName, "S");
  }
  FontMetrics fontNumMetrics = getFontMetrics(fontInfo->fontName, "5");
  FontMetrics fontHyphenMetrics = getFontMetrics(fontInfo->fontName, "-");
  FontMetrics spaceMetrics = getFontMetrics(fontInfo->fontName, ":");
  if(isStartLeft){
    int16_t strSingleWidth =
        (spaceMetrics.charWidth) * charCountForCalWidth.countSpace +
        (fontNumMetrics.charWidth) * charCountForCalWidth.countNum +
        (fontABCMetrics.charWidth) * charCountForCalWidth.countABC +
        (fontHyphenMetrics.charWidth) * charCountForCalWidth.countHyphen;
    x = g_panelWidthChain * matrixCore.x + strSingleWidth / 2;
  } else {
    x = g_panelWidthChain * matrixCore.x - 2;
  }
  int y = g_panelHeightChain * matrixCore.y + fontABCMetrics.height/2 + 1 ;

  uint16_t colorRGB565 = matrixColorManager.getColor(matrixCore.colorIndex1);

  dma_display->drawFastHLine(x, y, 3, colorRGB565);
}

void Display::drawStockBar(MatrixCore matrixCore) {
  StockIndex idx = stockManager.getIndex(STOCK_SLOT_OF(matrixCore.displayIndex));

  int16_t margin = (int16_t)(g_panelWidthChain * matrixCore.x);
  if (margin < 1) {
    margin = 1;
  }
  int16_t trackLeft = margin;
  int16_t trackRight = g_panelWidthChain - margin - 1;
  if (trackRight <= trackLeft) {
    return;
  }

  int16_t trackWidth = trackRight - trackLeft + 1;
  int16_t halfWidth = trackWidth / 2;
  int16_t centerX = trackLeft + halfWidth;

  int16_t barHeight = (g_panelWidthChain >= 128) ? STOCK_BAR_HEIGHT_WIDE
                                                 : STOCK_BAR_HEIGHT_NARROW;
  int16_t topY = (int16_t)(g_panelHeightChain * matrixCore.y) - barHeight / 2;
  if (topY < 0) {
    topY = 0;
  }
  if (topY + barHeight > g_panelHeightChain) {
    topY = g_panelHeightChain - barHeight;
  }

  // Baseline and centre tick stay lit even with no data, so an empty bar still
  // reads as "no move yet" rather than as a dead area of the panel.
  uint16_t trackColor = matrixColorManager.getColor(COLOR_IDX(COLOR_GRAY));
  dma_display->drawFastHLine(trackLeft, topY + barHeight / 2, trackWidth, trackColor);
  dma_display->drawFastVLine(centerX, topY, barHeight, trackColor);

  if (!idx.valid) {
    return;
  }

  float pct = idx.changePct;
  if (pct > STOCK_BAR_FULL_SCALE_PCT) {
    pct = STOCK_BAR_FULL_SCALE_PCT;
  } else if (pct < -STOCK_BAR_FULL_SCALE_PCT) {
    pct = -STOCK_BAR_FULL_SCALE_PCT;
  }

  int16_t length = (int16_t)((pct / STOCK_BAR_FULL_SCALE_PCT) * halfWidth);
  uint16_t barColor = stockManager.getChangeColor(idx.changePct);

  if (length > 0) {
    dma_display->fillRect(centerX, topY, length, barHeight, barColor);
  } else if (length < 0) {
    dma_display->fillRect(centerX + length, topY, -length, barHeight, barColor);
  } else {
    // A move too small to fill a pixel still gets the centre tick recoloured.
    dma_display->drawFastVLine(centerX, topY, barHeight, barColor);
  }
}

void Display::showPageInfo(){
  char buffer[16];
  sprintf(buffer, "%d-%d", matrixCoreManager.getCurrentPageIndex()+1, matrixCoreManager.getCurrentSecondaryIndex()+1);
  displayTextRGB(200,200,200, 0, g_panelHeightChain, u8g2_font_blipfest_07_tr, buffer);
}
void Display::displayString(const char* text, MatrixCore matrixCore){
  
  const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
      matrixCore.fontGroupIndex, matrixCore.fontIndex);
  FontMetrics fontABCMetrics = getFontMetrics(fontInfo->fontName, "A");
  int x = g_panelWidthChain * matrixCore.x;
  int y = g_panelHeightChain * matrixCore.y + fontABCMetrics.height/2 ;
  uint16_t colorRGB565 = matrixColorManager.getColor(matrixCore.colorIndex1);

  displayText(colorRGB565, x , y, fontInfo->fontName, text);
  
}

void Display::displayString(unsigned long elapsed, const char *nowStr,
                            const char *nowNextStr, int whereStart,
                            MatrixCore matrixCore) {
  int16_t x;
  int16_t leftX;
  int16_t centerX;
  int16_t rigthX;

  const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
      matrixCore.fontGroupIndex, matrixCore.fontIndex);

  charCountForCalWidth = analyzeCharInStr(nowStr);

  boolean flag = compareStringsFast(nowStr, nowNextStr);

  FontMetrics fontABCMetrics; // = getFontMetrics(fontInfo->fontName, "M");
  if(matrixCore.fontGroupIndex == 10 && matrixCore.fontIndex == 40){
    fontABCMetrics.charWidth = 13;
    fontABCMetrics.height = 13;
    charCountForCalWidth.reset();
    charCountForCalWidth.countABC =  strlen(nowStr) / 3;
  } else {
    fontABCMetrics = getFontMetrics(fontInfo->fontName, "M");
  }
  FontMetrics fontNumMetrics = getFontMetrics(fontInfo->fontName, "5");
  FontMetrics fontHyphenMetrics = getFontMetrics(fontInfo->fontName, "-");
  FontMetrics spaceMetrics = getFontMetrics(fontInfo->fontName, ":");
  switch (whereStart) {
  case 0: {
    leftX = g_panelWidthChain * matrixCore.x;
    x = leftX;
    break;
  }
  case 1: {
    int16_t strSingleWidth =
        (spaceMetrics.charWidth) * charCountForCalWidth.countSpace +
        (fontNumMetrics.charWidth) * charCountForCalWidth.countNum +
        (fontABCMetrics.charWidth) * charCountForCalWidth.countABC +
        (fontHyphenMetrics.charWidth) * charCountForCalWidth.countHyphen;
    centerX = g_panelWidthChain * matrixCore.x - strSingleWidth / 2;
    x = centerX;
    break;
  }
  case 2: {
    int16_t strSingleWidth =
        (spaceMetrics.charWidth) * charCountForCalWidth.countSpace +
        (fontNumMetrics.charWidth) * charCountForCalWidth.countNum +
        (fontABCMetrics.charWidth) * charCountForCalWidth.countABC +
        (fontHyphenMetrics.charWidth) * charCountForCalWidth.countHyphen;
    rigthX = g_panelWidthChain * matrixCore.x - strSingleWidth;
    x = rigthX;
    break;
  }
  default:
    break;
  }
  int y = g_panelHeightChain * matrixCore.y + fontABCMetrics.height/2 ;

  uint16_t colorRGB565 = matrixColorManager.getColor(matrixCore.colorIndex1);
  switch (matrixCore.animationType) {
  case ANIMATION_0:
    displayText(colorRGB565, x, y, fontInfo->fontName, nowStr);
    break;
  case ANIMATION_1:
    if (elapsed < ROLL_START_MS || elapsed > ROLL_END_MS) {
      displayText(colorRGB565, x, y, fontInfo->fontName, nowStr);
    } else {
      animationSpeed = (elapsed - ROLL_START_MS) /
                       (float)(ROLL_END_MS - ROLL_START_MS);
      displayText(scaleColorRGB565Custom(colorRGB565, animationSpeed, flag), x,
                  y - animationSpeed * fontNumMetrics.height * flag,
                  fontInfo->fontName, nowStr);
      displayText(scaleColorRGB565Forward(colorRGB565, animationSpeed, flag), x,
                  y + (1 - animationSpeed) * fontNumMetrics.height * flag,
                  fontInfo->fontName, nowNextStr);
    }
    break;
  case ANIMATION_2:
    if (elapsed < FADE_START_MS || elapsed > FADE_END_MS) {
      displayText(colorRGB565, x, y, fontInfo->fontName, nowStr);
    } else if (elapsed < FADE_MID_MS) {
      animationSpeed = (elapsed - FADE_START_MS) /
                       (float)(FADE_MID_MS - FADE_START_MS);
      displayText(scaleColorRGB565Custom(colorRGB565, animationSpeed, flag), x, y, fontInfo->fontName, nowStr);
    } else {
      animationSpeed = (elapsed - FADE_MID_MS) /
                       (float)(FADE_END_MS - FADE_MID_MS);
      displayText(scaleColorRGB565Forward(colorRGB565, animationSpeed, flag), x, y , fontInfo->fontName, nowNextStr);
    }
    break;
  case ANIMATION_3:
  case ANIMATION_4:
  case ANIMATION_5: {
    // Multi-byte UTF-8 (the Chinese weekday font group) cannot be walked one
    // byte at a time, so it keeps the static draw.
    // 中文是多字节 UTF-8，不能按字节拆，直接静态绘制。
    if (!isAsciiString(nowStr) || !isAsciiString(nowNextStr)) {
      displayText(colorRGB565, x, y, fontInfo->fontName, nowStr);
      break;
    }
    const float animT = pixelAnimProgress(matrixCore.animationType, elapsed);
    const int nextLen = (nowNextStr != NULL) ? (int)strlen(nowNextStr) : 0;
    int16_t charX = x;
    char one[2] = {0, 0};
    for (int i = 0; nowStr[i] != '\0'; i++) {
      const char chNow = nowStr[i];
      const char chNext = (i < nextLen) ? nowNextStr[i] : chNow;
      drawPixelAnimChar(matrixCore.animationType, chNow, chNext, animT,
                        colorRGB565, charX, y, fontInfo->fontName);
      one[0] = chNow;
      u8g2_for_adafruit_gfx.setFont(fontInfo->fontName);
      charX += u8g2_for_adafruit_gfx.getUTF8Width(one);
    }
    break;
  }
  default:
    displayText(colorRGB565, x, y, fontInfo->fontName, nowStr);
    break;
  }
}

bool Display::compareStringsFast(const char* nowStr, const char* nowNextStr) {
     // Handle null pointer cases
    if (nowStr == nowNextStr) return false;  // Same pointer or both NULL - equal
    if (!nowStr || !nowNextStr) return true;  // One is NULL - different
    
    // Compare word by word for better performance
    const size_t word_size = sizeof(size_t);
    
    // Align pointers if possible and compare byte by byte until aligned
    while (((uintptr_t)nowStr % word_size) != 0) {
        if (*nowStr != *nowNextStr || *nowStr == '\0') {
            return (*nowStr != *nowNextStr);  // Return true if different
        }
        nowStr++;
        nowNextStr++;
    }
    
    // Compare word by word
    const size_t* word1 = (const size_t*)nowStr;
    const size_t* word2 = (const size_t*)nowNextStr;
    
    // Check if second pointer is also aligned
    if (((uintptr_t)nowNextStr % word_size) == 0) {
        while (1) {
            size_t w1 = *word1;
            size_t w2 = *word2;
            
            if (w1 != w2) break;
            
            // Check for null terminator in the word
            // This uses a bit trick to detect zero bytes
            size_t has_zero = (w1 - 0x0101010101010101ULL) & ~w1 & 0x8080808080808080ULL;
            if (has_zero) return false;  // Strings are equal
            
            word1++;
            word2++;
        }
    }
    
    // Fall back to byte comparison
    nowStr = (const char*)word1;
    nowNextStr = (const char*)word2;
    
    while (*nowStr && (*nowStr == *nowNextStr)) {
        nowStr++;
        nowNextStr++;
    }
    
    return (*nowStr != *nowNextStr);
}

int16_t Display::getPageFlagTime() { return lineFlagTime; }
void Display::setPageFlagTime(int16_t pageFlagTime) {
  this->lineFlagTime = lineFlagTime;
}