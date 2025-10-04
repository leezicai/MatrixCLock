#pragma once

# define MAX_GROUP_0_METHOD_NUM 8
# define MAX_GROUP_1_METHOD_NUM 8
#include <string>

struct TimeData {
    int16_t year;
    int16_t month;
    int16_t day;
    int16_t mDay;
    int16_t hour24;
    int16_t minute;
    int16_t second;
    
    int16_t hour12;
    int16_t ampm;

	mutable int16_t flag = 1;
    mutable char temp_buffer[16];
    
    // Get year as string (4 digits)
    const char* getStrYear() const {
        sprintf(temp_buffer, "%04d", year);
        return temp_buffer;
    }
    
    // Get month as string (2 digits)
    const char* getStrMonth() const {
        sprintf(temp_buffer, "%02d", month);
        return temp_buffer;
    }
    
    // Get day as string (2 digits)
    const char* getStrDay() const {
        sprintf(temp_buffer, "%02d", day);
        return temp_buffer;
    }
    
    // Get hour24 as string (2 digits)
    const char* getStrHour24() const {
        sprintf(temp_buffer, "%02d", hour24);
        return temp_buffer;
    }
    
    // Get minute as string (2 digits)
    const char* getStrMinute() const {
        sprintf(temp_buffer, "%02d", minute);
        return temp_buffer;
    }
    
    // Get second as string (2 digits)
    const char* getStrSecond() const {
        sprintf(temp_buffer, "%02d", second);
        return temp_buffer;
    }
    
    // Get hour12 as string (2 digits)
    const char* getStrHour12() const {
        sprintf(temp_buffer, "%02d", hour12);
        return temp_buffer;
    }
    
    // Get AM/PM string
    const char* getAmpm() const {
		flag = 0;
        return (ampm == 0) ? "AM" : "PM";
    }
    
    // Get time string in 24-hour format (HH:MM:SS)
    const char* getTimeString() const {
		flag = 1;
        sprintf(temp_buffer, "%02d:%02d:%02d", hour24, minute, second);
        return temp_buffer;
    }
    
    // Get year-month-day format (YYYY-MM-DD)
    const char* getYearMonthDay() const {
		flag = 0;
        sprintf(temp_buffer, "%04d-%02d-%02d", year, month, day);
        return temp_buffer;
    }
    
    // Get 24-hour time format (HH:MM:SS)
    const char* getHour24_Minute_Second() const {
		flag = 1;
        sprintf(temp_buffer, "%02d:%02d:%02d", hour24, minute, second);
        return temp_buffer;
    }
    
    // Get 12-hour time format (HH:MM:SS)
    const char* getHour12_Minute_Second() const {
		flag = 1;
        sprintf(temp_buffer, "%02d:%02d:%02d", hour12, minute, second);
        return temp_buffer;
    }

    const char *getHour12_Minute_SecondAmpm() const {
      flag = 1;
      sprintf(temp_buffer, "%02d:%02d:%02d %s", hour12, minute, second,
              (ampm == 0) ? "AM" : "PM");
      return temp_buffer;
    }

    // Get 24-hour time format without seconds (HH:MM)
    const char* getHour24_Minute() const {
		flag = 1;
        sprintf(temp_buffer, "%02d:%02d", hour24, minute);
        return temp_buffer;
    }
    
    // Get 12-hour time format without seconds (HH:MM)
    const char* getHour12_Minute() const {
		flag = 1;
        sprintf(temp_buffer, "%02d:%02d", hour12, minute);
        return temp_buffer;
    }

    const char *getHour12_MinuteAmpm() const {
      flag = 0;
      sprintf(temp_buffer, "%02d:%02d %s", hour12, minute, 
              (ampm == 0) ? "AM" : "PM");
      return temp_buffer;
    }
};

class MatrixTimeData {
private:
    int16_t flag = 0;
public:
  int16_t previousCurrent(int16_t group, int16_t current);
  int16_t nextCurrent(int16_t group, int16_t current);
  const char* getStrStaff(TimeData& timeData,int16_t current);
  TimeData getTimeDataFromTimestamp(time_t now);

};

extern MatrixTimeData matrixTimeData;