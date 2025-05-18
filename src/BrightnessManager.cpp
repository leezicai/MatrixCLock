#include "BrightnessManager.h"

BrightnessManager::BrightnessManager(MatrixPanel_I2S_DMA* display, Data* appData)
    : _display(display), 
      _appData(appData), 
      _lastBrightness(10),
      _brightSamplingTime(0),
      _brightSamplingValue(0),
      _currentBrightness(0),
      _lastSampleTime(0),
      _lastCalculationTime(0) {
}

void BrightnessManager::init() {
    // 配置 ADC 为更高分辨率 (12 位)
    // Configure ADC for higher resolution (12-bit)
    analogReadResolution(12);
    
    // 初始化变量
    // Initialize variables
    _brightSamplingTime = 0;
    _brightSamplingValue = 0;
    _lastSampleTime = 0;
    _lastCalculationTime = 0;
    
    // 获取初始读数
    // Take initial readings
    for (int i = 0; i < 5; i++) {
        _brightSamplingValue += analogRead(LIGHT_ADC);
        _brightSamplingTime++;
        delay(10); // 读数之间的短暂延迟 Short delay between readings
    }
    
    // 计算初始亮度
    // Calculate initial brightness
    calculateBrightnessValue();
    
    // 初始化显示亮度
    // Initialize display brightness
    if (_display && _appData) {
        updateDisplayBrightness();
    }
    
    Serial.print("BrightnessManager initialized with brightness: ");
    Serial.println(_currentBrightness);
}

void BrightnessManager::handle() {
    // 处理自动亮度测量
    // Handle auto brightness measurement
    handleAutoBrightness();
    
    // 如果在自动模式下并且亮度已更改，则更新显示
    // If in auto mode and brightness has changed, update display
    if (_display && _appData && _appData->getAutoMode()) {
        updateDisplayBrightness();
    }
}

void BrightnessManager::handleAutoBrightness() {
    // 按固定间隔采集样本
    // Take samples at regular intervals
    if (millis() - _lastSampleTime >= SAMPLING_INTERVAL) {
        // 从光线传感器读取 ADC 值
        // Read ADC value from light sensor
        int sensorValue = analogRead(LIGHT_ADC);
        
        // 累加样本
        // Accumulate sample
        _brightSamplingValue += sensorValue;
        _brightSamplingTime++;
        
        // 更新上次采样时间
        // Update last sample time
        _lastSampleTime = millis();
    }
    
    // 每秒计算一次亮度
    // Calculate brightness once per second
    if (millis() - _lastCalculationTime >= CALCULATION_INTERVAL) {
        calculateBrightnessValue();
        _lastCalculationTime = millis();
    }
}

void BrightnessManager::calculateBrightnessValue() {
    if (!_appData) {
        return;
    }
    
    // 从样本计算平均值
    // Calculate average from samples
    int val = 0;
    if (_brightSamplingTime > 0) {
        val = _brightSamplingValue / _brightSamplingTime;
    }
    
    // 根据阈值映射亮度值
    // Map brightness value based on threshold
    if (val >= 700) {
        _currentBrightness = map(val, 700, 4095, 
                               _appData->getMinBrightness() + 1,
                               _appData->getMaxBrightness());
        _appData->setDynamicBrightness(_currentBrightness);
    } else {
        _currentBrightness = _appData->getMinBrightness();
        _appData->setDynamicBrightness(_currentBrightness);
    }
    
    // 打印调试信息
    // Print debug information
    Serial.print("Sample count: ");
    Serial.print(_brightSamplingTime);
    Serial.print(", Average value: ");
    Serial.print(val);
    Serial.print(", Dynamic brightness: ");
    Serial.println(_currentBrightness);
    
    // 重置计数器，为下一个计算周期做准备
    // Reset counters for next calculation cycle
    _brightSamplingTime = 0;
    _brightSamplingValue = 0;
}

void BrightnessManager::updateDisplayBrightness() {
    if (!_display || !_appData) {
        Serial.println("Cannot update brightness: invalid display or AppData");
        return;
    }
    
    // 根据模式设置获取亮度值
    // Get brightness value based on mode setting
    uint8_t brightness = _appData->getAutoMode() ? 
                         _appData->getDynamicBrightness() : 
                         _appData->getManualBrightness();
    
    // 仅在亮度已更改时更新
    // Only update if brightness has changed
    if (brightness != _lastBrightness) {
        _display->setBrightness8(brightness);
        _lastBrightness = brightness;
        
        Serial.print("Updated DMA display brightness to: ");
        Serial.println(brightness);
    }
}

void BrightnessManager::setAppData(Data* appData) {
    _appData = appData;
    if (_appData && _display) {
        updateDisplayBrightness();
    }
}

uint8_t BrightnessManager::getLastAppliedBrightness() const {
    return _lastBrightness;
}

void BrightnessManager::forceUpdate() {
    if (_display && _appData) {
        // 强制获取当前亮度值并应用
        // Force get current brightness value and apply it
        uint8_t brightness = _appData->getAutoMode() ? 
                             _appData->getDynamicBrightness() : 
                             _appData->getManualBrightness();
        
        _display->setBrightness8(brightness);
        _lastBrightness = brightness;
        
        Serial.print("Force updated DMA display brightness to: ");
        Serial.println(brightness);
    }
}