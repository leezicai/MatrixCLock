#include "ds3231.h"

// 全局变量，跟踪RTC模块是否可用
static bool _rtcAvailable = false;

// 初始化DS3231
bool initDS3231() {
  Wire.begin(SDA, SCL);
  _rtcAvailable = checkExternalRTC();
  return _rtcAvailable;
}

// 检查是否有外置时钟
bool checkExternalRTC() {
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(DS3231_ADDRESS);  // 0x68 是DS3231的I2C地址
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("DS3231 RTC detected!");
    return true;  // 检测到 RTC
  } else {
    Serial.println("DS3231 RTC not found!");
    return false; // 未检测到 RTC
  }
}

// 从RTC读取时间
bool readTimeFromRTC(struct tm *timeinfo) {
  if (!_rtcAvailable) return false;
  
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0); // 设置寄存器指针到第一个寄存器(秒)
  Wire.endTransmission();
  
  // 请求7个字节 (秒, 分, 时, 星期, 日, 月, 年)
  Wire.requestFrom(DS3231_ADDRESS, 7);
  
  if (Wire.available() >= 7) {
    timeinfo->tm_sec = bcdToDec(Wire.read() & 0x7F);
    timeinfo->tm_min = bcdToDec(Wire.read());
    timeinfo->tm_hour = bcdToDec(Wire.read() & 0x3F); // 24小时格式
    timeinfo->tm_wday = bcdToDec(Wire.read()); // 星期
    timeinfo->tm_mday = bcdToDec(Wire.read()); // 日
    timeinfo->tm_mon = bcdToDec(Wire.read()) - 1; // 月 (0-11)
    timeinfo->tm_year = bcdToDec(Wire.read()) + 100; // 年 (从1900年开始)
    
    // 设置tm_isdst为0，表示不使用夏令时
    timeinfo->tm_isdst = 0;
    
    Serial.printf("RTC Read: %02d/%02d/%04d %02d:%02d:%02d\n", 
      timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
      timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    return true;
  }
  
  return false;
}

// 将时间写入RTC
bool writeTimeToRTC(const struct tm *timeinfo) {
  if (!_rtcAvailable) return false;
  
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0); // 设置寄存器指针到第一个寄存器(秒)
  
  // 写入7个字节 (秒, 分, 时, 星期, 日, 月, 年)
  Wire.write(decToBcd(timeinfo->tm_sec));
  Wire.write(decToBcd(timeinfo->tm_min));
  Wire.write(decToBcd(timeinfo->tm_hour));
  Wire.write(decToBcd(timeinfo->tm_wday));
  Wire.write(decToBcd(timeinfo->tm_mday));
  Wire.write(decToBcd(timeinfo->tm_mon + 1)); // 月 (1-12)
  Wire.write(decToBcd(timeinfo->tm_year - 100)); // 年 (从2000年开始)
  
  byte error = Wire.endTransmission();
  
  Serial.printf("RTC Write: %02d/%02d/%04d %02d:%02d:%02d\n", 
    timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
    timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  
  return (error == 0);
}

// 检查DS3231是否可用
bool isDS3231Available() {
  return _rtcAvailable;
}

// 辅助函数：十进制转BCD
uint8_t decToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

// 辅助函数：BCD转十进制
uint8_t bcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}