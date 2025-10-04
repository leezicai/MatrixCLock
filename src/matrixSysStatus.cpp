
#include "matrixSysStatus.h"
#include "brightnessManager.h"
MatrixStatusManager matrixStatusManager;

MatrixStatusManager::MatrixStatusManager(){}

const char* MatrixStatusManager::getSysStatus(int index){
  switch (index) {
  case 0:
    return brightnessManager.getAutoMode() ? matrixSettings.getCommonWord(WORD_BRIGHTNESS_ON) : matrixSettings.getCommonWord(WORD_BRIGHTNESS_OFF);
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
  default:
    return "-";
  }
}


