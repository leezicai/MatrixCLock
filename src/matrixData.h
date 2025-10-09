#ifndef MATRIX_DATA_H
#define MATRIX_DATA_H

#include "matrixNvs.h"
#include "matrixCore.h"
#include "common_define.h"
#include "matrixSetting.h"

// NVS Key Definitions - System Settings
#define NVS_KEY_SSID "ssid"
#define NVS_KEY_PASSWORD "pwd"
#define NVS_KEY_TIMEZONE "timezone"
#define NVS_KEY_WIFI_CONFIGURED "wificfg"
#define NVS_KEY_AUTO_MODE "automode"
#define NVS_KEY_MIN_BRIGHTNESS "minbright"
#define NVS_KEY_MAX_BRIGHTNESS "maxbright"
#define NVS_KEY_MANUAL_BRIGHTNESS "manbright"
#define NVS_KEY_LANGUAGE "language"

#define NVS_KEY_PAGE_INDEX "page_index"

#define NVS_KEY_PANEL_WIDTH "panel_width"
#define NVS_KEY_PANEL_HEIGHT "panel_height"
#define NVS_KEY_PANEL_CHAIN "panel_chain"
#define NVS_KEY_PANEL_TYPE "panel_type"


// NVS Key Definitions - Display Settings
#define NVS_KEY_CONTRAST            "contrast"
#define NVS_KEY_ANIMATION_SPEED     "anim_speed"
#define NVS_KEY_COLOR_SCHEME        "color_scheme"
#define NVS_KEY_SCREEN_TIMEOUT      "screen_timeout"

// NVS Key Definitions - Page Data
#define NVS_KEY_PAGE_0_0            "Page_0_0"
#define NVS_KEY_PAGE_0_1            "Page_0_1"
#define NVS_KEY_PAGE_0_2            "Page_0_2"
#define NVS_KEY_PAGE_1_0            "Page_1_0"
#define NVS_KEY_PAGE_1_1            "Page_1_1"
#define NVS_KEY_PAGE_1_2            "Page_1_2"
#define NVS_KEY_PAGE_2_0            "Page_2_0"
#define NVS_KEY_PAGE_2_1            "Page_2_1"
#define NVS_KEY_PAGE_2_2            "Page_2_2"

// NVS Key Definitions - User Settings
#define NVS_KEY_PREMARY_PAGE "current_page"
#define NVS_KEY_SECOND_PAGE "total_pages"
#define NVS_KEY_LAST_UPDATE_TIME    "last_update"

// Default Values
#define DEFAULT_BRIGHTNESS          50
#define DEFAULT_MIN_BRIGHTNESS          3
#define DEFAULT_MAX_BRIGHTNESS          200


#define DEFAULT_VOLUME              50
#define DEFAULT_DEVICE_NAME         "Matrix Display"
#define DEFAULT_ANIMATION_SPEED     100
#define DEFAULT_CONTRAST            75
#define DEFAULT_COLOR_SCHEME        0
#define DEFAULT_SCREEN_TIMEOUT      30
#define DEFAULT_AUTO_START          true
#define DEFAULT_SLEEP_MODE          false
#define DEFAULT_CURRENT_PAGE        0
#define DEFAULT_TOTAL_PAGES         9
#define DEFAULT_LANGUAGE            "en"

class MatrixDataManager {
private:
public:
    MatrixDataManager();
    ~MatrixDataManager();
    
    // Initialize all NVS data with defaults (call in setup)
    void init();

    void savePanelWidth(int value);
    int16_t loadPanelWidth();

    void savePanelHeight(int value);
    int16_t loadPanelHeight();

    void savePanelChain(int value);
    int16_t loadPanelChain();

    void savePanelType(int value);
    int16_t loadPanelType();

    void setAutoMode(bool value);
    bool getAutoMode();

    void savePrimaryPage(int value);
    int loadPrimaryPage();

    // System Settings
    void setBrightness(int value);
    int getBrightness();

    void setMinBrightness(int value);
    int getMinBrightness();

    void setMaxBrightness(int value);
    int getMaxBrightness();

    void setManBrightness(int value);
    int getManBrightness();


	void setTimezone(int value);
    int getTimezone();
    
    
    void setWifiSSID(const String& ssid);
    String getWifiSSID();
    
    void setWifiPassword(const String& password);
    String getWifiPassword();
    
    void setAutoStart(bool enabled);
    bool getAutoStart();
    
    void setWifiConfig(bool enabled);
    bool getWifiConfig();
    
    void setLanguage(int16_t lang);
    int16_t getLanguage();
    
    // Display Settings
    void setContrast(int value);
    int getContrast();
    
    
    
 
    
   
    
	void savePage(int row, int16_t pageIndex);
    int16_t loadPage(int row);
    
    // Page Data Operations
    void savePage(int row, int col, const SecondaryPage& page);
    SecondaryPage loadPage(int row, int col, SecondaryPage defaultPage);
    
    void savePageByKey(const String& key, const SecondaryPage& page);
    SecondaryPage loadPageByKey(const String& key, SecondaryPage defaultPage);

    void savePageIndex(int row, int index);
    int loadPageIndex(int row);

    // Utility Methods
    void clearAllData();
    bool hasKey(const String& key);
    void resetToDefaults();
};

// Global instance for easy access
extern MatrixDataManager matrixDataManager;

#endif // MATRIX_NVS_H