#include "loading.h"
#include "matrixDma.h"

// The loading title. Kept here so drawTitle() and init() can never disagree
// about its length when the centring offset is computed.
// 加载页标题，放在这里保证 drawTitle() 和 init() 算居中时用的长度一致。
static const char *const TITLE_TEXT = "MatrixClock";
static const int16_t TITLE_LEN = 11;

// Two line split for narrow canvases. "Clock" starts under the 3rd character
// of "Matrix" (the "t"), so the lines interlock instead of stacking flush
// left, and the font stays the same 7x13B used on 128px panels.
// At 7px per char the pair spans (2 + 5) * 7 = 49px from the title origin.
// 窄画布用的两行拆分。"Clock" 从 "Matrix" 第 3 个字符（也就是 "t"）下方开始，
// 两行互相咬合而不是齐头堆叠，字体仍是 128 屏那套 7x13B。
// 每字 7px，两行从标题起点算总跨度 (2 + 5) * 7 = 49px。
static const char *const TITLE_LINE1 = "Matrix";
static const char *const TITLE_LINE2 = "Clock";
static const int16_t TITLE_LINE2_LEN = 5;
static const int16_t TITLE_LINE2_OFFSET_CHARS = 2;

// Font group / index pairs, see matrixFonts.cpp init().
// 字体组 / 索引对照，见 matrixFonts.cpp 的 init()。
#define FONT_GROUP_9   10  // FONT9 : 13px tall, 6-8px wide
#define FONT9_7X13B     4  // u8g2_font_7x13B_tr, 7px per char
#define FONT9_WQY13    40  // u8g2_font_wqy13_t_gb2312, the only CJK capable font

Loading loading;

Loading::Loading() {
  // Nothing panel dependent may be computed here: this constructor runs during
  // static initialisation, long before setup() calls setupDMA(), so g_panelType
  // and g_panelWidthChain are still 0. That is why the old switch(g_panelType)
  // always fell into case 0 and the 64x64 panel got the 128px wide layout.
  // 这里不能算任何跟屏幕有关的值：构造函数在静态初始化阶段执行，远早于
  // setup() 里的 setupDMA()，此时 g_panelType / g_panelWidthChain 都是 0。
  // 原来的 switch(g_panelType) 因此永远只会走 case 0，64x64 屏拿到了 128 的布局。
  barX = 0;
  barY = 0;
  midPanelHeight = 0;
  titleX = 0;
  barWidth = 0;
  barHeight = 8;
  radius = 1;
  titleFont = u8g2_font_7x13B_tf;
  titleCharWidth = 7;
  errMsgFont = u8g2_font_6x10_tf;
  errMsgCharWidth = 6;
  titleTwoLine = false;
  titleX2 = 0;
  titleLine2Y = 0.36f;
  flag = true; // ✅ 默认就是 true
  message = "";
  count = 0;
}

