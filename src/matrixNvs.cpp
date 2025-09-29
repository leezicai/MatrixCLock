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
void MatrixNvsManager::savePageIndex(int row, int16_t index) {
    String key = "page_" + String(row);
    preferences.putShort(key.c_str(), index);
}

int16_t MatrixNvsManager::loadPageIndex(int row) {
    String key = "page_" + String(row);
    return preferences.getShort(key.c_str(), 0);
}

// Clear all data
void MatrixNvsManager::clearAll() {
    preferences.clear();
}

// Check if key exists
bool MatrixNvsManager::hasKey(const String& key) {
    return preferences.isKey(key.c_str());
}