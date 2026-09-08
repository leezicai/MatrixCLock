// #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // 不再需要，已包含在 dma.h 中
#include "brightnessManager.h"
#include "btn.h"
#include "matrixDma.h"
#include "ds3231.h" // 添加DS3231头文件
#include "ec11.h"
#include "ec11_handler.h" // 引入我们的编码器事件处理模块
#include "net.h"          // 包含网络模块头文件 (Include Network module header)
#include "setAP.h"
#include "task.h"
#include "sht30.h"
#include <U8g2_for_Adafruit_GFX.h>
#include "matrixDisplay.h"
#include "page.h"
#include "loading.h"
#include "matrixCore.h"
#include "matrixData.h"
#include "matrixTimeData.h"
#include <iostream>
#include "max98357.h"
#include "alarm_iphone.h"
#include "alarm.h"

// 用于双缓冲的变量 - 保持全局指针
// 全局变量
MatrixPanel_I2S_DMA *dma_display = nullptr;
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

unsigned long lastMillisTime = 0;    // 上次执行时间
const unsigned long interval = 1000;
unsigned long duration = 3600; // 3.6秒加载动画
unsigned long elapsed = 0;

// Frame pacing for the HUB75 double buffer.
// flipDMABuffer() is non-blocking on the ESP32-S3: the buffer swap only takes
// effect once the DMA finishes the current sweep. Redrawing faster than one
// sweep would clear the buffer that is still being scanned out -> flicker.
// Must be >= one full frame period (min_refresh_rate in matrixDma.cpp).
unsigned long lastRenderTime = 0;
const unsigned long renderInterval = 33;  // ~30 fps

time_t lastSecond = 0;
time_t now;
TimeStrings timeNow;
TimeStrings timeNowNextSec;
TimeData timeDataNow;
TimeData timeDataNowNextSec;
DiffTimeStrings diffTimeStrings;
int16_t countUnderline = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 HUB75 WiFi Clock Starting (With RTC Support)");

  // Nvs同一空间只能初始化一次
  matrixNvsManager.initializeDefaults();

  // 初始化 DMA 显示
  if(matrixDmaManager.setupDMA()){
    dma_display = matrixDmaManager.getDisplay();
  } else {
    Serial.println("DMA Display setup failed, halting execution.");
  }
  brightnessManager.init(); // 在 nvs之后

  matrixFontManager.init();  // 在 nvs DMA brightnessManager  之后

  matrixCoreManager.initializeMatrixCores();

  matrixSettings.init();

  // Loading's layout depends on the panel geometry, which only exists after
  // setupDMA() has read the panel type from NVS. Its constructor runs during
  // static initialisation and cannot see it.
  // Loading 的布局依赖屏幕尺寸，而尺寸要等 setupDMA() 从 NVS 读出屏型后才有。
  // 它的构造函数在静态初始化阶段执行，看不到这些值。
  loading.init();

  // 温度传感器初始化
  if (sht30.begin()) {
    Serial.println("SHT30 sensor initialized successfully!");
  } else {
    Serial.println("Failed to initialize SHT30 sensor!");
  }

  buttonManager.begin();

  // 初始化编码器
  encoder->begin(true, 0, 100);

  // 配置按钮计时参数（可选）
  // 参数: 去抖时间(ms), 长按时间(ms), 双击时间间隔(ms)
  encoder->setButtonTiming(30, 800, 300);

  // 设置初始位置（可选）
  encoder->setPosition(50);

  // 初始化编码器事件处理器
  initEncoderHandlers();

  // 初始化DS3231
  rtc.begin(SDA, SCL);

  // 网络模块必须在 task 之前初始化：它设置 persistent(false) 和自动重连，
  // 而 method1Task 一启动就会去连 WiFi。
  // WiFi initialised before the tasks: setupNet() installs the persistence and
  // auto-reconnect policy that method1Task relies on the moment it starts.
  // The link is opened once here and then held for the whole session.
  setupNet();

  // task必须在设备初始化之后
  if (matrixDataManager.getWifiConfig()) {
    initTasks();
  }
  // Initialize MAX98357 I2S audio
  if (!max98357Manager.begin()) {
    Serial.println("Failed to initialize MAX98357!");
    while (1)
      delay(100);
  }

  alarmManager.begin();

  // 初始化 u8g2
  u8g2_for_adafruit_gfx.begin(*dma_display);

  int count = 0;
  unsigned long lastSetupRender = 0;
  while (!matrixDataManager.getWifiConfig()) {
    buttonManager.tick();
    if (count == 0) {
      Serial.println("Starting WiFi setup mode");
      loading.setLastMillsTime();
      loading.showSetupMsg();
      display.flipDMABuffer();
      wifiManager.beginSetup();
      count = 2;
      loading.setLastMillsTime();
      lastSetupRender = millis();
    } else if (millis() - lastSetupRender >= renderInterval) {
      // Same frame pacing as loop(): one redraw per DMA sweep at most.
      lastSetupRender = millis();
      loading.clearScreen();
      loading.switchSetupReady();
      loading.showSetupMsg();
      display.flipDMABuffer();
    }
    wifiManager.handleClient();
  }

  loading.setLastMillsTime();
  unsigned long lastLoadingRender = 0;
  while (matrixDataManager.getWifiConfig()) {
    buttonManager.tick();
    if (millis() - lastLoadingRender >= renderInterval) {
      lastLoadingRender = millis();
      if (loading.loadingAnimation()) {
        break;
      }
    }
  }

  lastMillisTime = millis();
  now = time(nullptr);
  timeNow = display.getTimeStrings(now);
  timeNowNextSec = display.getTimeStrings(now + 1);
  diffTimeStrings = display.compareTimeStrings(timeNow, timeNowNextSec);
  timeDataNow = matrixTimeData.getTimeDataFromTimestamp(now);
  timeDataNowNextSec = matrixTimeData.getTimeDataFromTimestamp(now + 1);
}

