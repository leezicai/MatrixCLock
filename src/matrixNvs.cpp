#include "matrixNvs.h"
#include "nvs_flash.h"

// Static member definition
const char* MatrixNvsManager::NVS_NAMESPACE = "matrix";

// Global instance
MatrixNvsManager matrixNvsManager;

MatrixNvsManager::MatrixNvsManager() {
}

MatrixNvsManager::~MatrixNvsManager() {
    preferences.end();
}

void MatrixNvsManager::initializeDefaults() {
    // Initialize NVS flash first
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        Serial.println("NVS partition needs erasing, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    
    if (err != ESP_OK) {
        Serial.printf("NVS flash init failed: %d\n", err);
        return;
    }
    
    // Begin preferences with the correct namespace "matrix"
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        Serial.println("Preferences init failed, trying to clear...");
        preferences.clear();
        preferences.end();
        if (!preferences.begin(NVS_NAMESPACE, false)) {
            Serial.println("Preferences init failed after clear!");
        } else {
            Serial.println("Preferences initialized successfully after clear");
        }
    } else {
        Serial.println("Preferences initialized successfully");
    }
}

// Commit changes to flash
void MatrixNvsManager::commit() {
    preferences.end();
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        Serial.println("Warning: Failed to reopen preferences after commit");
    }
}

// Bool operations
void MatrixNvsManager::saveBool(const String& key, bool value) {
    preferences.putBool(key.c_str(), value);
}

bool MatrixNvsManager::loadBool(const String& key, bool defaultValue) {
    return preferences.getBool(key.c_str(), defaultValue);
}

// Int operations
void MatrixNvsManager::saveInt(const String& key, int value) {
    preferences.putInt(key.c_str(), value);
}

int MatrixNvsManager::loadInt(const String& key, int defaultValue) {
    return preferences.getInt(key.c_str(), defaultValue);
}

// String operations
void MatrixNvsManager::saveString(const String& key, const String& value) {
    preferences.putString(key.c_str(), value);
}

String MatrixNvsManager::loadString(const String& key, const String& defaultValue) {
    return preferences.getString(key.c_str(), defaultValue);
}

// Helper method to serialize SecondaryPage to byte array
std::vector<uint8_t> MatrixNvsManager::serializeSecondaryPage(const SecondaryPage& page) {
    std::vector<uint8_t> data;
    
    // Store the number of MatrixCore elements
    size_t count = page.size();
    data.resize(sizeof(size_t) + count * sizeof(MatrixCore));
    
    // Copy count
    memcpy(data.data(), &count, sizeof(size_t));
    
    // Copy MatrixCore data
    if (count > 0) {
        memcpy(data.data() + sizeof(size_t), page.data(), count * sizeof(MatrixCore));
    }
    
    return data;
}

// Helper method to deserialize byte array to SecondaryPage
SecondaryPage MatrixNvsManager::deserializeSecondaryPage(const std::vector<uint8_t>& data) {
    SecondaryPage page;
    
    if (data.size() < sizeof(size_t)) {
        return page; // Invalid data
    }
    
    // Read count
    size_t count;
    memcpy(&count, data.data(), sizeof(size_t));
    
    // Validate data size
    if (data.size() != sizeof(size_t) + count * sizeof(MatrixCore)) {
        return page; // Invalid data size
    }
    
    // Read MatrixCore data
    if (count > 0) {
        page.resize(count);
        memcpy(page.data(), data.data() + sizeof(size_t), count * sizeof(MatrixCore));
    }
    
    return page;
}

// SecondaryPage operations
void MatrixNvsManager::saveSecondaryPage(const String& key, const SecondaryPage& page) {
    std::vector<uint8_t> serializedData = serializeSecondaryPage(page);
    preferences.putBytes(key.c_str(), serializedData.data(), serializedData.size());
}

SecondaryPage MatrixNvsManager::loadSecondaryPage(const String& key, const SecondaryPage& defaultValue) {
    size_t dataSize = preferences.getBytesLength(key.c_str());
    if (dataSize == 0) {
        return defaultValue; // Key doesn't exist or empty
    }
    
    std::vector<uint8_t> data(dataSize);
    size_t actualSize = preferences.getBytes(key.c_str(), data.data(), dataSize);
    
    if (actualSize != dataSize) {
        return defaultValue; // Read error
    }
    
    SecondaryPage result = deserializeSecondaryPage(data);
    return result.empty() ? defaultValue : result;
}

