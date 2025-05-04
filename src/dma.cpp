// dma.cpp
#include "dma.h"
#include <Arduino.h> // Required for Serial communication 用于串行通信

// Implementation of the DMA setup function
// DMA 设置函数的实现
MatrixPanel_I2S_DMA* setupDMA(uint8_t brightness) {
    Serial.println("Setting up DMA display...");

    // Configure HUB75 pins using definitions from dma.h
    // 使用 dma.h 中的定义配置 HUB75 引脚
    HUB75_I2S_CFG::i2s_pins pins;
    pins.r1 = R1_PIN; pins.g1 = G1_PIN; pins.b1 = B1_PIN;
    pins.r2 = R2_PIN; pins.g2 = G2_PIN; pins.b2 = B2_PIN;
    pins.a = A_PIN; pins.b = B_PIN; pins.c = C_PIN;
    pins.d = D_PIN; pins.e = E_PIN;
    pins.lat = LAT_PIN; pins.oe = OE_PIN; pins.clk = CLK_PIN;

    // Set up panel configuration using definitions from dma.h
    // 使用 dma.h 中的定义设置面板配置
    HUB75_I2S_CFG mxconfig(
        PANEL_WIDTH, PANEL_HEIGHT, PANEL_CHAIN, pins
    );

    // Enable double buffering for smoother updates
    // 启用双缓冲以实现更平滑的更新
    mxconfig.double_buff = true;

    // Optional: Clock phase setting (can be toggled if display is unstable)
    // 可选：时钟相位设置（如果显示不稳定可以切换）
    mxconfig.clkphase = false;

    // Optional: Specify driver IC if not default (e.g., FM6126A)
    // 可选：如果不是默认驱动 IC，则指定驱动 IC（例如 FM6126A）
    // mxconfig.driver = HUB75_I2S_CFG::FM6126A;

    // Create the display object dynamically
    // 动态创建显示对象
    MatrixPanel_I2S_DMA* display_ptr = new MatrixPanel_I2S_DMA(mxconfig);
    if (!display_ptr) {
         Serial.println("*** Failed to allocate memory for DMA display! ***");
         return nullptr; // Allocation failed
    }


    // Initialize the display driver
    // 初始化显示驱动
    if (!display_ptr->begin()) {
        Serial.println("*** Panel Init Failed! ***");
        delete display_ptr; // Clean up allocated memory before returning null
        return nullptr;     // Initialization failed
    }

    // Set the initial brightness
    // 设置初始亮度
    display_ptr->setBrightness8(brightness);
    Serial.print("DMA display brightness set to: ");
    Serial.println(brightness);

    Serial.println("DMA Panel Initialized Successfully.");

    // Return the pointer to the initialized object
    // 返回指向已初始化对象的指针
    return display_ptr;
}