#pragma once
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "common_define.h"
#include "matrixDisplay.h"
#include "matrixDma.h"

// 外部全局对象
extern MatrixPanel_I2S_DMA *dma_display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

// Loading 类
class Loading {
public:
    Loading();

    // Builds every panel dependent layout value.
    // MUST be called from setup() AFTER MatrixDmaManager::setupDMA(), because
    // the constructor runs during static initialisation, when g_panelType and
    // g_panelWidthChain are still 0.
    // 必须在 setup() 里、setupDMA() 之后调用：构造函数运行于静态初始化阶段，
    // 那时 g_panelType / g_panelWidthChain 还是 0。
    void init();

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
    void setLastMillsTime();
    boolean loadingAnimation();
    boolean isCount();
    void switchSetupReady();
    void switchWiFiErr();
    void switchNetWorkErr();

  private:
    int barX, barY, midPanelHeight, titleX, barWidth, barHeight, radius;

    // Fonts used by drawTitle() / drawErrMsg(). Chosen in init() from the panel
    // width so an 11 character title still fits a 64px wide canvas.
    // drawTitle / drawErrMsg 用的字体，init() 里按屏宽挑选，
    // 保证 11 个字符的标题在 64px 画布上也能完整显示。
    const uint8_t *titleFont;
    const uint8_t *errMsgFont;
    int16_t titleCharWidth;
    int16_t errMsgCharWidth;

    // Two line interlocking title, used when the canvas is too narrow to fit
    // "MatrixClock" on one line without shrinking the font (64px wide panels).
    // titleX2 / titleLine2Y position the second line.
    // 窄画布（64px 宽）用的两行咬合标题：不缩字体，把标题拆成两行，
    // titleX2 / titleLine2Y 决定第二行的位置。
    bool titleTwoLine;
    int titleX2;
    float titleLine2Y;

    SecondaryPage setupPage;
    SecondaryPage loadingPage;
    MatrixCore setupReady0;
    MatrixCore setupReady1;
    MatrixCore setupReady2;
    MatrixCore errorWiFiMsg;
    MatrixCore errorNetWorkMsg;
    unsigned long durationLoading = DURATION_LOADING;
    unsigned long durationSetup = DURATION_SETUP;
    unsigned long lastMillisTime = 0;
    int8_t setupFlag = 1;
    int8_t countSetup = 0;
    String message;
    bool flag; // true = 正常加载，false = 暂停
    int16_t count;
};

// ✅ 全局对象声明
extern Loading loading;