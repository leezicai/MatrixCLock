#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>
#include <time.h>
#include "matrixData.h"
#include "matrixSetting.h"
#include "matrixTimeUtils.h"

// Maximum number of alarms supported (can be changed)
#define MAX_ALARMS 3

// Maximum number of alarm tasks in 24 hours queue
#define MAX_ALARM_TASKS 50

// Days of week enum
enum DayOfWeek {
  SUNDAY = 0,
  MONDAY = 1,
  TUESDAY = 2,
  WEDNESDAY = 3,
  THURSDAY = 4,
  FRIDAY = 5,
  SATURDAY = 6
};

// Single alarm configuration structure
struct AlarmConfig {
  bool enabled;                // Alarm enabled/disabled
  uint8_t hour;               // Hour (0-23)
  uint8_t minute;             // Minute (0-59)
  bool daysOfWeek[7];         // Which days of week are enabled (Sun-Sat)
};

// Alarm task in queue (scheduled alarm instance)
struct AlarmTask {
  uint8_t alarmIndex;         // Which alarm triggered this task
  time_t triggerTime;         // Unix timestamp when alarm should trigger
  bool active;                // Is this task active
};

class AlarmManager {
private:
  AlarmConfig alarms[MAX_ALARMS];        // All alarm configurations
  AlarmTask taskQueue[MAX_ALARM_TASKS];  // 24-hour alarm task queue
  uint8_t currentIndex;                   // Current selected alarm index
  time_t lastCheckTime;                   // Last time we checked/updated tasks
  
  // Private helper methods
  void sortTaskQueue();                   // Sort task queue by trigger time
  void removeExpiredTasks();              // Remove tasks older than current time
  bool isTaskInQueue(uint8_t alarmIdx, time_t triggerTime);  // Check if task exists
  time_t getNextTriggerTime(uint8_t alarmIdx, time_t fromTime);  // Calculate next trigger
  void addTaskToQueue(uint8_t alarmIdx, time_t triggerTime);     // Add task to queue
  void removeTasksForAlarm(uint8_t alarmIdx);  // Remove all tasks for specific alarm
  
public:
  // Constructor
  AlarmManager();
  
  // Initialization
  void begin();
  
  // Alarm index management
  void nextAlarm();           // Switch to next alarm
  void prevAlarm();           // Switch to previous alarm
  uint8_t getCurrentIndex();  // Get current alarm index

  void incrementHour();
  void decrementHour();
  void incrementMinute();
  void decrementMinute();

  // Get/Set current alarm properties
  bool isEnabled();
  void setEnabled(bool enabled);
  
  uint8_t getHour();
  void setHour(uint8_t hour);
  
  uint8_t getMinute();
  void setMinute(uint8_t minute);
  
  bool getDayEnabled(DayOfWeek day);
  void setDayEnabled(DayOfWeek day, bool enabled);
  
  // Direct access to specific alarm (by index)
  bool isAlarmEnabled(uint8_t index);
  uint8_t getAlarmHour(uint8_t index);
  uint8_t getAlarmMinute(uint8_t index);
  bool getAlarmDayEnabled(uint8_t index, DayOfWeek day);
  
  // Task queue management
  void updateTaskQueue();     // Update 24-hour task queue
  void checkAndTrigger();     // Check if any alarm should trigger now
  
  // Get task queue info (for debugging)
  void printTaskQueue();
  uint8_t getActiveTaskCount();

  const char* getAlarmStr(int index);
};

// Global alarm manager instance
extern AlarmManager alarmManager;

#endif // ALARM_H