#include "matrixCore.h"
#include <algorithm>
#include <stdexcept>
#include "matrixData.h"
#include "brightnessManager.h"
#include "matrixDma.h"
#include "matrixColors.h"

extern uint8_t g_panelType;

MatrixCore::MatrixCore(float x, float y, int16_t fontGroupIndex, int16_t fontIndex,
                       int16_t colorIndex1, int16_t colorIndex2, int16_t displayGroup,
                       int16_t displayIndex, int16_t animationType)
    : x(x), y(y), fontGroupIndex(fontGroupIndex), fontIndex(fontIndex),
      colorIndex1(colorIndex1), colorIndex2(colorIndex2), displayGroup(displayGroup),
      displayIndex(displayIndex), animationType(animationType) {}

MatrixCoreManager::MatrixCoreManager() {
    // initializeMatrixCores();
}

// Restores a saved page, but falls back to the built-in default when the saved
// copy is from an older firmware layout.
//
// Two things give that away: a different element count, and any element still
// bound to displayGroup 9 - the intraday chart, which no longer exists. Both
// happen to the stock pages after this build, since they went from
// "header + chart" to one text row per value. Without the check NVS would keep
// handing back the old shape forever and the panel would show a header with
// blank space under it, with no way back short of wiping NVS.
static SecondaryPage loadPageChecked(int row, int col, const SecondaryPage &def) {
    SecondaryPage saved = matrixDataManager.loadPage(row, col, def);

    if (saved.size() != def.size()) {
        return def;
    }
    for (const MatrixCore &core : saved) {
        if (core.displayGroup == 9) {
            return def;
        }
    }
    return saved;
}

SecondaryPage MatrixCoreManager::getSecondaryPageSetting1(){
    return secondaryPageSetting1;
}
SecondaryPage MatrixCoreManager::getSecondaryPageSetting2(){
    return secondaryPageSetting2;
}

