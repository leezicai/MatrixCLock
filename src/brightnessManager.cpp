#include "brightnessManager.h"

BrightnessManager brightnessManager;

BrightnessManager::BrightnessManager()
    : _lastBrightness(10), _brightSamplingTime(0), _brightSamplingValue(0),
      _currentBrightness(0), _lastSampleTime(0), _lastCalculationTime(0) {}

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
    
    autoMode = matrixDataManager.getAutoMode();
    minBrightness = matrixDataManager.getMinBrightness();
    maxBrightness = matrixDataManager.getMaxBrightness();
    manBrightness = matrixDataManager.getManBrightness();

    setAutoMode(matrixDataManager.getAutoMode());
    
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
    if (dma_display) {
        updateDisplayBrightness();
    }
    
    // Serial.print("BrightnessManager initialized with brightness: ");
    // Serial.println(_currentBrightness);
}

void BrightnessManager::handle() {
    if (!dma_display) {
        return;
    }
    
    // Handle auto brightness measurement only in auto mode
    if (autoMode) {
        handleAutoBrightness();
    }
    
    // Always update display brightness regardless of mode
    // This ensures manual brightness changes take effect
    updateDisplayBrightness();
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
                               minBrightness + 1,
                               maxBrightness);
    } else {
        _currentBrightness = minBrightness;
    }
    
    // 打印调试信息
    // Print debug information
    // Serial.print("Sample count: ");
    // Serial.print(_brightSamplingTime);
    // Serial.print(", Average value: ");
    // Serial.print(val);
    // Serial.print(", Dynamic brightness: ");
    // Serial.println(_currentBrightness);
    
    // 重置计数器，为下一个计算周期做准备
    // Reset counters for next calculation cycle
    _brightSamplingTime = 0;
    _brightSamplingValue = 0;
}

void BrightnessManager::updateDisplayBrightness() {
    if (!dma_display) {
        Serial.println("Cannot update brightness: invalid display");
        return;
    }
    
    // 根据模式设置获取亮度值
    // Get brightness value based on mode setting
    uint8_t brightness = autoMode ? 
                         _currentBrightness : 
                         manBrightness;
    
    // 仅在亮度已更改时更新
    // Only update if brightness has changed
    if (brightness != _lastBrightness) {
        dma_display->setBrightness8(brightness);
        _lastBrightness = brightness;
        
        // Serial.println(brightness);
    }
}

uint8_t BrightnessManager::getLastAppliedBrightness() const {
    return _lastBrightness;
}

void BrightnessManager::forceUpdate() {
    if (dma_display) {
        // 强制获取当前亮度值并应用
        // Force get current brightness value and apply it
        uint8_t brightness = autoMode ? 
                             _currentBrightness : 
                             manBrightness;
        
        dma_display->setBrightness8(brightness);
        _lastBrightness = brightness;
        
        // Serial.print("Force updated DMA display brightness to: ");
        // Serial.println(brightness);
    }
}

bool BrightnessManager::getAutoMode() const {
    return autoMode;
}

void BrightnessManager::setAutoMode(bool mode) {
    autoMode = mode;
    matrixDataManager.setAutoMode(autoMode);
    forceUpdate();
}

// Minimum brightness
int BrightnessManager::getMinBrightness() const {
    return minBrightness;
}

void BrightnessManager::setMinBrightness(int brightness) {
    minBrightness = brightness;
}

// Minimum brightness increment/decrement
void BrightnessManager::incrementMinBrightness() {
    // minBrightness + 1 must be less than maxBrightness
    if (minBrightness + 1 < maxBrightness) {
        minBrightness++;
    } else {
        Serial.println("Cannot increase min brightness: would exceed or equal max brightness");
    }
    matrixDataManager.setMinBrightness(minBrightness);
}

void BrightnessManager::decrementMinBrightness() {
    // minBrightness - 1 must be >= 0
    if (minBrightness > 0) {
        minBrightness--;
    } else {
        Serial.println("Cannot decrease min brightness: already at minimum (0)");
    }
    matrixDataManager.setMinBrightness(minBrightness);
}



// Maximum brightness
int BrightnessManager::getMaxBrightness() const {
    return maxBrightness;
}

void BrightnessManager::setMaxBrightness(int brightness) {
    maxBrightness = brightness;
}

// Maximum brightness increment/decrement
void BrightnessManager::incrementMaxBrightness() {
    // maxBrightness + 1 must be <= 200
    if (maxBrightness < 200) {
        maxBrightness++;
    } else {
        Serial.println("Cannot increase max brightness: already at maximum (200)");
    }
    matrixDataManager.setMaxBrightness(maxBrightness);
}

void BrightnessManager::decrementMaxBrightness() {
    // maxBrightness - 1 must be >= minBrightness and >= 5
    int lowerLimit = max(minBrightness, 5);
    if (maxBrightness > lowerLimit) {
        maxBrightness--;
    } else {
        Serial.print("Cannot decrease max brightness: would be less than limit (");
        Serial.print(lowerLimit);
        Serial.println(")");
    }
    matrixDataManager.setMaxBrightness(maxBrightness);
}


// Manual brightness
int BrightnessManager::getManBrightness() const {
    return manBrightness;
}

void BrightnessManager::setManBrightness(int brightness) {
    manBrightness = brightness;
    if (!autoMode) {
      forceUpdate(); // Update display immediately in manual mode
    }
}
// Manual brightness increment/decrement
void BrightnessManager::incrementManBrightness() {
    // manBrightness + 1 must be <= 200
    if (manBrightness < 200) {
        manBrightness++;
    } else {
        Serial.println("Cannot increase manual brightness: already at maximum (200)");
    }
    matrixDataManager.setManBrightness(manBrightness);
}

void BrightnessManager::decrementManBrightness() {
    // manBrightness - 1 must be >= 2
    if (manBrightness > 2) {
        manBrightness--;
    } else {
        Serial.println("Cannot decrease manual brightness: already at minimum (1)");
    }
    matrixDataManager.setManBrightness(manBrightness);
}