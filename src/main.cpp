// #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // 不再需要，已包含在 dma.h 中
#include "dma.h" // <--- 包含 DMA 设置头文件 (Include DMA setup header)
#include "net.h" // 包含网络模块头文件 (Include Network module header)
#include "ds3231.h" // 添加DS3231头文件
#include <customFonts/FreeSans16pt7b.h> // 使用自定义字体

// 固定的显示位置 - 屏幕居中位置
#define FIXED_X 0   // 固定的X位置 (128宽度屏幕)
#define FIXED_Y 40  // 固定的Y位置 (64高度屏幕)


// 在全局变量部分添加以下变量
unsigned long lastNtpSyncTime = 0;   // 上次NTP同步时间
unsigned long lastRtcSyncTime = 0;   // 上次RTC同步系统时间
#define NTP_SYNC_INTERVAL 43200000   // 12小时 = 12 * 60 * 60 * 1000 毫秒
#define RTC_SYNC_INTERVAL 21600000   // 6小时 = 6 * 60 * 60 * 1000 毫秒


// 定义颜色
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F

// 用于双缓冲的变量 - 保持全局指针
// 全局变量
MatrixPanel_I2S_DMA *dma_display = nullptr;
unsigned long lastTimeUpdate = 0;
char lastDisplayedTimeString[9] = "";
bool hasExternalRTC = false; // 新增：跟踪是否有外置RTC

// 状态消息显示位置
#define STATUS_X 5
#define STATUS_Y 40 // 调整 Y 位置以避免与时间重叠

// 辅助函数：在屏幕上显示状态消息
void showStatusMessage(const char* message) {
  if (!dma_display) return; // 安全检查

  // 仅清除状态消息区域
  dma_display->fillRect(STATUS_X, STATUS_Y - 15, PANEL_WIDTH - STATUS_X*2, 20, COLOR_BLACK);
  dma_display->setCursor(STATUS_X, STATUS_Y);
  dma_display->setTextColor(COLOR_WHITE);
  dma_display->print(message);
  
  // 立即更新显示
  dma_display->flipDMABuffer();
}

