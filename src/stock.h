#ifndef STOCK_H
#define STOCK_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "matrixColors.h"

// ---------------------------------------------------------------------------
// Data source: Sina realtime index feed (hq.sinajs.cn).
//
// One plain HTTP GET returns all three indices in a ~220 byte body:
//
//   GET http://hq.sinajs.cn/list=s_sh000001,s_sz399001,s_sz399006
//   Referer: https://finance.sina.com.cn      <- mandatory, 403 without it
//
//   var hq_str_s_sh000001="<name>,3932.6992,2.5828,0.07,4773752,89790401";
//   var hq_str_s_sz399001="<name>,13774.92,257.946,1.91,587625706,104811487";
//   var hq_str_s_sz399006="<name>,3398.68,112.135,3.41,29325362,22823864";
//
// Fields after the name, in order:
//   1 index level          3932.6992
//   2 change points        2.5828
//   3 change percent       0.07      (no trailing % sign here)
//   4 volume               4773752
//   5 turnover, in 万元     89790401
//
// The name is GB18030 encoded and is never parsed - the panel renders the
// ASCII labels defined below instead, so no transcoding is needed anywhere.
//
// This replaces the previous Infoway batch_kline integration. Consequences
// worth knowing before touching anything here:
//
//   * No API key. stock_secret.h is deleted; nothing reads NVS "stock_api".
//   * No TLS. Plain HTTP over WiFiClient, so no mbedtls heap spike and no
//     multi-second handshake - the timeout and heap floor drop accordingly.
//   * No JSON. ArduinoJson is no longer pulled in by this module.
//   * No intraday chart. StockChart, fetchChart() and displayGroup 9 are
//     gone; the single index pages now show the five raw numbers instead.
//
// Volume units are NOT consistent across the two exchanges in this feed (the
// SSE figure is a hundred-hands scale, the SZSE one is hands), so the numbers
// are surfaced exactly as received rather than being normalised into 亿/万.
// ---------------------------------------------------------------------------

#define STOCK_QUOTE_URL_BASE   "http://hq.sinajs.cn/list="
#define STOCK_SINA_REFERER     "https://finance.sina.com.cn"
#define STOCK_SINA_USER_AGENT  "Mozilla/5.0 (MatrixClock ESP32)"

#define STOCK_CODE_SH          "s_sh000001"   // SSE Composite  上证指数
#define STOCK_CODE_SZ          "s_sz399001"   // SZSE Component 深证成指
#define STOCK_CODE_CY          "s_sz399006"   // ChiNext        创业板指
#define STOCK_CODES_ALL        STOCK_CODE_SH "," STOCK_CODE_SZ "," STOCK_CODE_CY
#define STOCK_QUOTE_URL        STOCK_QUOTE_URL_BASE STOCK_CODES_ALL

// Prefix of the javascript variable each row is assigned to.
#define STOCK_VAR_PREFIX       "hq_str_"

#define STOCK_INDEX_COUNT      3
#define STOCK_PAGE_INDEX       4    // primary page that holds the stock views

// Plain HTTP to a domestic host: seconds, not the tens of seconds a remote
// TLS handshake needed.
#define STOCK_HTTP_TIMEOUT_MS  8000

// The whole reply is around 220 bytes; this is the ceiling before the body is
// treated as something other than a quote feed (an error page, a redirect).
#define STOCK_BODY_MAX_LEN     1024

enum StockSlot {
    STOCK_SH = 0,
    STOCK_SZ = 1,
    STOCK_CY = 2
};

// ---------------------------------------------------------------------------
// Display encoding, used by MatrixCore.displayIndex on displayGroup 7.
//   displayIndex = slot * 10 + field
//
// Fields 0-4 and 7 keep the ids they had under the Infoway build so existing
// page layouts keep rendering the same thing. Fields 8 and 9 (the chart axis
// labels) are gone along with displayGroup 9.
// ---------------------------------------------------------------------------
#define STOCK_FIELD_PRICE        0   // "3932.70"
#define STOCK_FIELD_PCT          1   // "+0.07%"
#define STOCK_FIELD_AMOUNT       2   // "+2.58"
#define STOCK_FIELD_LABEL        3   // "SH"
#define STOCK_FIELD_LABEL_PRICE  4   // "SH 3932.70"
#define STOCK_FIELD_VOLUME       5   // "4773752"   raw, see the note above
#define STOCK_FIELD_TURNOVER     6   // "89790401"  raw, in 万元
#define STOCK_FIELD_LABEL_INT    7   // "SH 3933"   - for the 64px wide panel

#define STOCK_SLOT_OF(idx)       ((idx) / 10)
#define STOCK_FIELD_OF(idx)      ((idx) % 10)

