#include "task.h"
#include "ds3231.h"
#include "net.h"
#include "sht30.h"  // 新增SHT30头文件
#include "loading.h"
#include "common_define.h"
#include "alarm.h"
#include "stock.h"  // 新增股票指数模块

// 任务句柄
TaskHandle_t method1TaskHandle = NULL;
TaskHandle_t method2TaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;  // 新增传感器任务句柄
TaskHandle_t alarmTaskHandle = NULL;
TaskHandle_t stockTaskHandle = NULL;   // 新增股票指数任务句柄
TaskHandle_t netKeepAliveTaskHandle = NULL;  // WiFi 保活任务句柄

// Serialises the operations that lean on the radio. WiFi itself is brought up
// once at boot and never torn down, so this no longer brackets a connect and a
// disconnect - it just keeps an NTP sync and a reconnect attempt apart.
static SemaphoreHandle_t netMutex = NULL;

bool netMutexTake(uint32_t timeoutMs) {
  if (netMutex == NULL) {
    return true;  // not initialized yet, nothing else can be using the radio
  }
  return xSemaphoreTake(netMutex, timeoutMs / portTICK_PERIOD_MS) == pdTRUE;
}

void netMutexGive() {
  if (netMutex != NULL) {
    xSemaphoreGive(netMutex);
  }
}

// 初始化任务
void initTasks() {
  // 创建网络互斥锁（必须在任何任务启动之前）
  if (netMutex == NULL) {
    netMutex = xSemaphoreCreateMutex();
  }

  // 创建method1任务 - 每24小时执行一次
  xTaskCreate(
    method1Task,       // 任务函数
    "Method1_Task",    // 任务名称
    4096,              // 堆栈大小
    NULL,              // 任务参数
    1,                 // 优先级(1最低)
    &method1TaskHandle // 任务句柄
  );
  
  // 创建method2任务 - 每12小时执行一次
  xTaskCreate(
    method2Task,       // 任务函数
    "Method2_Task",    // 任务名称
    4096,              // 堆栈大小
    NULL,              // 任务参数
    1,                 // 优先级(1最低)
    &method2TaskHandle // 任务句柄
  );
  
  // 创建sensor任务 - 每3分钟执行一次SHT30读取
  xTaskCreate(
    sensorTask,        // 任务函数
    "Sensor_Task",     // 任务名称
    2048,              // 堆栈大小(传感器任务相对简单，可以使用较小的堆栈)
    NULL,              // 任务参数
    2,                 // 优先级(2，比其他任务稍高)
    &sensorTaskHandle  // 任务句柄
  );
  
   xTaskCreate(
    alarmTask,         // Task function
    "Alarm_Task",      // Task name
    4096,              // Stack size
    NULL,              // Task parameter
    3,                 // Priority (3, higher than sensor task)
    &alarmTaskHandle   // Task handle
  );

  // 创建stock任务 - 交易时段每分钟拉取一次指数
  // HTTPClient + WiFi stack needs a roomier stack than the sensor task, but
  // the Sina feed is plain HTTP and ~220 bytes, so this no longer has to
  // carry a TLS handshake and its buffers.
  xTaskCreate(
    stockTask,         // Task function
    "Stock_Task",      // Task name
    6144,              // Stack size (plain HTTP GET + a short response)
    NULL,              // Task parameter
    1,                 // Priority (1, lowest - display must never wait on it)
    &stockTaskHandle   // Task handle
  );

  // WiFi 保活 - 开机连上之后就不再断开，这个任务只负责掉线重连
  xTaskCreate(
    netKeepAliveTask,        // Task function
    "NetKeepAlive_Task",     // Task name
    3072,                    // Stack size
    NULL,                    // Task parameter
    1,                       // Priority (1, lowest)
    &netKeepAliveTaskHandle  // Task handle
  );

  Serial.println("所有任务已初始化");
}

