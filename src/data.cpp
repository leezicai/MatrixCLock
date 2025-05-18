/**
 * @file data.cpp
 * @brief Data类的实现文件
 */

 #include "data.h"

 // 创建全局实例，使其可以从其他文件访问
 Data AppData = Data();
 
 // NVS键名定义
 #define NVS_KEY_SSID "ssid"
 #define NVS_KEY_PASSWORD "pwd"
 #define NVS_KEY_LEVEL1_PAGE "lvl1page"
 #define NVS_KEY_LEVEL2_PAGE "lvl2page"
 #define NVS_KEY_WIFI_CONFIGURED "wificfg"
 #define NVS_KEY_TIMEZONE "timezone"
 #define NVS_KEY_ARRAY_PREFIX "arr"
 #define NVS_KEY_AUTO_MODE "automode"
 #define NVS_KEY_MIN_BRIGHTNESS "minbright"
 #define NVS_KEY_MAX_BRIGHTNESS "maxbright"
 #define NVS_KEY_MANUAL_BRIGHTNESS "manbright"
 
 Data::Data() 
     : nvs("appdata"),              // 使用"appdata"作为NVS命名空间
       mSSID(""),                   // 初始SSID为空
       mPassword(""),               // 初始密码为空
       mLevel1Page(1),              // 默认一级页序号为1
       mLevel2Page(0),              // 默认二级页序号为0
       mLevel3Page(0),              // 默认三级页序号为0(不存NVS)
       mWifiConfigured(false),      // 默认未配网
       mTimezone(0),                // 默认时区为0
       mAutoMode(false),            // 默认手动模式
       mMinBrightness(5),           // 默认最小亮度为5
       mMaxBrightness(200),         // 默认最大亮度为200
       mManualBrightness(10),      // 默认手动亮度为50
       mDynamicBrightness(50)      // 默认动态亮度为50
 {
     // 初始化三维数组
     for (int x = 0; x < MAX_X; x++) {
         for (int y = 0; y < MAX_Y; y++) {
             for (int z = 0; z < MAX_Z; z++) {
                 mArray[x][y][z] = {0, false}; // 默认值
             }
         }
     }
 }
 
 Data::~Data() {
     // 析构函数中确保数据已保存
     nvs.commit();
 }
 
 bool Data::init() {
     // 初始化NVS
     esp_err_t err = nvs.init();
     if (err != ESP_OK) {
         Serial.printf("NVS初始化失败: %s\n", nvs.errorToString(err));
         return false;
     }
     
     // 从NVS读取SSID
     bool exists = false;
     mSSID = nvs.readString(NVS_KEY_SSID, "", &exists);
     Serial.printf(mSSID.c_str());
     
     // 从NVS读取密码
     mPassword = nvs.readString(NVS_KEY_PASSWORD, "", &exists);
     Serial.printf(mSSID.c_str());

     // 从NVS读取一级页序号，默认为1
     mLevel1Page = nvs.readInt(NVS_KEY_LEVEL1_PAGE, 1, &exists);
     
     // 从NVS读取二级页序号，默认为0
     mLevel2Page = nvs.readInt(NVS_KEY_LEVEL2_PAGE, 0, &exists);
     
     // 从NVS读取配网状态，默认为false
     mWifiConfigured = nvs.readBool(NVS_KEY_WIFI_CONFIGURED, false, &exists);
     Serial.printf("Wifi configured: %s\n", mWifiConfigured ? "true" : "false");

     // 从NVS读取时区，默认为0
     mTimezone = nvs.readInt(NVS_KEY_TIMEZONE, 8, &exists);
     
     // 从NVS读取亮度自动模式，默认为false
     mAutoMode = nvs.readBool(NVS_KEY_AUTO_MODE, false, &exists);
     
     // 从NVS读取最小亮度，默认为5
     mMinBrightness = nvs.readInt(NVS_KEY_MIN_BRIGHTNESS, 5, &exists);
     
     // 从NVS读取最大亮度，默认为200
     mMaxBrightness = nvs.readInt(NVS_KEY_MAX_BRIGHTNESS, 200, &exists);
     
     // 从NVS读取手动亮度，默认为100
     mManualBrightness = nvs.readInt(NVS_KEY_MANUAL_BRIGHTNESS, 50, &exists);
     
     // 动态亮度不需要从NVS中读取，使用构造函数中的默认值
     
     // 加载三维数组数据
     loadArrayData();
     
     return true;
 }
 
 // SSID管理
 const char* Data::getSSID() const {
     return mSSID.c_str();
 }
 
 bool Data::setSSID(const char* ssid) {
     mSSID = ssid;
     esp_err_t err = nvs.writeString(NVS_KEY_SSID, mSSID.c_str());
     return (err == ESP_OK);
 }
 
 // 密码管理
 const char* Data::getPassword() const {
     return mPassword.c_str();
 }
 
 bool Data::setPassword(const char* password) {
     mPassword = password;
     esp_err_t err = nvs.writeString(NVS_KEY_PASSWORD, mPassword.c_str());
     return (err == ESP_OK);
 }
 
 // 一级页序号管理
 int Data::getLevel1Page() const {
     return mLevel1Page;
 }
 
 int Data::increaseLevel1Page(int value) {
     mLevel1Page += value;
     esp_err_t err = nvs.writeInt(NVS_KEY_LEVEL1_PAGE, mLevel1Page);
     return mLevel1Page;
 }
 
 int Data::decreaseLevel1Page(int value) {
     mLevel1Page -= value;
     if (mLevel1Page < 0) mLevel1Page = 0; // 防止小于0
     esp_err_t err = nvs.writeInt(NVS_KEY_LEVEL1_PAGE, mLevel1Page);
     return mLevel1Page;
 }
 
 // 二级页序号管理
 int Data::getLevel2Page() const {
     return mLevel2Page;
 }
 
 int Data::increaseLevel2Page(int value) {
     mLevel2Page += value;
     esp_err_t err = nvs.writeInt(NVS_KEY_LEVEL2_PAGE, mLevel2Page);
     return mLevel2Page;
 }
 
 int Data::decreaseLevel2Page(int value) {
     mLevel2Page -= value;
     if (mLevel2Page < 0) mLevel2Page = 0; // 防止小于0
     esp_err_t err = nvs.writeInt(NVS_KEY_LEVEL2_PAGE, mLevel2Page);
     return mLevel2Page;
 }
 
 // 三级页序号管理（不存NVS）
 int Data::getLevel3Page() const {
     return mLevel3Page;
 }
 
 int Data::increaseLevel3Page(int value) {
     mLevel3Page += value;
     return mLevel3Page;
 }
 
 int Data::decreaseLevel3Page(int value) {
     mLevel3Page -= value;
     if (mLevel3Page < 0) mLevel3Page = 0; // 防止小于0
     return mLevel3Page;
 }
 
 // 配网状态管理
 bool Data::getWifiConfigured() const {
     return mWifiConfigured;
 }
 
 bool Data::setWifiConfigured(bool configured) {
     mWifiConfigured = configured;
     esp_err_t err = nvs.writeBool(NVS_KEY_WIFI_CONFIGURED, mWifiConfigured);
     return (err == ESP_OK);
 }
 
 // 时区管理
 int Data::getTimezone() const {
     return mTimezone;
 }
 
 bool Data::setTimezone(int timezone) {
     mTimezone = timezone;
     esp_err_t err = nvs.writeInt(NVS_KEY_TIMEZONE, mTimezone);
     return (err == ESP_OK);
 }
 
 // 亮度自动模式管理
 bool Data::getAutoMode() const {
     return mAutoMode;
 }
 
 bool Data::setAutoMode(bool autoMode) {
     mAutoMode = autoMode;
     esp_err_t err = nvs.writeBool(NVS_KEY_AUTO_MODE, mAutoMode);
     return (err == ESP_OK);
 }
 
 // 最小亮度管理
 int Data::getMinBrightness() const {
     return mMinBrightness;
 }

 int Data::increaseMinBrightness(int value) {
   mMinBrightness += value;

   if (mMinBrightness == mMaxBrightness) {
       mMinBrightness = mMaxBrightness - 1;
   }
   // 限制最小亮度不小于0
   if (mMinBrightness < 0) {
       mMinBrightness = 0;
   }

   esp_err_t err = nvs.writeInt(NVS_KEY_MIN_BRIGHTNESS, mMinBrightness);
   return mMinBrightness;
 }

 // 最大亮度管理
 int Data::getMaxBrightness() const {
     return mMaxBrightness;
 }
 
 int Data::increaseMaxBrightness(int value) {
     mMaxBrightness += value;
     
     // 限制最大亮度在10-200之间
     if (mMaxBrightness == mMinBrightness) {
         mMaxBrightness = mMinBrightness + 1;
     }
     if (mMaxBrightness < 10) {
         mMaxBrightness = 10;
     } else if (mMaxBrightness > 200) {
         mMaxBrightness = 200;
     }

     esp_err_t err = nvs.writeInt(NVS_KEY_MAX_BRIGHTNESS, mMaxBrightness);
     return mMaxBrightness;
 }
 
 // 手动亮度管理
 int Data::getManualBrightness() const {
     return mManualBrightness;
 }
 
 int Data::increaseManualBrightness(int value) {
     mManualBrightness += value;
     
     // 限制手动亮度在10-200之间
     if (mManualBrightness < 10) {
         mManualBrightness = 10;
     } else if (mManualBrightness > 200) {
         mManualBrightness = 200;
     }
     
     esp_err_t err = nvs.writeInt(NVS_KEY_MANUAL_BRIGHTNESS, mManualBrightness);
     return mManualBrightness;
 }
 
 // 动态亮度管理（不存NVS）
 int Data::getDynamicBrightness() const {
     return mDynamicBrightness;
 }
 
 void Data::setDynamicBrightness(int value) {
     mDynamicBrightness = value;
     // 不存储到NVS
 }
 
 // 三维数组数据管理
 ItemData Data::getItemData(int x, int y, int z) const {
     if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y || z < 0 || z >= MAX_Z) {
         // 索引超出范围，返回默认值
         return {0, false};
     }
     return mArray[x][y][z];
 }
 
 bool Data::setItemData(int x, int y, int z, const ItemData& data) {
     if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y || z < 0 || z >= MAX_Z) {
         // 索引超出范围
         return false;
     }
     
     mArray[x][y][z] = data;
     
     // 生成键名，例如："arr_1_2_3"
     char key[20];
     snprintf(key, sizeof(key), "%s_%d_%d_%d", NVS_KEY_ARRAY_PREFIX, x, y, z);
     
     // 将结构体转换为字节数组
     uint8_t buffer[sizeof(ItemData)];
     size_t size = itemDataToBytes(data, buffer);
     
     // 将字节数组写入NVS
     esp_err_t err = nvs.writeBlob(key, buffer, size);
     return (err == ESP_OK);
 }
 
 bool Data::saveArrayData() {
     bool success = true;
     
     for (int x = 0; x < MAX_X; x++) {
         for (int y = 0; y < MAX_Y; y++) {
             for (int z = 0; z < MAX_Z; z++) {
                 // 生成键名
                 char key[20];
                 snprintf(key, sizeof(key), "%s_%d_%d_%d", NVS_KEY_ARRAY_PREFIX, x, y, z);
                 
                 // 转换结构体为字节数组
                 uint8_t buffer[sizeof(ItemData)];
                 size_t size = itemDataToBytes(mArray[x][y][z], buffer);
                 
                 // 写入NVS
                 esp_err_t err = nvs.writeBlob(key, buffer, size);
                 if (err != ESP_OK) {
                     success = false;
                 }
             }
         }
     }
     
     // 提交所有更改
     esp_err_t err = nvs.commit();
     return success && (err == ESP_OK);
 }
 
 bool Data::loadArrayData() {
     bool success = true;
     
     for (int x = 0; x < MAX_X; x++) {
         for (int y = 0; y < MAX_Y; y++) {
             for (int z = 0; z < MAX_Z; z++) {
                 // 生成键名
                 char key[20];
                 snprintf(key, sizeof(key), "%s_%d_%d_%d", NVS_KEY_ARRAY_PREFIX, x, y, z);
                 
                 // 检查键是否存在
                 bool exists = false;
                 uint8_t buffer[sizeof(ItemData)];
                 
                 // 读取数据
                 size_t size = nvs.readBlob(key, buffer, sizeof(buffer), &exists);
                 
                 if (exists && size > 0) {
                     // 解析数据
                     if (!bytesToItemData(buffer, mArray[x][y][z])) {
                         success = false;
                     }
                 }
             }
         }
     }
     
     return success;
 }
 
 size_t Data::itemDataToBytes(const ItemData& data, uint8_t* buffer) const {
     // 简单的序列化实现
     // 对于更复杂的结构体，可能需要更复杂的序列化方法
     size_t offset = 0;
     
     // 写入int value
     memcpy(buffer + offset, &data.value, sizeof(data.value));
     offset += sizeof(data.value);
     
     // 写入bool enabled
     buffer[offset] = data.enabled ? 1 : 0;
     offset += 1;
     
     return offset;
 }
 
 bool Data::bytesToItemData(const uint8_t* buffer, ItemData& data) const {
     // 简单的反序列化实现
     size_t offset = 0;
     
     // 读取int value
     memcpy(&data.value, buffer + offset, sizeof(data.value));
     offset += sizeof(data.value);
     
     // 读取bool enabled
     data.enabled = buffer[offset] != 0;
     
     return true;
 }