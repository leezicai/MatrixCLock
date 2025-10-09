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
};

// Global instance declaration for easy access across the project
// 全局实例声明，方便在整个项目中使用
extern MatrixDmaManager matrixDmaManager;

#endif // MATRIX_DMA_H