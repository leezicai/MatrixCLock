#include "task.h"
#include "ds3231.h"
#include "net.h"

// 任务句柄
TaskHandle_t method1TaskHandle = NULL;
TaskHandle_t method2TaskHandle = NULL;

// DS3231 rtc_task;

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
  
  Serial.println("所有任务已初始化");
}

// Method1任务函数 - 每24小时执行一次
void method1Task(void * parameter) {
  for(;;) {
    // 执行方法1
    Serial.println("执行方法1");
    connectNetWithRetry();
    if (isWiFiConnected())
    {
        rtc.syncNtpTime();
        rtc.begin(SDA, SCL);
        rtc.syncTimeToRTC();
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
    rtc.syncTimeToRTC();
    
    // 休眠12小时
    vTaskDelay(12 * 60 * 60 * 1000 / portTICK_PERIOD_MS);
  }
}