// Convenience methods for page naming
void MatrixNvsManager::savePage(int row, int col, const SecondaryPage& page) {
    String key = "Page_" + String(row) + "_" + String(col);
    saveSecondaryPage(key, page);
}

SecondaryPage MatrixNvsManager::loadPage(int row, int col, const SecondaryPage& defaultValue) {
    String key = "Page_" + String(row) + "_" + String(col);
    return loadSecondaryPage(key, defaultValue);
}

// Page index operations
void MatrixNvsManager::savePageIndex(int row, int index) {
    String key = "page_" + String(row);
    preferences.putInt(key.c_str(), index);
}

int MatrixNvsManager::loadPageIndex(int row) {
   char key[16];
   snprintf(key, sizeof(key), "page_%d", row);
   return preferences.getInt(key, 0);
}

// Clear all data
void MatrixNvsManager::clearAll() {
    preferences.clear();
}

// Check if key exists
bool MatrixNvsManager::hasKey(const String& key) {
    return preferences.isKey(key.c_str());
}

// Helper method to serialize AlarmConfig to byte array
std::vector<uint8_t> MatrixNvsManager::serializeAlarmConfig(const AlarmConfig& config) {
    std::vector<uint8_t> data;
    
    // Calculate total size needed
    // enabled (1 byte) + hour (1 byte) + minute (1 byte) + daysOfWeek (7 bytes)
    const size_t totalSize = sizeof(bool) + sizeof(uint8_t) + sizeof(uint8_t) + 7 * sizeof(bool);
    data.resize(totalSize);
    
    size_t offset = 0;
    
    // Copy enabled flag
    memcpy(data.data() + offset, &config.enabled, sizeof(bool));
    offset += sizeof(bool);
    
    // Copy hour
    memcpy(data.data() + offset, &config.hour, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    
    // Copy minute
    memcpy(data.data() + offset, &config.minute, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    
    // Copy daysOfWeek array
    memcpy(data.data() + offset, config.daysOfWeek, 7 * sizeof(bool));
    
    return data;
}

// Helper method to deserialize byte array to AlarmConfig
AlarmConfig MatrixNvsManager::deserializeAlarmConfig(const std::vector<uint8_t>& data) {
    AlarmConfig config;
    
    // Expected size: 1 + 1 + 1 + 7 = 10 bytes
    const size_t expectedSize = sizeof(bool) + sizeof(uint8_t) + sizeof(uint8_t) + 7 * sizeof(bool);
    
    if (data.size() != expectedSize) {
        // Invalid data size, return default values
        config.enabled = false;
        config.hour = 0;
        config.minute = 0;
        for (int i = 0; i < 7; i++) {
            config.daysOfWeek[i] = false;
        }
        return config;
    }
    
    size_t offset = 0;
    
    // Read enabled flag
    memcpy(&config.enabled, data.data() + offset, sizeof(bool));
    offset += sizeof(bool);
    
    // Read hour
    memcpy(&config.hour, data.data() + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    
    // Read minute
    memcpy(&config.minute, data.data() + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    
    // Read daysOfWeek array
    memcpy(config.daysOfWeek, data.data() + offset, 7 * sizeof(bool));
    
    return config;
}

// Save AlarmConfig to NVS
void MatrixNvsManager::saveAlarm(int index, const AlarmConfig& config) {
    char key[16];
    snprintf(key, sizeof(key), "alarm_%d", index);
    
    std::vector<uint8_t> serializedData = serializeAlarmConfig(config);
    preferences.putBytes(key, serializedData.data(), serializedData.size());
}

// Load AlarmConfig from NVS
AlarmConfig MatrixNvsManager::loadAlarm(int index, const AlarmConfig& defaultConfig) {
    char key[16];
    snprintf(key, sizeof(key), "alarm_%d", index);
    
    size_t dataSize = preferences.getBytesLength(key);
    
    // If key doesn't exist or empty, return default AlarmConfig
    if (dataSize == 0) {
        return defaultConfig;
    }
    
    std::vector<uint8_t> data(dataSize);
    size_t actualSize = preferences.getBytes(key, data.data(), dataSize);
    
    // If read error, return default AlarmConfig
    if (actualSize != dataSize) {
        return defaultConfig;
    }
    
    return deserializeAlarmConfig(data);
}