void Loading::init() {
  // All geometry is derived from the runtime canvas size, never from the
  // PANEL_WIDTH / PANEL_HEIGHT / PANEL_CHAIN macros - those are compile time
  // constants fixed at 128 / 64 / 1 and are wrong for every other panel type.
  // 所有尺寸都取自运行时画布大小，不再用 PANEL_WIDTH / PANEL_HEIGHT /
  // PANEL_CHAIN 宏 —— 那是编译期写死的 128 / 64 / 1，对其他屏型都是错的。
  barX = g_panelWidthChain * 0.2f / 2;
  barY = g_panelHeightChain * 0.78f;
  midPanelHeight = g_panelHeightChain / 2;
  barWidth = g_panelWidthChain * 0.8f;
  barHeight = 8;
  radius = 1;

  switch (g_panelType) {
  case PANEL_TYPE_128X64_X1:
  case PANEL_TYPE_64X64_X2: {
    // 128px wide canvas: 7x13B fits "MatrixClock" at 77px.
    // 128 宽画布：7x13B 下 "MatrixClock" 占 77px，放得下。
    titleFont = u8g2_font_7x13B_tf;
    titleCharWidth = 7;
    errMsgFont = u8g2_font_6x10_tf;
    errMsgCharWidth = 6;
    titleTwoLine = false;

    setupPage = {MatrixCore(0.03f, 0.16f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 2, 14, 0)};

    loadingPage = {MatrixCore(0.5f, 0.3f, FONT_GROUP_9, FONT9_7X13B, 3, 0, 2, 12, 0),

                   MatrixCore(0.1f, 0.56f, FONT_GROUP_9, FONT9_WQY13, 0, 0, 2, 13, 0),

                   MatrixCore(0.1f, 0.72f, 0, 0, 3, 0, 3, 9, 0)};

    setupReady0 = MatrixCore(0.03f, 0.16f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 2, 7, 0);
    setupReady1 = MatrixCore(0.03f, 0.44f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 2, 8, 0);
    setupReady2 = MatrixCore(0.03f, 0.72f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 3, 9, 0);

    errorWiFiMsg = MatrixCore(0.1f, 0.56f, FONT_GROUP_9, FONT9_WQY13, 0, 0, 2, 10, 0);

    errorNetWorkMsg = MatrixCore(0.1f, 0.56f, FONT_GROUP_9, FONT9_WQY13, 0, 0, 2, 11, 0);
    break;
  }
  case PANEL_TYPE_64X64_X1: {
    // 64px wide canvas: "MatrixClock" on one line needs 77px at 7x13B and gets
    // clipped. Rather than shrinking to a 5px font, the title is split over two
    // interlocking lines and keeps the exact same 7x13B used on 128px panels.
    // The status lines keep wqy13 - it is the only font in the table with CJK
    // glyphs, and a smaller one would break Chinese messages entirely.
    // 64 宽画布：7x13B 单行要 77px，会被切掉。这里不缩字体，而是把标题拆成
    // 两行咬合排布，字体与 128 屏完全一致。
    // 状态行保留 wqy13 —— 字体表里只有它带中文字形，换小字体中文会直接显示不出来。
    titleFont = u8g2_font_7x13B_tf;
    titleCharWidth = 7;
    errMsgFont = u8g2_font_4x6_tf;
    errMsgCharWidth = 4;
    titleTwoLine = true;
    titleLine2Y = 0.36f;

    setupPage = {MatrixCore(0.03f, 0.16f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 2, 14, 0)};

    // Vertical layout is retuned to make room for the second title line:
    // title 16 / 29 (13px apart = one line height), status 45, bar 50-58.
    // Horizontally the title starts at 0.08 -> x = 5, so "Matrix" spans
    // 5-46 and "Clock" 19-53, both inside the 64px canvas.
    // 纵向重排给标题第二行腾位置：标题基线 16 / 29（相差 13 即一个行高），
    // 状态行 45，进度条 50-58。横向标题从 0.08 起即 x = 5，
    // "Matrix" 占 5-46，"Clock" 占 19-53，都在 64px 画布内。
    loadingPage = {MatrixCore(0.15f, 0.16f, FONT_GROUP_9, FONT9_7X13B, 3, 0, 2, 12, 0),

                   MatrixCore(0.05f, 0.61f, FONT_GROUP_9, FONT9_WQY13, 0, 0, 2, 13, 0),

                   MatrixCore(0.1f, 0.72f, 0, 0, 3, 0, 3, 9, 0)};

    setupReady0 = MatrixCore(0.03f, 0.16f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 2, 7, 0);
    setupReady1 = MatrixCore(0.03f, 0.44f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 2, 8, 0);
    setupReady2 = MatrixCore(0.03f, 0.72f, FONT_GROUP_9, FONT9_WQY13, 3, 0, 3, 9, 0);

    errorWiFiMsg = MatrixCore(0.05f, 0.61f, FONT_GROUP_9, FONT9_WQY13, 0, 0, 2, 10, 0);

    errorNetWorkMsg = MatrixCore(0.05f, 0.61f, FONT_GROUP_9, FONT9_WQY13, 0, 0, 2, 11, 0);
    break;
  }
  default: {
    break;
  }
  }

  if (titleTwoLine && !loadingPage.empty()) {
    // Left aligned on the title element's own x rather than centred: the two
    // lines are positioned by hand, and centring them would fight the offset.
    // "Matrix" starts at x, "Clock" one offset further right.
    // 按标题元素自身的 x 左对齐，而不是居中：两行是手工摆位的，
    // 再做居中会和错位量打架。"Matrix" 从 x 起，"Clock" 再右移一个错位量。
    titleX = g_panelWidthChain * loadingPage[0].x;
    titleX2 = titleX + TITLE_LINE2_OFFSET_CHARS * titleCharWidth;
  } else {
    titleX = (g_panelWidthChain - TITLE_LEN * titleCharWidth) / 2;
    titleX2 = titleX;
  }
  if (titleX < 0) {
    titleX = 0;
  }
  if (titleX2 < 0) {
    titleX2 = 0;
  }
}

