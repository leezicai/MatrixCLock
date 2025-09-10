#include "task.h"
#include "ds3231.h"
#include "net.h"
#include "sht30.h"  // 新增SHT30头文件

// 任务句柄
TaskHandle_t method1TaskHandle = NULL;
TaskHandle_t method2TaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;  // 新增传感器任务句柄

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

// 传感器任务函数 - 每3分钟执行一次SHT30读取
void sensorTask(void * parameter) {
  for(;;) {
    // 执行SHT30数据读取
    Serial.println("执行传感器数据读取");
    
    if (sht30.readData()) {
      float temperature = sht30.getTemperature();
      float humidity = sht30.getHumidity();
      
      Serial.print("温度: ");
      Serial.print(temperature, 2);
      Serial.println(" °C");
      
      Serial.print("湿度: ");
      Serial.print(humidity, 2);
      Serial.println(" %");
    } else {
      Serial.println("SHT30读取失败");
    }
    
    // 休眠3分钟 (3 * 60 * 1000 毫秒)
    vTaskDelay(3 * 60 * 1000 / portTICK_PERIOD_MS);
  }
}