// displayTime 函数
void displayTime(const char* timeToShow) {
    if (strcmp(timeToShow, lastDisplayedTimeString) != 0) {
        Serial.print("Updating display: ");
        Serial.print(lastDisplayedTimeString);
        Serial.print(" -> ");
        Serial.println(timeToShow);
        if (dma_display) {
            // 确保字体和颜色是用于时间的
            dma_display->setFont(&FreeSans16pt7b);
            dma_display->setTextColor(COLOR_WHITE, COLOR_BLACK);

            dma_display->fillScreen(COLOR_BLACK); // 清除整个屏幕
            dma_display->setCursor(FIXED_X, FIXED_Y); // 时间的位置
            dma_display->print(timeToShow);
            dma_display->flipDMABuffer();
            strcpy(lastDisplayedTimeString, timeToShow); // 更新记录
            Serial.println("Display updated");
        } else {
            Serial.println("Error: dma_display is null in displayTime!");
            strcpy(lastDisplayedTimeString, timeToShow); // 仍然更新记录
        }
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 HUB75 WiFi Clock Starting (With RTC Support)");

  // 初始化DS3231
  Serial.println("Initializing DS3231 RTC module...");
  hasExternalRTC = initDS3231();
  if (hasExternalRTC) {
      Serial.println("DS3231 RTC module detected and initialized");
  } else {
      Serial.println("No DS3231 RTC module detected");
  }

  // 初始化 DMA 显示
  dma_display = setupDMA(10);
  if (!dma_display) {
      Serial.println("DMA Display setup failed, halting execution.");
      while (1) { delay(1000); }
  }
  // 基本显示设置
  dma_display->fillScreen(COLOR_BLACK);
  dma_display->setTextColor(COLOR_WHITE, COLOR_BLACK);
  dma_display->setFont(&FreeSans16pt7b);

  // 使用网络模块
  setupNet();

  // 处理时间来源优先级：首先尝试外置RTC，然后尝试网络
  char initialTimeStr[9] = "00:00:00";
  bool timeSet = false;

  // 1. 首先尝试从外置RTC获取时间
  if (hasExternalRTC) {
      showStatusMessage("RTC Read...");
      if (getTimeFromRTC()) { // 这是我们在net.cpp中添加的新函数
          showStatusMessage("RTC OK");
          delay(1000);
          getCurrentTimeString(initialTimeStr, sizeof(initialTimeStr));
          timeSet = true;
          bool wifiConnected = connectNetWithRetry();
          if(wifiConnected){
            bool timeSynced = syncNtpTime(); 
          }
          disconnectNet();
      } else {
          showStatusMessage("RTC Fail");
          delay(1000);
      }
  }

  // 2. 如果外置RTC没有可用时间，尝试WiFi/NTP
  if (!timeSet) {
      showStatusMessage("WiFi Conn..."); // 显示初始连接消息
      bool wifiConnected = connectNetWithRetry(); // 尝试连接 WiFi (阻塞)

      if (wifiConnected) {
          showStatusMessage("WiFi OK"); // 显示连接成功
          delay(1000); // 短暂显示

          showStatusMessage("NTP Syn..."); // 显示NTP同步消息
          delay(500);

          bool timeSynced = syncNtpTime(); // 尝试同步 NTP 时间 (阻塞)

          if (!timeSynced) {
              Serial.println("Failed to sync NTP time initially.");
              showStatusMessage("NTP Fail"); // 显示NTP失败
              delay(2000);
              // 保持 initialTimeStr 为 "00:00:00"
          } else {
              Serial.println("NTP Sync OK.");
              showStatusMessage("NTP OK"); // 显示NTP成功
              delay(1000);
              getCurrentTimeString(initialTimeStr, sizeof(initialTimeStr));
              timeSet = true;
              
              // 初始化lastNtpSyncTime变量，记录第一次成功同步的时间
              lastNtpSyncTime = millis();
          }

          // 时间同步后断开 WiFi
          disconnectNet();
          delay(1000);
      } else {
          Serial.println("Failed to connect to WiFi. Using default time.");
          showStatusMessage("WiFi Fail"); // 显示WiFi失败
          delay(2000);
          // 保持 initialTimeStr 为 "00:00:00"
      }
  }

  // 初始化同步时间记录变量
  lastRtcSyncTime = millis();
  if (!lastNtpSyncTime) { // 如果NTP同步没有成功，也初始化这个变量
      lastNtpSyncTime = millis();
  }

  // 首次显示时间
  Serial.print("Initial time display: ");
  Serial.println(initialTimeStr);
  displayTime(initialTimeStr); // 显示时间会覆盖状态消息区域
  lastTimeUpdate = millis();
}

void loop() {
  // 获取当前毫秒数，用于所有定时器
  unsigned long currentMillis = millis();
  
  // 每秒更新一次时间显示
  if (currentMillis - lastTimeUpdate >= 1000) {
      char currentTimeStr[9];
      if (getCurrentTimeString(currentTimeStr, sizeof(currentTimeStr))) {
          displayTime(currentTimeStr);
      } else {
          displayTime("00:00:00"); // 或 "No Time"
      }
      lastTimeUpdate = currentMillis;
  }

  // 定期从RTC同步系统时间 (每6小时一次)
  if (hasExternalRTC && (currentMillis - lastRtcSyncTime >= RTC_SYNC_INTERVAL)) {
      Serial.println("Performing scheduled RTC to system time sync...");
      struct tm timeinfo;
      if (readTimeFromRTC(&timeinfo)) {
          time_t t = mktime(&timeinfo);
          struct timeval now = { .tv_sec = t };
          settimeofday(&now, NULL);
          Serial.println("System time synchronized with RTC");
          
          // 更新最后同步时间
          lastRtcSyncTime = currentMillis;
      }
  }

  
  // 定期NTP同步 (每12小时一次)
  if (currentMillis - lastNtpSyncTime >= NTP_SYNC_INTERVAL) {
      Serial.println("Performing scheduled NTP time sync...");
      showStatusMessage("WiFi Conn...");
      
      // 尝试连接WiFi
      bool wifiConnected = connectNetWithRetry();
      
      if (wifiConnected) {
          showStatusMessage("NTP Syn...");
          bool timeSynced = syncNtpTime();
          
          if (timeSynced) {
              showStatusMessage("NTP OK");
              Serial.println("NTP sync successful");
              delay(1000);
              
              // 如果外置RTC可用，将同步后的时间更新到RTC
              // 注意：此操作已在syncNtpTime函数中处理
              
              // 更新最后NTP同步时间
              lastNtpSyncTime = currentMillis;
          } else {
              showStatusMessage("NTP Fail");
              Serial.println("NTP sync failed");
              delay(1000);
          }
          
          // 完成后断开WiFi连接
          disconnectNet();
      } else {
          showStatusMessage("WiFi Fail");
          Serial.println("Failed to connect to WiFi for NTP sync");
          delay(1000);
      }
      
      // 即使失败，也更新最后尝试时间，避免持续失败的尝试
      if (!wifiConnected) {
          lastNtpSyncTime = currentMillis;
      }
      
      // 清除状态消息，恢复显示时间
      char currentTimeStr[9];
      if (getCurrentTimeString(currentTimeStr, sizeof(currentTimeStr))) {
          displayTime(currentTimeStr);
      }
  }
}