void MatrixCoreManager::initializeMatrixCores() {
    // Create MatrixCore elements for different pages
    // Primary Page 0
    PrimaryPage primaryPage0;
    PrimaryPage primaryPage1;
    PrimaryPage primaryPage2;
    PrimaryPage primaryPage3;
    PrimaryPage primaryPage4;

    switch (g_panelType) {
        // 适配 128x64x1  64x64x2 屏幕
    case 0:
    case 1: {

      SecondaryPage secondaryPage0_0 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 0, 0),
          MatrixCore(0.50f, 0.16f, 10, 40, 5, 0, 3, 5, 0),

          MatrixCore(0.03f, 0.47f, 8, 1, 3, 0, 4, 8, 0),
          MatrixCore(0.36f, 0.47f, 8, 1, 3, 0, 4, 9, 0),
          MatrixCore(0.68f, 0.47f, 8, 1, 3, 0, 4, 10, 0),

          MatrixCore(0.03f, 0.79f, 8, 1, 3, 0, 4, 11, 0),
          MatrixCore(0.36f, 0.79f, 8, 1, 3, 0, 4, 12, 0),
          MatrixCore(0.68f, 0.79f, 8, 1, 3, 0, 4, 13, 0)};

      // 特殊处理,使用已有变量
      secondaryPageSetting1 = {MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 1, 1),

                          MatrixCore(0.03f, 0.47f, 10, 40, 5, 0, 2, 2, 1),
                          MatrixCore(0.52f, 0.47f, 10, 40, 3, 0, 3, 0, 1),

                          MatrixCore(0.03f, 0.79f, 8, 1, 3, 0, 3, 2, 1),
                          MatrixCore(0.52f, 0.79f, 8, 1, 3, 0, 3, 3, 1)};

      // 特殊处理,使用已有变量
      secondaryPageSetting2 = {MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 1, 1),

                           MatrixCore(0.03f, 0.47f, 10, 40, 3, 0, 2, 2, 1),
                           MatrixCore(0.52f, 0.47f, 10, 40, 3, 0, 3, 0, 1),

                           MatrixCore(0.52f, 0.79f, 8, 1, 3, 0, 3, 1, 1)};

      SecondaryPage secondaryPage0_2 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 5, 1),

          MatrixCore(0.03f, 0.47f, 10, 40, 3, 0, 3, 4, 1),
      };

       SecondaryPage secondaryPage0_3 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 5, 7, 0),
          MatrixCore(0.40f, 0.16f, 10, 40, 0, 0, 5, 8, 0),
          MatrixCore(0.68, 0.16f, 10, 40, 5, 0, 5, 12, 0),

          MatrixCore(0.03f, 0.38f, 10, 40, 3, 0, 5, 1, 0),
          MatrixCore(0.36f, 0.38f, 10, 40, 3, 0, 5, 2, 0),
          MatrixCore(0.68f, 0.38f, 10, 40, 3, 0, 5, 3, 0),

          MatrixCore(0.03f, 0.6f, 10, 40, 3, 0, 5, 4, 0),
          MatrixCore(0.25f, 0.6f, 10, 40, 3, 0, 5, 5, 0),
          MatrixCore(0.47f, 0.6f, 10, 40, 3, 0, 5, 6, 0),
          MatrixCore(0.69f, 0.6f, 10, 40, 3, 0, 5, 0, 0),

          MatrixCore(0.03f, 0.82f, 8, 1, 3, 0, 5, 9, 0),
          MatrixCore(0.15f, 0.82f, 8, 1, 3, 0, 5, 10, 0),
          MatrixCore(0.22f, 0.82f, 8, 1, 3, 0, 5, 11, 0),
          
      };

      // ---- Screen setting page ----
      // Element 0 is the title, element 1 cycles the panel type, element 2
      // commits it and reboots. Rotating on element 1 alone changes nothing,
      // so the user can browse the options without triggering a restart.
      SecondaryPage secondaryPage0_4 = {
          MatrixCore(0.03f, 0.16f, 10, 40, 5, 0, 2, 17, 1),  // "Screen"
          MatrixCore(0.03f, 0.47f, 10, 0, 3, 0, 3, 16, 1),   // 128x64 / 64x64x2 / 64x64
          MatrixCore(0.03f, 0.79f, 10, 40, 3, 0, 2, 18, 1),  // "Apply"

          MatrixCore(0.53f, 0.16f, 10, 40, 5, 0, 2, 17, 1),  // "Screen"      (right half)
          MatrixCore(0.53f, 0.47f, 10, 0, 3, 0, 3, 16, 1),   // panel option  (right half)
          MatrixCore(0.53f, 0.79f, 10, 40, 3, 0, 2, 18, 1)}; // "Apply"       (right half)

       secondaryPage0_0 = matrixDataManager.loadPage(0, 0, secondaryPage0_0);
       secondaryPage0_4 = matrixDataManager.loadPage(0, 4, secondaryPage0_4);
       primaryPage0.push_back(secondaryPage0_0);
       if (brightnessManager.getAutoMode()) {
         primaryPage0.push_back(secondaryPageSetting1);
       } else {
         primaryPage0.push_back(secondaryPageSetting2);
       }
      primaryPage0.push_back(secondaryPage0_2);
      primaryPage0.push_back(secondaryPage0_3);
      primaryPage0.push_back(secondaryPage0_4);
      interface.push_back(primaryPage0);

      // 24 getHour24_Minute_Second
      SecondaryPage secondaryPage1_0 = {MatrixCore(0.495f, 0.45f, 1, 0, 0, 0, 0, 1, 1)};
      // 24 getHour24_Minute
      SecondaryPage secondaryPage1_1 = { MatrixCore(0.495f, 0.45f, 0, 0, 1, 0, 0, 4, 2) };
      // 12 getHour12_MinuteAmpm
      SecondaryPage secondaryPage1_2 = { MatrixCore(0.495f, 0.45f, 2, 0, 2, 0, 0, 6, 0) };
      // 12 getHour12_Minute_Second
      SecondaryPage secondaryPage1_3 = { MatrixCore(0.495f, 0.45f, 1, 0, 3, 0, 0, 2, 0) };
      // 12 getHour12_Minute
      SecondaryPage secondaryPage1_4 = { MatrixCore(0.495f, 0.43f, 0, 0, 4, 0, 0, 5, 0) };

      secondaryPage1_0 = matrixDataManager.loadPage(1, 0, secondaryPage1_0);
      secondaryPage1_1 = matrixDataManager.loadPage(1, 1, secondaryPage1_1);
      secondaryPage1_2 = matrixDataManager.loadPage(1, 2, secondaryPage1_2);
      secondaryPage1_3 = matrixDataManager.loadPage(1, 3, secondaryPage1_3);
      secondaryPage1_4 = matrixDataManager.loadPage(1, 4, secondaryPage1_4);

      primaryPage1.push_back(secondaryPage1_0);
      primaryPage1.push_back(secondaryPage1_1);
      primaryPage1.push_back(secondaryPage1_2);
      primaryPage1.push_back(secondaryPage1_3);
      primaryPage1.push_back(secondaryPage1_4);

      interface.push_back(primaryPage1);

      // Primary Page 2
      SecondaryPage secondaryPage2_0 = {MatrixCore(0.5f, 0.25f, 9, 0, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};
      SecondaryPage secondaryPage2_1 = {MatrixCore(0.5f, 0.15f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.495f, 0.55f, 0, 0, 1, 0, 0, 4, 2)};
      SecondaryPage secondaryPage2_2 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 2, 0)};
      SecondaryPage secondaryPage2_3 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 2, 0, 0, 0, 0, 6, 0)};
      SecondaryPage secondaryPage2_4 = {MatrixCore(0.5f, 0.15f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.495f, 0.55f, 0, 0, 1, 0, 0, 5, 2)};
      SecondaryPage secondaryPage2_5 = {MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
                          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0)};

      secondaryPage2_0 = matrixDataManager.loadPage(2, 0, secondaryPage2_0);
      secondaryPage2_1 = matrixDataManager.loadPage(2, 1, secondaryPage2_1);
      secondaryPage2_2 = matrixDataManager.loadPage(2, 2, secondaryPage2_2);
      secondaryPage2_3 = matrixDataManager.loadPage(2, 3, secondaryPage2_3);
      secondaryPage2_4 = matrixDataManager.loadPage(2, 4, secondaryPage2_4);
      secondaryPage2_5 = matrixDataManager.loadPage(2, 5, secondaryPage2_5);

      primaryPage2.push_back(secondaryPage2_0);
      primaryPage2.push_back(secondaryPage2_1);
      primaryPage2.push_back(secondaryPage2_2);
      primaryPage2.push_back(secondaryPage2_3);
      primaryPage2.push_back(secondaryPage2_4);
      primaryPage2.push_back(secondaryPage2_5);

      interface.push_back(primaryPage2);

      SecondaryPage secondaryPage3_0 = {
          MatrixCore(0.5f, 0.25f, 10, 40, 1, 0, 1, 3, 0),
          MatrixCore(0.5f, 0.65f, 1, 0, 0, 0, 0, 1, 0),
          MatrixCore(1.0f, 0.90f, 12, 0, 0, 0, 6, 6, 0)
        };

      // Was loadPage(2, 0, ...) - collided with secondaryPage2_0's key
      secondaryPage3_0 = matrixDataManager.loadPage(3, 0, secondaryPage3_0);

      primaryPage3.push_back(secondaryPage3_0);

      interface.push_back(primaryPage3);

      // ---- Primary page 4 : stock indices ----
      // displayGroup 7 = stock text, displayGroup 8 = gain / loss bar.
      // displayIndex = slot * 10 + field, see stock.h for the field ids.
      //   slot 0 = SSE Composite, 1 = SZSE Component, 2 = ChiNext
      // displayGroup 9 (the intraday chart) is gone along with the Infoway
      // integration; the Sina feed carries no intraday series.

      // Pages 0-2: one index each, showing every number the feed carries -
      // level, change points, change percent, volume and turnover.
      //
      // Row 1 is the label plus the level in 10x20 digits, readable across the
      // room; the widest string, "SZ 13774.92", is 11 chars = 110px and still
      // clears 128. Rows 2-4 drop to the 7px ASCII font: change points and
      // percent share a row, then volume and turnover take one each - both run
      // to nine digits, which is 63px and would not fit beside anything else.
      //
      // Every stock element is left aligned (displayGroup 7 draws with
      // whereStart 0), so x is the left edge rather than a centre point. One
      // margin for all five pages keeps the labels in a single column as you
      // page through them.
      SecondaryPage secondaryPage4_0 = {
          MatrixCore(0.03f, 0.13f, 6, 0, 0, 0, 7, 4, 0),     // "SH 3932.70"
          MatrixCore(0.03f, 0.42f, 10, 40, 0, 0, 7, 2, 0),   // "+2.58"
          MatrixCore(0.52f, 0.42f, 10, 40, 0, 0, 7, 1, 0),   // "+0.07%"
          MatrixCore(0.03f, 0.66f, 10, 40, 0, 0, 7, 5, 0),   // volume
          MatrixCore(0.03f, 0.88f, 10, 40, 0, 0, 7, 6, 0)};  // turnover, 万元

      SecondaryPage secondaryPage4_1 = {
          MatrixCore(0.03f, 0.13f, 6, 0, 0, 0, 7, 14, 0),
          MatrixCore(0.03f, 0.42f, 10, 40, 0, 0, 7, 12, 0),
          MatrixCore(0.52f, 0.42f, 10, 40, 0, 0, 7, 11, 0),
          MatrixCore(0.03f, 0.66f, 10, 40, 0, 0, 7, 15, 0),
          MatrixCore(0.03f, 0.88f, 10, 40, 0, 0, 7, 16, 0)};

      SecondaryPage secondaryPage4_2 = {
          MatrixCore(0.03f, 0.13f, 6, 0, 0, 0, 7, 24, 0),
          MatrixCore(0.03f, 0.42f, 10, 40, 0, 0, 7, 22, 0),
          MatrixCore(0.52f, 0.42f, 10, 40, 0, 0, 7, 21, 0),
          MatrixCore(0.03f, 0.66f, 10, 40, 0, 0, 7, 25, 0),
          MatrixCore(0.03f, 0.88f, 10, 40, 0, 0, 7, 26, 0)};

      // Page 3: two indices, one row each. Each row is coloured by its own
      // gain / loss, so the page reads at a glance.
      SecondaryPage secondaryPage4_3 = {
          MatrixCore(0.03f, 0.27f, 6, 0, 0, 0, 7, 4, 0),     // "SH 3932.70"
          MatrixCore(0.03f, 0.72f, 6, 0, 0, 0, 7, 14, 0)};   // "SZ 13774.92"

      // Page 4: all three indices, one row each.
      SecondaryPage secondaryPage4_4 = {
          MatrixCore(0.03f, 0.17f, 6, 0, 0, 0, 7, 4, 0),
          MatrixCore(0.03f, 0.50f, 6, 0, 0, 0, 7, 14, 0),
          MatrixCore(0.03f, 0.83f, 6, 0, 0, 0, 7, 24, 0)};

      secondaryPage4_0 = loadPageChecked(4, 0, secondaryPage4_0);
      secondaryPage4_1 = loadPageChecked(4, 1, secondaryPage4_1);
      secondaryPage4_2 = loadPageChecked(4, 2, secondaryPage4_2);
      secondaryPage4_3 = loadPageChecked(4, 3, secondaryPage4_3);
      secondaryPage4_4 = loadPageChecked(4, 4, secondaryPage4_4);

      primaryPage4.push_back(secondaryPage4_0);
      primaryPage4.push_back(secondaryPage4_1);
      primaryPage4.push_back(secondaryPage4_2);
      primaryPage4.push_back(secondaryPage4_3);
      primaryPage4.push_back(secondaryPage4_4);

      interface.push_back(primaryPage4);
      break;
    }
    // Adapt to 64x64x1 panel
    // g_panelWidthChain = 64, g_panelHeightChain = 64
    // Vertical anchors: 13px CJK font is safe within y = 0.11 ~ 0.86
    // Horizontal budget: 64px. CJK glyph = 13px, wqy13 ASCII = 7px,
    //                    t0_11 / 6x13 ASCII = 6px
    case 2: {

      // ---- Primary page 0 / secondary 0 : date-time setting ----
      // rows: 0.12 label / 0.35 timezone / 0.58 Y-M-D / 0.80 H-M-S
      SecondaryPage secondaryPage0_0 = {
          MatrixCore(0.03f, 0.12f, 10, 40, 5, 0, 2, 0, 0),  // "Set-Time"
          MatrixCore(0.03f, 0.35f, 10, 40, 5, 0, 3, 5, 0),  // "TZ:+08"

          MatrixCore(0.03f, 0.58f, 11, 0, 3, 0, 4, 8, 0),   // year   2..26
          MatrixCore(0.45f, 0.58f, 11, 0, 3, 0, 4, 9, 0),   // month 29..41
          MatrixCore(0.72f, 0.58f, 11, 0, 3, 0, 4, 10, 0),  // day   46..58

          MatrixCore(0.03f, 0.80f, 11, 0, 3, 0, 4, 11, 0),  // hour
          MatrixCore(0.45f, 0.80f, 11, 0, 3, 0, 4, 12, 0),  // minute
          MatrixCore(0.72f, 0.80f, 11, 0, 3, 0, 4, 13, 0)}; // second

      // ---- Brightness setting page, auto mode variant ----
      secondaryPageSetting1 = {
          MatrixCore(0.03f, 0.12f, 10, 40, 5, 0, 2, 1, 1),  // "Bright"

          MatrixCore(0.03f, 0.40f, 10, 40, 5, 0, 2, 2, 1),  // "Auto"
          MatrixCore(0.60f, 0.40f, 10, 40, 3, 0, 3, 0, 1),  // On / Off

          MatrixCore(0.03f, 0.70f, 11, 0, 3, 0, 3, 2, 1),   // min brightness
          MatrixCore(0.52f, 0.70f, 11, 0, 3, 0, 3, 3, 1)};  // max brightness

      // ---- Brightness setting page, manual mode variant ----
      secondaryPageSetting2 = {
          MatrixCore(0.03f, 0.12f, 10, 40, 5, 0, 2, 1, 1),  // "Bright"

          MatrixCore(0.03f, 0.40f, 10, 40, 3, 0, 2, 2, 1),  // "Auto"
          MatrixCore(0.60f, 0.40f, 10, 40, 3, 0, 3, 0, 1),  // On / Off

          MatrixCore(0.60f, 0.70f, 11, 0, 3, 0, 3, 1, 1)};  // manual brightness

      // ---- Language setting page ----
      SecondaryPage secondaryPage0_2 = {
          MatrixCore(0.03f, 0.20f, 10, 40, 5, 0, 2, 5, 1),  // "Languge"
          MatrixCore(0.03f, 0.55f, 10, 40, 3, 0, 3, 4, 1)}; // "En" / current

      // ---- Alarm setting page ----
      // 64px wide fits only 3 weekday toggles per row (CJK toggle = 20px),
      // so the 13 elements are spread over 5 rows. Element order is kept
      // identical to the 128x64 layout so encoder navigation is unchanged.
      SecondaryPage secondaryPage0_3 = {
          MatrixCore(0.03f, 0.10f, 10, 40, 5, 0, 5, 7, 0),  // "Alarm0"

          MatrixCore(0.03f, 0.29f, 10, 40, 0, 0, 5, 8, 0),  // On / Off
          MatrixCore(0.60f, 0.29f, 10, 40, 5, 0, 5, 12, 0), // volume

          MatrixCore(0.02f, 0.48f, 10, 40, 3, 0, 5, 1, 0),  // Mon
          MatrixCore(0.34f, 0.48f, 10, 40, 3, 0, 5, 2, 0),  // Tue
          MatrixCore(0.66f, 0.48f, 10, 40, 3, 0, 5, 3, 0),  // Wed

          MatrixCore(0.02f, 0.66f, 10, 40, 3, 0, 5, 4, 0),  // Thu
          MatrixCore(0.34f, 0.66f, 10, 40, 3, 0, 5, 5, 0),  // Fri
          MatrixCore(0.66f, 0.66f, 10, 40, 3, 0, 5, 6, 0),  // Sat

          MatrixCore(0.02f, 0.84f, 10, 40, 3, 0, 5, 0, 0),  // Sun
          MatrixCore(0.38f, 0.84f, 10, 0, 3, 0, 5, 9, 0),   // hour
          MatrixCore(0.57f, 0.84f, 10, 0, 3, 0, 5, 10, 0),  // ":"
          MatrixCore(0.68f, 0.84f, 10, 0, 3, 0, 5, 11, 0)}; // minute

      // ---- Screen setting page ----
      // "64x64x2" in 6x13 = 7 * 6 = 42px, fits the 64px row.
      SecondaryPage secondaryPage0_4 = {
          MatrixCore(0.03f, 0.20f, 10, 40, 5, 0, 2, 17, 1),  // "Screen"
          MatrixCore(0.03f, 0.52f, 10, 0, 3, 0, 3, 16, 1),   // 128x64 / 64x64x2 / 64x64
          MatrixCore(0.03f, 0.84f, 10, 40, 3, 0, 2, 18, 1)}; // "Apply"

      secondaryPage0_0 = matrixDataManager.loadPage(0, 0, secondaryPage0_0);
      secondaryPage0_4 = matrixDataManager.loadPage(0, 4, secondaryPage0_4);
      primaryPage0.push_back(secondaryPage0_0);
      if (brightnessManager.getAutoMode()) {
        primaryPage0.push_back(secondaryPageSetting1);
      } else {
        primaryPage0.push_back(secondaryPageSetting2);
      }
      primaryPage0.push_back(secondaryPage0_2);
      primaryPage0.push_back(secondaryPage0_3);
      primaryPage0.push_back(secondaryPage0_4);
      interface.push_back(primaryPage0);

      // ---- Primary page 1 : full screen clock ----
      // font group 10 idx 0  = 6x13, "HH:MM:SS" -> 8 * 7 = 56px
      // font group 3  idx 3  = fub17, "HH:MM"   -> 4 * 12 + 7 = 55px
      // 24h HH:MM:SS
      SecondaryPage secondaryPage1_0 = {
          MatrixCore(0.50f, 0.45f, 10, 0, 0, 0, 0, 1, 1)};
      // 24h HH:MM (large)
      SecondaryPage secondaryPage1_1 = {  // ✅
          MatrixCore(0.50f, 0.45f, 3, 3, 1, 0, 0, 4, 2)};
      // 12h HH:MM AM/PM
      SecondaryPage secondaryPage1_2 = {
          MatrixCore(0.50f, 0.45f, 10, 0, 2, 0, 0, 6, 0)};
      // 12h HH:MM:SS
      SecondaryPage secondaryPage1_3 = {
          MatrixCore(0.50f, 0.45f, 10, 0, 3, 0, 0, 2, 0)};
      // 12h HH:MM (large)
      SecondaryPage secondaryPage1_4 = {
          MatrixCore(0.50f, 0.45f, 3, 3, 4, 0, 0, 5, 0)};

      secondaryPage1_0 = matrixDataManager.loadPage(1, 0, secondaryPage1_0);
      secondaryPage1_1 = matrixDataManager.loadPage(1, 1, secondaryPage1_1);
      secondaryPage1_2 = matrixDataManager.loadPage(1, 2, secondaryPage1_2);
      secondaryPage1_3 = matrixDataManager.loadPage(1, 3, secondaryPage1_3);
      secondaryPage1_4 = matrixDataManager.loadPage(1, 4, secondaryPage1_4);

      primaryPage1.push_back(secondaryPage1_0);
      primaryPage1.push_back(secondaryPage1_1);
      primaryPage1.push_back(secondaryPage1_2);
      primaryPage1.push_back(secondaryPage1_3);
      primaryPage1.push_back(secondaryPage1_4);

      interface.push_back(primaryPage1);

      // ---- Primary page 2 : date + clock ----
      // Date uses displayIndex 4 (month + day) or 1 (short weekday).
      // Index 3 / 5 are NOT usable here: Chinese renders 6 CJK glyphs = 78px.
      SecondaryPage secondaryPage2_0 = {
          MatrixCore(0.50f, 0.22f, 10, 0, 1, 0, 1, 4, 0),
          MatrixCore(0.50f, 0.62f, 10, 0, 0, 0, 0, 1, 0)};
      SecondaryPage secondaryPage2_1 = {
          MatrixCore(0.50f, 0.20f, 10, 0, 1, 0, 1, 4, 0),
          MatrixCore(0.50f, 0.60f, 3, 3, 1, 0, 0, 4, 2)};
      SecondaryPage secondaryPage2_2 = {
          MatrixCore(0.50f, 0.22f, 10, 0, 1, 0, 1, 4, 0),
          MatrixCore(0.50f, 0.62f, 10, 0, 0, 0, 0, 2, 0)};
      SecondaryPage secondaryPage2_3 = {
          MatrixCore(0.50f, 0.22f, 10, 0, 1, 0, 1, 4, 0),
          MatrixCore(0.50f, 0.62f, 10, 0, 0, 0, 0, 6, 0)};
      SecondaryPage secondaryPage2_4 = {
          MatrixCore(0.50f, 0.20f, 10, 0, 1, 0, 1, 4, 0),
          MatrixCore(0.50f, 0.60f, 3, 3, 1, 0, 0, 5, 2)};
      SecondaryPage secondaryPage2_5 = {
          MatrixCore(0.50f, 0.22f, 10, 0, 1, 0, 1, 1, 0),
          MatrixCore(0.50f, 0.62f, 10, 0, 0, 0, 0, 1, 0)};

      secondaryPage2_0 = matrixDataManager.loadPage(2, 0, secondaryPage2_0);
      secondaryPage2_1 = matrixDataManager.loadPage(2, 1, secondaryPage2_1);
      secondaryPage2_2 = matrixDataManager.loadPage(2, 2, secondaryPage2_2);
      secondaryPage2_3 = matrixDataManager.loadPage(2, 3, secondaryPage2_3);
      secondaryPage2_4 = matrixDataManager.loadPage(2, 4, secondaryPage2_4);
      secondaryPage2_5 = matrixDataManager.loadPage(2, 5, secondaryPage2_5);

      primaryPage2.push_back(secondaryPage2_0);
      primaryPage2.push_back(secondaryPage2_1);
      primaryPage2.push_back(secondaryPage2_2);
      primaryPage2.push_back(secondaryPage2_3);
      primaryPage2.push_back(secondaryPage2_4);
      primaryPage2.push_back(secondaryPage2_5);

      interface.push_back(primaryPage2);

      // ---- Primary page 3 : date + clock + temperature ----
      // Temperature is right aligned (displayGroup 6), x = 1.0 -> ends at 64.
      SecondaryPage secondaryPage3_0 = {
          MatrixCore(0.50f, 0.18f, 10, 0, 1, 0, 1, 4, 0),   // date
          MatrixCore(0.50f, 0.50f, 10, 0, 0, 0, 0, 1, 0),   // HH:MM:SS
          MatrixCore(0.03f, 0.85f, 12, 0, 0, 0, 3, 6, 0),   // temp,     left  2..20
          MatrixCore(0.95f, 0.85f, 12, 0, 0, 0, 6, 10, 0)}; // humidity, right 46..64

      secondaryPage3_0 = matrixDataManager.loadPage(3, 0, secondaryPage3_0);

      primaryPage3.push_back(secondaryPage3_0);

      interface.push_back(primaryPage3);

      // ---- Primary page 4 : stock indices ----
      // Same element order as the 128x64 layout so encoder navigation matches.
      // displayGroup 9 (the intraday chart) is gone; pages 0-2 now carry the
      // five numbers the Sina feed returns, one per row.
      //
      // Five rows of 6x13 into 64px leaves no slack, so the anchors are an
      // even 0.10 / 0.30 / 0.50 / 0.70 / 0.90 - 12.8px apart. Digits have no
      // descenders, so a 13px font clears that; the page indicator does clip
      // the last row for the few seconds it is on screen after a page change.
      //
      // Field 7 is used for the header instead of field 4 because it drops the
      // decimals ("SZ 13775", 8 chars at 7px = 56px). Field 4 would need 77px
      // and overflow the row - the exact decimals are only on the 128px panel.
      // Volume and turnover run to nine digits = 63px, which is why the left
      // edge sits at 0.01 rather than the 0.05 the old layout used.
      SecondaryPage secondaryPage4_0 = {
          MatrixCore(0.01f, 0.10f, 10, 0, 0, 0, 7, 7, 0),    // "SH 3933"
          MatrixCore(0.01f, 0.30f, 10, 0, 0, 0, 7, 2, 0),    // "+2.58"
          MatrixCore(0.01f, 0.50f, 10, 0, 0, 0, 7, 1, 0),    // "+0.07%"
          MatrixCore(0.01f, 0.70f, 10, 0, 0, 0, 7, 5, 0),    // volume
          MatrixCore(0.01f, 0.90f, 10, 0, 0, 0, 7, 6, 0)};   // turnover, 万元

      SecondaryPage secondaryPage4_1 = {
          MatrixCore(0.01f, 0.10f, 10, 0, 0, 0, 7, 17, 0),
          MatrixCore(0.01f, 0.30f, 10, 0, 0, 0, 7, 12, 0),
          MatrixCore(0.01f, 0.50f, 10, 0, 0, 0, 7, 11, 0),
          MatrixCore(0.01f, 0.70f, 10, 0, 0, 0, 7, 15, 0),
          MatrixCore(0.01f, 0.90f, 10, 0, 0, 0, 7, 16, 0)};

      SecondaryPage secondaryPage4_2 = {
          MatrixCore(0.01f, 0.10f, 10, 0, 0, 0, 7, 27, 0),
          MatrixCore(0.01f, 0.30f, 10, 0, 0, 0, 7, 22, 0),
          MatrixCore(0.01f, 0.50f, 10, 0, 0, 0, 7, 21, 0),
          MatrixCore(0.01f, 0.70f, 10, 0, 0, 0, 7, 25, 0),
          MatrixCore(0.01f, 0.90f, 10, 0, 0, 0, 7, 26, 0)};

      // Two indices, one row each.
      SecondaryPage secondaryPage4_3 = {
          MatrixCore(0.05f, 0.30f, 10, 0, 0, 0, 7, 7, 0),
          MatrixCore(0.05f, 0.72f, 10, 0, 0, 0, 7, 17, 0)};

      // All three indices, one row each.
      SecondaryPage secondaryPage4_4 = {
          MatrixCore(0.05f, 0.18f, 10, 0, 0, 0, 7, 7, 0),
          MatrixCore(0.05f, 0.50f, 10, 0, 0, 0, 7, 17, 0),
          MatrixCore(0.05f, 0.82f, 10, 0, 0, 0, 7, 27, 0)};

      secondaryPage4_0 = loadPageChecked(4, 0, secondaryPage4_0);
      secondaryPage4_1 = loadPageChecked(4, 1, secondaryPage4_1);
      secondaryPage4_2 = loadPageChecked(4, 2, secondaryPage4_2);
      secondaryPage4_3 = loadPageChecked(4, 3, secondaryPage4_3);
      secondaryPage4_4 = loadPageChecked(4, 4, secondaryPage4_4);

      primaryPage4.push_back(secondaryPage4_0);
      primaryPage4.push_back(secondaryPage4_1);
      primaryPage4.push_back(secondaryPage4_2);
      primaryPage4.push_back(secondaryPage4_3);
      primaryPage4.push_back(secondaryPage4_4);

      interface.push_back(primaryPage4);
      break;
    }
    default: {

      break;
    }
    }

    // Initialize history array with same size as primary pages, all starting at 0
    history.resize(interface.size(), 0);
    for (size_t i = 0; i < history.size(); i++) {
      history[i] = matrixDataManager.loadPageIndex(i);
    }

    // Clamp restored indices: NVS may hold values from another panel layout
    if (interface.empty()) {
      pageIndex = 0;
      secondaryIndex = 0;
      elementGroupIndex = 0;
      return;
    }

    pageIndex = matrixDataManager.loadPrimaryPage();
    if (pageIndex < 0 || pageIndex >= static_cast<int16_t>(interface.size())) {
      pageIndex = 0;
    }
    secondaryIndex = history[pageIndex];
    if (secondaryIndex < 0 ||
        secondaryIndex >= static_cast<int16_t>(interface[pageIndex].size())) {
      secondaryIndex = 0;
    }
    elementGroupIndex = 0;

}