void Loading::drawFrame() {
    dma_display->fillRoundRect(barX, barY, barWidth, barHeight,radius, dma_display->color565(150, 150, 150));
    dma_display->flipDMABuffer();
}

void Loading::updateProgress(float percent) {
    int fillWidth =
        (int)((barWidth - 2) * percent + 0.5f);
    dma_display->fillRect(barX + 1, barY + 1, barWidth - 2, barHeight - 2,
                          dma_display->color565(0, 0, 0));
   if (fillWidth > 0) {
      dma_display->fillRect(barX + 1, barY + 1, fillWidth, barHeight - 2,
                            dma_display->color565(0, 200, 0));
    }
}

void Loading::showSetupMsg() {
  const char *text;
  unsigned long elapsed = millis() - lastMillisTime;
  float percent;
  if (elapsed <= durationSetup) {
    percent = (float)elapsed / durationSetup;
  } else {
    setupFlag = -setupFlag;
    lastMillisTime = durationSetup + lastMillisTime;
    percent = 0;
  }
  // The scroll distance is one full canvas width. It used to be hard coded to
  // 128, which on a 64px panel made the text jump two screens per cycle.
  // 滚动距离就是一整屏宽度。原来写死 128，在 64 宽屏上等于一次滚两屏，所以会跳。
  const int16_t scrollSpan = g_panelWidthChain;
  for (MatrixCore &matrixCore : setupPage) {
      text = matrixSettings.getCommonWord(
          static_cast<CommonWordIndex>(matrixCore.displayIndex));
      const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
          matrixCore.fontGroupIndex, matrixCore.fontIndex);
      FontMetrics fontABCMetrics =
          display.getFontMetrics(fontInfo->fontName, "A");
      int x1;
      int x2;
      if (setupFlag >= 0) {
        x1 = g_panelWidthChain * matrixCore.x - percent * scrollSpan;
        x2 = x1 + scrollSpan;
      } else {
        x1 = g_panelWidthChain * matrixCore.x + (1 - percent) * scrollSpan;
        x2 = x1 - scrollSpan;
      }
      int y = g_panelHeightChain * matrixCore.y + fontABCMetrics.height / 2;
      uint16_t colorRGB565 =
          matrixColorManager.getColor(matrixCore.colorIndex1);
      display.displayText(colorRGB565, x1, y, fontInfo->fontName, text);

      display.displayText(colorRGB565, x2, y, fontInfo->fontName, text);
    }
}

