#include "matrixDma.h"
#include <Arduino.h> // Required for Serial communication 用于串行通信

uint16_t g_panelWidth = 0;
uint16_t g_panelWidthChain = 0;
uint16_t g_panelHeight = 0;
uint16_t g_panelHeightChain = 0;
uint8_t g_panelChain = 0;

// Global instance definition
// 全局实例定义
MatrixDmaManager matrixDmaManager;

// Constructor implementation 构造函数实现
MatrixDmaManager::MatrixDmaManager() {
}

// Destructor implementation 析构函数实现
MatrixDmaManager::~MatrixDmaManager() {
    if (display_ptr) {
        delete display_ptr;
        display_ptr = nullptr;
    }
}

// Implementation of the DMA setup function
// DMA 设置函数的实现
bool MatrixDmaManager::setupDMA() {
    Serial.println("Setting up DMA display...");
    
    // Initialize member variables from matrixDataManager
    // 从 matrixDataManager 初始化成员变量
    display_ptr = nullptr;
    panelWidth = matrixDataManager.loadPanelWidth();
    panelHeight = matrixDataManager.loadPanelHeight();
    panelChain = matrixDataManager.loadPanelChain();

	g_panelWidth = panelWidth;
	g_panelWidthChain = panelWidth * panelChain;
    g_panelHeight = panelHeight;
    g_panelHeightChain = panelHeight * panelChain;
    g_panelChain = panelChain;
    
    // Configure HUB75 pins using definitions from common_define.h
    // 使用 common_define.h 中的定义配置 HUB75 引脚
    HUB75_I2S_CFG::i2s_pins pins;
    if(strcmp(PANEL_NAME, PANEL_NAME_TC7528) == 0){
        pins.r1 = R1_PIN; pins.g1 = B1_PIN; pins.b1 = G1_PIN;
        pins.r2 = R2_PIN; pins.g2 = B2_PIN; pins.b2 = G2_PIN;
    } else {
        pins.r1 = R1_PIN; pins.g1 = G1_PIN; pins.b1 = B1_PIN;
        pins.r2 = R2_PIN; pins.g2 = G2_PIN; pins.b2 = B2_PIN;
    }
    pins.a = A_PIN; pins.b = B_PIN; pins.c = C_PIN;
    pins.d = D_PIN; pins.e = E_PIN;
    pins.lat = LAT_PIN; pins.oe = OE_PIN; pins.clk = CLK_PIN;
    
    // Set up panel configuration using class member variables
    // 使用类成员变量设置面板配置
    HUB75_I2S_CFG mxconfig(
        panelWidth, panelHeight, panelChain, pins
    );
    mxconfig.min_refresh_rate = 30;
    
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
    display_ptr = new MatrixPanel_I2S_DMA(mxconfig);
    if (!display_ptr) {
        Serial.println("*** Failed to allocate memory for DMA display! ***");
        return false; // Allocation failed
    }
    
    // Initialize the display driver
    // 初始化显示驱动
    if (!display_ptr->begin()) {
        Serial.println("*** Panel Init Failed! ***");
        delete display_ptr;
        display_ptr = nullptr;
        return false; // Initialization failed
    }
    
    // Set the initial brightness
    // 设置初始亮度
    display_ptr->setBrightness8(DEFAULT_BRIGHTNESS);
    Serial.print("DMA display brightness set to: ");
    Serial.println(DEFAULT_BRIGHTNESS);
    
    Serial.print("Panel configured: ");
    Serial.print(panelWidth);
    Serial.print("x");
    Serial.print(panelHeight);
    Serial.print(" (chain: ");
    Serial.print(panelChain);
    Serial.println(")");
    
    Serial.println("DMA Panel Initialized Successfully.");
    
    return true;
}

// Get display pointer 获取显示指针
MatrixPanel_I2S_DMA* MatrixDmaManager::getDisplay() {
    return display_ptr;
}

// Getter implementations 获取方法实现
uint16_t MatrixDmaManager::getPanelWidth() const {
    return panelWidth;
}

uint16_t MatrixDmaManager::getPanelHeight() const {
    return panelHeight;
}

uint8_t MatrixDmaManager::getPanelChain() const {
    return panelChain;
}

// Setter implementations 设置方法实现
void MatrixDmaManager::setPanelWidth(uint16_t width) {
    panelWidth = width;
}

void MatrixDmaManager::setPanelHeight(uint16_t height) {
    panelHeight = height;
}

void MatrixDmaManager::setPanelChain(uint8_t chain) {
    panelChain = chain;
}