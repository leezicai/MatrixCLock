#include "timeUtils.h"
#include <time.h>

// Global instance definition
TimeUtils timeUtils;
// Static member initialization
const char* TimeUtils::monthFullNames[12] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const char* TimeUtils::monthShortNames[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char* TimeUtils::monthChineseNames[12] = {
    "一月", "二月", "三月", "四月", "五月", "六月",
    "七月", "八月", "九月", "十月", "十一月", "十二月"
};

const char* TimeUtils::dayFullNames[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

const char* TimeUtils::dayShortNames[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char* TimeUtils::dayChineseFullNames[7] = {
    "星期日", "星期一", "星期二", "星期三",
    "星期四", "星期五", "星期六"
};

const char* TimeUtils::dayChineseShortNames[7] = {
    "周日", "周一", "周二", "周三", "周四", "周五", "周六"
};

// Month methods
String TimeUtils::getMonthFullName(int tm_mon) {
    if (tm_mon >= 0 && tm_mon <= 11) {
        return String(monthFullNames[tm_mon]);
    }
    return String("-");
}

String TimeUtils::getMonthShortName(int tm_mon) {
    if (tm_mon >= 0 && tm_mon <= 11) {
        return String(monthShortNames[tm_mon]);
    }
    return String("-");
}

String TimeUtils::getMonthZhName(int tm_mon) {
    if (tm_mon >= 0 && tm_mon <= 11) {
        return String(monthChineseNames[tm_mon]);
    }
    return String("-");
}
String TimeUtils::getMonthZhNameSuffix() {
    return String("月");
}

// Day of week methods
String TimeUtils::getMDayFullName(int tm_wday) {
    if (tm_wday >= 0 && tm_wday <= 6) {
        return String(dayFullNames[tm_wday]);
    }
    return String("-");
}

String TimeUtils::getMDayShortName(int tm_wday) {
    if (tm_wday >= 0 && tm_wday <= 6) {
        return String(dayShortNames[tm_wday]);
    }
    return String("-");
}

String TimeUtils::getMDayZhFullName(int tm_wday) {
    if (tm_wday >= 0 && tm_wday <= 6) {
        return String(dayChineseFullNames[tm_wday]);
    }
    return String("-");
}
String TimeUtils::getMDayZhFullNamePrefix() {
    return String("星期");
}

String TimeUtils::getMDayZhShortName(int tm_wday) {
    if (tm_wday >= 0 && tm_wday <= 6) {
        return String(dayChineseShortNames[tm_wday]);
    }
    return String("-");
}
String TimeUtils::getMDayZhShortNamePrefix() {
    return String("周");
}