boolean Loading::loadingAnimation(){
  const char *text;
  CharCount charCount;
  unsigned long elapsed = millis() - lastMillisTime;
  float percent = (float)elapsed / durationLoading;
  if (percent > 1.0f) {
    percent = 1.0f; // 限制最大100%
    return true;
  }
  if (isCount()) {
    for (size_t i = 0; i < loadingPage.size(); ++i) {
      MatrixCore &matrixCore = loadingPage[i];

      const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
          matrixCore.fontGroupIndex, matrixCore.fontIndex);
      FontMetrics fontABCMetrics =
          display.getFontMetrics(fontInfo->fontName, "A");
      int y = g_panelHeightChain * matrixCore.y + fontABCMetrics.height / 2;
      text = matrixSettings.getCommonWord(
          static_cast<CommonWordIndex>(loadingPage[i].displayIndex));
      charCount = display.analyzeCharInStr(text);
      int16_t strSingleWidth = (fontABCMetrics.charWidth + 1) * charCount.countABC;
      uint16_t colorRGB565 =
          matrixColorManager.getColor(matrixCore.colorIndex1);
      int16_t x;
      switch (i) {
      case 0:
        if (titleTwoLine) {
          // Fixed two line title. The text comes from the constants rather
          // than getCommonWord() because the split point is part of the
          // layout - "MatrixClock" is a product name and never localised.
          // 固定的两行标题。文字取自常量而非 getCommonWord()，因为拆分点属于
          // 排版的一部分；"MatrixClock" 是产品名，不做本地化。
          int y2 = g_panelHeightChain * titleLine2Y + fontABCMetrics.height / 2;
          display.displayText(colorRGB565, titleX, y, fontInfo->fontName,
                              TITLE_LINE1);
          display.displayText(colorRGB565, titleX2, y2, fontInfo->fontName,
                              TITLE_LINE2);
          break;
        }
        x = g_panelWidthChain * matrixCore.x - strSingleWidth / 2;
        // strSingleWidth adds a 1px gap per character that u8g2 already
        // includes in its advance, so it over-estimates and can push a wide
        // string past the left edge. Clamping keeps the first column visible.
        // strSingleWidth 每个字符多算了 1px 间距（u8g2 的 advance 已含间距），
        // 属于高估，长串会被推出左边界。夹住 0 保证首列不被切。
        if (x < 0) {
          x = 0;
        }
        display.displayText(colorRGB565, x, y, fontInfo->fontName, text);
        break;
      case 1:
        x = g_panelWidthChain * matrixCore.x;
        display.displayText(colorRGB565, x, y, fontInfo->fontName, text);
        break;
      default:
        break;
      }
    }
  }
  MatrixCore &matrixCore = loadingPage[2];
  uint16_t colorRGB565 = matrixColorManager.getColor(matrixCore.colorIndex1);
  int x = g_panelWidthChain * matrixCore.x;
  int y = g_panelHeightChain * matrixCore.y + barHeight / 2;
  int bX = x + 1;
  int bY = y + 1;

  dma_display->fillRoundRect(x, y, barWidth, barHeight, radius,
                             dma_display->color565(150, 150, 150));

  int fillWidth = (int)((barWidth - 2) * percent + 0.5f);
  dma_display->fillRect(bX, bY, barWidth - 2, barHeight - 2,
                        dma_display->color565(0, 0, 0));
  if (fillWidth > 0) {
    dma_display->fillRect(bX, bY, fillWidth, barHeight - 2,
                          dma_display->color565(0, 200, 0));
  }
    dma_display->flipDMABuffer();
    return false;
}

boolean Loading::isCount(){
    if(count == 0){
        return true;
    }
    count++;
    return false;
}
void Loading::switchSetupReady() {
  setupPage.resize(3);
  setupPage[0] = setupReady0;
  setupPage[1] = setupReady1;
  setupPage[2] = setupReady2;
}
void Loading::switchWiFiErr() {
    loadingPage[1] = errorWiFiMsg;
}
void Loading::switchNetWorkErr() {
    loadingPage[1] = errorNetWorkMsg;
}

void Loading::drawTitle() {
    if (titleTwoLine) {
        display.displayTextRGB(0, 255, 0, titleX, midPanelHeight, titleFont, TITLE_LINE1);
        display.displayTextRGB(0, 255, 0, titleX2, midPanelHeight + 13, titleFont, TITLE_LINE2);
        return;
    }
    display.displayTextRGB(0, 255, 0, titleX, midPanelHeight, titleFont, TITLE_TEXT);
}

void Loading::drawErrMsg() {
    // Centred on the actual message length instead of the old fixed offset,
    // which assumed an 11 character string on a 128px canvas.
    // 按消息实际长度居中，不再用原来那个假设「128 画布 + 11 字符」的固定偏移。
    int errMsgX = (g_panelWidthChain - (int)message.length() * errMsgCharWidth) / 2;
    if (errMsgX < 0) {
        errMsgX = 0;
    }
    display.displayTextRGB(255, 0, 0, errMsgX, midPanelHeight + 13, errMsgFont, message.c_str());
}

void Loading::flipDMABuffer(){
    dma_display->flipDMABuffer();
}

void Loading::clearScreen() { 
    dma_display->clearScreen();
}

void Loading::setFlag(bool value) {
    flag = value;
}

bool Loading::getFlag() const {
    return flag;
}

void Loading::setMessage(String msg) {
    message = msg;
}

String Loading::getMessage(){
    return message;
}

void Loading::setDuration(unsigned long dur){
    durationLoading = dur;
}

unsigned long Loading::getDuration(){
    return durationLoading;
}

void Loading::setLastMillsTime(){
    lastMillisTime = millis();
}