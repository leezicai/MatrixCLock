#include "loading.h"

Loading loading;

Loading::Loading(){
  barX = PANEL_WIDTH * PANEL_CHAIN * 0.2 / 2;
  barY = PANEL_HEIGHT * PANEL_CHAIN * 0.78;
  midPaneEhight = PANEL_HEIGHT * PANEL_CHAIN / 2;
  titleX = (PANEL_WIDTH * PANEL_CHAIN - (7 * 11 + 10)) / 2;
  errMsgX = (PANEL_WIDTH * PANEL_CHAIN - (7 * 11 + 10)) / 2 - 10;
  barWidth = PANEL_WIDTH * 0.8;
  barHeight = 8;
  radius = 1;
  flag = true; // ✅ 默认就是 true
  message = "";
  duration = DURATION;
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
    display.displayTextRGB(0, 255, 0, 0, 15, u8g2_font_wqy13_t_gb2312, SETUP_WIFI_MSG);
    display.displayTextRGB(0, 255, 0, 0, 30, u8g2_font_6x10_tf, SETUP_WIFI_NAME);
    display.displayTextRGB(0, 255, 0, 0, 45, u8g2_font_wqy13_t_gb2312, SETUP_WIFI_ADDRESS);
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