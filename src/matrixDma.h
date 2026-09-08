#ifndef MATRIX_DMA_H
#define MATRIX_DMA_H

// Include the necessary library header for the MatrixPanel type definition
// 包含 MatrixPanel 类型定义所需的库头文件
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "common_define.h"
#include "matrixData.h"

extern uint16_t g_panelWidth;
extern uint16_t g_panelWidthChain;
extern uint16_t g_panelHeight;
extern uint16_t g_panelHeightChain;
extern uint8_t g_panelChain;
extern uint8_t g_panelType;

/**
 * @brief Physical geometry behind one panel type
 * 每种屏幕类型对应的物理参数
 */
struct PanelGeometry {
    uint16_t width;      // single panel width
    uint16_t height;     // single panel height
    uint8_t  chain;      // number of panels chained horizontally
    const char *name;    // label shown on the screen setting page
};

// Indexed by PANEL_TYPE_* from common_define.h
extern const PanelGeometry PANEL_GEOMETRIES[PANEL_TYPE_COUNT];


/**
 * @brief Matrix DMA Manager class for HUB75 LED panel control
 * 用于 HUB75 LED 面板控制的矩阵 DMA 管理类
 */
class MatrixDmaManager {
private:
    MatrixPanel_I2S_DMA* display_ptr;
    uint16_t panelWidth;
    uint16_t panelHeight;
    uint8_t panelChain;
    uint8_t panelType;

    // Panel type selected on the setting page but not applied yet.
    // Applying it rewrites NVS and reboots - see applyPendingPanelType().
    // 设置页上选中但尚未生效的屏幕类型
    uint8_t pendingPanelType;

public:
    /**
     * @brief Constructor - initializes with default values
     * 构造函数 - 使用默认值初始化
     */
    MatrixDmaManager();
    
    /**
     * @brief Destructor - cleans up display resources
     * 析构函数 - 清理显示资源
     */
    ~MatrixDmaManager();

    /**
     * @brief Initializes the HUB75 DMA display
     * 初始化 HUB75 DMA 显示
     * 
     * @return bool True if initialization successful, false otherwise
     * 如果初始化成功返回 true，否则返回 false
     */
    bool setupDMA();

    /**
     * @brief Gets the display pointer
     * 获取显示指针
     * 
     * @return MatrixPanel_I2S_DMA* Pointer to display object
     * 指向显示对象的指针
     */
    MatrixPanel_I2S_DMA* getDisplay();

    // Getter methods 获取方法
    uint16_t getPanelWidth() const;
    void setPanelWidth(uint16_t width);

    uint16_t getPanelHeight() const;
    void setPanelHeight(uint16_t height);

    uint8_t getPanelChain() const;
    void setPanelChain(uint8_t chain);

    uint8_t getPanelType() const;
    void setPanelType(uint8_t type);

    // ---- Screen setting page 屏幕设置页 ----

    /**
     * @brief Panel type currently selected on the setting page
     * 设置页上当前选中的屏幕类型
     */
    uint8_t getPendingPanelType() const;

    /**
     * @brief Cycle the selection forward / backward (does NOT take effect yet)
     * 循环切换选中项（此时还未生效）
     */
    void nextPendingPanelType();
    void prevPendingPanelType();

    /**
     * @brief True when the selection differs from the running panel type
     * 选中项与当前运行的屏幕类型不同时返回 true
     */
    bool isPendingPanelTypeDirty() const;

    /**
     * @brief Human readable label for a panel type, e.g. "128x64"
     * 屏幕类型的显示名，例如 "128x64"
     */
    static const char *getPanelTypeName(uint8_t type);

    /**
     * @brief Persist the pending panel type (type + width/height/chain) to NVS
     * 把选中的屏幕类型（含宽/高/链数）写入 NVS
     *
     * @return bool True when something was written and a reboot is required
     * 写入成功且需要重启时返回 true
     */
    bool applyPendingPanelType();
};

// Global instance declaration for easy access across the project
// 全局实例声明，方便在整个项目中使用
extern MatrixDmaManager matrixDmaManager;

#endif // MATRIX_DMA_H