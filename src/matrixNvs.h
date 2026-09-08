#ifndef MATRIX_NVS_H
#define MATRIX_NVS_H

#include <Preferences.h>
#include <vector>
#include <cstdint>
#include <string>
#include "matrixCore.h"
#include "alarm.h"

struct AlarmConfig;

struct MatrixCore;
typedef std::vector<MatrixCore> SecondaryPage;

class MatrixNvsManager {
private:
  Preferences preferences;
  static const char *NVS_NAMESPACE;

  // Helper methods for SecondaryPage serialization
  std::vector<uint8_t> serializeSecondaryPage(const SecondaryPage &page);
  SecondaryPage deserializeSecondaryPage(const std::vector<uint8_t> &data);

  std::vector<uint8_t> serializeAlarmConfig(const AlarmConfig &config);
  AlarmConfig deserializeAlarmConfig(const std::vector<uint8_t> &data);

public:
  MatrixNvsManager();
  ~MatrixNvsManager();

  // Initialize all default values (call in setup)
  void initializeDefaults();

  // Commit changes to flash (must call after write operations)
  void commit();

  // Bool operations
  void saveBool(const String &key, bool value);
  bool loadBool(const String &key, bool defaultValue);

  // Int operations
  void saveInt(const String &key, int value);
  int loadInt(const String &key, int defaultValue);

  // String operations
  void saveString(const String &key, const String &value);
  String loadString(const String &key, const String &defaultValue);

  // SecondaryPage operations
  void saveSecondaryPage(const String &key, const SecondaryPage &page);
  SecondaryPage loadSecondaryPage(const String &key,
                                  const SecondaryPage &defaultValue);

  // Convenience methods for common page naming patterns
  void savePage(int row, int col, const SecondaryPage &page);
  SecondaryPage loadPage(int row, int col, const SecondaryPage &defaultValue);

  // Page index operations
  void savePageIndex(int row, int index);
  int loadPageIndex(int row);

  // Clear all data
  void clearAll();

  // Check if key exists
  bool hasKey(const String &key);

  void saveAlarm(int index, const AlarmConfig &config);
  AlarmConfig loadAlarm(int index, const AlarmConfig& defaultConfig);

};

// Global instance for easy access
extern MatrixNvsManager matrixNvsManager;

#endif // MATRIX_NVS_H