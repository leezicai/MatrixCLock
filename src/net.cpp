// net.cpp
#include "net.h"
#include <WiFi.h>
#include <time.h>
#include <Arduino.h>
#include "ds3231.h"
// #include <sys/time.h> 

// --- WiFi 和 NTP 配置 (保持不变) ---
// --- WiFi and NTP Configuration (Keep unchanged) ---
const char* ssid       = "CatNet";
const char* password = "wopaodekuai.1234";
const char* ntpServer = "ntp.aliyun.com";
const long  gmtOffset_sec = 8 * 3600;
const int   daylightOffset_sec = 0;

// --- WiFi 连接常量 ---
// --- WiFi Connection Constants ---
// User requested 20 attempts for the retry function
#define MAX_WIFI_ATTEMPTS 10
#define WIFI_CONNECT_TIMEOUT_MS 5000

static char _currentTimeString[9] = "00:00:00";
static bool _isTimeValid = false;
static NetState _currentNetState = NET_DISCONNECTED; // Use new enum

// --- 内部辅助函数: 更新本地时间 (保持不变) ---
// --- Internal Helper Function: Update Local Time (Keep unchanged) ---
static bool updateLocalTimeInternal() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        // If time was previously valid, keep the last known time
        return _isTimeValid;
    }

    sprintf(_currentTimeString, "%02d:%02d:%02d",
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec);

    if (!_isTimeValid) {
          Serial.print("Time synchronized successfully: ");
          Serial.println(_currentTimeString);
    }
    _isTimeValid = true;
    // Update state if currently syncing
    if (_currentNetState == NET_SYNCING_TIME) {
        _currentNetState = NET_TIME_SYNC_SUCCESS;
    }
    return true;
}

void setupNet() {
    _currentNetState = NET_DISCONNECTED;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true); // Ensure a clean start
    Serial.println("Network module initialized.");
}

/**
 * @brief Attempts a single WiFi connection.
 *
 * Sets WiFi mode to station, attempts to begin connection,
 * waits for connection or timeout, and updates state.
 *
 * @return true if the connection is successful, false otherwise.
 */
bool attemptWiFiConnectOnce() {
    Serial.println("Attempting single WiFi connection...");
    _currentNetState = NET_CONNECTING_WIFI; // Update state

    WiFi.mode(WIFI_STA); // Ensure mode is set before beginning
    WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT_MS) {
        // Can call yield() or delay(10) here to allow other tasks to run
        delay(100); // Reduce delay for faster response but still avoid full blocking
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi Connected");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        _currentNetState = NET_WIFI_CONNECTED; // Update state
        return true;
    } else {
        Serial.println("Single connection attempt failed.");
        // Disconnect explicitly after a failed attempt to ensure clean state for next try
        WiFi.disconnect(true);
        return false;
    }
}

/**
 * @brief Attempts to connect to WiFi multiple times using attemptWiFiConnectOnce().
 *
 * Tries up to MAX_WIFI_ATTEMPTS times. Returns true on the first successful connection.
 *
 * @return true if connection is successful within any attempt, false otherwise (after MAX_WIFI_ATTEMPTS).
 */
bool connectNetWithRetry() {
    Serial.println("Attempting WiFi connection with retries...");

    for (int attempt = 1; attempt <= MAX_WIFI_ATTEMPTS; attempt++) {
        Serial.print("Starting attempt ");
        Serial.print(attempt);
        Serial.print("/");
        Serial.print(MAX_WIFI_ATTEMPTS);
        Serial.println("...");

        // Call the single attempt function
        if (attemptWiFiConnectOnce()) {
            Serial.print("Connection successful on attempt ");
            Serial.println(attempt);
            return true; // Success on this attempt, exit retry loop
        } else {
             Serial.println("Attempt failed, preparing for next try...");
             // Add a small delay between retries
             delay(500);
        }
    }

    Serial.println("WiFi connection failed after all attempts.");
    _currentNetState = NET_WIFI_FAILED; // Update final state after all retries fail
    return false; // All attempts failed
}


