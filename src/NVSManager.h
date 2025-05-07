/**
 * @file NVSManager.h
 * @brief Non-volatile storage manager for ESP32S3
 * 
 * This class provides an interface for reading and writing data to the
 * ESP32's Non-Volatile Storage (NVS).
 */

#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include "nvs.h"
#include "nvs_flash.h"
#include <string>

/**
 * @class NVSManager
 * @brief Class to manage operations on ESP32's Non-Volatile Storage
 */
class NVSManager {
public:
    /**
     * @brief Constructor for NVSManager
     * @param namespace_name The namespace to use for NVS operations
     */
    NVSManager(const char* namespace_name = "storage");

    /**
     * @brief Destructor for NVSManager
     */
    ~NVSManager();

    /**
     * @brief Initialize the NVS flash
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t init();

    /**
     * @brief Write an integer value to NVS
     * @param key The key under which to store the value
     * @param value The integer value to store
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t writeInt(const char* key, int32_t value);

    /**
     * @brief Read an integer value from NVS
     * @param key The key to read
     * @param defaultValue Value to return if key doesn't exist
     * @param[out] valueExists Optional pointer to boolean that will be set to true if key exists
     * @return The integer value read, or defaultValue if key doesn't exist
     */
    int32_t readInt(const char* key, int32_t defaultValue = 0, bool* valueExists = nullptr);
    
    /**
     * @brief Write a boolean value to NVS
     * @param key The key under which to store the value
     * @param value The boolean value to store
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t writeBool(const char* key, bool value);

    /**
     * @brief Read a boolean value from NVS
     * @param key The key to read
     * @param defaultValue Value to return if key doesn't exist
     * @param[out] valueExists Optional pointer to boolean that will be set to true if key exists
     * @return The boolean value read, or defaultValue if key doesn't exist
     */
    bool readBool(const char* key, bool defaultValue = false, bool* valueExists = nullptr);

    /**
     * @brief Write a float value to NVS
     * @param key The key under which to store the value
     * @param value The float value to store
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t writeFloat(const char* key, float value);

    /**
     * @brief Read a float value from NVS
     * @param key The key to read
     * @param defaultValue Value to return if key doesn't exist
     * @param[out] valueExists Optional pointer to boolean that will be set to true if key exists
     * @return The float value read, or defaultValue if key doesn't exist
     */
    float readFloat(const char* key, float defaultValue = 0.0f, bool* valueExists = nullptr);

    /**
     * @brief Write a string value to NVS
     * @param key The key under which to store the value
     * @param value The string value to store
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t writeString(const char* key, const std::string& value);

    /**
     * @brief Read a string value from NVS
     * @param key The key to read
     * @param defaultValue Value to return if key doesn't exist
     * @param[out] valueExists Optional pointer to boolean that will be set to true if key exists
     * @return The string value read, or defaultValue if key doesn't exist
     */
    std::string readString(const char* key, const std::string& defaultValue = "", bool* valueExists = nullptr);

    /**
     * @brief Write a blob (binary data) to NVS
     * @param key The key under which to store the value
     * @param data Pointer to the data to store
     * @param length Length of the data in bytes
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t writeBlob(const char* key, const void* data, size_t length);

    /**
     * @brief Read a blob (binary data) from NVS
     * @param key The key to read
     * @param data Buffer to store the read data
     * @param maxLength Maximum length of the buffer
     * @param[out] valueExists Optional pointer to boolean that will be set to true if key exists
     * @return The number of bytes read, or 0 if key doesn't exist
     */
    size_t readBlob(const char* key, void* data, size_t maxLength, bool* valueExists = nullptr);

    /**
     * @brief Check if a key exists in NVS
     * @param key The key to check
     * @return true if the key exists, false otherwise
     */
    bool keyExists(const char* key);

    /**
     * @brief Remove a key-value pair from NVS
     * @param key The key to remove
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t removeKey(const char* key);

    /**
     * @brief Erase all key-value pairs in the namespace
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t eraseAll();

    /**
     * @brief Commit any pending changes to NVS
     * @return ESP_OK if successful, error code otherwise
     */
    esp_err_t commit();

    /**
     * @brief Get the last error that occurred
     * @return The last error code
     */
    esp_err_t getLastError() const { return mLastError; }

    /**
     * @brief Convert an NVS error code to a string
     * @param err The error code to convert
     * @return A string representation of the error
     */
    static const char* errorToString(esp_err_t err);

private:
    nvs_handle_t mHandle;        ///< Handle for NVS operations
    esp_err_t mLastError;        ///< Last error that occurred
    const char* mNamespace;      ///< Namespace being used
    bool mInitialized;           ///< Whether NVS has been initialized
};

#endif // NVS_MANAGER_H