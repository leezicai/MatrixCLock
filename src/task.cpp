#include "task.h"
#include "ds3231.h"
#include "net.h"
#include "sht30.h"  // 新增SHT30头文件
#include "loading.h"
#include "common_define.h"
#include "alarm.h"

// 任务句柄
TaskHandle_t method1TaskHandle = NULL;
TaskHandle_t method2TaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;  // 新增传感器任务句柄
TaskHandle_t alarmTaskHandle = NULL;

// 初始化任务
void initTasks() {
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
  Serial.println("所有任务已初始化");
}

// Method1任务函数 - 每24小时执行一次
void method1Task(void * parameter) {
  for(;;) {
    // 执行方法1
    Serial.println("执行方法1");
    attemptWiFiConnectOnce();
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
    disconnectNet();
    
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
  // Initialize alarm manager
  alarmManager.begin();
  
  uint32_t checkCounter = 0;
  const uint32_t CHECKS_PER_HOUR = 360;  // 3600 seconds / 10 seconds = 360 checks
  
  for(;;) {
    // Check if any alarm should trigger
    alarmManager.checkAndTrigger();
    
    // Update task queue every hour (360 * 10 seconds = 3600 seconds = 1 hour)
    checkCounter++;
    if (checkCounter >= CHECKS_PER_HOUR) {
      Serial.println("Hourly alarm queue update...");
      alarmManager.updateTaskQueue();
      checkCounter = 0;
    }
    
    // Wait 10 seconds before next check
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
  }
}