#ifndef SET_AP_H
#define SET_AP_H
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <vector>
#include <string>
#include "matrixData.h"

class SetAP {
public:
    SetAP();
    
    // Start setup process
    void beginSetup();
    
    // Scan nearby WiFi networks
    void scanWifiNetworks();
    
    // Create access point
    void createAccessPoint();
    
    // Handle web server requests
    void handleClient();
    
    // Get submitted information
    String getSSID() const;
    String getPassword() const;
    int getTimezone() const;
    String getScreenOption() const;
    
    // Restart ESP32S3
    void restartESP();
    
private:
    WebServer server;
    DNSServer dnsServer;
    
    std::vector<String> wifiNetworks;
    
    String selectedSSID;
    String wifiPassword;
    int timezone;
    String screenOption;
    
    // Web page handler functions
    void handleRoot();
    void handleSubmit();
    void handleNotFound();
    
    // Generate HTML page
    String generateHTML();
};

extern SetAP wifiManager;
#endif // SET_AP_H