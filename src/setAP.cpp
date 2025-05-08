#include "setAP.h"
#include "data.h"

#define AP_SSID "AAA"  // Access point name
#define AP_IP IPAddress(10, 0, 0, 1)
#define AP_SUBNET IPAddress(255, 255, 255, 0)
#define DNS_PORT 53

SetAP::SetAP() : 
    server(80),
    timezone(0) {
}

void SetAP::beginSetup() {
    // Scan for available networks
    scanWifiNetworks();
    
    // Create access point
    createAccessPoint();
    
    // Set up DNS server to redirect all domains to the AP IP
    dnsServer.start(DNS_PORT, "*", AP_IP);
    
    // Set up web server handlers
    server.on("/", HTTP_GET, [this]() { this->handleRoot(); });
    server.on("/submit", HTTP_POST, [this]() { this->handleSubmit(); });
    server.onNotFound([this]() { this->handleNotFound(); });
    server.begin();
    
    Serial.println("AP setup complete. Connect to '" + String(AP_SSID) + "' and visit http://10.0.0.1");
}

void SetAP::scanWifiNetworks() {
    Serial.println("Scanning WiFi networks...");
    wifiNetworks.clear();
    
    int networksFound = WiFi.scanNetworks();
    
    for (int i = 0; i < networksFound; i++) {
        // Only add 2.4GHz networks (channels 1-13)
        if (WiFi.channel(i) <= 13) {
            wifiNetworks.push_back(WiFi.SSID(i));
        }
    }
    
    Serial.println("Found " + String(wifiNetworks.size()) + " 2.4GHz networks");
}

void SetAP::createAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_IP, AP_SUBNET);
    WiFi.softAP(AP_SSID);
    
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());
}

void SetAP::handleClient() {
    dnsServer.processNextRequest();
    server.handleClient();
}

String SetAP::getSSID() const {
    return selectedSSID;
}

String SetAP::getPassword() const {
    return wifiPassword;
}

int SetAP::getTimezone() const {
    return timezone;
}

void SetAP::restartESP() {
    Serial.println("Restarting ESP32S3...");
    delay(1000);
    ESP.restart();
}

void SetAP::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void SetAP::handleSubmit() {
    selectedSSID = server.arg("ssid");
    wifiPassword = server.arg("password");
    timezone = server.arg("timezone").toInt();
    
    String response = "<html><head>";
    response += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    response += "<style>body{font-family:Arial,sans-serif;margin:20px;text-align:center;}</style>";
    response += "</head><body>";
    response += "<h1>WiFi Setup Successful!</h1>";
    response += "<p>SSID: " + selectedSSID + "</p>";
    response += "<p>Password: " + wifiPassword + "</p>";
    response += "<p>Timezone: " + String(timezone) + "</p>";
    response += "<p>Device will restart in 3 seconds...</p>";
    response += "</body></html>";
    
    server.send(200, "text/html", response);
    
    AppData.setSSID(selectedSSID.c_str());
    AppData.setPassword(wifiPassword.c_str());
    AppData.setTimezone(timezone);
    AppData.setWifiConfigured(true);
    // 延迟3秒后重启
    delay(3000);
    restartESP();
}

void SetAP::handleNotFound() {
    // Redirect all requests to the configuration page
    server.sendHeader("Location", "http://10.0.0.1", true);
    server.send(302, "text/plain", "");
}

String SetAP::generateHTML() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>WiFi Setup</title>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;margin:20px;max-width:600px;margin:0 auto;padding:15px;}";
    html += "h1{text-align:center;color:#333;}";
    html += "label{display:block;margin:15px 0 5px;font-weight:bold;}";
    html += "select,input{width:100%;padding:8px;margin-bottom:10px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}";
    html += "button{background:#4CAF50;color:white;padding:10px 15px;border:none;border-radius:4px;cursor:pointer;width:100%;font-size:16px;}";
    html += "button:hover{background:#45a049;}";
    html += "</style></head><body>";
    html += "<h1>WiFi Setup</h1>";
    html += "<form action='/submit' method='post'>";
    
    html += "<label for='ssid'>Select WiFi Network:</label>";
    html += "<select name='ssid' id='ssid' required>";
    for (const auto& network : wifiNetworks) {
        html += "<option value='" + network + "'>" + network + "</option>";
    }
    html += "</select>";
    
    html += "<label for='password'>WiFi Password:</label>";
    html += "<input type='text' id='password' name='password' required>";
    
    html += "<label for='timezone'>Select Timezone (-12 to 14):</label>";
    html += "<select name='timezone' id='timezone'>";
    for (int i = -12; i <= 14; i++) {
        html += "<option value='" + String(i) + "'>" + String(i) + "</option>";
    }
    html += "</select>";
    
    html += "<button type='submit'>Submit</button>";
    html += "</form></body></html>";
    
    return html;
}