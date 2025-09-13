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

// 固定的显示位置 - 屏幕居中位置
#define FIXED_X 0  // 固定的X位置 (128宽度屏幕)
#define FIXED_Y 40 // 固定的Y位置 (64高度屏幕)

// 在全局变量部分添加以下变量
unsigned long lastNtpSyncTime = 0; // 上次NTP同步时间
unsigned long lastRtcSyncTime = 0; // 上次RTC同步系统时间
#define NTP_SYNC_INTERVAL 43200000 // 12小时 = 12 * 60 * 60 * 1000 毫秒
#define RTC_SYNC_INTERVAL 21600000 // 6小时 = 6 * 60 * 60 * 1000 毫秒

// 定义颜色
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F

// 用于双缓冲的变量 - 保持全局指针
// 全局变量
MatrixPanel_I2S_DMA *dma_display = nullptr;
unsigned long lastTimeUpdate = 0;
char lastDisplayedTimeString[20] = "";
bool hasExternalRTC = false; // 新增：跟踪是否有外置RTC

SetAP wifiManager;

unsigned long lastTime = 0;    // 上次执行时间
const unsigned long interval = 1000; // 10 秒 = 10000 毫秒


U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;


// 状态消息显示位置
#define STATUS_X 5
#define STATUS_Y 40 // 调整 Y 位置以避免与时间重叠

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "ESP32-S3 Time: %Y-%m-%d %H:%M:%S");
}

void printRTCTime() {
  char dateTime[32];
  rtc.getStringDateTime(dateTime, sizeof(dateTime));
  Serial.print("DS3231 Time (UTC): ");
  Serial.println(dateTime);
}

// 辅助函数：在屏幕上显示状态消息
void showStatusMessage(const char *message) {
  if (!dma_display)
    return; // 安全检查

  // 仅清除状态消息区域
  dma_display->fillRect(STATUS_X, STATUS_Y - 15, PANEL_WIDTH - STATUS_X * 2, 20,
                        COLOR_BLACK);
  dma_display->setCursor(STATUS_X, STATUS_Y);
  dma_display->setTextColor(COLOR_WHITE);
  dma_display->print(message);

  // 立即更新显示
  dma_display->flipDMABuffer();
}



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
  // 基本显示设置
  dma_display->fillScreen(COLOR_BLACK);
  dma_display->setTextColor(COLOR_WHITE, COLOR_BLACK);

  u8g2_for_adafruit_gfx.begin(*dma_display);

  // 处理时间来源优先级：首先尝试外置RTC，然后尝试网络
  char initialTimeStr[20] = "////-//-// //://://"; // 增加字符串长度
  bool timeSet = false;

  // 初始化同步时间记录变量
  lastRtcSyncTime = millis();
  if (!lastNtpSyncTime) { // 如果NTP同步没有成功，也初始化这个变量
    lastNtpSyncTime = millis();
  }

 
  lastTimeUpdate = millis();
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
  unsigned long startTime = millis();
  unsigned long duration = 3600; // 3.6秒加载动画
  loading.drawFrame();
  loading.drawTitle();
  loading.flipDMABuffer();
  while (appData.getWifiConfigured()) {
    unsigned long elapsed = millis() - startTime;
    float percent = (float)elapsed / duration;
    if (percent > 1.0f) {
      percent = 1.0f; // 限制最大100%
    }
    loading.drawErrMsg();
    if (loading.getFlag() && elapsed <= duration) {
      loading.updateProgress(percent);
      loading.flipDMABuffer();
    } else {
      break;
    }
  }
}

void loop() {
  
  // u8g2_for_adafruit_gfx.setFont(u8g2_font_wqy13_t_gb2312);
  // u8g2_for_adafruit_gfx.setForegroundColor(dma_display->color565(255, 0, 0));
  // u8g2_for_adafruit_gfx.setCursor(0, 15);
  // u8g2_for_adafruit_gfx.print("你好1234世界");
  // dma_display->fillScreen(COLOR_BLACK); 
  // display.displayTextRGB(u8g2_for_adafruit_gfx, 255, 0, 0, 0, 15, u8g2_font_wqy13_t_gb2312, "你好1234世界");
  // TimeStrings timeInfo = display.getTimeStrings();
  // display.displayTextRGB(u8g2_for_adafruit_gfx, 255, 0, 0, 0, 30,
  //                        u8g2_font_wqy13_t_gb2312,
  //                        timeInfo.getDateString().c_str());
  // display.displayTextRGB(u8g2_for_adafruit_gfx, 0, 255, 0, 0, 45,
  //                        u8g2_font_wqy13_t_gb2312,
  //                        timeInfo.getTimeString24().c_str());
  // dma_display->flipDMABuffer();
  // switch(page.getCurrentFirstClassPage()){
  //   case PAGE_ZERO:
  //     Serial.print("PAGE_ZERO: ");
  //     switch(page.getCurrentSecondClassPage()){
  //       case PAGE_ZERO_ZERO:
  //         Serial.print(page.getCurrentSecondClassPage());
  //         break;
  //       default:
  //         Serial.println("default zero");
  //         break;
  //     }
  //     break;
  //   case PAGE_ONE:
  //      Serial.print("PAGE_ONE: ");
  //      switch(page.getCurrentSecondClassPage()){
  //       case PAGE_ONE_ZERO:
  //         Serial.print(page.getCurrentSecondClassPage());
  //         break;
  //       case PAGE_ONE_ONE:
  //         Serial.print(page.getCurrentSecondClassPage());
  //         break;
  //       case PAGE_ONE_TWO:
  //          Serial.print(page.getCurrentSecondClassPage());
  //         break;
  //       default:
  //         Serial.println("default one");
  //         break;
  //     }
  //     break;
  //   default:
  //     Serial.print("PAGE_UNKNOWN: ");
  //     break;
  // }
  

  time_t now = time(nullptr);
  TimeStrings timeNow = display.getTimeStrings(now);
  TimeStrings timeNowNextSec = display.getTimeStrings(now+1);
  unsigned long currentMillis = millis();

  if (currentMillis - lastTime >= interval) {

    
    lastTime = currentMillis;  // 记录这次的时间

    Serial.print("Temperature: ");
    Serial.print(sht30.getTemperature());
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(sht30.getHumidity());
    Serial.println(" %");
  }
  // 每秒更新一次时间显示
  if (currentMillis - lastTimeUpdate >= 1000) {

    dma_display->fillScreen(COLOR_BLACK);
    display.displayTextRGB(255, 0, 0, 0, 15, u8g2_font_wqy13_t_gb2312,
                           "你好1234世界");

    display.displayTextRGB(255, 0, 0, 0, 30, u8g2_font_wqy13_t_gb2312,
                           timeNow.getDateString().c_str());
    display.displayTextRGB(0, 255, 0, 0, 45, u8g2_font_wqy13_t_gb2312,
                           timeNow.getTimeString24().c_str());
    dma_display->flipDMABuffer();
  }

  buttonManager.tick();

  // 处理编码器事件 - 现在只需要这一行代码就可以处理所有编码器事件
  handleEncoderEvents(encoder);

  brightnessManager.handle(); // 这将处理自动亮度采样和更新
}