// Update history when secondary page changes
void MatrixCoreManager::updateHistory() {
    if (pageIndex < static_cast<int16_t>(history.size())) {
        matrixDataManager.savePageIndex(pageIndex, secondaryIndex);
        history[pageIndex] = secondaryIndex;
    }
}
void MatrixCoreManager::updatePage() { matrixDataManager.savePrimaryPage(pageIndex); }

// Primary page navigation - Circular switching
void MatrixCoreManager::nextPrimaryPage() {
    updateHistory();  // Save current secondary page position before switching
    
    if (!interface.empty()) {
        pageIndex = (pageIndex + 1) % static_cast<int16_t>(interface.size());
        // Restore last secondary page position for the new primary page
        if (pageIndex < static_cast<int16_t>(history.size())) {
            secondaryIndex = history[pageIndex];
        }
    }

    updatePage();

    resetElementGroup();  // Reset to first element when switching primary page
}

void MatrixCoreManager::prevPrimaryPage() {
    updateHistory();  // Save current secondary page position before switching
    
    if (!interface.empty()) {
        pageIndex = (pageIndex - 1 + static_cast<int16_t>(interface.size())) % static_cast<int16_t>(interface.size());
        // Restore last secondary page position for the new primary page
        if (pageIndex < static_cast<int16_t>(history.size())) {
            secondaryIndex = history[pageIndex];
        }
    }
    updatePage();
    resetElementGroup();  // Reset to first element when switching primary page
}

