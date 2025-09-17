#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <Arduino.h>

class TimeUtils {
public:
    // Month methods (tm_mon: 0-11, January=0)
    String getMonthFullName(int tm_mon);
    String getMonthShortName(int tm_mon);
    String getMonthZhName(int tm_mon);
    String getMonthZhNameSuffix();
    
    // Day of week methods (tm_wday: 0-6, Sunday=0)
    String getMDayFullName(int tm_wday);
    String getMDayShortName(int tm_wday);
    String getMDayZhFullName(int tm_wday);
    String getMDayZhFullNamePrefix();
    String getMDayZhShortName(int tm_wday);
    String getMDayZhShortNamePrefix();
    

private:
    // Static arrays for month names
    static const char* monthFullNames[12];
    static const char* monthShortNames[12];
    static const char* monthChineseNames[12];
    
    // Static arrays for day names  
    static const char* dayFullNames[7];
    static const char* dayShortNames[7];
    static const char* dayChineseFullNames[7];
    static const char* dayChineseShortNames[7];
};

// Global instance declaration
extern TimeUtils timeUtils;

#endif // TIMEUTILS_H