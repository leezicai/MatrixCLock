#ifndef DS3231_H
#define DS3231_H

#include <Arduino.h>
#include <Wire.h>
#include <time.h>

// 定义DS3231的I2C引脚
#define SDA 1
#define SCL 2
#define DS3231_ADDRESS 0x68

// DS3231功能函数声明
bool initDS3231();
bool checkExternalRTC();
bool readTimeFromRTC(struct tm *timeinfo);
bool writeTimeToRTC(const struct tm *timeinfo);
bool isDS3231Available();

// 时间转换辅助函数
uint8_t decToBcd(uint8_t val);
uint8_t bcdToDec(uint8_t val);

#endif // DS3231_H