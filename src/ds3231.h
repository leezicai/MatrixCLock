#ifndef DS3231_H
#define DS3231_H
#include <Arduino.h>
#include <Wire.h>
#include "time.h"
#include "common_define.h"
#include "matrixData.h"

// DS3231 I2C address
#define DS3231_ADDRESS 0x68

// DS3231 register addresses
#define DS3231_SECONDS_REG 0x00
#define DS3231_MINUTES_REG 0x01
#define DS3231_HOURS_REG 0x02
#define DS3231_DAY_REG 0x03
#define DS3231_DATE_REG 0x04
#define DS3231_MONTH_REG 0x05
#define DS3231_YEAR_REG 0x06
#define DS3231_CONTROL_REG 0x0E
#define DS3231_STATUS_REG 0x0F
#define DS3231_AGING_OFFSET_REG 0x10
#define DS3231_TEMP_MSB_REG 0x11
#define DS3231_TEMP_LSB_REG 0x12

// NTP constants
const int NTP_PACKET_SIZE = 48;
const int NTP_PORT = 123;

class DS3231 {
private:
    TwoWire *wire;
    bool _isConnected;
    
    // Utility functions
    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
    
public:
    DS3231();
    
    // Method 1: Initialize DS3231
    bool begin(int sda_pin, int scl_pin);
    
    // Method 2: Check if DS3231 is connected
    bool isConnected();
    
    // Method 3: Sync time from ESP32-S3 to DS3231
    bool syncTimeToRTC();
    
    // Method 4: Sync time from DS3231 to ESP32-S3
    bool syncTimeFromRTC();
    
    // Utility methods
    bool setRTCDateTime(tm timeinfo);
    bool getRTCDateTime(tm* timeinfo);
    void getStringDateTime(char* buffer, size_t bufferSize);
    float getTemperature();
    void sendNTPpacket(const char *address);
    bool syncNtpTime();
    
    // Time adjustment methods for ESP32-S3 with RTC sync
    bool adjustTime(int years, int months, int days, int hours, int minutes, int seconds);
    
    // Year adjustment methods
    bool addYear();
    bool subtractYear();
    
    // Month adjustment methods
    bool addMonth();
    bool subtractMonth();
    
    // Day adjustment methods
    bool addDay();
    bool subtractDay();
    
    // Hour adjustment methods
    bool addHour();
    bool subtractHour();
    
    // Minute adjustment methods
    bool addMinute();
    bool subtractMinute();
    
    // Second adjustment methods
    bool addSecond();
    bool subtractSecond();
};

extern DS3231 rtc;

#endif /* DS3231_H */