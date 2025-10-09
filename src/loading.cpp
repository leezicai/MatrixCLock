#include "loading.h"

Loading loading;

Loading::Loading(){
  barX = PANEL_WIDTH * PANEL_CHAIN * 0.2 / 2;
  barY = PANEL_HEIGHT * PANEL_CHAIN * 0.78;
  midPaneEhight = PANEL_HEIGHT * PANEL_CHAIN / 2;
  titleX = (PANEL_WIDTH * PANEL_CHAIN - (7 * 11 + 10)) / 2;
  errMsgX = (PANEL_WIDTH * PANEL_CHAIN - (7 * 11 + 10)) / 2 - 10;
  barHeight = 8;
  radius = 1;
  flag = true; // ✅ 默认就是 true
  message = "";
//   duration = DURATION;
  count = 0;

  
  setupPage = {MatrixCore(0.03f, 0.16f, 10, 40, 3, 0, 2, 7, 0),

               MatrixCore(0.03f, 0.44f, 10, 40, 3, 0, 2, 8, 0),

               MatrixCore(0.03f, 0.72f, 10, 40, 3, 0, 3, 9, 0)};

  loadingPage = {MatrixCore(0.5f, 0.3f, 10, 4, 3, 0, 2, 12, 0),

                 MatrixCore(0.1f, 0.56f, 10, 40, 0, 0, 2, 13, 0),

                 MatrixCore(0.1f, 0.72f, 0, 0, 3, 0, 3, 9, 0)};

  errorWiFiMsg = MatrixCore(0.1f, 0.56f, 10, 40, 0, 0, 2, 10, 0);

  errorNetWorkMsg = MatrixCore(0.1f, 0.56f, 10, 40, 0, 0, 2, 11, 0);
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

void Loading::showSetupMsg(){
    const char *text;
    for (MatrixCore &matrixCore : setupPage) {
      text = matrixSettings.getCommonWord(
          static_cast<CommonWordIndex>(matrixCore.displayIndex));
      const FontInfo *fontInfo = matrixFontManager.getCurrentFont(
          matrixCore.fontGroupIndex, matrixCore.fontIndex);
      FontMetrics fontABCMetrics =
          display.getFontMetrics(fontInfo->fontName, "A");
      int x = g_panelWidthChain * matrixCore.x;
      int y = g_panelHeightChain * matrixCore.y + fontABCMetrics.height / 2;
      uint16_t colorRGB565 =
          matrixColorManager.getColor(matrixCore.colorIndex1);
   
      display.displayText(colorRGB565, x, y, fontInfo->fontName, text);
    } 
    // display.displayTextRGB(0, 255, 0, 0, 15, u8g2_font_wqy13_t_gb2312, SETUP_WIFI_MSG);
    // display.displayTextRGB(0, 255, 0, 0, 30, u8g2_font_6x10_tf, SETUP_WIFI_NAME);
    // display.displayTextRGB(0, 255, 0, 0, 45, u8g2_font_wqy13_t_gb2312, SETUP_WIFI_ADDRESS);
}

boolean Loading::loadingAnimation(){
  barWidth = g_panelWidthChain * 0.8;
  const char *text;
  CharCount charCount;
  unsigned long elapsed = millis() - lastMillisTime;
  float percent = (float)elapsed / duration;
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
        x = g_panelWidthChain * matrixCore.x - strSingleWidth / 2;
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
void Loading::switchWiFiErr() {
    loadingPage[1] = errorWiFiMsg;
}
void Loading::switchNetWorkErr() {
    loadingPage[1] = errorNetWorkMsg;
}

void Loading::drawTitle() {
    display.displayTextRGB(0, 255, 0, titleX, midPaneEhight, u8g2_font_7x13B_tf, "MatrixClock");
}

void Loading::drawErrMsg() {
    display.displayTextRGB(255, 0, 0, errMsgX, midPaneEhight + 13, u8g2_font_6x10_tf, message.c_str());
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
    duration = dur;
}

unsigned long Loading::getDuration(){
    return duration;
}

void Loading::setLastMillsTime(){
    lastMillisTime = millis();
}