// #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // 不再需要，已包含在 dma.h 中
#include "dma.h"    // <--- 包含 DMA 设置头文件 (Include DMA setup header)
#include "net.h"    // 包含网络模块头文件 (Include Network module header)
#include "ds3231.h" // 添加DS3231头文件
#include "task.h"
#include "btn.h"
#include "ec11.h"
#include "ec11_handler.h" // 引入我们的编码器事件处理模块
#include "data.h"
#include "setAP.h"

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

// 状态消息显示位置
#define STATUS_X 5
#define STATUS_Y 40 // 调整 Y 位置以避免与时间重叠

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "ESP32-S3 Time: %Y-%m-%d %H:%M:%S");
}

void printRTCTime()
{
  char dateTime[32];
  rtc.getStringDateTime(dateTime, sizeof(dateTime));
  Serial.print("DS3231 Time (UTC): ");
  Serial.println(dateTime);
}

// 辅助函数：在屏幕上显示状态消息
void showStatusMessage(const char *message)
{
  if (!dma_display)
    return; // 安全检查

  // 仅清除状态消息区域
  dma_display->fillRect(STATUS_X, STATUS_Y - 15, PANEL_WIDTH - STATUS_X * 2, 20, COLOR_BLACK);
  dma_display->setCursor(STATUS_X, STATUS_Y);
  dma_display->setTextColor(COLOR_WHITE);
  dma_display->print(message);

  // 立即更新显示
  dma_display->flipDMABuffer();
}

// displayTime 函数
void displayTime(const char *timeToShow)
{
  if (strcmp(timeToShow, lastDisplayedTimeString) != 0)
  {
    if (dma_display)
    {
      dma_display->setTextColor(COLOR_WHITE, COLOR_BLACK);
      dma_display->fillScreen(COLOR_BLACK); // 清除整个屏幕

      // 分离日期和时间部分以便分行显示
      char datePart[11]; // YYYY-MM-DD + 结束符
      char timePart[9];  // HH:MM:SS + 结束符

      // 提取日期部分
      strncpy(datePart, timeToShow, 10);
      datePart[10] = '\0';

      // 提取时间部分
      strncpy(timePart, timeToShow + 11, 8);
      timePart[8] = '\0';

      // 显示日期在第一行
      dma_display->setCursor(FIXED_X, FIXED_Y - 18); // 日期位置
      dma_display->print(datePart);

      // 显示时间在第二行
      dma_display->setCursor(FIXED_X, FIXED_Y + 10); // 时间位置
      dma_display->print(timePart);

      dma_display->flipDMABuffer();
      strcpy(lastDisplayedTimeString, timeToShow); // 更新记录
    }
    else
    {
      Serial.println("Error: dma_display is null in displayTime!");
      strcpy(lastDisplayedTimeString, timeToShow); // 仍然更新记录
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nESP32 HUB75 WiFi Clock Starting (With RTC Support)");

  buttonManager.begin();

  // 初始化编码器
  // EC11* encoder = EC11::getInstance(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, 4);  // 步数为1
  encoder->begin(true, 0, 100);

  // 配置按钮计时参数（可选）
  // 参数: 去抖时间(ms), 长按时间(ms), 双击时间间隔(ms)
  encoder->setButtonTiming(30, 800, 300);

  // 设置初始位置（可选）
  encoder->setPosition(50);

  // 初始化编码器事件处理器
  initEncoderHandlers();

  // Method 1: Initialize DS3231
  if (rtc.begin(SDA, SCL))
  {
    Serial.println("DS3231 initialized successfully");
  }
  else
  {
    Serial.println("Failed to initialize DS3231");
  }
  AppData.init();
  Serial.println("AppData1");
  Serial.println(AppData.getWifiConfigured());
  Serial.println("AppData2");
  if (AppData.getWifiConfigured())
  {
    initTasks();
  }

  // task必须在设备初始化之后

  // Method 2: Check if DS3231 is connected
  if (rtc.isConnected())
  {
    Serial.println("DS3231 is connected");

    // Read current time from DS3231
    printRTCTime();

    // Read temperature from DS3231
    Serial.print("DS3231 Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
  }
  else
  {
    Serial.println("DS3231 is not connected");
  }

  if (rtc.syncTimeFromRTC())
  {
    Serial.println("Time synced from DS3231 to ESP32-S3 successfully");
    printLocalTime();
  }
  else
  {
    Serial.println("Failed to sync time from DS3231");
  }

  // 初始化DS3231
  Serial.println("Initializing DS3231 RTC module...");

  // 初始化 DMA 显示
  dma_display = setupDMA(10);
  if (!dma_display)
  {
    Serial.println("DMA Display setup failed, halting execution.");
    while (1)
    {
      delay(1000);
    }
  }
  // 基本显示设置
  dma_display->fillScreen(COLOR_BLACK);
  dma_display->setTextColor(COLOR_WHITE, COLOR_BLACK);

  // 处理时间来源优先级：首先尝试外置RTC，然后尝试网络
  char initialTimeStr[20] = "////-//-// //://://"; // 增加字符串长度
  bool timeSet = false;

  // 初始化同步时间记录变量
  lastRtcSyncTime = millis();
  if (!lastNtpSyncTime)
  { // 如果NTP同步没有成功，也初始化这个变量
    lastNtpSyncTime = millis();
  }

  // 首次显示时间
  Serial.print("Initial time display: ");
  Serial.println(initialTimeStr);
  displayTime(initialTimeStr); // 显示时间会覆盖状态消息区域
  lastTimeUpdate = millis();
  int count = 0;
  while (!AppData.getWifiConfigured())
  {
        if(count == 0)
        {
            Serial.println("Starting WiFi setup mode");
            wifiManager.beginSetup();
            count = 2;
        }
        wifiManager.handleClient();
        buttonManager.tick();
  }
}

void loop()
{

  unsigned long currentMillis = millis();

  // 每秒更新一次时间显示
  if (currentMillis - lastTimeUpdate >= 1000)
  {
    char currentTimeStr[20]; // 增加字符串长度
    if (getCurrentTimeString(currentTimeStr, sizeof(currentTimeStr)))
    {
      displayTime(currentTimeStr);
    }
    else
    {
      displayTime("0000-00-00 00:00:00"); // 修改默认时间格式
    }
    lastTimeUpdate = currentMillis;
  }

  buttonManager.tick();

  // 处理编码器事件 - 现在只需要这一行代码就可以处理所有编码器事件
  handleEncoderEvents(encoder);
}