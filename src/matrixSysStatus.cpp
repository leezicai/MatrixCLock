
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
  default:
    return "-";
  }
}


