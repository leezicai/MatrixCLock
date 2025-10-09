#include "setAP.h"

#define AP_SSID "MatrixClock"  // Access point name
#define AP_IP IPAddress(10, 0, 0, 1)
#define AP_SUBNET IPAddress(255, 255, 255, 0)
#define DNS_PORT 53

SetAP wifiManager;

SetAP::SetAP() : 
    server(80),
    timezone(0),
    screenOption("128x64x1"),
    language("en") {
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

String SetAP::getScreenOption() const {
    return screenOption;
}

String SetAP::getLanguage() const {
    return language;
}

void SetAP::restartESP() {
    Serial.println("Restarting ESP32S3...");
    delay(200);
    ESP.restart();
}

void SetAP::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void SetAP::handleSubmit() {
    selectedSSID = server.arg("ssid");
    wifiPassword = server.arg("password");
    timezone = server.arg("timezone").toInt();
    screenOption = server.arg("screen");
    language = server.arg("language");
    
    // Determine panel dimensions and type based on screen option
    int panelWidth, panelHeight, panelChain, panelType;
    if (screenOption == "128x64x1") {
        panelWidth = 128;
        panelHeight = 64;
        panelChain = 1;
        panelType = 0;  // Index for 128x64x1
    } else if (screenOption == "64x64x2") {
        panelWidth = 64;
        panelHeight = 64;
        panelChain = 2;
        panelType = 1;  // Index for 64x64x2
    } else if (screenOption == "64x64x1") {
        panelWidth = 64;
        panelHeight = 64;
        panelChain = 1;
        panelType = 2;  // Index for 64x64x1
    } else {
        // Default fallback
        panelWidth = 128;
        panelHeight = 64;
        panelChain = 1;
        panelType = 0;  // Default to type 0
    }
    
    // Set language based on detected browser language
    if (language == "zh") {
        matrixDataManager.setLanguage(Language::LANG_CHINESE);
    } else if (language == "en") {
        matrixDataManager.setLanguage(Language::LANG_ENGLISH);
    } else {
        // Default to English for all other languages
        matrixDataManager.setLanguage(Language::LANG_ENGLISH);
    }
    
    String response = "<html><head>";
    response += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    response += "<meta charset='UTF-8'>";
    response += "<style>body{font-family:Arial,sans-serif;margin:20px;text-align:center;}</style>";
    response += "</head><body>";
    response += "<h1>WiFi Setup Successful!</h1>";
    response += "<p>SSID: " + selectedSSID + "</p>";
    response += "<p>Password: " + wifiPassword + "</p>";
    response += "<p>Timezone: " + String(timezone) + "</p>";
    response += "<p>Screen: " + screenOption + "</p>";
    response += "<p>Panel Width: " + String(panelWidth) + "</p>";
    response += "<p>Panel Height: " + String(panelHeight) + "</p>";
    response += "<p>Panel Chain: " + String(panelChain) + "</p>";
    response += "<p>Panel Type: " + String(panelType) + "</p>";
    response += "<p>Language: " + language + "</p>";
    response += "<p>Device will restart in 2 seconds...</p>";
    response += "</body></html>";
    
    server.send(200, "text/html", response);
    
    // Save WiFi configuration
    matrixDataManager.setWifiSSID(selectedSSID.c_str());
    matrixDataManager.setWifiPassword(wifiPassword.c_str());
    matrixDataManager.setTimezone(timezone);
    matrixDataManager.setWifiConfig(true);
    
    // Save panel configuration
    matrixDataManager.savePanelWidth(panelWidth);
    matrixDataManager.savePanelHeight(panelHeight);
    matrixDataManager.savePanelChain(panelChain);
    matrixDataManager.savePanelType(panelType);  // Save panel type (0, 1, or 2)
   
    // Restart after 2 seconds
    delay(2000);
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
    html += "<title>Setup</title>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;margin:20px;max-width:600px;margin:0 auto;padding:15px;}";
    html += "h1{text-align:center;color:#333;}";
    html += "label{display:block;margin:15px 0 5px;font-weight:bold;}";
    html += "select,input{width:100%;padding:8px;margin-bottom:10px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}";
    html += "button{background:#4CAF50;color:white;padding:10px 15px;border:none;border-radius:4px;cursor:pointer;width:100%;font-size:16px;}";
    html += "button:hover{background:#45a049;}";
    html += ".screen-info{background:#f0f0f0;padding:10px;margin:10px 0;border-radius:4px;font-size:14px;}";
    html += "</style>";
    
    // Add JavaScript for dynamic screen info display and language detection
    html += "<script>";
    
    // Detect browser language on page load and auto-select
    html += "window.onload = function() {";
    html += "  var userLang = navigator.language || navigator.userLanguage;";
    html += "  var langSelect = document.getElementById('language');";
    html += "  if (userLang.startsWith('zh')) {";
    html += "    langSelect.value = 'zh';";
    html += "  } else {";
    html += "    langSelect.value = 'en';";
    html += "  }";
    html += "  updateScreenInfo();";
    html += "};";
    
    // Update screen info based on selection
    html += "function updateScreenInfo() {";
    html += "  var screen = document.getElementById('screen').value;";
    html += "  var info = document.getElementById('screenInfo');";
    html += "  if (screen === '128x64x1') {";
    html += "    info.innerHTML = 'PANEL_WIDTH: 128<br>PANEL_HEIGHT: 64<br>PANEL_CHAIN: 1';";
    html += "  } else if (screen === '64x64x2') {";
    html += "    info.innerHTML = 'PANEL_WIDTH: 64<br>PANEL_HEIGHT: 64<br>PANEL_CHAIN: 2';";
    html += "  } else if (screen === '64x64x1') {";
    html += "    info.innerHTML = 'PANEL_WIDTH: 64<br>PANEL_HEIGHT: 64<br>PANEL_CHAIN: 1';";
    html += "  }";
    html += "}";
    html += "</script>";
    
    html += "</head><body>";
    html += "<h1>Setup</h1>";
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
    
    html += "<label for='screen'>Screen Option:</label>";
    html += "<select name='screen' id='screen' onchange='updateScreenInfo()' required>";
    html += "<option value='128x64x1' selected>128x64x1</option>";
    html += "<option value='64x64x2'>64x64x2</option>";
    html += "<option value='64x64x1'>64x64x1</option>";
    html += "</select>";
    
    html += "<div class='screen-info' id='screenInfo'>";
    html += "PANEL_WIDTH: 128<br>PANEL_HEIGHT: 64<br>PANEL_CHAIN: 1";
    html += "</div>";
    
    html += "<label for='language'>Language:</label>";
    html += "<select name='language' id='language' required>";
    html += "<option value='zh'>中文</option>";
    html += "<option value='en'>English</option>";
    html += "</select>";
    
    html += "<button type='submit'>Submit</button>";
    html += "</form></body></html>";
    
    return html;
}