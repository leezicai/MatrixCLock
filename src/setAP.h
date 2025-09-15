#ifndef SET_AP_H
#define SET_AP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <vector>
#include <string>

class SetAP {
public:
    SetAP();
    
    // 启动配置流程
    void beginSetup();
    
    // 扫描周围的WiFi网络
    void scanWifiNetworks();
    
    // 创建热点
    void createAccessPoint();
    
    // 处理Web服务器请求
    void handleClient();
    
    // 获取提交的信息
    String getSSID() const;
    String getPassword() const;
    int getTimezone() const;
    
    // 重启ESP32S3
    void restartESP();

private:
    WebServer server;
    DNSServer dnsServer;
    
    std::vector<String> wifiNetworks;
    
    String selectedSSID;
    String wifiPassword;
    int timezone;
    
    // Web页面处理函数
    void handleRoot();
    void handleSubmit();
    void handleNotFound();
    
    // 生成HTML页面
    String generateHTML();
};

extern SetAP wifiManager;

#endif // SET_AP_H