void MatrixCoreManager::resetPrimaryPage() {
    updateHistory();  // Save current position before reset
    pageIndex = 0;
    if (!history.empty()) {
        secondaryIndex = history[0];
    }
    updatePage();
    resetElementGroup();
}

// Secondary page navigation - Circular switching
void MatrixCoreManager::nextSecondaryPage() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        !interface[pageIndex].empty()) {
        secondaryIndex = (secondaryIndex + 1) % static_cast<int16_t>(interface[pageIndex].size());
        updateHistory();  // Update history when secondary page changes
    }
    updatePage();
    resetElementGroup();  // Reset to first element when switching secondary page
}

void MatrixCoreManager::prevSecondaryPage() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        !interface[pageIndex].empty()) {
        secondaryIndex = (secondaryIndex - 1 + static_cast<int16_t>(interface[pageIndex].size())) % static_cast<int16_t>(interface[pageIndex].size());
        updateHistory();  // Update history when secondary page changes
    }
    updatePage();
    resetElementGroup();  // Reset to first element when switching secondary page
}

void MatrixCoreManager::resetSecondaryPage() {
    secondaryIndex = 0;
    updateHistory();  // Update history when secondary page changes
    updatePage();
    resetElementGroup();
}

// Element group navigation - Now navigates individual MatrixCore elements
void MatrixCoreManager::nextElementGroup() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        !interface[pageIndex][secondaryIndex].empty()) {
        elementGroupIndex = (elementGroupIndex + 1) % static_cast<int16_t>(interface[pageIndex][secondaryIndex].size());
    }
}