// Method1任务函数 - 每24小时执行一次
void method1Task(void * parameter) {
  for(;;) {
    // 执行方法1
    Serial.println("执行方法1");

    // Hold the radio for the whole connect / sync / disconnect sequence so the
    // stock task cannot tear the connection down mid NTP sync.
    if (netMutexTake(60 * 1000)) {
      ensureWiFiConnected();
      if (isWiFiConnected()) {
        bool syncNtpTimeFlag = rtc.syncNtpTime();
        bool rtcFlag = rtc.begin(SDA, SCL);
        if (!syncNtpTimeFlag) {
          // loading.setMessage(LOADING_ERR_MSG_FAIL_NETWORK);
          loading.switchNetWorkErr();
          rtc.syncTimeFromRTC();
        } else {
          if(rtcFlag){
             bool syncTimeToRTCFlag = rtc.syncTimeToRTC();
          } else{
            rtc.syncTimeFromRTC();
          }
        }
      } else {
        // loading.setMessage(LOADING_ERR_MSG_FAIL_WIFI);
        loading.switchWiFiErr();
        rtc.syncTimeFromRTC();
      }
      // The link stays up from here on - no disconnectNet().
      netMutexGive();
    } else {
      Serial.println("方法1: 获取网络锁超时，跳过本次同步");
    }
    
    // 这里是method1的具体实现代码
    
    // 休眠24小时
    vTaskDelay(24 * 60 * 60 * 1000 / portTICK_PERIOD_MS);
  }
}

// Method2任务函数 - 每12小时执行一次
void method2Task(void * parameter) {
  for(;;) {
    // 执行方法2
    Serial.println("执行方法2");
    bool rtcFlag = rtc.begin(SDA, SCL);
    if(rtcFlag){
      rtc.syncTimeToRTC();
    }
    
    // 休眠4小时
    vTaskDelay(4 * 60 * 60 * 1000 / portTICK_PERIOD_MS);
  }
}

// 传感器任务函数 - 每3分钟执行一次SHT30读取
void sensorTask(void * parameter) {
  for(;;) {
    // 执行SHT30数据读取
    Serial.println("执行传感器数据读取");
    
    if (sht30.readData()) {
    } else {
      Serial.println("SHT30读取失败");
    }
    
    // 休眠10分钟 (10 * 60 * 1000 毫秒)
    vTaskDelay(10 * 60 * 1000 / portTICK_PERIOD_MS);
  }
}

void alarmTask(void * parameter) {
  // Loads the NVS config immediately so the settings page always shows real
  // values. The queue it builds here may be garbage when the clock has not
  // been synced yet - the jump detector below rebuilds it as soon as the wall
  // clock becomes real.
  alarmManager.begin();

  const uint32_t CHECK_INTERVAL_MS = 10 * 1000;
  const uint32_t CHECKS_PER_HOUR = 360;  // 3600 s / 10 s

  // The queue stores absolute timestamps, so any jump of the wall clock makes
  // it stale: NTP sync at boot, an RTC restore when offline, or a manual
  // time / timezone edit from the EC11 settings page. Normal drift across one
  // period is ~10 s, so anything beyond 30 s (or going backwards) is a jump.
  const long CLOCK_JUMP_TOLERANCE = 30;

  uint32_t checkCounter = 0;
  time_t lastNow = time(nullptr);

  for(;;) {
    time_t now = time(nullptr);
    long drift = (long)(now - lastNow);
    lastNow = now;

    if (drift < 0 || drift > CLOCK_JUMP_TOLERANCE) {
      Serial.printf("Clock jumped by %ld s, rebuilding alarm queue\n", drift);
      alarmManager.updateTaskQueue();
      checkCounter = 0;
    }

    // Check if any alarm should trigger
    alarmManager.checkAndTrigger();

    // Periodic refresh, once an hour
    checkCounter++;
    if (checkCounter >= CHECKS_PER_HOUR) {
      Serial.println("Hourly alarm queue update...");
      alarmManager.updateTaskQueue();
      checkCounter = 0;
    }

    // Wait 10 seconds before next check
    vTaskDelay(CHECK_INTERVAL_MS / portTICK_PERIOD_MS);
  }
}

