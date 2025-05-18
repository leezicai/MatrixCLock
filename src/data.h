/**
 * @file data.h
 * @brief 数据管理类，用于ESP32S3的数据存储和管理
 * 
 * 这个类提供了一个接口，用于管理设备配置数据并与非易失性存储(NVS)进行交互
 */

 #ifndef DATA_H
 #define DATA_H
 
 #include "NVSManager.h"
 #include <string>
 #include <Arduino.h>
 
 // 三维数组中存储的结构体定义
 struct ItemData {
     int value;
     bool enabled;
     // 可根据需要添加更多字段
 };
 
 /**
  * @class Data
  * @brief 用于管理设备配置和状态数据的类
  */
 class Data {
 public:
     /**
      * @brief Data类的构造函数
      */
     Data();
 
     /**
      * @brief Data类的析构函数
      */
     ~Data();
 
     /**
      * @brief 初始化数据管理类，从NVS读取所有存储的值
      * @return 如果初始化成功返回true，否则返回false
      */
     bool init();
 
     // SSID管理
     /**
      * @brief 获取当前的SSID
      * @return 当前的SSID字符串
      */
     const char* getSSID() const;
 
     /**
      * @brief 设置SSID并存储到NVS
      * @param ssid 要设置的SSID
      * @return 设置成功返回true，否则返回false
      */
     bool setSSID(const char* ssid);
 
     // 密码管理
     /**
      * @brief 获取当前的密码
      * @return 当前的密码字符串
      */
     const char* getPassword() const;
 
     /**
      * @brief 设置密码并存储到NVS
      * @param password 要设置的密码
      * @return 设置成功返回true，否则返回false
      */
     bool setPassword(const char* password);
 
     // 页面序号管理
     /**
      * @brief 获取当前一级页序号
      * @return 当前一级页序号
      */
     int getLevel1Page() const;
 
     /**
      * @brief 增加一级页序号
      * @param value 要增加的值
      * @return 增加后的一级页序号
      */
     int increaseLevel1Page(int value = 1);
 
     /**
      * @brief 减少一级页序号
      * @param value 要减少的值
      * @return 减少后的一级页序号
      */
     int decreaseLevel1Page(int value = 1);
 
     /**
      * @brief 获取当前二级页序号
      * @return 当前二级页序号
      */
     int getLevel2Page() const;
 
     /**
      * @brief 增加二级页序号
      * @param value 要增加的值
      * @return 增加后的二级页序号
      */
     int increaseLevel2Page(int value = 1);
 
     /**
      * @brief 减少二级页序号
      * @param value 要减少的值
      * @return 减少后的二级页序号
      */
     int decreaseLevel2Page(int value = 1);
 
     /**
      * @brief 获取当前三级页序号
      * @return 当前三级页序号
      */
     int getLevel3Page() const;
 
     /**
      * @brief 增加三级页序号
      * @param value 要增加的值
      * @return 增加后的三级页序号
      */
     int increaseLevel3Page(int value = 1);
 
     /**
      * @brief 减少三级页序号
      * @param value 要减少的值
      * @return 减少后的三级页序号
      */
     int decreaseLevel3Page(int value = 1);
 
     // 配网状态管理
     /**
      * @brief 获取当前配网状态
      * @return 如果已配网返回true，否则返回false
      */
     bool getWifiConfigured() const;
 
     /**
      * @brief 设置配网状态并存储到NVS
      * @param configured 要设置的配网状态
      * @return 设置成功返回true，否则返回false
      */
     bool setWifiConfigured(bool configured);
 
     // 时区管理
     /**
      * @brief 获取当前时区
      * @return 当前时区值
      */
     int getTimezone() const;
 
     /**
      * @brief 设置时区并存储到NVS
      * @param timezone 要设置的时区值
      * @return 设置成功返回true，否则返回false
      */
     bool setTimezone(int timezone);
 
     // 三维数组数据管理
     /**
      * @brief 获取三维数组中指定位置的数据
      * @param x 第一维索引
      * @param y 第二维索引
      * @param z 第三维索引
      * @return 指定位置的ItemData结构体
      */
     ItemData getItemData(int x, int y, int z) const;
 
     /**
      * @brief 设置三维数组中指定位置的数据并存储到NVS
      * @param x 第一维索引
      * @param y 第二维索引
      * @param z 第三维索引
      * @param data 要设置的ItemData结构体
      * @return 设置成功返回true，否则返回false
      */
     bool setItemData(int x, int y, int z, const ItemData& data);
 
     /**
      * @brief 保存所有三维数组数据到NVS
      * @return 保存成功返回true，否则返回false
      */
     bool saveArrayData();
     
     // 亮度模式管理
     /**
      * @brief 获取当前亮度模式
      * @return 如果是自动模式返回true，否则返回false
      */
     bool getAutoMode() const;
     
     /**
      * @brief 设置亮度模式并存储到NVS
      * @param autoMode 要设置的亮度模式，true为自动，false为手动
      * @return 设置成功返回true，否则返回false
      */
     bool setAutoMode(bool autoMode);
     
     // 最小亮度管理
     /**
      * @brief 获取最小亮度值
      * @return 当前最小亮度值
      */
     int getMinBrightness() const;
     
     /**
      * @brief 增加最小亮度值并存储到NVS
      * @param value 要增加的值，可为正或负
      * @return 增加后的最小亮度值
      */
     int increaseMinBrightness(int value);
     
     // 最大亮度管理
     /**
      * @brief 获取最大亮度值
      * @return 当前最大亮度值
      */
     int getMaxBrightness() const;
     
     /**
      * @brief 增加最大亮度值并存储到NVS
      * @param value 要增加的值，可为正或负
      * @return 增加后的最大亮度值
      */
     int increaseMaxBrightness(int value);
     
     // 手动亮度管理
     /**
      * @brief 获取手动亮度值
      * @return 当前手动亮度值
      */
     int getManualBrightness() const;
     
     /**
      * @brief 增加手动亮度值并存储到NVS
      * @param value 要增加的值，可为正或负
      * @return 增加后的手动亮度值
      */
     int increaseManualBrightness(int value);
     
     // 动态亮度管理（不存储在NVS）
     /**
      * @brief 获取动态亮度值
      * @return 当前动态亮度值
      */
     int getDynamicBrightness() const;
     
     /**
      * @brief 设置动态亮度值（不存储到NVS）
      * @param value 要设置的动态亮度值
      */
     void setDynamicBrightness(int value);
 
 private:
     NVSManager nvs;                     // NVS管理器实例
     
     // 基本配置
     std::string mSSID;                  // WiFi SSID
     std::string mPassword;              // WiFi 密码
     int mLevel1Page;                    // 当前一级页序号
     int mLevel2Page;                    // 当前二级页序号
     int mLevel3Page;                    // 当前三级页序号 (不存NVS)
     bool mWifiConfigured;               // 配网状态
     int mTimezone;                      // 时区信息
     
     // 亮度配置
     bool mAutoMode;                     // 亮度模式：true为自动，false为手动
     int mMinBrightness;                 // 最小亮度值
     int mMaxBrightness;                 // 最大亮度值
     int mManualBrightness;              // 手动亮度值
     int mDynamicBrightness;             // 动态亮度值（不存NVS）
     
     // 三维数组配置
     static const int MAX_X = 5;         // 第一维大小
     static const int MAX_Y = 5;         // 第二维大小
     static const int MAX_Z = 5;         // 第三维大小
     ItemData mArray[MAX_X][MAX_Y][MAX_Z]; // 三维数组
 
     /**
      * @brief 从NVS加载三维数组数据
      * @return 加载成功返回true，否则返回false
      */
     bool loadArrayData();
 
     /**
      * @brief 将ItemData结构体转换为可存储的字节数组
      * @param data 要转换的ItemData结构体
      * @param buffer 输出缓冲区
      * @return 转换后的字节数
      */
     size_t itemDataToBytes(const ItemData& data, uint8_t* buffer) const;
 
     /**
      * @brief 从字节数组解析ItemData结构体
      * @param buffer 包含数据的字节数组
      * @param data 输出的ItemData结构体
      * @return 成功解析返回true，否则返回false
      */
     bool bytesToItemData(const uint8_t* buffer, ItemData& data) const;
 };
 
 // 外部声明，使其他文件可以访问此实例
 extern Data AppData;
 
 #endif // DATA_H