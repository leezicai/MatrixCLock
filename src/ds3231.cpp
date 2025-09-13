#include "ds3231.h"
#include <ESP32Time.h>
#include <WiFiUdp.h>
#include "data.h"

// Global offset variable (8 hours in seconds)
const char* ntpServer = "ntp7.aliyun.com";
// int gmtOffset_sec = 8 * 3600;
// const long  gmtOffset_sec = 8 * 3600;
const int   daylightOffset_sec = 0;
ESP32Time esp32Time;
WiFiUDP udp;
byte packetBuffer[NTP_PACKET_SIZE];

DS3231 rtc;

DS3231::DS3231() {
    wire = &Wire;
    _isConnected = false;
    // this->gmtOffset_sec = ::gmtOffset_sec; // 使用作用域解析运算符访问全局变量
}

// Method 1: Initialize DS3231
bool DS3231::begin(int sda_pin, int scl_pin) {
    wire->begin(sda_pin, scl_pin);
    
    // Check if the device is responding
    wire->beginTransmission(DS3231_ADDRESS);
    bool deviceFound = (wire->endTransmission() == 0);
    
    if (deviceFound) {
        _isConnected = true;
        
        // Set control register (enable oscillator, disable square wave output)
        wire->beginTransmission(DS3231_ADDRESS);
        wire->write(DS3231_CONTROL_REG);
        wire->write(0x00);
        wire->endTransmission();
    }
    
    return _isConnected;
}

// Method 2: Check if DS3231 is connected
bool DS3231::isConnected() {
    wire->beginTransmission(DS3231_ADDRESS);
    bool deviceFound = (wire->endTransmission() == 0);
    _isConnected = deviceFound;
    return _isConnected;
}

// Method 3: Sync time from ESP32-S3 to DS3231
bool DS3231::syncTimeToRTC() {
    if (!_isConnected) {
        this->begin(SDA, SCL);
    }
    
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return false;
    }
    
    // Convert from ESP32-S3's local time to UTC for DS3231
    time_t now;
    time(&now);
    Serial.printf("Current time1: %d\n", now);
    now -= (appData.getTimezone() * 3600); // Convert to UTC time 
    Serial.printf("Current time2: %d\n", now);
    gmtime_r(&now, &timeinfo);
    
    return setRTCDateTime(timeinfo);
}

// Method 4: Sync time from DS3231 to ESP32-S3
bool DS3231::syncTimeFromRTC() {
    Serial.println("===== Starting syncTimeFromRTC =====");
    
    if (!_isConnected) {
        return false;
    }
    
    struct tm timeinfo;
    if (!getRTCDateTime(&timeinfo)) {
        Serial.println("Failed to read time from DS3231");
        return false;
    }
    
    // Fix year if it's incorrectly set (e.g., 1925)
    if (timeinfo.tm_year < 120) { // Years before 2020
        Serial.println("Detected invalid year, fixing to 2024");
        timeinfo.tm_year = 124; // 2024 (1900 + 124)
    }
    // Create a copy of the UTC time
    struct tm localtimeinfo = timeinfo;
    
    // Apply timezone offset manually
    int hoursToAdd = appData.getTimezone();
    
    // Add hours and handle date rollover
    localtimeinfo.tm_hour += hoursToAdd;
    
    // Check if we need to adjust the date
    if (localtimeinfo.tm_hour >= 24) {
        localtimeinfo.tm_hour -= 24;
        localtimeinfo.tm_mday += 1;
        
        // Simple month length logic - can be improved
        int daysInMonth = 31;
        if (localtimeinfo.tm_mon == 3 || localtimeinfo.tm_mon == 5 || 
            localtimeinfo.tm_mon == 8 || localtimeinfo.tm_mon == 10) {
            daysInMonth = 30;
        } else if (localtimeinfo.tm_mon == 1) { // February
            bool isLeapYear = ((localtimeinfo.tm_year % 4 == 0 && 
                              localtimeinfo.tm_year % 100 != 0) || 
                              localtimeinfo.tm_year % 400 == 0);
            daysInMonth = isLeapYear ? 29 : 28;
        }
        
        if (localtimeinfo.tm_mday > daysInMonth) {
            localtimeinfo.tm_mday = 1;
            localtimeinfo.tm_mon += 1;
            if (localtimeinfo.tm_mon >= 12) {
                localtimeinfo.tm_mon = 0;
                localtimeinfo.tm_year += 1;
            }
        }
    }
    
    // Now convert to time_t for system time setting
    localtimeinfo.tm_isdst = 0; // No daylight saving
    time_t localTime = mktime(&localtimeinfo);
    
    if (localTime == -1) {
        Serial.println("Error converting time to timestamp");
        return false;
    }
    
    struct timeval tv;
    tv.tv_sec = localTime;
    tv.tv_usec = 0;
    
    if (settimeofday(&tv, NULL) != 0) {
        Serial.println("Error setting system time");
        return false;
    }
    
    // Verify time was set correctly
    struct tm verifyTimeinfo;
    if(!getLocalTime(&verifyTimeinfo)) {
        Serial.println("Failed to get local time after setting");
        return false;
    }
    return true;
}

// Utility function: Convert BCD to decimal
uint8_t DS3231::bcdToDec(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}

// Utility function: Convert decimal to BCD
uint8_t DS3231::decToBcd(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}

