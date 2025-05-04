// dma.h
#ifndef DMA_DISPLAY_SETUP_H
#define DMA_DISPLAY_SETUP_H

// Include the necessary library header for the MatrixPanel type definition
// 包含 MatrixPanel 类型定义所需的库头文件
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Define HUB75E pin connections to ESP32-S3
// 定义 HUB75E 引脚到 ESP32-S3 的连接
#define R1_PIN 8
#define G1_PIN 14
#define B1_PIN 17
#define R2_PIN 16
#define G2_PIN 13
#define B2_PIN 15
#define A_PIN 7
#define B_PIN 11
#define C_PIN 6
#define D_PIN 10
#define E_PIN 12
#define LAT_PIN 9
#define OE_PIN 4
#define CLK_PIN 5

// Define panel configuration
// 定义面板配置
#define PANEL_WIDTH 128
#define PANEL_HEIGHT 64
#define PANEL_CHAIN 1

/**
 * @brief Initializes the HUB75 DMA display.
 * Configures pins, panel size, creates the display object,
 * calls begin(), and sets initial brightness.
 * 初始化 HUB75 DMA 显示。
 * 配置引脚、面板尺寸、创建显示对象、调用 begin() 并设置初始亮度。
 *
 * @param brightness Initial brightness level (0-255). 初始亮度级别 (0-255)。
 * @return MatrixPanel_I2S_DMA* Pointer to the initialized display object,
 * or nullptr if initialization fails.
 * 指向已初始化显示对象的指针，如果初始化失败则为 nullptr。
 */
MatrixPanel_I2S_DMA* setupDMA(uint8_t brightness = 10); // Default brightness 10

#endif // DMA_DISPLAY_SETUP_H