void MatrixCoreManager::prevElementGroup() {
    if (pageIndex < static_cast<int16_t>(interface.size()) && 
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        !interface[pageIndex][secondaryIndex].empty()) {
        elementGroupIndex = (elementGroupIndex - 1 + static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) % static_cast<int16_t>(interface[pageIndex][secondaryIndex].size());
    }
}

void MatrixCoreManager::resetElementGroup() {
    elementGroupIndex = 0;
}

void MatrixCoreManager::button2LongPressStart() { button2LongPressFlag = 1; }
void MatrixCoreManager::button2LongPressEnd() { button2LongPressFlag = 0; }
int16_t MatrixCoreManager::getButtton2LongPressFlag() { return button2LongPressFlag ; }

int16_t MatrixCoreManager::getLineFlagTime() { return lineFlagTime;}
void MatrixCoreManager::setLineFlagTime(int16_t lineFlagTime) { 
    this->lineFlagTime = lineFlagTime; 
}

int16_t MatrixCoreManager::getPageFlagTime() { return pageFlagTime;}
void MatrixCoreManager::setPageFlagTime(int16_t pageFlagTime) { 
    this->pageFlagTime = pageFlagTime; 
}

// Modification methods (removed the first overload as requested)
void MatrixCoreManager::modifyElement(int16_t pageIndex, int16_t secondaryIndex, 
                                   int16_t elementGroupIndex, const MatrixCore& newElement) {
    // In 3-layer structure, elementGroupIdx is the element index in secondary page
    // elementIdx parameter is now redundant but kept for API compatibility

    if (secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        interface[pageIndex][secondaryIndex][elementGroupIndex] = newElement;
    }
}
void MatrixCoreManager::modifyCurrentElement(const MatrixCore& newElement) {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        interface[pageIndex][secondaryIndex][elementGroupIndex] = newElement;
    }

    matrixDataManager.savePage(getCurrentPageIndex(), getCurrentSecondaryIndex(), interface[pageIndex][secondaryIndex]);
}

