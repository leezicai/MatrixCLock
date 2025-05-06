#include "DS3231.h"

// Global offset variable (8 hours in seconds)
int gmtOffset_sec = 8 * 3600;

DS3231::DS3231() {
    wire = &Wire;
    _isConnected = false;
    this->gmtOffset_sec = ::gmtOffset_sec; // 使用作用域解析运算符访问全局变量
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
        return false;
    }
    
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain ESP32-S3 time");
        return false;
    }
    
    // Convert from ESP32-S3's local time (GMT+8) to UTC for DS3231
    time_t now;
    time(&now);
    // now -= gmtOffset_sec; // Convert to UTC time 本身就是UTC时间 不需要再减一次
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
    int hoursToAdd = this->gmtOffset_sec / 3600;
    
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