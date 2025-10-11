#include "alarm.h"
#include "max98357.h"  // Assume this is your audio manager
#include "alarm_iphone.h"


// Global instance
AlarmManager alarmManager;

// Constructor
AlarmManager::AlarmManager() {}

// Initialization
void AlarmManager::begin() {
  currentIndex = matrixDataManager.loadAlarmIndex();
  lastCheckTime = 0;

  // Initialize all alarms to disabled
  AlarmConfig alarmConfigs[MAX_ALARMS] = {
      {false, 7, 0, {false, true, true, true, true, true, false}},
      {false, 7, 0, {false, true, true, true, true, true, false}},
      {false, 7, 0, {false, true, true, true, true, true, false}}};

  for (uint8_t i = 0; i < MAX_ALARMS; i++) {
    alarms[i] = matrixDataManager.loadAlarm(i, alarmConfigs[i]);
  }

  // Initialize task queue
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    taskQueue[i].active = false;
    taskQueue[i].alarmIndex = 0;
    taskQueue[i].triggerTime = 0;
  }

  updateTaskQueue();

  // Initial task queue update
}

// Switch to next alarm
void AlarmManager::nextAlarm() {
  currentIndex = (currentIndex + 1) % MAX_ALARMS;
  matrixDataManager.saveAlarmIndex(currentIndex);
}

// Switch to previous alarm
void AlarmManager::prevAlarm() {
  currentIndex = (currentIndex - 1 + MAX_ALARMS) % MAX_ALARMS;
  matrixDataManager.saveAlarmIndex(currentIndex);
}

// Get current alarm index
uint8_t AlarmManager::getCurrentIndex() {
  return currentIndex;
}

void AlarmManager::incrementHour() {
  alarms[currentIndex].hour = (alarms[currentIndex].hour + 1) % 24;
  matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
  updateTaskQueue();  // Update queue when time changes
}

void AlarmManager::decrementHour() {
  alarms[currentIndex].hour = (alarms[currentIndex].hour - 1 + 24) % 24;
  matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
  updateTaskQueue();  // Update queue when time changes
}

void AlarmManager::incrementMinute() {
  alarms[currentIndex].minute = (alarms[currentIndex].minute + 1) % 60;
  matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
  updateTaskQueue();  // Update queue when time changes
}

void AlarmManager::decrementMinute() {
  alarms[currentIndex].minute = (alarms[currentIndex].minute - 1 + 60) % 60;
  matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
  updateTaskQueue();  // Update queue when time changes
}

// Get/Set current alarm enabled state
bool AlarmManager::isEnabled() {
  return alarms[currentIndex].enabled;
}

void AlarmManager::setEnabled(bool enabled) {
  alarms[currentIndex].enabled = enabled;
	matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
  updateTaskQueue();  // Update queue when alarm is enabled/disabled
}

// Get/Set current alarm hour
uint8_t AlarmManager::getHour() {
  return alarms[currentIndex].hour;
}

void AlarmManager::setHour(uint8_t hour) {
  if (hour < 24) {
    alarms[currentIndex].hour = hour;
	matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
    updateTaskQueue();  // Update queue when time changes
  }
}

// Get/Set current alarm minute
uint8_t AlarmManager::getMinute() {
  return alarms[currentIndex].minute;
}

void AlarmManager::setMinute(uint8_t minute) {
  if (minute < 60) {
    alarms[currentIndex].minute = minute;
	matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
    updateTaskQueue();  // Update queue when time changes
  }
}

// Get/Set day enabled for current alarm
bool AlarmManager::getDayEnabled(DayOfWeek day) {
  if (day >= 0 && day <= 6) {
    return alarms[currentIndex].daysOfWeek[day];
  }
  return false;
}

void AlarmManager::setDayEnabled(DayOfWeek day, bool enabled) {
  if (day >= 0 && day <= 6) {
    alarms[currentIndex].daysOfWeek[day] = enabled;
	matrixDataManager.saveAlarm(currentIndex, alarms[currentIndex]);
    updateTaskQueue();  // Update queue when days change
  }
}