// 股票指数任务
//
// Cadence is decided by StockManager, which knows the trading phase and
// whether the stock page is the one on screen:
//   09:30 - 15:00  once a minute while the stock page is showing
//   15:00 - 16:00  every 15 minutes, picking up the settled close
//   otherwise      every 10 minutes
//
// One GET covers all three indices, so a cycle is always exactly one request.
// There is no second call for a chart any more - the intraday series and the
// whole chart-scheduling half of this loop went with the Infoway integration.
void stockTask(void * parameter) {
  // Let method1Task finish its boot time NTP sync first: every scheduling
  // decision below depends on the wall clock being right.
  vTaskDelay(45 * 1000 / portTICK_PERIOD_MS);

  stockManager.begin();

  // Retry gap while the panel still has nothing to show at all. Without it a
  // boot onto a flaky link would sit blank until the next idle interval.
  const uint32_t COLD_RETRY_MS = 60UL * 1000UL;

  uint32_t lastQuoteMs = 0;
  bool firstRun = true;

  for(;;) {
    if (stockManager.isEnabled()) {
      // Housekeeping hook. Nothing expires on its own now that the chart is
      // gone; this only rolls the day counter.
      stockManager.maintainSession();

      StockPhase phase = stockManager.getPhase();

      // Cold start. Everything the panel shows lives in RAM, so after a reboot
      // there is nothing on screen and, outside trading hours, nothing would
      // ever fetch it. One catch-up pass runs whenever the cache is empty,
      // whatever the hour - at 08:35 on a Monday that pulls Friday's close,
      // which is what the page is supposed to show.
      bool coldQuote = !stockManager.hasData();

      uint32_t quoteInterval = stockManager.getQuoteIntervalMs();
      uint32_t sinceQuote = (uint32_t)(millis() - lastQuoteMs);

      bool quoteDue = false;
      if (phase != STOCK_PHASE_CLOSED) {
        quoteDue = firstRun || sinceQuote >= quoteInterval;
      } else if (coldQuote) {
        // Retry faster than the idle cadence while the screen is still blank.
        quoteDue = firstRun || sinceQuote >= COLD_RETRY_MS;
      }

      if (quoteDue) {
        if (netMutexTake(90 * 1000)) {
          ensureWiFiConnected();

          if (isWiFiConnected()) {
            // One request covers all three indices - the Sina list endpoint
            // takes a comma separated code list.
            stockManager.fetchQuotes();
            // Stamped either way: a failed fetch has to back off too.
            lastQuoteMs = millis();
            firstRun = false;
          } else {
            Serial.println("股票指数拉取跳过: WiFi未连接");
          }

          netMutexGive();
        } else {
          Serial.println("股票指数: 获取网络锁超时，跳过本次拉取");
        }
      }
    }

    // Wake often enough to catch a phase boundary or a page change, cheap
    // enough that it costs nothing when there is nothing to do.
    vTaskDelay(20 * 1000 / portTICK_PERIOD_MS);
  }
}

// WiFi 保活任务 - 开机连上之后链路一直保持，这里只处理掉线重连
void netKeepAliveTask(void * parameter) {
  // method1Task owns the boot time connect; give it a head start.
  vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);

  bool wasConnected = false;

  for(;;) {
    if (!isWiFiConnected()) {
      // WiFi.setAutoReconnect() covers most drops on its own. This is the
      // fallback for when the stack has stopped trying, typically after the
      // AP was unreachable for a while.
      if (netMutexTake(30 * 1000)) {
        if (!isWiFiConnected()) {
          Serial.println("WiFi 掉线，尝试重连");
          if (ensureWiFiConnected()) {
            Serial.println("WiFi 重连成功");
          }
        }
        netMutexGive();
      }
      wasConnected = false;
    } else if (!wasConnected) {
      // Only on the transition back to connected. A reconnect handled inside
      // the WiFi stack does not go through attemptWiFiConnectOnce(), so the
      // sleep setting has to be re-applied - but re-applying it every 30
      // seconds just spams the log and fights whatever the stock task is
      // doing to the radio mid-request.
      setWiFiLowPower();
      wasConnected = true;
    }

    vTaskDelay(30 * 1000 / portTICK_PERIOD_MS);
  }
}