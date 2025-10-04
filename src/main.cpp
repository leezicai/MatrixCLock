// #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // 不再需要，已包含在 dma.h 中
#include "brightnessManager.h"
#include "btn.h"
#include "dma.h"    // <--- 包含 DMA 设置头文件 (Include DMA setup header)
#include "ds3231.h" // 添加DS3231头文件
#include "ec11.h"
#include "ec11_handler.h" // 引入我们的编码器事件处理模块
#include "net.h"          // 包含网络模块头文件 (Include Network module header)
#include "setAP.h"
#include "task.h"
#include "sht30.h"
#include <U8g2_for_Adafruit_GFX.h>
#include "matrixDisplay.h"
#include "page.h"
#include "loading.h"
#include "matrixCore.h"
#include "matrixData.h"
#include "matrixTimeData.h"
#include <iostream>


#include <customFonts/FreeSans16pt7b.h> // 使用自定义字体


// 用于双缓冲的变量 - 保持全局指针
// 全局变量
MatrixPanel_I2S_DMA *dma_display = nullptr;
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

unsigned long lastMillisTime = 0;    // 上次执行时间
const unsigned long interval = 1000;
unsigned long duration = 3600; // 3.6秒加载动画
unsigned long elapsed = 0;

time_t lastSecond = 0;
time_t now;
TimeStrings timeNow;
TimeStrings timeNowNextSec;
TimeData timeDataNow;
TimeData timeDataNowNextSec;
DiffTimeStrings diffTimeStrings;
int16_t countUnderline = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 HUB75 WiFi Clock Starting (With RTC Support)");
  
  // Nvs同一空间只能初始化一次
  matrixNvsManager.initializeDefaults();

  // 温度传感器初始化
  if (sht30.begin()) {
    Serial.println("SHT30 sensor initialized successfully!");
  } else {
    Serial.println("Failed to initialize SHT30 sensor!");
  }

  buttonManager.begin();

  // 初始化编码器
  // EC11* encoder = EC11::getInstance(ROTARY_ENCODER_A_PIN,
  // ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN,
  // 4);  // 步数为1
  encoder->begin(true, 0, 100);

  // 配置按钮计时参数（可选）
  // 参数: 去抖时间(ms), 长按时间(ms), 双击时间间隔(ms)
  encoder->setButtonTiming(30, 800, 300);

  // 设置初始位置（可选）
  encoder->setPosition(50);

  // 初始化编码器事件处理器
  initEncoderHandlers();

    // 初始化DS3231
  rtc.begin(SDA, SCL);

  // task必须在设备初始化之后
  if (matrixDataManager.getWifiConfig()) {
    initTasks();
  }


  // 初始化 DMA 显示
  dma_display = setupDMA(50);
  if (!dma_display) {
    Serial.println("DMA Display setup failed, halting execution.");
    while (1) {
      delay(1000);
    }
  }
  brightnessManager.init();

  // 初始化 u8g2
  u8g2_for_adafruit_gfx.begin(*dma_display);

  Serial.println("--------------------------------");
  Serial.println(matrixDataManager.getWifiConfig());
  Serial.println(matrixDataManager.getWifiSSID());
  Serial.println(matrixDataManager.getWifiPassword());
  Serial.println(matrixDataManager.getTimezone());
  
  int count = 0;
  while (!matrixDataManager.getWifiConfig()) {
    if (count == 0) {
      Serial.println("Starting WiFi setup mode");
      loading.showSetupMsg();
      loading.flipDMABuffer();
      wifiManager.beginSetup();
      count = 2;
    }
    wifiManager.handleClient();
    buttonManager.tick();
  }
 
  lastMillisTime = millis();
  loading.drawFrame();
  loading.drawTitle();
  loading.flipDMABuffer();
  while (matrixDataManager.getWifiConfig()) {
    unsigned long elapsed = millis() - lastMillisTime;
    float percent = (float)elapsed / duration;
    if (percent > 1.0f) {
      percent = 1.0f; // 限制最大100%
    }
    loading.drawErrMsg();
    if (loading.getFlag() && elapsed < duration) {
      loading.updateProgress(percent);
      loading.flipDMABuffer();
    } else {
      loading.clearScreen();
      loading.flipDMABuffer();
      break;
    }
  }

  lastMillisTime = millis();
  now = time(nullptr);
  timeNow = display.getTimeStrings(now);
  timeNowNextSec = display.getTimeStrings(now + 1);
  diffTimeStrings = display.compareTimeStrings(timeNow, timeNowNextSec);
  timeDataNow = matrixTimeData.getTimeDataFromTimestamp(now);
  timeDataNowNextSec = matrixTimeData.getTimeDataFromTimestamp(now + 1);

  std::vector<const uint8_t *> fontList = {
u8g2_font_micro_tr,
u8g2_font_micro_mr,
u8g2_font_4x6_tr,
u8g2_font_4x6_mr,
u8g2_font_tom_thumb_4x6_tr,
u8g2_font_tom_thumb_4x6_mr,
u8g2_font_tinytim_tr,
u8g2_font_baby_tr,
u8g2_font_blipfest_07_tr,
u8g2_font_chikita_tr,
u8g2_font_pixelle_micro_tr,
u8g2_font_trixel_square_tr
  };
  // 对应的字体名称数组（用于显示）
  std::vector<String> fontNames = {
    "u8g2_font_10x20_tr",
    "u8g2_font_10x20_mr",
    "u8g2_font_10x20_t_cyrillic",
    "u8g2_font_t0_22_tr",
    "u8g2_font_t0_22_mr",
    "u8g2_font_t0_22b_tr",
    "u8g2_font_t0_22b_mr",
    "u8g2_font_crox4tb_tr",
    "u8g2_font_gb24st_t_2",
    "u8g2_font_timB14_tr",
    "u8g2_font_timR14_tr",
    "u8g2_font_lubB12_tr",
    "u8g2_font_lubBI12_tr",
    "u8g2_font_lubI12_tr",
    "u8g2_font_lubR12_tr",

    // --- 14 ---
    "u8g2_font_profont22_tr",
    "u8g2_font_profont22_mr",
    "u8g2_font_calibration_gothic_nbp_tr",
    "u8g2_font_crox4hb_tr",
    "u8g2_font_unifont_t_72_73",
    "u8g2_font_courR18_tr",
    "u8g2_font_helvB14_tr",
    "u8g2_font_helvR14_tr",
    "u8g2_font_ncenB14_tr",
    "u8g2_font_ncenR14_tr",
    "u8g2_font_luBIS14_tr",
    "u8g2_font_luBS14_tr",
    "u8g2_font_luIS14_tr",
    "u8g2_font_fub14_tr",
    "u8g2_font_fur14_tr",

    // --- 15 ---
    "u8g2_font_VCR_OSD_tr",
    "u8g2_font_VCR_OSD_mr",
    "u8g2_font_VCR_OSD_mu",
    "u8g2_font_courB18_tr",
    "u8g2_font_lubB14_tr",
    "u8g2_font_lubBI14_tr",
    "u8g2_font_lubI14_tr",
    "u8g2_font_lubR14_tr",

    // --- 16 ---
    "u8g2_font_crox5tb_tr",
    "u8g2_font_crox5t_tr",
    "u8g2_font_inr16_mr",
    "u8g2_font_inb16_mr",
    "u8g2_font_logisoso16_tr"
    };

  Serial.println("=== U8g2 Font Information ===\n");

  // 遍历字体列表
  for (size_t i = 0; i < fontList.size(); i++) {
    // 设置当前字体
    u8g2_for_adafruit_gfx.setFont(fontList[i]);
    FontMetrics fontABCMetrics = display.getFontMetrics(fontList[i], "W");
    FontMetrics fontNumMetrics = display.getFontMetrics(fontList[i], "0");
    FontMetrics fontSpaceMetrics = display.getFontMetrics(fontList[i], ":");

    // 打印字体信息
    Serial.println("Font: " + fontNames[i]);
    Serial.println("  '0' - Width: " + String(fontNumMetrics.charWidth) +
                   "px, Height: " + String(fontNumMetrics.charWidth) + "px");
    Serial.println("  'W' - Width: " + String(fontABCMetrics.charWidth) +
                   "px, Height: " + String(fontABCMetrics.height) + "px");
    Serial.println("  ':' - Width: " + String(fontSpaceMetrics.charWidth) +
                   "px, Height: " + String(fontSpaceMetrics.charWidth) + "px");
    Serial.println("------------------------");
  }
}