// Direct access to specific alarm by index
bool AlarmManager::isAlarmEnabled(uint8_t index) {
  if (index < MAX_ALARMS) {
    return alarms[index].enabled;
  }
  return false;
}

uint8_t AlarmManager::getAlarmHour(uint8_t index) {
  if (index < MAX_ALARMS) {
    return alarms[index].hour;
  }
  return 0;
}

uint8_t AlarmManager::getAlarmMinute(uint8_t index) {
  if (index < MAX_ALARMS) {
    return alarms[index].minute;
  }
  return 0;
}

bool AlarmManager::getAlarmDayEnabled(uint8_t index, DayOfWeek day) {
  if (index < MAX_ALARMS && day >= 0 && day <= 6) {
    return alarms[index].daysOfWeek[day];
  }
  return false;
}

// Calculate next trigger time for an alarm
time_t AlarmManager::getNextTriggerTime(uint8_t alarmIdx, time_t fromTime) {
  if (alarmIdx >= MAX_ALARMS || !alarms[alarmIdx].enabled) {
    return 0;
  }
  
  struct tm timeinfo;
  localtime_r(&fromTime, &timeinfo);
  
  // Check next 8 days to find matching day of week
  for (int dayOffset = 0; dayOffset < 8; dayOffset++) {
    time_t checkTime = fromTime + (dayOffset * 86400);  // 86400 = seconds in a day
    struct tm checkInfo;
    localtime_r(&checkTime, &checkInfo);
    
    // Set alarm time
    checkInfo.tm_hour = alarms[alarmIdx].hour;
    checkInfo.tm_min = alarms[alarmIdx].minute;
    checkInfo.tm_sec = 0;
    
    time_t alarmTime = mktime(&checkInfo);
    
    // Check if this day is enabled and time is in future
    if (alarms[alarmIdx].daysOfWeek[checkInfo.tm_wday] && alarmTime > fromTime) {
      return alarmTime;
    }
  }
  
  return 0;
}

// Check if task already exists in queue
bool AlarmManager::isTaskInQueue(uint8_t alarmIdx, time_t triggerTime) {
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    if (taskQueue[i].active && 
        taskQueue[i].alarmIndex == alarmIdx && 
        taskQueue[i].triggerTime == triggerTime) {
      return true;
    }
  }
  return false;
}

// Add task to queue
void AlarmManager::addTaskToQueue(uint8_t alarmIdx, time_t triggerTime) {
  // Find empty slot
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    if (!taskQueue[i].active) {
      taskQueue[i].active = true;
      taskQueue[i].alarmIndex = alarmIdx;
      taskQueue[i].triggerTime = triggerTime;
      
      Serial.printf("Added alarm task: Alarm[%d] at %ld\n", alarmIdx, triggerTime);
      return;
    }
  }
  Serial.println("Warning: Task queue full!");
}

// Remove all tasks for specific alarm
void AlarmManager::removeTasksForAlarm(uint8_t alarmIdx) {
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    if (taskQueue[i].active && taskQueue[i].alarmIndex == alarmIdx) {
      taskQueue[i].active = false;
      Serial.printf("Removed task for alarm[%d]\n", alarmIdx);
    }
  }
}

// Remove expired tasks (older than current time)
void AlarmManager::removeExpiredTasks() {
  time_t now = time(nullptr);
  
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    if (taskQueue[i].active && taskQueue[i].triggerTime < now) {
      taskQueue[i].active = false;
    }
  }
}

// Sort task queue by trigger time (bubble sort, simple for small arrays)
void AlarmManager::sortTaskQueue() {
  for (uint8_t i = 0; i < MAX_ALARM_TASKS - 1; i++) {
    for (uint8_t j = 0; j < MAX_ALARM_TASKS - i - 1; j++) {
      if (taskQueue[j].active && taskQueue[j + 1].active) {
        if (taskQueue[j].triggerTime > taskQueue[j + 1].triggerTime) {
          // Swap
          AlarmTask temp = taskQueue[j];
          taskQueue[j] = taskQueue[j + 1];
          taskQueue[j + 1] = temp;
        }
      } else if (!taskQueue[j].active && taskQueue[j + 1].active) {
        // Move active tasks to front
        AlarmTask temp = taskQueue[j];
        taskQueue[j] = taskQueue[j + 1];
        taskQueue[j + 1] = temp;
      }
    }
  }
}

