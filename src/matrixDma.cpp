#include "matrixDma.h"
#include <Arduino.h> // Required for Serial communication 用于串行通信

uint16_t g_panelWidth = 0;
uint16_t g_panelWidthChain = 0;
uint16_t g_panelHeight = 0;
uint16_t g_panelHeightChain = 0;
uint8_t g_panelChain = 0;
uint8_t g_panelType = 0;

// Geometry table - the panel type is the single source of truth.
// Chaining extends the canvas horizontally only, the height never changes.
// 屏幕类型 -> 物理参数。链式拼接只加宽，不加高。
const PanelGeometry PANEL_GEOMETRIES[PANEL_TYPE_COUNT] = {
    {128, 64, 1, "128x64"},   // PANEL_TYPE_128X64_X1
    { 64, 64, 2, "64x64x2"},  // PANEL_TYPE_64X64_X2
    { 64, 64, 1, "64x64"},    // PANEL_TYPE_64X64_X1
};

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

    // The panel type drives everything else. Width / height / chain are still
    // kept in NVS for compatibility, but they are re-derived from the type on
    // every boot so a half written config can never produce a broken geometry.
    // 以 panelType 为唯一真相源，每次开机由它反推宽/高/链数
    panelType = (uint8_t)matrixDataManager.loadPanelType();
    if (panelType >= PANEL_TYPE_COUNT) {
        panelType = PANEL_TYPE_128X64_X1;
    }
    pendingPanelType = panelType;

    const PanelGeometry &geometry = PANEL_GEOMETRIES[panelType];
    panelWidth = geometry.width;
    panelHeight = geometry.height;
    panelChain = geometry.chain;

    // Keep the stored values in sync (only writes when they actually drifted)
    // 保持 NVS 中的冗余字段同步，只在不一致时才写
    if (matrixDataManager.loadPanelWidth() != (int16_t)panelWidth ||
        matrixDataManager.loadPanelHeight() != (int16_t)panelHeight ||
        matrixDataManager.loadPanelChain() != (int16_t)panelChain) {
        matrixDataManager.savePanelWidth(panelWidth);
        matrixDataManager.savePanelHeight(panelHeight);
        matrixDataManager.savePanelChain(panelChain);
        matrixNvsManager.commit();
    }

	g_panelWidth = panelWidth;
	g_panelWidthChain = panelWidth * panelChain;
    g_panelHeight = panelHeight;
    // Chaining is horizontal, so the canvas height equals a single panel height.
    // 链式拼接是横向的，画布高度就是单块屏的高度
    g_panelHeightChain = panelHeight;
    g_panelChain = panelChain;
    g_panelType = panelType;

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
void MatrixDmaManager::setPanelWidth(uint16_t width) {
    panelWidth = width;
}

uint16_t MatrixDmaManager::getPanelHeight() const {
    return panelHeight;
}
void MatrixDmaManager::setPanelHeight(uint16_t height) {
    panelHeight = height;
}

uint8_t MatrixDmaManager::getPanelChain() const {
    return panelChain;
}
void MatrixDmaManager::setPanelChain(uint8_t chain) {
    panelChain = chain;
}

uint8_t MatrixDmaManager::getPanelType() const {
    return panelType;
}
void MatrixDmaManager::setPanelType(uint8_t type) {
    panelType = type;
}

// ---- Screen setting page 屏幕设置页 ----

uint8_t MatrixDmaManager::getPendingPanelType() const {
    return pendingPanelType;
}

void MatrixDmaManager::nextPendingPanelType() {
    pendingPanelType = (pendingPanelType + 1) % PANEL_TYPE_COUNT;
}

void MatrixDmaManager::prevPendingPanelType() {
    pendingPanelType = (pendingPanelType + PANEL_TYPE_COUNT - 1) % PANEL_TYPE_COUNT;
}

bool MatrixDmaManager::isPendingPanelTypeDirty() const {
    return pendingPanelType != panelType;
}

const char *MatrixDmaManager::getPanelTypeName(uint8_t type) {
    if (type >= PANEL_TYPE_COUNT) {
        return "-";
    }
    return PANEL_GEOMETRIES[type].name;
}

bool MatrixDmaManager::applyPendingPanelType() {
    if (pendingPanelType >= PANEL_TYPE_COUNT || pendingPanelType == panelType) {
        return false; // Nothing to do, no reboot needed
    }

    const PanelGeometry &geometry = PANEL_GEOMETRIES[pendingPanelType];

    // Page layouts are stored per panel type (see matrixNvs.cpp), so switching
    // the type simply makes a different key set active - nothing to migrate.
    // 页面数据按屏幕类型分 key 存储，切换类型等于换一套 key，无需迁移
    matrixDataManager.savePanelType(pendingPanelType);
    matrixDataManager.savePanelWidth(geometry.width);
    matrixDataManager.savePanelHeight(geometry.height);
    matrixDataManager.savePanelChain(geometry.chain);
    matrixNvsManager.commit();

    Serial.print("Panel type switched to: ");
    Serial.println(geometry.name);

    return true; // Caller must reboot for the new geometry to take effect
}