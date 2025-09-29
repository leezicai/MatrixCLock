#include "matrixData.h"

// Global instance
MatrixDataManager matrixDataManager;

MatrixDataManager::MatrixDataManager() {
}

MatrixDataManager::~MatrixDataManager() {
    // dataManager is a reference to global object, don't delete
}

void MatrixDataManager::init() {
    // Initialize the underlying data manager
    // matrixNvsManager.initializeDefaults();
    
    // No need to pre-write default values
    // All get methods already have default values that will be returned
    // when keys don't exist. This saves NVS space and flash write cycles.
}

void MatrixDataManager::setAutoMode(bool value) {
	matrixNvsManager.saveBool(NVS_KEY_AUTO_MODE, value);
}
bool MatrixDataManager::getAutoMode() {
	return matrixNvsManager.loadBool(NVS_KEY_AUTO_MODE, false);
}



// System Settings Implementation
void MatrixDataManager::setBrightness(int value) {
    matrixNvsManager.saveInt(NVS_KEY_MANUAL_BRIGHTNESS, value);
	matrixNvsManager.commit();
}

int MatrixDataManager::getBrightness() {
    return matrixNvsManager.loadInt(NVS_KEY_MANUAL_BRIGHTNESS, DEFAULT_BRIGHTNESS);
}
void MatrixDataManager::setTimezone(int value) {
  matrixNvsManager.saveInt(NVS_KEY_TIMEZONE, value);
  matrixNvsManager.commit();
}
int MatrixDataManager::getTimezone() {
  return matrixNvsManager.loadInt(NVS_KEY_TIMEZONE, 8);
}

void MatrixDataManager::setMinBrightness(int value) {
  matrixNvsManager.saveInt(NVS_KEY_MIN_BRIGHTNESS, value);
  matrixNvsManager.commit();
}
int MatrixDataManager::getMinBrightness() {
  return matrixNvsManager.loadInt(NVS_KEY_MIN_BRIGHTNESS, DEFAULT_BRIGHTNESS);
}

void MatrixDataManager::setMaxBrightness(int value) {
  matrixNvsManager.saveInt(NVS_KEY_MAX_BRIGHTNESS, value);
  matrixNvsManager.commit();
}
int MatrixDataManager::getMaxBrightness() {
  return matrixNvsManager.loadInt(NVS_KEY_MAX_BRIGHTNESS, DEFAULT_BRIGHTNESS);
}

void MatrixDataManager::setManBrightness(int value) {
  matrixNvsManager.saveInt(NVS_KEY_MANUAL_BRIGHTNESS, value);
}
int MatrixDataManager::getManBrightness() {
  return matrixNvsManager.loadInt(NVS_KEY_MANUAL_BRIGHTNESS, DEFAULT_BRIGHTNESS);
}


void MatrixDataManager::setWifiSSID(const String& ssid) {
    matrixNvsManager.saveString(NVS_KEY_SSID, ssid);
	matrixNvsManager.commit();
}

String MatrixDataManager::getWifiSSID() {
    return matrixNvsManager.loadString(NVS_KEY_SSID, "");
}

void MatrixDataManager::setWifiPassword(const String& password) {
    matrixNvsManager.saveString(NVS_KEY_PASSWORD, password);
	matrixNvsManager.commit();
}

String MatrixDataManager::getWifiPassword() {
    return matrixNvsManager.loadString(NVS_KEY_PASSWORD, "");
}

void MatrixDataManager::setAutoStart(bool enabled) {
    matrixNvsManager.saveBool(NVS_KEY_AUTO_MODE, enabled);
	matrixNvsManager.commit();
}

bool MatrixDataManager::getAutoStart() {
    return matrixNvsManager.loadBool(NVS_KEY_AUTO_MODE, DEFAULT_AUTO_START);
}

void MatrixDataManager::setWifiConfig(bool enabled) {
    matrixNvsManager.saveBool(NVS_KEY_WIFI_CONFIGURED, enabled);
	matrixNvsManager.commit();
}

bool MatrixDataManager::getWifiConfig() {
    return matrixNvsManager.loadBool(NVS_KEY_WIFI_CONFIGURED, false);
}

void MatrixDataManager::setLanguage(int16_t lang) {
    matrixNvsManager.saveInt(NVS_KEY_LANGUAGE, lang);
	matrixNvsManager.commit();
}

int16_t MatrixDataManager::getLanguage() {
    return matrixNvsManager.loadInt(NVS_KEY_LANGUAGE, 0);
}

// Display Settings Implementation
void MatrixDataManager::setContrast(int value) {
    matrixNvsManager.saveInt(NVS_KEY_CONTRAST, value);
	matrixNvsManager.commit();
}

int MatrixDataManager::getContrast() {
    return matrixNvsManager.loadInt(NVS_KEY_CONTRAST, DEFAULT_CONTRAST);
}

void MatrixDataManager::savePage(int row, int16_t pageIndex) {
  matrixNvsManager.savePageIndex(row, pageIndex);
  matrixNvsManager.commit();
}
int16_t MatrixDataManager::loadPage(int row) {
  return matrixNvsManager.loadPageIndex(row);
}

// Page Data Operations Implementation
void MatrixDataManager::savePage(int row, int col, const SecondaryPage& page) {
    matrixNvsManager.savePage(row, col, page);
	matrixNvsManager.commit();
}

SecondaryPage MatrixDataManager::loadPage(int row, int col) {
    SecondaryPage defaultPage;
    return matrixNvsManager.loadPage(row, col, defaultPage);
}

void MatrixDataManager::savePageByKey(const String& key, const SecondaryPage& page) {
    matrixNvsManager.saveSecondaryPage(key, page);
	matrixNvsManager.commit();
}

SecondaryPage MatrixDataManager::loadPageByKey(const String& key) {
    SecondaryPage defaultPage;
    return matrixNvsManager.loadSecondaryPage(key, defaultPage);
}



// Utility Methods Implementation
void MatrixDataManager::clearAllData() {
    matrixNvsManager.clearAll();
	matrixNvsManager.commit();
}

bool MatrixDataManager::hasKey(const String& key) {
    return matrixNvsManager.hasKey(key);
}

void MatrixDataManager::resetToDefaults() {
    clearAllData();
    init();
}