// ---------------------------------------------------------------------------
// Session geometry. The A-share day is 09:30-11:30 and 13:00-15:00.
// ---------------------------------------------------------------------------
#define STOCK_MIN_AM_OPEN        570  // 09:30
#define STOCK_MIN_AM_CLOSE       690  // 11:30
#define STOCK_MIN_PM_OPEN        780  // 13:00
#define STOCK_MIN_PM_CLOSE       900  // 15:00
#define STOCK_MIN_POSTCLOSE_END  960  // 16:00
#define STOCK_MIN_WEEKEND_SAMPLE 540  // 09:00

// ---------------------------------------------------------------------------
// Refresh schedule.
//
// The feed is free and the body is a couple of hundred bytes, so the interval
// is now driven by how fresh the panel should look rather than by an API
// budget: one call per minute while the market is open, a slow tick outside
// it, and a slower one still when the stock page is not on screen.
// ---------------------------------------------------------------------------
#define STOCK_QUOTE_MS_SESSION   (60UL * 1000UL)
#define STOCK_QUOTE_MS_IDLE      (10UL * 60UL * 1000UL)  // stock page not shown
#define STOCK_POSTCLOSE_MS       (15UL * 60UL * 1000UL)

#define STOCK_STALE_MS           (40UL * 60UL * 1000UL)

// Self-imposed ceiling. Sina publishes no quota; this exists so a scheduling
// bug cannot turn into a request loop. A full session at one call a minute
// costs about 330.
#define STOCK_DAILY_CALL_BUDGET  2000

// Without TLS the request needs almost nothing, but a floor still keeps the
// call out of the way when the renderer is under pressure.
#define STOCK_MIN_FREE_HEAP      20000

// Gain / loss bar (displayGroup 8). No page uses it, but
// Display::drawStockBar() is still compiled.
#define STOCK_BAR_FULL_SCALE_PCT 5.0f
#define STOCK_BAR_HEIGHT_WIDE    6
#define STOCK_BAR_HEIGHT_NARROW  4

#define STOCK_STR_BUFFERS        6
#define STOCK_STR_LEN            24

enum StockColorMode {
    STOCK_COLOR_RED_UP   = 0,   // CN convention: red = gain (default)
    STOCK_COLOR_GREEN_UP = 1
};

enum StockPhase {
    STOCK_PHASE_CLOSED = 0,
    STOCK_PHASE_SESSION,      // 09:30 - 15:00
    STOCK_PHASE_POSTCLOSE,    // 15:00 - 16:00
    STOCK_PHASE_WEEKEND       // one sample from 09:00
};

struct StockIndex {
    char  code[16];       // "s_sh000001"
    char  label[8];       // short label for the panel, e.g. "SH"
    float price;          // index level
    float preClose;       // derived: price - changeAmt
    float changePct;
    float changeAmt;
    float volume;         // as received, see the unit note at the top
    float turnover;       // as received, in 万元
    bool  valid;
};

class StockManager {
public:
    StockManager();
    ~StockManager();

    void begin();

    // One GET, all three indices.
    bool fetchQuotes();

    // ---- settings ----
    // These are read from a RAM cache, not from NVS. getColorMode() is called
    // for every stock element on every frame, and a flash read per element at
    // 30fps is not something to put in the render path.
    bool           isEnabled();
    void           setEnabled(bool enabled);
    StockColorMode getColorMode();
    void           setColorMode(StockColorMode mode);
    void           toggleColorMode();

    // Re-reads every setting from NVS. Call after the setup page writes one.
    void reloadSettings();

    // ---- data access ----
    StockIndex getIndex(int slot);
    bool       hasData();
    bool       isStale();

    // ---- display helpers ----
    uint16_t    getChangeColor(float changePct);
    int16_t     getChangeColorIndex(int16_t displayIndex);
    const char *getStockStr(int16_t displayIndex);
    float       getChangePctFor(int16_t displayIndex);

    // ---- scheduling ----
    StockPhase getPhase();
    uint32_t   getQuoteIntervalMs();

    // Kept so the task loop has a single housekeeping hook. There are no
    // charts to expire any more, so this only rolls the day counter.
    void maintainSession();

    // Quota accounting, RAM only: writing a counter to NVS on every call would
    // burn flash for a number a reboot can safely lose.
    uint16_t getCallsToday();
    uint16_t getCallsRemaining();
    bool     hasBudgetFor(uint8_t calls);

private:
    StockIndex        _indices[STOCK_INDEX_COUNT];
    SemaphoreHandle_t _mutex;
    uint32_t          _lastOkMs;
    bool              _hasData;

    uint16_t          _callsToday;
    int16_t           _callDay;

    // Settings cache
    bool              _settingsLoaded;
    bool              _enabledCache;
    StockColorMode    _colorModeCache;

    char              _strBuf[STOCK_STR_BUFFERS][STOCK_STR_LEN];
    uint8_t           _strBufIndex;

    const char *codeForSlot(int slot);
    bool  applyRow(int slot, const String &row);
    bool  countCall();
    bool  lock(uint32_t timeoutMs);
    void  unlock();
    char *nextStrBuf();
    void  seedLabels();
    void  ensureSettings();
};

extern StockManager stockManager;

#endif // STOCK_H