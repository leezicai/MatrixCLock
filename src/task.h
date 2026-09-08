#ifndef TASKS_H
#define TASKS_H
#include <Arduino.h>

// 初始化任务
void initTasks();

// 任务函数声明
void method1Task(void * parameter);
void method2Task(void * parameter);
void sensorTask(void * parameter);  // 新增传感器任务
void alarmTask(void * parameter);  // 新增传感器任务
void stockTask(void * parameter);  // 新增股票指数任务
void netKeepAliveTask(void * parameter);  // WiFi 保活任务

// Radio guard.
// WiFi is held open from boot now, so this no longer guards connect/disconnect
// pairs. It still serialises the operations that reconfigure or lean on the
// link - the NTP sync and a reconnect attempt - so they cannot interleave.
bool netMutexTake(uint32_t timeoutMs);
void netMutexGive();

// 任务句柄（如果外部需要访问）
extern TaskHandle_t method1TaskHandle;
extern TaskHandle_t method2TaskHandle;
extern TaskHandle_t sensorTaskHandle;  // 新增传感器任务句柄
extern TaskHandle_t alarmTaskHandle;  // 新增传感器任务句柄
extern TaskHandle_t stockTaskHandle;  // 新增股票指数任务句柄
extern TaskHandle_t netKeepAliveTaskHandle;  // WiFi 保活任务句柄

#endif // TASKS_H