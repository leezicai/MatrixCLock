#ifndef TASKS_H
#define TASKS_H
#include <Arduino.h>

// 初始化任务
void initTasks();

// 任务函数声明
void method1Task(void * parameter);
void method2Task(void * parameter);
void sensorTask(void * parameter);  // 新增传感器任务

// 任务句柄（如果外部需要访问）
extern TaskHandle_t method1TaskHandle;
extern TaskHandle_t method2TaskHandle;
extern TaskHandle_t sensorTaskHandle;  // 新增传感器任务句柄

#endif // TASKS_H