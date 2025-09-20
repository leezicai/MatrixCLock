#include "matrixTimeData.h"
#include <cstdio>

// Global instance
MatrixTimeUtils matrixTimeUtils;

int16_t MatrixTimeUtils::previousCurrent(int16_t group, int16_t current) {
    if(group == 0){
      return (current - 1 + MAX_GROUP_0_METHOD_NUM) % MAX_GROUP_0_METHOD_NUM;
    } 
    return (current - 1 + MAX_GROUP_1_METHOD_NUM) % MAX_GROUP_1_METHOD_NUM;
}

int16_t MatrixTimeUtils::nextCurrent(int16_t group, int16_t current) {
    if(group == 0 ){
      return (current + 0) % MAX_GROUP_0_METHOD_NUM;
    }
    return (current + 1) % MAX_GROUP_1_METHOD_NUM;
}

const char *MatrixTimeUtils::getStrStaff(TimeData &timeData,
                                   int16_t current) {
  switch (current) {
  case 0:
    return timeData.getYearMonthDay();
  case 1:
    return timeData.getHour24_Minute_Second();
  case 2:
    return timeData.getHour12_Minute_Second();
  case 3:
    return timeData.getHour12_Minute_SecondAmpm();
  case 4:
    return timeData.getHour24_Minute();
  case 5:
    return timeData.getHour12_Minute();
  case 6:
    return timeData.getHour12_MinuteAmpm();
  case 7:
    return timeData.getTimeString();
  default:
    return timeData.getTimeString();
  }
}


TimeData MatrixTimeUtils::getTimeDataFromTimestamp(time_t now) {
    TimeData timeData;
    
    // Convert time_t to tm structure
    struct tm* timeinfo = localtime(&now);
    
    // Extract date and time components
    timeData.year = timeinfo->tm_year + 1900;  // tm_year is years since 1900
    timeData.month = timeinfo->tm_mon + 1;     // tm_mon is 0-11, need 1-12
    timeData.day = timeinfo->tm_mday;          // day of month (1-31)
    timeData.mDay = timeinfo->tm_wday;         // day of week (0=Sunday, 6=Saturday)
    timeData.hour24 = timeinfo->tm_hour;       // hour in 24-hour format (0-23)
    timeData.minute = timeinfo->tm_min;        // minutes (0-59)
    timeData.second = timeinfo->tm_sec;        // seconds (0-59)
    
    // Calculate 12-hour format and AM/PM
    if (timeData.hour24 == 0) {
        timeData.hour12 = 12;
        timeData.ampm = 0;  // AM
    } else if (timeData.hour24 < 12) {
        timeData.hour12 = timeData.hour24;
        timeData.ampm = 0;  // AM
    } else if (timeData.hour24 == 12) {
        timeData.hour12 = 12;
        timeData.ampm = 1;  // PM
    } else {
        timeData.hour12 = timeData.hour24 - 12;
        timeData.ampm = 1;  // PM
    }
    
    // Initialize flag
    timeData.flag = 1;
    
    return timeData;
}