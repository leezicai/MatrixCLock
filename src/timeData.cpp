#include "timeData.h"
#include <cstdio>

// Global instance
TimeUtils timeUtils;

int TimeUtils::previousCurrent(int current) {
    return (current - 1 + MAX_METHOD_NUM) % MAX_METHOD_NUM;
}

int TimeUtils::nextCurrent(int current) {
    return (current + 1) % MAX_METHOD_NUM;
}

const char* TimeUtils::getStrStaff(TimeData& timeData, int current) {
    switch(current) {
        case 0: return timeData.getYearMonthDay();
        case 1: return timeData.getHour24_Minute_Second();  
        case 2: return timeData.getHour12_Minute_Second();
        case 3: return timeData.getHour12_Minute_SecondAmpm();
        case 4: return timeData.getHour24_Minute();
        case 5: return timeData.getHour12_Minute();
        case 6: return timeData.getHour12_MinuteAmpm();
        case 7: return timeData.getTimeString();
        default: return timeData.getTimeString();
    }
}