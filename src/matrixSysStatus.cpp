
#include "matrixSysStatus.h"
#include "brightnessManager.h"
MatrixStatusManager matrixStatusManager;

MatrixStatusManager::MatrixStatusManager(){}

const char* MatrixStatusManager::getSysStatus(int index){
  switch (index) {
  case 0:
    return brightnessManager.getAutoMode() ? matrixSettings.getCommonWord(WORD_ON) : matrixSettings.getCommonWord(WORD_OFF);
  case 1:
    itoa(brightnessManager.getManBrightness(), intBuffer, 10);
    return intBuffer;
  case 2:
    itoa(brightnessManager.getMinBrightness(), intBuffer, 10);
    return intBuffer;
  case 3:
    itoa(brightnessManager.getMaxBrightness(), intBuffer, 10);
    return intBuffer;
  case 4:
    return matrixSettings.getCommonWord(WORD_CURRENT_LANGUGE);
  case 5:
    {
      int timezone = rtc.getTimezone();
      // Format timezone with sign and 2 digits (e.g., "+08", "-12")
      sprintf(intBuffer, "%s:%+03d", matrixSettings.getCommonWord(WORD_TIMEZONE), timezone);
      return intBuffer;
    }
  case 6:
    // Temperature in Celsius (integer)
    sprintf(intBuffer, "%d°C", (int)sht30.getTemperature());
    return intBuffer;
  case 7:
    // Temperature in Celsius (1 decimal place)
    sprintf(intBuffer, "%.1f°C", sht30.getTemperature());
    return intBuffer;
  case 8:
    // Temperature in Fahrenheit (integer)
    sprintf(intBuffer, "%d°F", (int)sht30.getTemperatureF());
    return intBuffer;
  case 9:
    // Temperature in Fahrenheit (1 decimal place)
    sprintf(intBuffer, "%.1f°F", sht30.getTemperatureF());
    return intBuffer;
  case 10:
    // Humidity (integer)
    sprintf(intBuffer, "%d%%", (int)sht30.getHumidity());
    return intBuffer;
  case 11:
    // Humidity (1 decimal place)
    sprintf(intBuffer, "%.1f%%", sht30.getHumidity());
    return intBuffer;
  default:
    return "-";
  }
}


