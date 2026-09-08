#include "ds3231.h"
#include <ESP32Time.h>
#include <WiFiUdp.h>

// Global offset variable (8 hours in seconds)
const char* ntpServer = "ntp7.aliyun.com";
const int daylightOffset_sec = 0;
ESP32Time esp32Time;
WiFiUDP udp;
byte packetBuffer[NTP_PACKET_SIZE];

DS3231 rtc;

DS3231::DS3231() {
    wire = &Wire;
    _isConnected = false;
    timezone = 0; // Default timezone UTC+0
}

// Method 1: Initialize DS3231
bool DS3231::begin(int sda_pin, int scl_pin) {
    wire->begin(sda_pin, scl_pin);
    
    // Load timezone from matrixDataManager
    timezone = matrixDataManager.getTimezone();
    Serial.printf("DS3231 initialized with timezone: UTC%+d\n", timezone);
    
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
    Serial.printf("Current local time: %d\n", now);
    now -= (timezone * 3600); // Convert to UTC time using cached timezone
    Serial.printf("Current UTC time: %d\n", now);
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
    
    // Apply timezone offset manually using cached timezone
    int hoursToAdd = timezone;
    
    // Add hours and handle date rollover
    localtimeinfo.tm_hour += hoursToAdd;
    
    // Check if we need to adjust the date
    if (localtimeinfo.tm_hour >= 24) {
        localtimeinfo.tm_hour -= 24;
        localtimeinfo.tm_mday += 1;
        
        // Simple month length logic
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
    } else if (localtimeinfo.tm_hour < 0) {
        // Handle negative hours (crossing to previous day)
        localtimeinfo.tm_hour += 24;
        localtimeinfo.tm_mday -= 1;
        
        if (localtimeinfo.tm_mday < 1) {
            localtimeinfo.tm_mon -= 1;
            if (localtimeinfo.tm_mon < 0) {
                localtimeinfo.tm_mon = 11;
                localtimeinfo.tm_year -= 1;
            }
            
            // Get days in previous month
            int daysInMonth = 31;
            if (localtimeinfo.tm_mon == 3 || localtimeinfo.tm_mon == 5 || 
                localtimeinfo.tm_mon == 8 || localtimeinfo.tm_mon == 10) {
                daysInMonth = 30;
            } else if (localtimeinfo.tm_mon == 1) {
                bool isLeapYear = ((localtimeinfo.tm_year % 4 == 0 && 
                                  localtimeinfo.tm_year % 100 != 0) || 
                                  localtimeinfo.tm_year % 400 == 0);
                daysInMonth = isLeapYear ? 29 : 28;
            }
            localtimeinfo.tm_mday = daysInMonth;
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
    
    Serial.printf("Time synced from RTC with timezone UTC%+d\n", timezone);
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
    if (wire->requestFrom(static_cast<uint8_t>(DS3231_ADDRESS), BUFFER_SIZE) != BUFFER_SIZE) {
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

// Send NTP request packet to specified NTP server
void DS3231::sendNTPpacket(const char *address) {
    // Clear buffer
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // Initialize NTP request values
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision

    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // Send UDP packet
    udp.beginPacket(address, NTP_PORT);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

/**
 * Get time from NTP server and sync to ESP32S3, using ESP32Time library
 * @return bool - true if successful, false otherwise
 */
bool DS3231::syncNtpTime() {
    Serial.println("Starting NTP time synchronization with ESP32Time...");
    
    // Start UDP
    udp.begin(NTP_PORT);
    
    // Send NTP request
    sendNTPpacket(ntpServer);
    
    // Wait for response, maximum 2.5 seconds
    unsigned long startWait = millis();
    while (millis() - startWait < 2500) {
        int packetSize = udp.parsePacket();
        if (packetSize >= NTP_PACKET_SIZE) {
            Serial.println("NTP response received");
            
            // Read UDP packet contents
            udp.read(packetBuffer, NTP_PACKET_SIZE);
            
            // Extract timestamp starting from byte 40
            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            
            // Combine into large integer (NTP timestamp)
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            
            // NTP time starts from 1900, Unix time starts from 1970
            // Difference is 70 years (2208988800 seconds)
            const unsigned long seventyYears = 2208988800UL;
            unsigned long epoch = secsSince1900 - seventyYears;
            
            // Set ESP32Time with timezone offset
            esp32Time.setTime(epoch + timezone * 3600);
            
            // Print current time to verify
            Serial.print("NTP time synchronized: ");
            Serial.println(esp32Time.getTime("%Y-%m-%d %H:%M:%S"));
            
            return true;
        }
        delay(100);
    }
    
    Serial.println("NTP time synchronization failed - timeout waiting for response.");
    return false;
}

// Update the adjustTime method
bool DS3231::adjustTime(int years, int months, int days, int hours, int minutes, int seconds) {
    // Get current system time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to get current system time");
        return false;
    }
    
    // Apply adjustments
    timeinfo.tm_year += years;
    timeinfo.tm_mon += months;
    timeinfo.tm_mday += days;
    timeinfo.tm_hour += hours;
    timeinfo.tm_min += minutes;
    timeinfo.tm_sec += seconds;
    
    // Normalize the time structure (handle overflows/underflows)
    timeinfo.tm_isdst = -1; // Let mktime determine DST
    time_t adjustedTime = mktime(&timeinfo);
    
    if (adjustedTime == -1) {
        Serial.println("Error: Invalid time after adjustment");
        return false;
    }
    
    // Set the adjusted time to system
    struct timeval tv;
    tv.tv_sec = adjustedTime;
    tv.tv_usec = 0;
    
    if (settimeofday(&tv, NULL) != 0) {
        Serial.println("Error setting adjusted system time");
        return false;
    }
    
    // Sync the adjusted time to RTC
    bool syncResult = syncTimeToRTC();
    
    if (syncResult) {
        Serial.printf("Time adjusted successfully: %s", ctime(&adjustedTime));
    } else {
        Serial.println("Time adjusted but failed to sync to RTC");
    }
    
    return syncResult;
}

// Year adjustment methods
bool DS3231::addYear() {
    Serial.println("Adding 1 year to current time...");
    return adjustTime(1, 0, 0, 0, 0, 0);
}

bool DS3231::subtractYear() {
    Serial.println("Subtracting 1 year from current time...");
    return adjustTime(-1, 0, 0, 0, 0, 0);
}

// Month adjustment methods
bool DS3231::addMonth() {
    Serial.println("Adding 1 month to current time...");
    return adjustTime(0, 1, 0, 0, 0, 0);
}

bool DS3231::subtractMonth() {
    Serial.println("Subtracting 1 month from current time...");
    return adjustTime(0, -1, 0, 0, 0, 0);
}

// Day adjustment methods
bool DS3231::addDay() {
    Serial.println("Adding 1 day to current time...");
    return adjustTime(0, 0, 1, 0, 0, 0);
}

bool DS3231::subtractDay() {
    Serial.println("Subtracting 1 day from current time...");
    return adjustTime(0, 0, -1, 0, 0, 0);
}

// Hour adjustment methods
bool DS3231::addHour() {
    Serial.println("Adding 1 hour to current time...");
    return adjustTime(0, 0, 0, 1, 0, 0);
}

bool DS3231::subtractHour() {
    Serial.println("Subtracting 1 hour from current time...");
    return adjustTime(0, 0, 0, -1, 0, 0);
}

// Minute adjustment methods
bool DS3231::addMinute() {
    Serial.println("Adding 1 minute to current time...");
    return adjustTime(0, 0, 0, 0, 1, 0);
}

bool DS3231::subtractMinute() {
    Serial.println("Subtracting 1 minute from current time...");
    return adjustTime(0, 0, 0, 0, -1, 0);
}

// Second adjustment methods
bool DS3231::addSecond() {
    Serial.println("Adding 1 second to current time...");
    return adjustTime(0, 0, 0, 0, 0, 1);
}

bool DS3231::subtractSecond() {
    Serial.println("Subtracting 1 second from current time...");
    return adjustTime(0, 0, 0, 0, 0, -1);
}

/**
 * Add one hour to timezone offset and resync from RTC
 * Updates the timezone setting and reloads time from RTC with new timezone
 * RTC stores UTC time which remains unchanged
 * @return bool - true if successful
 */
bool DS3231::addTimezone() {
    Serial.println("Adding 1 hour to timezone...");
    
    // Increment local timezone cache
    int newTimezone = timezone + 1;
    
    // Optional: Limit timezone range to reasonable values (-12 to +14)
    if (newTimezone > 14) {
        Serial.println("Warning: Timezone exceeds maximum (+14), capping at +14");
        newTimezone = 14;
    }
    
    // Update the timezone in matrixDataManager
    matrixDataManager.setTimezone(newTimezone);
    
    // Update local cache
    timezone = newTimezone;
    
    // Resync time from RTC with new timezone offset
    // RTC stores UTC time, syncTimeFromRTC will apply the new timezone offset
    bool result = syncTimeFromRTC();
    
    if (result) {
        Serial.printf("Timezone adjusted to: UTC%+d\n", timezone);
        Serial.println("Time resynced from RTC with new timezone");
    } else {
        Serial.println("Failed to resync time from RTC");
        // Rollback timezone change if sync failed
        timezone -= 1;
        matrixDataManager.setTimezone(timezone);
    }
    
    return result;
}

/**
 * Subtract one hour from timezone offset and resync from RTC
 * Updates the timezone setting and reloads time from RTC with new timezone
 * RTC stores UTC time which remains unchanged
 * @return bool - true if successful
 */
bool DS3231::subtractTimezone() {
    Serial.println("Subtracting 1 hour from timezone...");
    
    // Decrement local timezone cache
    int newTimezone = timezone - 1;
    
    // Optional: Limit timezone range to reasonable values (-12 to +14)
    if (newTimezone < -12) {
        Serial.println("Warning: Timezone below minimum (-12), capping at -12");
        newTimezone = -12;
    }
    
    // Update the timezone in matrixDataManager
    matrixDataManager.setTimezone(newTimezone);
    
    // Update local cache
    timezone = newTimezone;
    
    // Resync time from RTC with new timezone offset
    bool result = syncTimeFromRTC();
    
    if (result) {
        Serial.printf("Timezone adjusted to: UTC%+d\n", timezone);
        Serial.println("Time resynced from RTC with new timezone");
    } else {
        Serial.println("Failed to resync time from RTC");
        // Rollback timezone change if sync failed
        timezone += 1;
        matrixDataManager.setTimezone(timezone);
    }
    
    return result;
}

/**
 * Get current timezone offset
 * @return int - timezone offset in hours
 */
int DS3231::getTimezone() const {
    return timezone;
}