// Swap/Replace a secondary page with a new one
void MatrixCoreManager::swapSecondaryPage(int16_t pageIdx, int16_t secondaryIdx, 
                                          const SecondaryPage& newSecondaryPage) {
    // Validate pageIdx is within bounds
    if (pageIdx < 0 || pageIdx >= static_cast<int16_t>(interface.size())) {
        return; // Invalid primary page index
    }
    
    // Validate secondaryIdx is within bounds
    if (secondaryIdx < 0 || secondaryIdx >= static_cast<int16_t>(interface[pageIdx].size())) {
        return; // Invalid secondary page index
    }
    
    // Replace the secondary page
    interface[pageIdx][secondaryIdx] = newSecondaryPage;
    
    // If we're currently on this page, check if elementGroupIndex is still valid
    if (pageIndex == pageIdx && secondaryIndex == secondaryIdx) {
        // If current elementGroupIndex exceeds new page size, reset to 0
        if (elementGroupIndex >= static_cast<int16_t>(newSecondaryPage.size())) {
            resetElementGroup();
        }
        // Otherwise keep the current elementGroupIndex unchanged
    }
}

// Getter methods
const MatrixCore* MatrixCoreManager::getCurrentElementGroup() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        return &interface[pageIndex][secondaryIndex][elementGroupIndex];
    }
    return nullptr;
}