void loop() {

  
  now = time(nullptr);
  elapsed = millis() - lastMillisTime; 
  
    // 每秒更新一次时间显示
  if (elapsed >= 1000) {

    timeNow = display.getTimeStrings(now);
    timeNowNextSec = display.getTimeStrings(now + 1);
    diffTimeStrings = display.compareTimeStrings(timeNow, timeNowNextSec);
    timeDataNow = matrixTimeData.getTimeDataFromTimestamp(now);
    timeDataNowNextSec = matrixTimeData.getTimeDataFromTimestamp(now + 1);
    lastSecond = now;
    if(matrixCoreManager.getLineFlagTime() < 6){
      matrixCoreManager.setLineFlagTime(matrixCoreManager.getLineFlagTime() + 1);
    }
    if(matrixCoreManager.getPageFlagTime() < 6){
      matrixCoreManager.setPageFlagTime(matrixCoreManager.getPageFlagTime() + 1);
    }
    lastMillisTime = lastMillisTime + elapsed;
  }

  const SecondaryPage *secondaryPage =
      matrixCoreManager.getCurrentSecondaryPage();
  if (secondaryPage != nullptr) {
    display.clearScreen();
    
    for (size_t i = 0; i < secondaryPage->size(); ++i) {
      const auto &matrixCore = (*secondaryPage)[i];
           display.displayString(elapsed, timeDataNow, timeDataNowNextSec,
                            matrixCore);
           display.displayTextRGB(255, 0, 0, 15+10*i, PANEL_HEIGHT * PANEL_CHAIN,
                                  u8g2_font_blipfest_07_tr,
                                  matrixCore.fontIndex + 1);
           if (matrixCoreManager.getLineFlagTime() < 4 &&
               i == matrixCoreManager.getCurrentElementGroupIndex()) {
             display.displayUnderline(matrixCore);
           }
           if (matrixCoreManager.getPageFlagTime() < 5) {
             display.showPageInfo();
           }
    }
    display.flipDMABuffer();
  }


  buttonManager.tick();

  // 处理编码器事件 - 现在只需要这一行代码就可以处理所有编码器事件
  handleEncoderEvents(encoder);

  brightnessManager.handle(); // 这将处理自动亮度采样和更新
}