void disconnectNet() {
    Serial.println("Disconnecting WiFi...");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    _currentNetState = NET_DISCONNECTED; // Update state
    Serial.println("WiFi disconnected and module turned off.");
}

bool isWiFiConnected() {
    // This function provides the current WiFi connection status directly from the hardware
    return (WiFi.status() == WL_CONNECTED);
}

bool syncNtpTime() {
    if (!isWiFiConnected()) {
        Serial.println("NTP sync failed: WiFi not connected.");
        _currentNetState = NET_TIME_SYNC_FAILED; // 修正：使用_currentNetState而不是*currentNetState
        return false;
    }
    
    Serial.println("Configuring time using NTP server...");
    _currentNetState = NET_SYNCING_TIME; // 修正：使用_currentNetState而不是*currentNetState
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Waiting for NTP time synchronization...");
    
    int ntpAttempts = 0;
    bool timeSuccess = false;
    // 尝试最多20次(大约10秒)以确保NTP同步稳健
    // 注意: configTime启动一个进程，updateLocalTime检查是否完成
    // 根据网络条件，可能需要更长的等待/更多尝试
    while (ntpAttempts < 20) { // 增加尝试次数以提高稳健性
        // updateLocalTimeInternal现在在成功时将_currentNetState更新为NET_TIME_SYNC_SUCCESS
        timeSuccess = updateLocalTimeInternal();
        if (timeSuccess) {
            // updateLocalTimeInternal已将状态设置为NET_TIME_SYNC_SUCCESS
            break; // 获取时间成功，提前退出循环
        }
        delay(500); // 等待并重试检查时间
        ntpAttempts++;
        Serial.print(".");
    }
    
    Serial.println();
    // 检查循环后的状态以查看是否成功获取时间
    if (_currentNetState == NET_TIME_SYNC_SUCCESS) {
        Serial.println("NTP time synchronization successful.");
        
        // 如果外置RTC可用，将NTP同步的时间也写入RTC
       // 在syncNtpTime()函数中的相关代码部分：
        if (isDS3231Available())
        {
            struct tm timeinfo;
            if (getLocalTime(&timeinfo))
            {
                // 此时timeinfo已经是本地时间（东八区），可以直接写入RTC
                Serial.print("Updating RTC with local time: ");
                Serial.printf("%02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

                if (writeTimeToRTC(&timeinfo))
                {
                    Serial.println("External RTC updated successfully!");
                }
                else
                {
                    Serial.println("Failed to update external RTC!");
                }
            }
        }

        // 时间已由updateLocalTimeInternal更新，状态已设置
        return true; // 同步成功
    } else {
        Serial.println("NTP time synchronization failed after all attempts.");
        _currentNetState = NET_TIME_SYNC_FAILED; // 修正：使用_currentNetState而不是*currentNetState
        _isTimeValid = false; // 标记时间为无效
        strcpy(_currentTimeString, "00:00:00"); // 重置时间字符串
        return false; // 同步失败
    }
}

// 添加一个新函数用于从外置RTC获取时间 - 修改版
bool getTimeFromRTC() {
    if (!isDS3231Available()) {
        Serial.println("External RTC not available");
        return false;
    }

    struct tm timeinfo_local; // 用于存储从RTC读取的本地时间
    if (readTimeFromRTC(&timeinfo_local)) {
        Serial.printf("RTC Read (Local Time): %04d-%02d-%02d %02d:%02d:%02d\n",
                      timeinfo_local.tm_year + 1900, timeinfo_local.tm_mon + 1, timeinfo_local.tm_mday,
                      timeinfo_local.tm_hour, timeinfo_local.tm_min, timeinfo_local.tm_sec);

        // --- 手动计算正确的 UTC time_t ---
        timeinfo_local.tm_isdst = 0; // 明确指定非夏令时

        // 调用 mktime。假设它基于某种默认行为处理 timeinfo_local。
        // 我们需要的结果是对应的 UTC time_t 值。
        time_t t_temp = mktime(&timeinfo_local);

        if (t_temp == -1) {
            Serial.println("mktime failed during RTC init. RTC data might be invalid.");
            // 可以增加打印 timeinfo_local 各个字段的值来调试
            return false;
        }
        Serial.printf("mktime produced temporary epoch: %ld (based on system default interpretation)\n", t_temp);

        // 校正 t_temp 得到实际的 UTC epoch 秒数。
        // 因为 timeinfo_local 是 UTC+8，所以对应的 UTC 时间戳要减去偏移量。
        time_t t_correct_utc = t_temp - gmtOffset_sec; // gmtOffset_sec 是 8 * 3600
        Serial.printf("Calculated Correct UTC epoch by subtracting offset: %ld\n", t_correct_utc);
        // --- 计算结束 ---

        // 使用计算出的正确的 UTC time_t 设置系统时间
        struct timeval now = { .tv_sec = t_correct_utc };
        settimeofday(&now, NULL);
        Serial.println("System time set using manually calculated UTC epoch from RTC local time.");

        // 重要：此时系统 UTC 基准时间已设置，但 getLocalTime() 需要知道 gmtOffset_sec 才能工作。
        // 这通常由 configTime() 完成。我们依赖于 setup() 中调用 syncNtpTime() 时执行的 configTime()。
        // 如果 configTime 从未运行，getLocalTime() 可能只返回 UTC。

        // 更新内部时间字符串，使用 getLocalTime()
        // 这依赖于 settimeofday 成功设置了基准时间 *并且* configTime 设置了偏移量
        if (updateLocalTimeInternal()) {
            Serial.print("Internal time string updated via getLocalTime: ");
            Serial.println(_currentTimeString);
            _isTimeValid = true;
            // 可以在这里更新网络状态，表示时间已从RTC获取
            _currentNetState = NET_TIME_SYNC_SUCCESS; // 或者一个表示RTC同步成功的状态
            return true;
        } else {
            Serial.println("Failed to update internal time string after setting time from RTC (maybe offset not set by configTime yet?).");
            _isTimeValid = false;
            _currentNetState = NET_TIME_SYNC_FAILED;
            return false;
        }
    }

    Serial.println("Failed to read time from external RTC");
    _currentNetState = NET_TIME_SYNC_FAILED; // 更新状态
    return false;
}
bool getCurrentTimeString(char* timeBuffer, size_t bufferSize) {
    if (bufferSize < 20) { // 需要更大的缓冲区来存储"YYYY-MM-DD HH:MM:SS"
        return false;
    }
    
    // 从RTC读取最新时间
    if (isDS3231Available()) {
        struct tm timeinfo;
        if (readTimeFromRTC(&timeinfo)) {
            // 格式化为"年-月-日 时:分:秒"
            sprintf(timeBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
                    timeinfo.tm_year + 1900, // 年份需要加1900
                    timeinfo.tm_mon + 1,     // 月份从0开始，需要加1
                    timeinfo.tm_mday,        // 日期
                    timeinfo.tm_hour,        // 小时
                    timeinfo.tm_min,         // 分钟
                    timeinfo.tm_sec);        // 秒钟
            return true;
        }
    }
    
    // 如果没有RTC或RTC读取失败，尝试使用系统时间
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        // 格式化为"年-月-日 时:分:秒"
        sprintf(timeBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
                timeinfo.tm_year + 1900, // 年份需要加1900
                timeinfo.tm_mon + 1,     // 月份从0开始，需要加1
                timeinfo.tm_mday,        // 日期
                timeinfo.tm_hour,        // 小时
                timeinfo.tm_min,         // 分钟
                timeinfo.tm_sec);        // 秒钟
        return true;
    } else {
        // 如果时间无效，显示默认值
        strncpy(timeBuffer, "0000-00-00 00:00:00", bufferSize - 1);
        timeBuffer[bufferSize - 1] = '\0';
        return false;
    }
}

NetState getNetState() {
    // This function returns the last known state based on our internal state variable.
    // For real-time WiFi status, use isWiFiConnected().
    // The state variable _currentNetState is updated by connectNetWithRetry,
    // disconnectNet, syncNtpTime, and internally by updateLocalTimeInternal.
    return _currentNetState;
}