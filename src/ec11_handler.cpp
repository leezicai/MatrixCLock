/**
 * EC11 Rotary Encoder Handler
 * 实现函数映射处理EC11旋转编码器的事件
 */

#include "ec11_handler.h"
#include "page.h"
#include <Arduino.h>

// 定义EC11编码器事件处理函数 顺时针旋转
void handleClockwiseRotation(EC11 *encoder) {
  MatrixCore matrixCore = matrixCoreManager.getCurrentMatrixCore();
  Serial.println(matrixCore.colorIndex1);
  matrixCore.colorIndex1 = matrixColorManager.nextIndex(matrixCore.colorIndex1);
  matrixCoreManager.modifyCurrentElement(matrixCore);

  matrixCoreManager.setLineFlagTime(0);

    max98357Manager.stop();
}

// 逆时针旋转
void handleCounterClockwiseRotation(EC11 *encoder) {
  MatrixCore matrixCore = matrixCoreManager.getCurrentMatrixCore();
  matrixCore.colorIndex1 =
      matrixColorManager.previousIndex(matrixCore.colorIndex1);
  matrixCoreManager.modifyCurrentElement(matrixCore);

  matrixCoreManager.setLineFlagTime(0);

  max98357Manager.stop();
}

void handleButtonPress(EC11 *encoder) { Serial.println("按钮被按下"); }

void handleButtonClick(EC11 *encoder) {
  matrixCoreManager.setLineFlagTime(0);
  matrixCoreManager.nextElementGroup();
  Serial.println("按钮被点击（短按）");
}

void handleButtonDoubleClick(EC11 *encoder) {
  matrixCoreManager.setLineFlagTime(0);
  matrixCoreManager.prevElementGroup();
  Serial.println("按钮被双击");
}

void handleButtonLongPress(EC11 *encoder) { 
  matrixCoreManager.printAllSecondaryPages();
  Serial.println("按钮被长按"); 
}

// 按下 顺时针旋转
void handleButtonPressedRotateCW(EC11 *encoder) {
  


    MatrixCore matrixCore;
  switch (matrixCoreManager.getCurrentPageIndex()) {
  case 0:
  case 1:
    matrixCore = matrixCoreManager.getCurrentMatrixCore();
    matrixCore.fontIndex = matrixFontManager.switchToNextFontIndex(
        matrixCore.fontGroupIndex, matrixCore.fontIndex);
    matrixCoreManager.modifyCurrentElement(matrixCore);
    break;
  case 2:
    switch (matrixCoreManager.getCurrentSecondaryIndex()) {
    case 0:
      switch (matrixCoreManager.getCurrentElementGroupIndex()) {
      case 0:
        break;
      case 1:
        rtc.addYear();
        break;
      case 2:
        rtc.addMonth();
        break;
      case 3:
        rtc.addDay();
        break;
      case 4:
        rtc.addHour();
        break;
      case 5:
        rtc.addMinute();
        break;
      case 6:
        rtc.addSecond();
        break;
      default:
        break;
      }
      break;
    case 1:
      switch (matrixCoreManager.getCurrentElementGroupIndex()) {
      case 0:
      case 1:
        break;
      case 2:
        brightnessManager.setAutoMode(!brightnessManager.getAutoMode());
        if (brightnessManager.getAutoMode()) {
          matrixCoreManager.swapSecondaryPage(
              2, 1, matrixCoreManager.getSecondaryPage2_1());
        } else {
          matrixCoreManager.swapSecondaryPage(
              2, 1, matrixCoreManager.getSecondaryPage2_1_());
        }
        break;
      case 3:
        if (brightnessManager.getAutoMode()) {
          brightnessManager.incrementMinBrightness();
        } else {
          brightnessManager.incrementManBrightness();
        }
        break;
      case 4:
        if (brightnessManager.getAutoMode()) {
          brightnessManager.incrementMaxBrightness();
        }
        break;
      default:
        break;
      }
      break;
    case 2:
      switch (matrixCoreManager.getCurrentElementGroupIndex()) {
      case 0:
        break;
      case 1:
        matrixSettings.nextLanguage();
        break;
      default:
        break;
      }
      break;
    case 3: {
      int i = matrixCoreManager.getCurrentElementGroupIndex();
      switch (i) {
      case 0:
        alarmManager.nextAlarm();
        break;
      case 1:
        alarmManager.setEnabled(!alarmManager.isEnabled());
        break;
      case 2:
        max98357Manager.addVolume();
        break;
      case 3:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(1),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(1)));
        break;
      case 4:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(2),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(2)));
        break;
      case 5:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(3),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(3)));
        break;
      case 6:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(4),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(4)));
        break;
      case 7:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(5),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(5)));
        break;
      case 8:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(6),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(6)));
        break;
      case 9:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(0),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(0)));
        break;
      case 10:
        alarmManager.incrementHour();
        break;
      case 11:
        break;
      case 12:
        alarmManager.incrementMinute();
        break;
      default:
        break;
      }
      break;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
  matrixCoreManager.setLineFlagTime(0);



  // int32_t pos = encoder->getPosition();
  // Serial.print("按住按钮顺时针旋转，当前位置: ");
  // Serial.println(pos);
}

