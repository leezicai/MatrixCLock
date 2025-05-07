/**
 * @file NVSManager.cpp
 * @brief Implementation of NVSManager class for ESP32S3
 */

 #include "NVSManager.h"
 #include "esp_log.h"
 #include <cstring>
 
 static const char* TAG = "NVSManager";
 
 NVSManager::NVSManager(const char* namespace_name) :
     mHandle(0),
     mLastError(ESP_OK),
     mNamespace(namespace_name),
     mInitialized(false) {
 }
 
 NVSManager::~NVSManager() {
     if (mInitialized) {
         nvs_close(mHandle);
     }
 }
 
 esp_err_t NVSManager::init() {
     // Initialize NVS flash
     mLastError = nvs_flash_init();
     if (mLastError == ESP_ERR_NVS_NO_FREE_PAGES || mLastError == ESP_ERR_NVS_NEW_VERSION_FOUND) {
         // NVS partition was truncated or is a new version
         ESP_LOGW(TAG, "Erasing NVS flash due to initialization error");
         ESP_ERROR_CHECK(nvs_flash_erase());
         mLastError = nvs_flash_init();
     }
 
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to initialize NVS: %s", errorToString(mLastError));
         return mLastError;
     }
 
     // Open the NVS namespace
     mLastError = nvs_open(mNamespace, NVS_READWRITE, &mHandle);
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to open NVS namespace '%s': %s", mNamespace, errorToString(mLastError));
         return mLastError;
     }
 
     mInitialized = true;
     ESP_LOGI(TAG, "NVS initialized successfully with namespace '%s'", mNamespace);
     return ESP_OK;
 }
 
 esp_err_t NVSManager::writeInt(const char* key, int32_t value) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     mLastError = nvs_set_i32(mHandle, key, value);
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to set int32 value for key '%s': %s", key, errorToString(mLastError));
         return mLastError;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 int32_t NVSManager::readInt(const char* key, int32_t defaultValue, bool* valueExists) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 
     int32_t value = defaultValue;
     mLastError = nvs_get_i32(mHandle, key, &value);
 
     if (mLastError == ESP_OK) {
         if (valueExists) *valueExists = true;
         return value;
     } else if (mLastError == ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGD(TAG, "Key '%s' not found in NVS, returning default value", key);
         if (valueExists) *valueExists = false;
         mLastError = ESP_OK; // Reset error state since this is an expected condition
         return defaultValue;
     } else {
         ESP_LOGE(TAG, "Error reading int32 value for key '%s': %s", key, errorToString(mLastError));
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 }
 
 esp_err_t NVSManager::writeBool(const char* key, bool value) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     // Store boolean as uint8_t (0 = false, 1 = true)
     uint8_t bool_value = value ? 1 : 0;
     mLastError = nvs_set_u8(mHandle, key, bool_value);
     
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to set boolean value for key '%s': %s", key, errorToString(mLastError));
         return mLastError;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 bool NVSManager::readBool(const char* key, bool defaultValue, bool* valueExists) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 
     uint8_t value = defaultValue ? 1 : 0;
     mLastError = nvs_get_u8(mHandle, key, &value);
 
     if (mLastError == ESP_OK) {
         if (valueExists) *valueExists = true;
         return (value != 0);
     } else if (mLastError == ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGD(TAG, "Key '%s' not found in NVS, returning default boolean value", key);
         if (valueExists) *valueExists = false;
         mLastError = ESP_OK; // Reset error state
         return defaultValue;
     } else {
         ESP_LOGE(TAG, "Error reading boolean value for key '%s': %s", key, errorToString(mLastError));
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 }
 
 esp_err_t NVSManager::writeFloat(const char* key, float value) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     // NVS doesn't have a native float type, so we'll store it as raw bytes
     mLastError = nvs_set_blob(mHandle, key, &value, sizeof(float));
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to set float value for key '%s': %s", key, errorToString(mLastError));
         return mLastError;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 float NVSManager::readFloat(const char* key, float defaultValue, bool* valueExists) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 
     float value = defaultValue;
     size_t required_size = sizeof(float);
     mLastError = nvs_get_blob(mHandle, key, &value, &required_size);
 
     if (mLastError == ESP_OK) {
         if (valueExists) *valueExists = true;
         return value;
     } else if (mLastError == ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGD(TAG, "Key '%s' not found in NVS, returning default float value", key);
         if (valueExists) *valueExists = false;
         mLastError = ESP_OK; // Reset error state
         return defaultValue;
     } else {
         ESP_LOGE(TAG, "Error reading float value for key '%s': %s", key, errorToString(mLastError));
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 }
 
 esp_err_t NVSManager::writeString(const char* key, const std::string& value) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     mLastError = nvs_set_str(mHandle, key, value.c_str());
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to set string value for key '%s': %s", key, errorToString(mLastError));
         return mLastError;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 std::string NVSManager::readString(const char* key, const std::string& defaultValue, bool* valueExists) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 
     // First get required size
     size_t required_size = 0;
     mLastError = nvs_get_str(mHandle, key, nullptr, &required_size);
     
     if (mLastError == ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGD(TAG, "Key '%s' not found in NVS, returning default string value", key);
         if (valueExists) *valueExists = false;
         mLastError = ESP_OK; // Reset error state
         return defaultValue;
     } else if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Error determining string size for key '%s': %s", key, errorToString(mLastError));
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 
     // Allocate buffer and read string
     char* buffer = new char[required_size];
     mLastError = nvs_get_str(mHandle, key, buffer, &required_size);
     
     if (mLastError == ESP_OK) {
         if (valueExists) *valueExists = true;
         std::string result(buffer);
         delete[] buffer;
         return result;
     } else {
         ESP_LOGE(TAG, "Error reading string value for key '%s': %s", key, errorToString(mLastError));
         delete[] buffer;
         if (valueExists) *valueExists = false;
         return defaultValue;
     }
 }
 
 esp_err_t NVSManager::writeBlob(const char* key, const void* data, size_t length) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     mLastError = nvs_set_blob(mHandle, key, data, length);
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to set blob for key '%s': %s", key, errorToString(mLastError));
         return mLastError;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 size_t NVSManager::readBlob(const char* key, void* data, size_t maxLength, bool* valueExists) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         if (valueExists) *valueExists = false;
         return 0;
     }
 
     // First get required size
     size_t required_size = 0;
     mLastError = nvs_get_blob(mHandle, key, nullptr, &required_size);
     
     if (mLastError == ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGD(TAG, "Key '%s' not found in NVS, returning empty blob", key);
         if (valueExists) *valueExists = false;
         mLastError = ESP_OK; // Reset error state
         return 0;
     } else if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Error determining blob size for key '%s': %s", key, errorToString(mLastError));
         if (valueExists) *valueExists = false;
         return 0;
     }
 
     // Check if the buffer is large enough
     if (required_size > maxLength) {
         ESP_LOGW(TAG, "Buffer too small for blob data (key '%s'). Required: %d, Available: %d", 
                 key, required_size, maxLength);
         if (valueExists) *valueExists = true; // The key exists, but we couldn't read it completely
         mLastError = ESP_ERR_NVS_INVALID_LENGTH;
         return 0;
     }
 
     // Read the blob data
     mLastError = nvs_get_blob(mHandle, key, data, &required_size);
     
     if (mLastError == ESP_OK) {
         if (valueExists) *valueExists = true;
         return required_size;
     } else {
         ESP_LOGE(TAG, "Error reading blob data for key '%s': %s", key, errorToString(mLastError));
         if (valueExists) *valueExists = false;
         return 0;
     }
 }
 
 bool NVSManager::keyExists(const char* key) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return false;
     }
 
     // 直接尝试获取键的大小，这会告诉我们键是否存在
     size_t required_size;
     esp_err_t err = nvs_get_blob(mHandle, key, NULL, &required_size);
     
     if (err == ESP_OK) {
         return true;
     } else if (err == ESP_ERR_NVS_NOT_FOUND) {
         // 键不存在不是错误，所以我们不设置mLastError
         return false;
     } else {
         mLastError = err;
         ESP_LOGE(TAG, "Error checking if key '%s' exists: %s", key, errorToString(mLastError));
         return false;
     }
 }
 
 esp_err_t NVSManager::removeKey(const char* key) {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     mLastError = nvs_erase_key(mHandle, key);
     if (mLastError != ESP_OK && mLastError != ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGE(TAG, "Failed to erase key '%s': %s", key, errorToString(mLastError));
         return mLastError;
     }
 
     // If the key didn't exist, it's not an error for our purposes
     if (mLastError == ESP_ERR_NVS_NOT_FOUND) {
         ESP_LOGW(TAG, "Key '%s' not found during removal attempt", key);
         mLastError = ESP_OK;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 esp_err_t NVSManager::eraseAll() {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     mLastError = nvs_erase_all(mHandle);
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to erase all keys: %s", errorToString(mLastError));
         return mLastError;
     }
 
     // Commit the changes
     mLastError = commit();
     return mLastError;
 }
 
 esp_err_t NVSManager::commit() {
     if (!mInitialized) {
         mLastError = ESP_ERR_INVALID_STATE;
         ESP_LOGE(TAG, "NVS not initialized, call init() first");
         return mLastError;
     }
 
     mLastError = nvs_commit(mHandle);
     if (mLastError != ESP_OK) {
         ESP_LOGE(TAG, "Failed to commit changes to NVS: %s", errorToString(mLastError));
     }
     return mLastError;
 }
 
 const char* NVSManager::errorToString(esp_err_t err) {
     switch (err) {
         case ESP_OK: return "No error";
         case ESP_ERR_NVS_NOT_INITIALIZED: return "NVS not initialized";
         case ESP_ERR_NVS_NOT_FOUND: return "Key not found";
         case ESP_ERR_NVS_TYPE_MISMATCH: return "Type mismatch";
         case ESP_ERR_NVS_READ_ONLY: return "NVS is read-only";
         case ESP_ERR_NVS_NOT_ENOUGH_SPACE: return "Not enough space in NVS partition";
         case ESP_ERR_NVS_NEW_VERSION_FOUND: return "New version found";
         case ESP_ERR_NVS_NO_FREE_PAGES: return "No free pages";
         case ESP_ERR_NVS_INVALID_NAME: return "Invalid name";
         case ESP_ERR_NVS_INVALID_HANDLE: return "Invalid handle";
         case ESP_ERR_NVS_INVALID_LENGTH: return "Invalid length";
         case ESP_ERR_NVS_PAGE_FULL: return "Page full";
         case ESP_ERR_NVS_PART_NOT_FOUND: return "Partition not found";
         default: return "Unknown error";
     }
 }