// #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // 不再需要，已包含在 dma.h 中
#include "brightnessManager.h"
#include "btn.h"
#include "data.h"
#include "dma.h"    // <--- 包含 DMA 设置头文件 (Include DMA setup header)
#include "ds3231.h" // 添加DS3231头文件
#include "ec11.h"
#include "ec11_handler.h" // 引入我们的编码器事件处理模块
#include "net.h"          // 包含网络模块头文件 (Include Network module header)
#include "setAP.h"
#include "task.h"
#include "sht30.h"
#include <U8g2_for_Adafruit_GFX.h>
#include "display.h"
#include "page.h"
#include "loading.h"


#include <customFonts/FreeSans16pt7b.h> // 使用自定义字体


// 用于双缓冲的变量 - 保持全局指针
// 全局变量
MatrixPanel_I2S_DMA *dma_display = nullptr;
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

unsigned long lastMillisTime = 0;    // 上次执行时间
const unsigned long interval = 1000;
unsigned long duration = 3600; // 3.6秒加载动画
unsigned long elapsed = 0;

time_t lastSecond = 0;
time_t now;
TimeStrings timeNow;
TimeStrings timeNowNextSec;
DiffTimeStrings diffTimeStrings;

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 HUB75 WiFi Clock Starting (With RTC Support)");
  
  appData.init(); // 永久数据 优先初始化

  // 温度传感器初始化
  if (sht30.begin()) {
    Serial.println("SHT30 sensor initialized successfully!");
  } else {
    Serial.println("Failed to initialize SHT30 sensor!");
  }

  buttonManager.begin();

  // 初始化编码器
  // EC11* encoder = EC11::getInstance(ROTARY_ENCODER_A_PIN,
  // ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN,
  // 4);  // 步数为1
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

  // task必须在设备初始化之后
  if (appData.getWifiConfigured()) {
    initTasks();
  }


  // 初始化 DMA 显示
  dma_display = setupDMA(50);
  if (!dma_display) {
    Serial.println("DMA Display setup failed, halting execution.");
    while (1) {
      delay(1000);
    }
  }
  brightnessManager.init();

  // 初始化 u8g2
  u8g2_for_adafruit_gfx.begin(*dma_display);

  
  int count = 0;
  while (!appData.getWifiConfigured()) {
    if (count == 0) {
      Serial.println("Starting WiFi setup mode");
      loading.showSetupMsg();
      loading.flipDMABuffer();
      wifiManager.beginSetup();
      count = 2;
    }
    wifiManager.handleClient();
    buttonManager.tick();
  }
 
  lastMillisTime = millis();
  loading.drawFrame();
  loading.drawTitle();
  loading.flipDMABuffer();
  while (appData.getWifiConfigured()) {
    unsigned long elapsed = millis() - lastMillisTime;
    float percent = (float)elapsed / duration;
    if (percent > 1.0f) {
      percent = 1.0f; // 限制最大100%
    }
    loading.drawErrMsg();
    if (loading.getFlag() && elapsed < duration) {
      loading.updateProgress(percent);
      loading.flipDMABuffer();
    } else {
      loading.clearScreen();
      loading.flipDMABuffer();
      break;
    }
  }

  lastMillisTime = millis();
  now = time(nullptr);
  timeNow = display.getTimeStrings(now);
  timeNowNextSec = display.getTimeStrings(now + 1);
  diffTimeStrings = display.compareTimeStrings(timeNow, timeNowNextSec);
}

void loop() {

  
  now = time(nullptr);
  elapsed = millis() - lastMillisTime;
  
    // 每秒更新一次时间显示
  if (elapsed >= 1000) {

    timeNow = display.getTimeStrings(now);
    timeNowNextSec = display.getTimeStrings(now + 1);
    diffTimeStrings = display.compareTimeStrings(timeNow, timeNowNextSec);
    lastSecond = now;

    lastMillisTime = lastMillisTime + elapsed;
  }

  
  switch(page.getCurrentFirstClassPage()){
    case PAGE_0:
      switch(page.getCurrentSecondClassPage()){
        case PAGE_0_0:
          display.clearScreen();
          display.displayHourMinuteSecond(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 8, 12,
              u8g2_font_t0_22b_tn, "2", ":");
          display.flipDMABuffer();
          break;
        case PAGE_0_1:
          display.clearScreen();
          display.displayHourMinuteSecond(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 8, 12,
              u8g2_font_crox5hb_tf, "2", ":");
          display.flipDMABuffer();
          break;
        case PAGE_0_2:
          display.clearScreen();
          display.displayHourMinuteSecond(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 8, 12,
              u8g2_font_fub17_tf, "2", ":");
          display.flipDMABuffer();
          break;
        case PAGE_0_3:
          display.clearScreen();
          display.displayHourMinuteSecond(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 8, 12,
              u8g2_font_ncenB18_tn, "2", ":");
          display.flipDMABuffer();
          break;
        case PAGE_0_4:
          display.clearScreen();
          display.displayHourMinuteSecond(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 8, 12,
              u8g2_font_luBS18_tf, "2", ":");
          display.flipDMABuffer();
          break;
        case PAGE_0_5:
          display.clearScreen();
          display.displayHourMinuteSecond(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 8, 12,
              u8g2_font_crox4hb_tn, "2", ":");
          display.flipDMABuffer();
          break;
        default:
          break;
      }
      break;
    case PAGE_1:
       switch(page.getCurrentSecondClassPage()){
        case PAGE_1_0:
          display.clearScreen();
          display.displayHourMinute(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 12, 20,
              u8g2_font_helvB18_tf);
          display.flipDMABuffer();
          break;
        case PAGE_1_1:
          display.clearScreen();
          display.displayYearMonthDay(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 12, 20,
              u8g2_font_luBS19_tf);
          display.flipDMABuffer();
          break;
        case PAGE_1_2:
          display.clearScreen();
          display.displayMonthDay(
              elapsed, timeNow, timeNowNextSec, diffTimeStrings,
              page.getCurrentAnimationType(), 0xF800, 0, 30, 12, 20,
              u8g2_font_fur20_tf);
          display.flipDMABuffer();
          break;
        default:
          break;
      }
      break;
    default:
      Serial.print("PAGE_UNKNOWN: ");
      break;
  }
  





  buttonManager.tick();

  // 处理编码器事件 - 现在只需要这一行代码就可以处理所有编码器事件
  handleEncoderEvents(encoder);

  brightnessManager.handle(); // 这将处理自动亮度采样和更新
}