// 按下 逆时针旋转
void handleButtonPressedRotateCCW(EC11 *encoder) {




  MatrixCore matrixCore;

  switch (matrixCoreManager.getCurrentPageIndex()) {
  case 0:
  case 1:
    matrixCore = matrixCoreManager.getCurrentMatrixCore();
    matrixCore.fontIndex = matrixFontManager.switchToPreFontIndex(
        matrixCore.fontGroupIndex, matrixCore.fontIndex);
    matrixCoreManager.modifyCurrentElement(matrixCore);
    break;
  case 2:
    switch (matrixCoreManager.getCurrentSecondaryIndex()) {
    case 0:
      switch (matrixCoreManager.getCurrentElementGroupIndex()) {
      case 0:
        break;
      case 1:
        rtc.subtractYear();
        break;
      case 2:
        rtc.subtractMonth();
        break;
      case 3:
        rtc.subtractDay();
        break;
      case 4:
        rtc.subtractHour();
        break;
      case 5:
        rtc.subtractMinute();
        break;
      case 6:
        rtc.subtractSecond();
        break;
      default:
        break;
      }
      break;
    case 1:
      switch (matrixCoreManager.getCurrentElementGroupIndex()) {
      case 0:
      case 1:
        break;
      case 2:
        brightnessManager.setAutoMode(!brightnessManager.getAutoMode());
        if (brightnessManager.getAutoMode()) {
          matrixCoreManager.swapSecondaryPage(
              2, 1, matrixCoreManager.getSecondaryPage2_1());
        } else {
          matrixCoreManager.swapSecondaryPage(
              2, 1, matrixCoreManager.getSecondaryPage2_1_());
        }
        break;
      case 3:
        if (brightnessManager.getAutoMode()) {
          brightnessManager.decrementMinBrightness();
        } else {
          brightnessManager.decrementManBrightness();
        }
        break;
      case 4:
        if (brightnessManager.getAutoMode()) {
          brightnessManager.decrementMaxBrightness();
        }
        break;
      default:
        break;
      }
      break;
    case 2:
      switch (matrixCoreManager.getCurrentElementGroupIndex()) {
      case 0:
        break;
      case 1:
        matrixSettings.previousLanguage();
        break;
      default:
        break;
      }
      break;
    case 3: {
      int i = matrixCoreManager.getCurrentElementGroupIndex();
      switch (i) {
      case 0:
        alarmManager.prevAlarm();
        break;
      case 1:
        alarmManager.setEnabled(!alarmManager.isEnabled());
        break;
      case 2:
        max98357Manager.subVolume();
        break;
      case 3:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(1),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(1)));
        break;
      case 4:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(2),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(2)));
        break;
      case 5:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(3),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(3)));
        break;
      case 6:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(4),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(4)));
        break;
      case 7:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(5),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(5)));
        break;
      case 8:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(6),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(6)));
        break;
      case 9:
        alarmManager.setDayEnabled(
            static_cast<DayOfWeek>(0),
            !alarmManager.getDayEnabled(static_cast<DayOfWeek>(0)));
        break;
      case 10:
        alarmManager.decrementHour();
        break;
      case 11:
        break;
      case 12:
        alarmManager.decrementMinute();
        break;
      default:
        break;
      }
      break;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
  matrixCoreManager.setLineFlagTime(0);

  // int32_t pos = encoder->getPosition();
  // Serial.print("按住按钮并逆时针旋转，当前位置: ");
  // Serial.println(pos);
}

// 使用函数指针定义事件类型及其处理函数
typedef bool (EC11::*EventCheckMethod)(); // 定义事件检测方法指针类型
typedef void (*EventHandlerFunc)(EC11 *); // 定义事件处理函数指针类型

// 创建事件处理器结构体
struct EncoderEventHandler {
  EventCheckMethod checkEvent;  // 检查事件的方法指针
  EventHandlerFunc handleEvent; // 处理事件的函数指针
};

// 声明全局事件处理映射表
EncoderEventHandler encoderEventHandlers[] = {
    {&EC11::rotatedClockwise, handleClockwiseRotation},
    {&EC11::rotatedCounterClockwise, handleCounterClockwiseRotation},
    {&EC11::buttonPressed, handleButtonPress},
    {&EC11::buttonClicked, handleButtonClick},
    {&EC11::buttonDoubleClicked, handleButtonDoubleClick},
    {&EC11::buttonLongPressed, handleButtonLongPress},
    {&EC11::buttonPressedAndRotatedClockwise, handleButtonPressedRotateCW},
    {&EC11::buttonPressedAndRotatedCounterClockwise,
     handleButtonPressedRotateCCW}};

// 计算事件处理器数组的大小
const size_t EVENT_HANDLERS_COUNT =
    sizeof(encoderEventHandlers) / sizeof(encoderEventHandlers[0]);

// 初始化函数 - 如果需要额外的初始化，可以在这里添加
void initEncoderHandlers() {
  // 目前不需要额外初始化，但保留此函数以便将来扩展
  // 例如可以在这里注册自定义事件处理器
}

// 主处理函数 - 在主循环中调用
void handleEncoderEvents(EC11 *encoder) {
  // 必须先调用update以检测事件
  encoder->update();

  // 使用函数映射检查和处理所有编码器事件
  for (size_t i = 0; i < EVENT_HANDLERS_COUNT; i++) {
    // 检查是否发生了事件
    if ((encoder->*(encoderEventHandlers[i].checkEvent))()) {
      // 调用对应的处理函数
      encoderEventHandlers[i].handleEvent(encoder);
    }
  }
}