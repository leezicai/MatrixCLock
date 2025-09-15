#pragma once
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "common_define.h"
#include "display.h"

// 外部全局对象
extern MatrixPanel_I2S_DMA *dma_display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

// Loading 类
class Loading {
public:
    Loading();

    void drawFrame();   // 方法一: 绘制进度条外框
    void drawTitle();        // 方法二: 绘制标题
    void drawErrMsg();
    void updateProgress(float percent); // 方法三: 更新进度条
    void showSetupMsg();
    void flipDMABuffer();
    void clearScreen();

    void setFlag(bool flag);  // 设置标记
    bool getFlag() const;     // 获取标记

    void setMessage(String message);
    String getMessage();

    void setDuration(unsigned long duration);
    unsigned long getDuration();

  private:
    int barX, barY, midPaneEhight, titleX, errMsgX, barWidth, barHeight, radius;
    unsigned long duration;
    String message;
    bool flag; // true = 正常加载，false = 暂停
};

// ✅ 全局对象声明
extern Loading loading;