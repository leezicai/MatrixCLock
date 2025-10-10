#include "matrixFonts.h"
#include "string.h"
#include <U8g2_for_Adafruit_GFX.h>

// FontInfo constructor implementation
FontInfo::FontInfo(const uint8_t* fontName, float offsetSepX, float offsetSepY, float offsetFontX, float offsetFontY)
    : fontName(fontName), offsetSepX(offsetSepX), offsetSepY(offsetSepY), offsetFontX(offsetFontX), offsetFontY(offsetFontY) {
}

// Constructor implementation
MatrixFontManager::MatrixFontManager() {
    // Constructor can be empty since initialization is done in init()
    // init();
}

// Global SimpleFontManager object instance
MatrixFontManager matrixFontManager;

// Initialize all font groups and add them to the two-level structure
void MatrixFontManager::init() {
    // Clear existing data
    fontGroups.clear();
    
    // Create small fonts group
    FontGroup Font24_42PX;

    Font24_42PX.push_back(FontInfo(u8g2_font_fub30_tr, 0.1, 0.1, 0, 0)); // 0
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso32_tr, 0.13, 0.05, 0, 0)); // 0
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso30_tr, 0.13, 0, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso34_tr, 0.18, 0, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_inb24_mr, 0, 0.1, 0, 0)); // 

    Font24_42PX.push_back(FontInfo(u8g2_font_lubB24_tr, 0, 0.1, 0, 0)); // 
    Font24_42PX.push_back(FontInfo(u8g2_font_luBS24_tr, 0.15, 0.1, 0, 0)); // 
    Font24_42PX.push_back(FontInfo(u8g2_font_fub25_tr, 0.15, 0.1, 0, 0)); // 
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso28_tr, 0.25, 0, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_osb29_tr, 0, 0.1, 0, 0));

    Font24_42PX.push_back(FontInfo(u8g2_font_osr29_tr, 0, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_fur30_tr, 0, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_inr30_t_cyrillic, 0.22, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_inr27_mr, 0, 0.1, 0, 0));  // 1
    Font24_42PX.push_back(FontInfo(u8g2_font_inb27_mr, 0, 0.15, 0, 0));  // 2

    Font24_42PX.push_back(FontInfo(u8g2_font_inr30_mr, 0, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_inb30_mr, 0, 0.15, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_fub35_tr, 0.23, 0.1, 0, 0)); // 0
    Font24_42PX.push_back(FontInfo(u8g2_font_fur35_tr, 0.15, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_osb35_tr, 0, 0.1, 0, 0));

    Font24_42PX.push_back(FontInfo(u8g2_font_osr35_tr, -0.09, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso38_tr, 0.21, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_inr38_t_cyrillic, 0.23, 0.1, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_inr33_t_cyrillic, 0.18, 0.1, 0, 0));
    
    // Create medium fonts group
    FontGroup Font18_21PX_HMS;

    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_fub20_tr, 0.3, 0.1, 0, 0)); // 27
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_ncenB18_tr, 0.3, 0.1, 0, 0)); // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_ncenR18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_lubB18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_lubI18_tr, 0.3, 0.1, 0, 0)); // 

    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_lubR18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_luBS18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_luIS18_tr, 0.3, 0.1, 0, 0));  //
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_luRS18_tr, 0.3, 0.1, 0, 0)); // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_osb18_tr, 0.3, 0.1, 0, 0));  //  

    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_osr18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_logisoso18_tr, 0.3, 0.1, 0, 0));  //
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_profont29_tr, 0.3, 0.1, 0, 0)); //  
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_courR24_tr, 0.3, 0.1, 0, 0));  //  
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_helvB18_tr, 0.3, 0.1, 0, 0));  // 

    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_helvR18_tr, 0.3, 0.1, 0, 0));  //
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_lubI19_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_lubR19_tr, 0.3, 0.1, 0, 0));  //
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_luBS19_tr, 0.3, 0.1, 0, 0)); // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_luIS19_tr, 0.3, 0.1, 0, 0)); // 

    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_luRS19_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_fur20_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_osb21_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_osr21_tr, 0.3, 0.1, 0, 0));  //

    Font18_21PX_HMS.push_back(FontInfo(u8g2_font_logisoso20_tr, 0.3, 0.1, 0, 0)); // 
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso28_tr, 0.25, 0, 0, 0));
    Font24_42PX.push_back(FontInfo(u8g2_font_logisoso30_tr, 0.13, 0, 0, 0));

    FontGroup Font18_21PX_HM_AM;

    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_helvB18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_ncenB18_tr, 0.3, 0.1, 0, 0)); // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_ncenR18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_lubB18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_lubR18_tr, 0.3, 0.1, 0, 0));  // 

    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_luBS18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_osb18_tr, 0.3, 0.1, 0, 0));  //  
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_osr18_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_helvR18_tr, 0.3, 0.1, 0, 0));  //
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_lubR19_tr, 0.3, 0.1, 0, 0));  // 

    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_luRS19_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_fub20_tr, 0.3, 0.1, 0, 0)); //  
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_fur20_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_osb21_tr, 0.3, 0.1, 0, 0));  // 
    Font18_21PX_HM_AM.push_back(FontInfo(u8g2_font_logisoso20_tr, 0.3, 0.1, 0, 0)); // 

    
    // Create large fonts group
    FontGroup FONT16_17;
    FONT16_17.push_back(FontInfo(u8g2_font_crox5h_tr, 0.3, 0.1, 0, 0));  //   
    FONT16_17.push_back(FontInfo(u8g2_font_timB18_tr, 0.3, 0.1, 0, 0)); //  
    FONT16_17.push_back(FontInfo(u8g2_font_timR18_tr, 0.3, 0.1, 0, 0)); // 
    FONT16_17.push_back(FontInfo(u8g2_font_fub17_tr, 0.3, 0.1, 0, 0));  // 
    FONT16_17.push_back(FontInfo(u8g2_font_fur17_tr, 0.3, 0.1, 0, 0));  // 

    
    FontGroup FONT15;
    FONT15.push_back(FontInfo(u8g2_font_VCR_OSD_tr, 0, 0.1, 0, 0));  // 
    FONT15.push_back(FontInfo(u8g2_font_lubB14_tr, 0, 0.1, 0, 0)); // 
    FONT15.push_back(FontInfo(u8g2_font_lubR14_tr, 0, 0.1, 0, 0));  //

    FontGroup FONT14;
    FONT14.push_back(FontInfo(u8g2_font_profont22_tr, 0, 0.1, 0, 0));  // 
    FONT14.push_back(FontInfo(u8g2_font_crox4hb_tr, 0, 0.1, 0, 0)); // 
    FONT14.push_back(FontInfo(u8g2_font_courR18_tr, 0, 0.1, 0, 0));  //

    FONT14.push_back(FontInfo(u8g2_font_helvB14_tr, 0, 0.1, 0, 0));  //
    FONT14.push_back(FontInfo(u8g2_font_helvR14_tr, 0, 0.1, 0, 0));  //
    FONT14.push_back(FontInfo(u8g2_font_ncenB14_tr, 0, 0.1, 0, 0));  //
    FONT14.push_back(FontInfo(u8g2_font_ncenR14_tr, 0, 0.1, 0, 0));  //

    FONT14.push_back(FontInfo(u8g2_font_luBS14_tr, 0, 0.1, 0, 0));  //
    FONT14.push_back(FontInfo(u8g2_font_fub14_tr, 0, 0.1, 0, 0));  //
    FONT14.push_back(FontInfo(u8g2_font_fur14_tr, 0, 0.1, 0, 0));  //

    FontGroup FONT13;
    FONT13.push_back(FontInfo(u8g2_font_10x20_tr, 0, 0.1, 0, 0));  // 
    FONT13.push_back(FontInfo(u8g2_font_10x20_t_cyrillic, 0, 0.1, 0, 0)); // 
    FONT13.push_back(FontInfo(u8g2_font_t0_22_tr, 0, 0.1, 0, 0));  // 
    FONT13.push_back(FontInfo(u8g2_font_t0_22b_tr, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_crox4tb_tr, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_gb24st_t_2, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_timB14_tr, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_timR14_tr, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_lubB12_tr, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_lubBI12_tr, 0, 0.1, 0, 0));  //
    FONT13.push_back(FontInfo(u8g2_font_lubI12_tr, 0, 0.1, 0, 0));  //
    // Add all groups to the main FontGroups container

    FontGroup FONT12;
    // FontInfo push_back statements for each font
    FONT12.push_back(
        FontInfo(u8g2_font_cupcakemetoyourleader_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_tenstamps_mr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_heavybottom_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_lastapprenticebold_tr, 0, 0.1, 0, 0)); 
    FONT12.push_back(FontInfo(u8g2_font_HelvetiPixelOutline_tr, 0, 0.1, 0, 0));

    FONT12.push_back(FontInfo(u8g2_font_crox3cb_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_crox3c_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_crox3hb_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_crox3h_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_helvB12_tr, 0, 0.1, 0, 0));

    FONT12.push_back(FontInfo(u8g2_font_helvR12_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_ncenB12_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_ncenR12_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_luBS12_tr, 0, 0.1, 0, 0));
    FONT12.push_back(FontInfo(u8g2_font_luIS12_tr, 0, 0.1, 0, 0));

    FONT12.push_back(FontInfo(u8g2_font_luRS12_tr, 0, 0.1, 0, 0));
    FONT12.push_back(
        FontInfo(u8g2_font_lucasarts_scumm_subtitle_o_tr, 0, 0.1, 0, 0));

    FontGroup FONT11;
    FONT11.push_back(FontInfo(u8g2_font_t0_17_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_t0_17b_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_t0_18_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_t0_18b_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_profont17_tr, 0, 0.1, 0, 0));

    FONT11.push_back(FontInfo(u8g2_font_shylock_nbp_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_sticker_mel_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_squirrel_tr, 0, 0.1, 0, 0));

    FONT11.push_back(FontInfo(u8g2_font_bauhaus2015_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_frikativ_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_ImpactBits_tr, 0, 0.1, 0, 0));
    FONT11.push_back(
        FontInfo(u8g2_font_Untitled16PixelSansSerifBitmap_tr, 0, 0.1, 0, 0));

    FONT11.push_back(FontInfo(u8g2_font_Pixellari_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_crox3tb_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_crox3t_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_cu12_tr, 0, 0.1, 0, 0));

    FONT11.push_back(FontInfo(u8g2_font_courB14_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_courR14_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_helvB10_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_helvR10_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_ncenB10_tr, 0, 0.1, 0, 0));

    FONT11.push_back(FontInfo(u8g2_font_ncenR10_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_timB12_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_timR12_tr, 0, 0.1, 0, 0));
    FONT11.push_back(
        FontInfo(u8g2_font_lucasarts_scumm_subtitle_r_tr, 0, 0.1, 0, 0));
    FONT11.push_back(FontInfo(u8g2_font_fub11_tr, 0, 0.1, 0, 0));

    FONT11.push_back(FontInfo(u8g2_font_fur11_tr, 0, 0.1, 0, 0));

    FontGroup FONT10;
    // Generate push_back code for each font
    FONT10.push_back(FontInfo(u8g2_font_7x14_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_7x14B_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_8x13B_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_9x15_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_9x15B_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_9x18_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_9x18B_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_t0_15_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_t0_15b_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_t0_16_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_t0_16b_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_mercutio_basic_nbp_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_mercutio_sc_nbp_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_rosencrantz_nbp_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_guildenstern_nbp_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_beanstalk_mel_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_mademoiselle_mel_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_oldwizard_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_tenfatguys_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_halftone_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_CursivePixel_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_DigitalDiscoThin_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_DigitalDisco_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_crox2cb_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_crox2c_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_crox2hb_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_crox2h_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_crox2tb_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_crox2t_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_unifont_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_courB12_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_courR12_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_timB10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_timR10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_lubB10_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_lubBI10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_lubI10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_luBIS10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_lubR10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_luBS10_tr, 0, 0.1, 0, 0));

    FONT10.push_back(FontInfo(u8g2_font_luRS10_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_pxplusibmvga9_tr, 0, 0.1, 0, 0));
    FONT10.push_back(FontInfo(u8g2_font_pxplusibmvga8_tr, 0, 0.1, 0, 0));

    FontGroup FONT9;
    // Generate FontInfo objects for each font
    FONT9.push_back(FontInfo(u8g2_font_6x13_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_6x13B_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_6x13O_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_7x13_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_7x13B_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_7x13O_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_8x13_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_8x13O_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_t0_13_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_t0_13b_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_t0_14_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_t0_14b_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_profont15_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_glasstown_nbp_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_smart_patrol_nbp_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_prospero_bold_nbp_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_prospero_nbp_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_balthasar_regular_nbp_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_miranda_nbp_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_nine_by_five_nbp_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_missingplanet_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_cube_mel_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_press_mel_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_repress_mel_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_questgiver_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_koleeko_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_tenthinguys_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_tenthinnerguys_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_tooseornament_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_fancypixels_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_BBSesque_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_Born2bSportyV2_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_Engrish_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_TimesNewPixel_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_crox1cb_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_crox1c_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_crox1hb_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_crox1h_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_crox1tb_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_crox1t_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_wqy13_t_gb2312, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_courB10_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_courR10_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_lubB08_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_lubBI08_tr, 0, 0.1, 0, 0));

    FONT9.push_back(FontInfo(u8g2_font_lubI08_tr, 0, 0.1, 0, 0));
    FONT9.push_back(FontInfo(u8g2_font_lubR08_tr, 0, 0.1, 0, 0));

    FontGroup FONT8;
    FONT8.push_back(FontInfo(u8g2_font_t0_11_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_t0_11b_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_t0_12_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_t0_12b_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_profont12_tr, 0, 0.1, 0, 0));

    FONT8.push_back(FontInfo(u8g2_font_diodesemimono_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_bitcasual_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_nerhoe_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_HelvetiPixel_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_Wizzard_tr, 0, 0.1, 0, 0));

    FONT8.push_back(FontInfo(u8g2_font_helvB08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_helvR08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_ncenB08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_ncenR08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_luBIS08_tr, 0, 0.1, 0, 0));

    FONT8.push_back(FontInfo(u8g2_font_luBS08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_luIS08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_luRS08_tr, 0, 0.1, 0, 0));
    FONT8.push_back(FontInfo(u8g2_font_pressstart2p_8r, 0, 0.1, 0, 0));


    FontGroup FONT7;
    FONT7.push_back(FontInfo(u8g2_font_6x10_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_6x12_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_profont11_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_mozart_nbp_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_roentgen_nbp_tr, 0, 0.1, 0, 0));

    FONT7.push_back(FontInfo(u8g2_font_finderskeepers_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_sirclivethebold_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_sirclive_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_IPAandRUSLCD_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_BitTypeWriter_tr, 0, 0.1, 0, 0));

    FONT7.push_back(FontInfo(u8g2_font_timB08_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_timR08_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_lucasfont_alternate_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_haxrcorp4089_tr, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_pxplusibmcgathin_8r, 0, 0.1, 0, 0));

    FONT7.push_back(FontInfo(u8g2_font_pxplusibmcga_8r, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_pxplustandynewtv_8r, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_artossans8_8r, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_artosserif8_8r, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_torussansbold8_8r, 0, 0.1, 0, 0));

    FONT7.push_back(FontInfo(u8g2_font_victoriabold8_8r, 0, 0.1, 0, 0));
    FONT7.push_back(FontInfo(u8g2_font_victoriamedium8_8r, 0, 0.1, 0, 0));

    FontGroup FONT6;

    FONT6.push_back(FontInfo(u8g2_font_5x7_tr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_5x7_mr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_5x8_tr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_5x8_mr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_profont10_tr, 0, 0.1, 0, 0));

    FONT6.push_back(FontInfo(u8g2_font_profont10_mr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_synchronizer_nbp_tr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_courB08_tr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_courR08_tr, 0, 0.1, 0, 0));
    FONT6.push_back(FontInfo(u8g2_font_pcsenior_8r, 0, 0.1, 0, 0));

    FontGroup FONT5;

    FONT5.push_back(FontInfo(u8g2_font_blipfest_07_tr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_micro_tr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_micro_mr, 0, 0.1, 0, 0)); 
    FONT5.push_back(FontInfo(u8g2_font_4x6_tr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_4x6_mr, 0, 0.1, 0, 0));

    FONT5.push_back(FontInfo(u8g2_font_tom_thumb_4x6_tr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_tom_thumb_4x6_mr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_tinytim_tr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_baby_tr, 0, 0.1, 0, 0));
    FONT5.push_back(FontInfo(u8g2_font_chikita_tr, 0, 0.1, 0, 0));


    fontGroups.push_back(Font24_42PX); // 0
    fontGroups.push_back(Font18_21PX_HMS); // 1
    fontGroups.push_back(Font18_21PX_HM_AM); // 2
    fontGroups.push_back(FONT16_17); // 3
    fontGroups.push_back(FONT15); // 4
    fontGroups.push_back(FONT14); // 5
    fontGroups.push_back(FONT13); // 6
    fontGroups.push_back(FONT12); // 7
    fontGroups.push_back(FONT11); // 8
    fontGroups.push_back(FONT10); // 9
    fontGroups.push_back(FONT9); // 10
    fontGroups.push_back(FONT8); // 11
    fontGroups.push_back(FONT7); // 12
    fontGroups.push_back(FONT6); // 13
    fontGroups.push_back(FONT5); // 14
}

