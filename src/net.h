#ifndef NET_H
#define NET_H

#include <Arduino.h>
#include <stddef.h> // 为size_t定义
#include "loading.h"
#include "matrixData.h"

// 网络模块状态枚举
enum NetState {
    NET_DISCONNECTED,        // 未连接WiFi
    NET_CONNECTING_WIFI,     // 正在连接WiFi
    NET_WIFI_CONNECTED,      // WiFi已连接
    NET_WIFI_FAILED,         // WiFi连接失败
    NET_SYNCING_TIME,        // 正在同步NTP时间
    NET_TIME_SYNC_SUCCESS,   // NTP时间同步成功
    NET_TIME_SYNC_FAILED     // NTP时间同步失败
};

// 网络设置和初始化
void setupNet();

// WiFi连接函数
bool attemptWiFiConnectOnce();
bool connectNetWithRetry();
void disconnectNet();
bool isWiFiConnected();
void setWiFiLowPower();

// 时间同步和获取函数
// bool syncNtpTime();
bool getCurrentTimeString(char* timeBuffer, size_t bufferSize);

// 获取网络状态
NetState getNetState();

#endif // NET_H