// Set date and time on DS3231
bool DS3231::setRTCDateTime(tm timeinfo) {
    if (!_isConnected) {
        this->begin(SDA, SCL);
        return false;
    }
    
    wire->beginTransmission(DS3231_ADDRESS);
    wire->write(DS3231_SECONDS_REG); // Start at seconds register
    wire->write(decToBcd(timeinfo.tm_sec));
    wire->write(decToBcd(timeinfo.tm_min));
    wire->write(decToBcd(timeinfo.tm_hour));
    wire->write(decToBcd(timeinfo.tm_wday + 1)); // Day of week, DS3231 uses 1-7, tm uses 0-6
    wire->write(decToBcd(timeinfo.tm_mday));     // Day of month
    
    // DS3231 month register has century bit (bit 7)
    uint8_t monthReg = decToBcd(timeinfo.tm_mon + 1); // tm_mon is 0-11, DS3231 uses 1-12
    
    // Set century bit for years >= 2000
    if (timeinfo.tm_year >= 100) {
        monthReg |= 0x80;  // Set bit 7 for century (years >= 2000)
    }
    wire->write(monthReg);
    
    // Year is offset from 1900 in tm struct, and DS3231 only stores last two digits
    wire->write(decToBcd(timeinfo.tm_year % 100));
    
    return (wire->endTransmission() == 0);
}

// Get date and time from DS3231
bool DS3231::getRTCDateTime(tm* timeinfo) {
    if (!_isConnected) {
        return false;
    }
    
    wire->beginTransmission(DS3231_ADDRESS);
    wire->write(DS3231_SECONDS_REG);
    if (wire->endTransmission() != 0) {
        return false;
    }
    
    const uint8_t BUFFER_SIZE = 7; // From seconds to year (7 registers)
    if (wire->requestFrom(DS3231_ADDRESS, BUFFER_SIZE) != BUFFER_SIZE) {
        return false;
    }
    
    timeinfo->tm_sec = bcdToDec(wire->read());
    timeinfo->tm_min = bcdToDec(wire->read());
    timeinfo->tm_hour = bcdToDec(wire->read() & 0x3F); // Mask high bits (12/24 hour mode)
    timeinfo->tm_wday = bcdToDec(wire->read()) - 1;    // Convert from DS3231 (1-7) to tm (0-6)
    timeinfo->tm_mday = bcdToDec(wire->read());
    
    uint8_t monthReg = wire->read();
    bool century = bitRead(monthReg, 7);
    timeinfo->tm_mon = bcdToDec(monthReg & 0x7F) - 1;  // Convert from DS3231 (1-12) to tm (0-11)
    
    timeinfo->tm_year = bcdToDec(wire->read());
    if (century) {
        timeinfo->tm_year += 100;  // Years 2000-2099
    }
    
    // These fields are not provided by DS3231
    timeinfo->tm_isdst = -1;
    
    return true;
}

// Get temperature from DS3231
float DS3231::getTemperature() {
    if (!_isConnected) {
        return -273.15; // Error value
    }
    
    wire->beginTransmission(DS3231_ADDRESS);
    wire->write(DS3231_TEMP_MSB_REG);
    wire->endTransmission();
    
    if (wire->requestFrom(DS3231_ADDRESS, 2) != 2) {
        return -273.15;
    }
    
    int8_t msb = wire->read();
    uint8_t lsb = wire->read() >> 6;  // Upper 2 bits of LSB
    
    return msb + (lsb * 0.25);
}

// Get formatted date and time string
void DS3231::getStringDateTime(char* buffer, size_t bufferSize) {
    struct tm timeinfo;
    
    if (getRTCDateTime(&timeinfo)) {
        strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", &timeinfo);
    } else {
        snprintf(buffer, bufferSize, "Error reading RTC");
    }
}


// 手动用esp32Time 获取时间

// 发送 NTP 请求包到指定的 NTP 服务器
void DS3231::sendNTPpacket(const char *address)
{
    // 清空缓冲区
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // 初始化 NTP 请求的值
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision

    // 8 字节的零作为 Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // 发送 UDP 包
    udp.beginPacket(address, NTP_PORT);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

/**
 * 从 NTP 服务器获取时间并同步给 ESP32S3，完全使用 ESP32Time 库
 * @return bool - 同步成功返回 true，失败返回 false
 */
bool DS3231::syncNtpTime()
{
    Serial.println("Starting NTP time synchronization with ESP32Time...");
    // 启动 UDP
    udp.begin(NTP_PORT);
    // 发送 NTP 请求
    sendNTPpacket(ntpServer);
    // 等待响应，最多等待 5 秒
    unsigned long startWait = millis();
    while (millis() - startWait < 2500)
    {
        int packetSize = udp.parsePacket();
        if (packetSize >= NTP_PACKET_SIZE)
        {
            Serial.println("NTP response received");
            // 读取 UDP 包内容
            udp.read(packetBuffer, NTP_PACKET_SIZE);
            // 从第 40 字节开始提取时间戳
            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            // 组合为大整数（NTP 时间戳）
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            // NTP 时间从 1900 年开始，Unix 时间从 1970 年开始
            // 二者相差 70 年（2208988800 秒）
            const unsigned long seventyYears = 2208988800UL;
            unsigned long epoch = secsSince1900 - seventyYears;
            // 设置 ESP32Time 时间（添加时区偏移）
            // ESP32Time 已经在创建实例时设置了时区，或者可以在setTime时加入偏移
            esp32Time.setTime(epoch + appData.getTimezone() * 3600);
            // 打印当前时间以验证
            Serial.print("NTP time synchronized: ");
            Serial.println(esp32Time.getTime("%Y-%m-%d %H:%M:%S"));
            return true;
        }
        delay(100);
    }
    Serial.println("NTP time synchronization failed - timeout waiting for response.");
    return false;
}