// Get current font info based on group index and font index within group
const FontInfo* MatrixFontManager::getCurrentFont(int16_t groupIndex, int16_t index) {
    // Validate group index
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return nullptr;  // Invalid group index
    }
    
    const FontGroup& currentGroup = fontGroups[groupIndex];
    
    // Validate font index within the group
    if (index < 0 || index >= static_cast<int16_t>(currentGroup.size())) {
        return nullptr;  // Invalid font index
    }
    
    // Return pointer to the font info
    return &(currentGroup[index]);
}

// Switch to next font index within the same group (circular)
int16_t MatrixFontManager::switchToNextFontIndex(int16_t groupIndex, int16_t index) {
    // Validate group index
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return -1;  // Invalid group index
    }
    
    const FontGroup& currentGroup = fontGroups[groupIndex];
    
    // Validate current font index
    if (index < 0 || index >= static_cast<int16_t>(currentGroup.size())) {
        return -1;  // Invalid current font index
    }
    
    // Calculate next index (circular)
    int16_t nextIndex = (index + 1) % static_cast<int16_t>(currentGroup.size());
    
    return nextIndex;
}

// Switch to previous font index within the same group (circular)
int16_t MatrixFontManager::switchToPreFontIndex(int16_t groupIndex, int16_t index) {
    // Validate group index
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return -1;  // Invalid group index
    }
    
    const FontGroup& currentGroup = fontGroups[groupIndex];
    
    // Validate current font index
    if (index < 0 || index >= static_cast<int16_t>(currentGroup.size())) {
        return -1;  // Invalid current font index
    }
    
    // Calculate previous index (circular)
    int16_t groupSize = static_cast<int16_t>(currentGroup.size());
    int16_t previousIndex = (index - 1 + groupSize) % groupSize;
    
    return previousIndex;
}

// Get total group count
size_t MatrixFontManager::getGroupCount() const {
    return fontGroups.size();
}

// Get font count in specific group
size_t MatrixFontManager::getFontCount(int16_t groupIndex) const {
    if (groupIndex < 0 || groupIndex >= static_cast<int16_t>(fontGroups.size())) {
        return 0;  // Invalid group index
    }
    
    return fontGroups[groupIndex].size();
}