// Update 24-hour task queue
void AlarmManager::updateTaskQueue() {
  time_t now = time(nullptr);
  time_t next24Hours = now + (24 * 3600);
  
  Serial.println("=== UPDATE TASK QUEUE START ===");
  
  // Remove expired tasks
  removeExpiredTasks();
  
  // For each alarm, rebuild its tasks from scratch
  for (uint8_t i = 0; i < MAX_ALARMS; i++) {
    // ✅ Always remove old tasks first (whether enabled or not)
    removeTasksForAlarm(i);
    
    if (!alarms[i].enabled) {
      continue;  // Skip disabled alarms
    }
    
    // Find all occurrences in next 24 hours
    time_t checkTime = now;
    while (checkTime < next24Hours) {
      time_t nextTrigger = getNextTriggerTime(i, checkTime);
      
      if (nextTrigger == 0 || nextTrigger >= next24Hours) {
        break;
      }
      
      // Add task (no need to check if exists, we just cleared all)
      addTaskToQueue(i, nextTrigger);
      
      checkTime = nextTrigger + 60;
    }
  }
  
  sortTaskQueue();
  lastCheckTime = now;
  
  Serial.println("=== UPDATE TASK QUEUE END ===");
  printTaskQueue();
}

// Check if any alarm should trigger now
void AlarmManager::checkAndTrigger() {
  time_t now = time(nullptr);
  
  // Check first active task in queue (queue is sorted)
  if (taskQueue[0].active) {
    // Trigger if within 60 seconds window (to avoid missing due to check interval)
    if (abs((long)(taskQueue[0].triggerTime - now)) <= 30) {
      uint8_t alarmIdx = taskQueue[0].alarmIndex;
      
      Serial.printf("*** ALARM TRIGGERED: Alarm[%d] ***\n", alarmIdx);
      
      // Play alarm sound
      max98357Manager.playAlarm(alarm_iphone_wav, alarm_iphone_wav_len, 20);
      
      // Mark task as inactive (remove it)
      taskQueue[0].active = false;
      
      // Re-sort queue to move inactive task to back
      sortTaskQueue();
      
      // Schedule next occurrence of this alarm
      updateTaskQueue();
    }
  }
}

// Print task queue for debugging
void AlarmManager::printTaskQueue() {
  Serial.println("\n=== Alarm Task Queue ===");
  
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    if (taskQueue[i].active) {
      struct tm timeinfo;
      localtime_r(&taskQueue[i].triggerTime, &timeinfo);
      
      Serial.printf("[%d] Alarm[%d] - %04d-%02d-%02d %02d:%02d\n",
        count++,
        taskQueue[i].alarmIndex,
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min
      );
    }
  }
  
  if (count == 0) {
    Serial.println("(No active tasks)");
  }
  Serial.println("========================\n");
}

// Get active task count
uint8_t AlarmManager::getActiveTaskCount() {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_ALARM_TASKS; i++) {
    if (taskQueue[i].active) {
      count++;
    }
  }
  return count;
}

const char* AlarmManager::getAlarmStr(int index){
  static char buffer[30];  // unified buffer, 30 bytes is enough for all cases
  
  switch (index) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    snprintf(buffer, sizeof(buffer), "%s%s",
             matrixTimeUtils.getShortShortWeekday(index),
             getDayEnabled(static_cast<DayOfWeek>(index)) ? "+" : "-");
    return buffer;
    
  case 7:
    snprintf(buffer, sizeof(buffer), "%s%d", 
             matrixSettings.getCommonWord(WORD_ALARM), currentIndex);
    return buffer;
  
  case 8:
    return alarms[currentIndex].enabled
               ? matrixSettings.getCommonWord(WORD_ON)
               : matrixSettings.getCommonWord(WORD_OFF);
               
  case 9:
    snprintf(buffer, sizeof(buffer), "%u", getHour());
    return buffer;
  
  case 10:
    return ":";
    
  case 11:
    snprintf(buffer, sizeof(buffer), "%u", getMinute());
    return buffer;
  
  default:
    return "";
  }
}