const SecondaryPage* MatrixCoreManager::getCurrentSecondaryPage() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size())) {
        
        return &interface[pageIndex][secondaryIndex];
    }
    return nullptr;
}

const PrimaryPage* MatrixCoreManager::getCurrentPrimaryPage() const {
    if (pageIndex < static_cast<int16_t>(interface.size())) {
        return &interface[pageIndex];
    }
    return nullptr;
}

// New method: Get current MatrixCore element
const MatrixCore& MatrixCoreManager::getCurrentMatrixCore() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size()) &&
        elementGroupIndex < static_cast<int16_t>(interface[pageIndex][secondaryIndex].size())) {
        
        return interface[pageIndex][secondaryIndex][elementGroupIndex];
    }
    
    // Throw exception if indices are invalid
    throw std::out_of_range("Invalid indices for getCurrentMatrixCore()");
}

size_t MatrixCoreManager::getSecondaryPageCount() const {
    if (pageIndex < static_cast<int16_t>(interface.size())) {
        return interface[pageIndex].size();
    }
    return 0;
}

size_t MatrixCoreManager::getElementGroupCount() const {
    if (pageIndex < static_cast<int16_t>(interface.size()) &&
        secondaryIndex < static_cast<int16_t>(interface[pageIndex].size())) {
        return interface[pageIndex][secondaryIndex].size();
    }
    return 0;
}

