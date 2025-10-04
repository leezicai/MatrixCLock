/**
 * EC11 Rotary Encoder Handler
 * 使用函数映射处理EC11旋转编码器的事件
 */

#ifndef EC11_HANDLER_H
#define EC11_HANDLER_H

#include "ec11.h" // 包含EC11头文件
#include "matrixColors.h"
#include "matrixCore.h"
#include "matrixFonts.h"
#include "ds3231.h"
#include "brightnessManager.h"
#include "matrixSetting.h"

#include "animation.h"

// 初始化编码器事件处理器
void initEncoderHandlers();

// 处理编码器事件 - 在主循环中调用
void handleEncoderEvents(EC11* encoder);

#endif // EC11_HANDLER_H