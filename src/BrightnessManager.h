#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "data.h"
#include "common_define.h"

extern MatrixPanel_I2S_DMA *dma_display;
extern Data appData;  // Changed from extern Data *appData;

// Configuration parameters
// #define LIGHT_ADC 4                // Light sensor ADC pin (GPIO 1)
#define SAMPLING_INTERVAL 100       // Sampling interval (milliseconds)
#define CALCULATION_INTERVAL 1000   // Calculation interval (milliseconds)

/**
 * @brief 管理显示亮度的综合类，包括自动亮度检测和显示亮度设置
 * Comprehensive class that manages display brightness, including auto-brightness 
 * detection and display brightness settings
 */
class BrightnessManager {
public:
    /**
     * @brief BrightnessManager 的构造函数
     * Constructor for BrightnessManager
     */
    BrightnessManager();
    
    /**
     * @brief 初始化亮度管理器
     * Initialize the brightness manager
     */
    void init();
    
    /**
     * @brief 处理亮度管理的主函数，应在主循环中调用
     * Main function for brightness management, should be called in the main loop
     */
    void handle();
    
    /**
     * @brief 根据当前 AppData 设置更新显示亮度
     * Updates the display brightness based on current AppData settings
     */
    void updateDisplayBrightness();
    
    /**
     * @brief 获取最后应用的亮度值
     * Gets the last applied brightness value
     * 
     * @return uint8_t 最后应用的亮度
     *         Last applied brightness
     */
    uint8_t getLastAppliedBrightness() const;
    
    /**
     * @brief 强制更新亮度（忽略上次更新时间）
     * Force brightness update (ignoring last update time)
     */
    void forceUpdate();

private:
    uint8_t _lastBrightness;        // 最后应用的亮度 Last applied brightness
    
    // 自动亮度检测相关成员
    // Auto brightness detection related members
    int _brightSamplingTime;        // 亮度样本数量 Number of brightness samples
    int _brightSamplingValue;       // 累积样本值 Accumulated sample value
    int _currentBrightness;         // 当前亮度值 Current brightness value
    unsigned long _lastSampleTime;      // 上次采样时间 Last sample time
    unsigned long _lastCalculationTime; // 上次计算时间 Last calculation time
    
    /**
     * @brief 从累积样本计算亮度值
     * Calculate brightness value from accumulated samples
     */
    void calculateBrightnessValue();
    
    /**
     * @brief 处理自动亮度功能
     * Handle auto brightness functionality
     */
    void handleAutoBrightness();
};

extern BrightnessManager brightnessManager;

#endif // BRIGHTNESS_MANAGER_H