// History related methods
int16_t MatrixCoreManager::getHistoryForPage(int16_t primaryPageIdx) const {
    if (primaryPageIdx >= 0 && primaryPageIdx < static_cast<int16_t>(history.size())) {
        return history[primaryPageIdx];
    }
    return 0;
}

void MatrixCoreManager::setHistoryForPage(int16_t primaryPageIdx, int16_t secondaryPageIdx) {
    if (primaryPageIdx >= 0 && primaryPageIdx < static_cast<int16_t>(history.size())) {
        history[primaryPageIdx] = secondaryPageIdx;
    }
}

void MatrixCoreManager::printAllSecondaryPages() const {
    // Iterate through all primary pages
    for (size_t i = 0; i < interface.size(); i++) {
        const PrimaryPage& primaryPage = interface[i];
        
        // Iterate through all secondary pages in current primary page
        for (size_t j = 0; j < primaryPage.size(); j++) {
            const SecondaryPage& secondaryPage = primaryPage[j];
            
            // Print variable name with indices
            printf("secondaryPage%d_%d = {", 
                   static_cast<int>(i), 
                   static_cast<int>(j));
            
            // Print each MatrixCore element in the secondary page
            for (size_t k = 0; k < secondaryPage.size(); k++) {
                const MatrixCore& core = secondaryPage[k];
                
                printf("MatrixCore(%.3ff, %.3ff, %d, %d, %d, %d, %d, %d, %d)",
                       core.x, core.y,
                       core.fontGroupIndex, core.fontIndex,
                       core.colorIndex1, core.colorIndex2,
                       core.displayGroup, core.displayIndex,
                       core.animationType);
                
                // Add comma if not the last element
                if (k < secondaryPage.size() - 1) {
                    printf(",\n                          ");
                }
            }
            
            printf("};\n\n");
        }
    }
}
// Global instance
MatrixCoreManager matrixCoreManager;