void loop() {

  max98357Manager.loop();

  now = time(nullptr);
  elapsed = millis() - lastMillisTime;

  // 每秒更新一次时间显示
  if (elapsed >= 1000) {

    timeNow = display.getTimeStrings(now);
    timeNowNextSec = display.getTimeStrings(now + 1);
    diffTimeStrings = display.compareTimeStrings(timeNow, timeNowNextSec);
    timeDataNow = matrixTimeData.getTimeDataFromTimestamp(now);
    timeDataNowNextSec = matrixTimeData.getTimeDataFromTimestamp(now + 1);
    lastSecond = now;
    if(matrixCoreManager.getLineFlagTime() < 6){
      matrixCoreManager.setLineFlagTime(matrixCoreManager.getLineFlagTime() + 1);
    }
    if(matrixCoreManager.getPageFlagTime() < 6){
      matrixCoreManager.setPageFlagTime(matrixCoreManager.getPageFlagTime() + 1);
    }
    lastMillisTime = lastMillisTime + elapsed;
    // The strings above have just advanced to the new second, so the local
    // elapsed has to advance with them. Leaving it at its stale >= 1000 value
    // makes the render block below draw a frame that mixes the new second's
    // data with the previous second's animation progress - a one-frame wrong
    // digit that only shows up when the 1000ms tick and the 33ms render land
    // in the same loop iteration.
    // 秒进位后数据已经翻篇，elapsed 必须跟着重算。否则渲染块会拿到
    // "新数据 + 旧进度"，在进位和渲染撞到同一次循环时闪一帧错误数字。
    elapsed = millis() - lastMillisTime;
  }

  if (millis() - lastRenderTime >= renderInterval) {
    lastRenderTime = millis();

    const SecondaryPage *secondaryPage =
        matrixCoreManager.getCurrentSecondaryPage();
    if (secondaryPage != nullptr) {
      display.clearScreen();

      for (size_t i = 0; i < secondaryPage->size(); ++i) {
        const auto &matrixCore = (*secondaryPage)[i];
        if (matrixCoreManager.getLineFlagTime() > 4 ||
            i != matrixCoreManager.getCurrentElementGroupIndex()) {
          display.displayString(elapsed, timeDataNow, timeDataNowNextSec,
                                matrixCore, false);
        } else {
          display.displayString(elapsed, timeDataNow, timeDataNowNextSec,
                                matrixCore, true);
        }
      }
      if (matrixCoreManager.getPageFlagTime() < 5) {
        display.showPageInfo();
      }
      display.flipDMABuffer();
    }
  }

  buttonManager.tick();

  // 处理编码器事件 - 现在只需要这一行代码就可以处理所有编码器事件
  handleEncoderEvents(encoder);

  brightnessManager.handle(); // 这将处理自动亮度采样和更新
}