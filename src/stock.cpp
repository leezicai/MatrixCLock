#include "stock.h"
#include "matrixData.h"
#include "matrixCore.h"
#include "net.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Global instance
StockManager stockManager;

// Request order matches the StockSlot enum, but the reply is matched by name
// rather than by position - Sina echoes the codes back in the URL order today,
// and relying on that would break silently if it ever stopped.
static const char *kSinaCodes[STOCK_INDEX_COUNT] = {
    STOCK_CODE_SH, STOCK_CODE_SZ, STOCK_CODE_CY};

// ---------------------------------------------------------------------------
// Body parsing
// ---------------------------------------------------------------------------

// Pulls the quoted payload out of one `var hq_str_<code>="...";` line.
// Returns false when the line is missing, and also when it is present but
// empty - Sina answers an unknown or suspended code with `=""`.
static bool extractRow(const String &body, const char *code, String &row) {
    String key = String(STOCK_VAR_PREFIX) + code;

    int keyPos = body.indexOf(key);
    if (keyPos < 0) {
        return false;
    }
    int start = body.indexOf('"', keyPos + (int)key.length());
    if (start < 0) {
        return false;
    }
    int end = body.indexOf('"', start + 1);
    if (end < 0 || end <= start + 1) {
        return false;
    }

    row = body.substring(start + 1, end);
    return true;
}

// Comma separated field by index. Field 0 is the GB18030 index name and is
// never read: everything this module uses sits in fields 1..5 and is plain
// ASCII, so the body needs no transcoding.
static bool csvField(const String &row, int index, String &out) {
    int start = 0;
    for (int i = 0; i < index; ++i) {
        start = row.indexOf(',', start);
        if (start < 0) {
            return false;
        }
        start++;
    }

    int end = row.indexOf(',', start);
    out = (end < 0) ? row.substring(start) : row.substring(start, end);
    out.trim();
    return out.length() > 0;
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

StockManager::StockManager()
    : _mutex(nullptr), _lastOkMs(0), _hasData(false),
      _callsToday(0), _callDay(-1),
      _settingsLoaded(false), _enabledCache(true),
      _colorModeCache(STOCK_COLOR_RED_UP), _strBufIndex(0) {
    memset(_indices, 0, sizeof(_indices));
    memset(_strBuf, 0, sizeof(_strBuf));
    seedLabels();
}

StockManager::~StockManager() {
    if (_mutex != nullptr) {
        vSemaphoreDelete(_mutex);
        _mutex = nullptr;
    }
}

// Codes and labels are static, so the display can render placeholders before
// the stock task has started.
void StockManager::seedLabels() {
    strncpy(_indices[STOCK_SH].code, STOCK_CODE_SH, sizeof(_indices[0].code) - 1);
    strncpy(_indices[STOCK_SZ].code, STOCK_CODE_SZ, sizeof(_indices[0].code) - 1);
    strncpy(_indices[STOCK_CY].code, STOCK_CODE_CY, sizeof(_indices[0].code) - 1);

    strncpy(_indices[STOCK_SH].label, "SH", sizeof(_indices[0].label) - 1);
    strncpy(_indices[STOCK_SZ].label, "SZ", sizeof(_indices[0].label) - 1);
    strncpy(_indices[STOCK_CY].label, "CY", sizeof(_indices[0].label) - 1);
}

void StockManager::begin() {
    if (_mutex == nullptr) {
        _mutex = xSemaphoreCreateMutex();
    }
    seedLabels();
    reloadSettings();

    Serial.printf("Stock manager initialized (Sina, no key): %s\n",
                  STOCK_QUOTE_URL);
}

bool StockManager::lock(uint32_t timeoutMs) {
    if (_mutex == nullptr) {
        return true;
    }
    return xSemaphoreTake(_mutex, timeoutMs / portTICK_PERIOD_MS) == pdTRUE;
}

void StockManager::unlock() {
    if (_mutex != nullptr) {
        xSemaphoreGive(_mutex);
    }
}

const char *StockManager::codeForSlot(int slot) {
    if (slot < 0 || slot >= STOCK_INDEX_COUNT) {
        return STOCK_CODE_SH;
    }
    return kSinaCodes[slot];
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Loads the cache on first use. begin() runs on the stock task well into the
// boot, but the renderer may ask for a colour before that, so this cannot
// wait for it.
void StockManager::ensureSettings() {
    if (!_settingsLoaded) {
        reloadSettings();
    }
}

void StockManager::reloadSettings() {
    _enabledCache = matrixDataManager.getStockEnabled();

    int mode = matrixDataManager.getStockColorMode();
    _colorModeCache = (mode == STOCK_COLOR_GREEN_UP) ? STOCK_COLOR_GREEN_UP
                                                     : STOCK_COLOR_RED_UP;

    _settingsLoaded = true;
}

bool StockManager::isEnabled() {
    ensureSettings();
    return _enabledCache;
}

void StockManager::setEnabled(bool enabled) {
    matrixDataManager.setStockEnabled(enabled);
    _enabledCache = enabled;
}

StockColorMode StockManager::getColorMode() {
    ensureSettings();
    return _colorModeCache;
}

void StockManager::setColorMode(StockColorMode mode) {
    matrixDataManager.setStockColorMode((int)mode);
    _colorModeCache = mode;
}

void StockManager::toggleColorMode() {
    setColorMode(getColorMode() == STOCK_COLOR_RED_UP ? STOCK_COLOR_GREEN_UP
                                                      : STOCK_COLOR_RED_UP);
}

// ---------------------------------------------------------------------------
// Quota accounting
// ---------------------------------------------------------------------------

uint16_t StockManager::getCallsToday() {
    return _callsToday;
}

uint16_t StockManager::getCallsRemaining() {
    if (_callsToday >= STOCK_DAILY_CALL_BUDGET) {
        return 0;
    }
    return (uint16_t)(STOCK_DAILY_CALL_BUDGET - _callsToday);
}

bool StockManager::hasBudgetFor(uint8_t calls) {
    return getCallsRemaining() >= calls;
}

// Rolls the counter at midnight and refuses once the ceiling is reached.
// Every outgoing request passes through here, so the limit lives in one place.
bool StockManager::countCall() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 20)) {
        if (_callDay != (int16_t)timeinfo.tm_yday) {
            _callDay    = (int16_t)timeinfo.tm_yday;
            _callsToday = 0;
            Serial.println("Stock: daily call counter reset");
        }
    }

    if (_callsToday >= STOCK_DAILY_CALL_BUDGET) {
        return false;
    }
    _callsToday++;
    return true;
}

// ---------------------------------------------------------------------------
// Networking
// ---------------------------------------------------------------------------

// Fills one slot from its CSV row. Returns false when the row is present but
// carries no usable level, which is what a halted or not-yet-open index looks
// like - the previous values stay on the panel rather than flashing to zero.
bool StockManager::applyRow(int slot, const String &row) {
    String levelStr;
    String amountStr;
    String pctStr;
    String volumeStr;
    String turnoverStr;

    if (!csvField(row, 1, levelStr)) {
        return false;
    }

    float price = levelStr.toFloat();
    if (price <= 0.0f) {
        return false;
    }

    float amount   = csvField(row, 2, amountStr)   ? amountStr.toFloat()   : 0.0f;
    float pct      = csvField(row, 3, pctStr)      ? pctStr.toFloat()      : 0.0f;
    float volume   = csvField(row, 4, volumeStr)   ? volumeStr.toFloat()   : 0.0f;
    float turnover = csvField(row, 5, turnoverStr) ? turnoverStr.toFloat() : 0.0f;

    float preClose = price - amount;
    // Sina already sends the percentage, so this only covers a row that
    // omitted it. Guarding the divisor keeps a zero previous close - which
    // happens on the very first print of a newly listed index - out of the
    // arithmetic.
    if (pct == 0.0f && preClose > 0.0f && amount != 0.0f) {
        pct = (amount / preClose) * 100.0f;
    }

    if (!lock(200)) {
        return false;
    }
    _indices[slot].price     = price;
    _indices[slot].preClose  = preClose;
    _indices[slot].changeAmt = amount;
    _indices[slot].changePct = pct;
    _indices[slot].volume    = volume;
    _indices[slot].turnover  = turnover;
    _indices[slot].valid     = true;
    unlock();

    return true;
}

bool StockManager::fetchQuotes() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Stock: skipped, WiFi down");
        return false;
    }
    if (!countCall()) {
        Serial.println("Stock: daily budget spent, request skipped");
        return false;
    }
    if (ESP.getFreeHeap() < STOCK_MIN_FREE_HEAP) {
        Serial.printf("Stock: skipped, free heap %u below floor\n",
                      (unsigned)ESP.getFreeHeap());
        return false;
    }

    // Wake the radio for the exchange. Modem sleep parks the receiver between
    // beacons, which adds a beacon interval of latency to every packet.
    setWiFiFullPower();

    // A local, not a static: a client that has seen a mid-transfer reset keeps
    // a half-torn-down socket, and the next request then fails in a way that
    // looks nothing like the original error.
    WiFiClient client;
    client.setTimeout(STOCK_HTTP_TIMEOUT_MS / 1000);

    HTTPClient http;
    if (!http.begin(client, STOCK_QUOTE_URL)) {
        client.stop();
        setWiFiLowPower();
        Serial.println("Stock: http.begin failed");
        return false;
    }
    http.setConnectTimeout(STOCK_HTTP_TIMEOUT_MS);
    http.setTimeout(STOCK_HTTP_TIMEOUT_MS);
    http.setReuse(false);
    // Both headers are load bearing. Without the Referer the endpoint answers
    // 403; without a user agent some edges drop the request outright.
    http.addHeader("Referer", STOCK_SINA_REFERER);
    http.setUserAgent(STOCK_SINA_USER_AGENT);

    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        // Negative values are transport errors rather than HTTP statuses:
        // -1 refused, -5 connection lost, -11 read timeout.
        Serial.printf("Stock: GET %d (%s)\n", code,
                      http.errorToString(code).c_str());
        http.end();
        client.stop();
        setWiFiLowPower();
        return false;
    }

    String payload = http.getString();
    http.end();
    client.stop();
    setWiFiLowPower();

    if (payload.length() == 0) {
        Serial.println("Stock: empty body");
        return false;
    }
    if (payload.length() > STOCK_BODY_MAX_LEN) {
        // Three index rows are ~220 bytes. Anything this far over is not the
        // quote feed - an interstitial or an error page - and parsing it would
        // only produce confusing results.
        Serial.printf("Stock: body %u bytes, not a quote feed\n",
                      (unsigned)payload.length());
        return false;
    }

    int updated = 0;
    for (int slot = 0; slot < STOCK_INDEX_COUNT; ++slot) {
        String row;
        if (!extractRow(payload, kSinaCodes[slot], row)) {
            continue;
        }
        if (!applyRow(slot, row)) {
            continue;
        }

        updated++;
        Serial.printf("Stock: %s %.2f %+.2f %+.2f%% vol=%.0f amt=%.0f\n",
                      _indices[slot].label, _indices[slot].price,
                      _indices[slot].changeAmt, _indices[slot].changePct,
                      _indices[slot].volume, _indices[slot].turnover);
    }

    if (updated == 0) {
        Serial.printf("Stock: no index row parsed, body starts: %s\n",
                      payload.substring(0, 120).c_str());
        return false;
    }

    _hasData  = true;
    _lastOkMs = millis();
    Serial.printf("Stock: %d/%d quotes updated (call %d)\n",
                  updated, STOCK_INDEX_COUNT, _callsToday);
    return true;
}

// ---------------------------------------------------------------------------
// Data access
// ---------------------------------------------------------------------------

StockIndex StockManager::getIndex(int slot) {
    StockIndex copy;
    memset(&copy, 0, sizeof(copy));

    if (slot < 0 || slot >= STOCK_INDEX_COUNT) {
        return copy;
    }
    if (!lock(50)) {
        return copy;
    }
    copy = _indices[slot];
    unlock();
    return copy;
}

bool StockManager::hasData() {
    return _hasData;
}

bool StockManager::isStale() {
    if (!_hasData) {
        return true;
    }
    return (uint32_t)(millis() - _lastOkMs) > STOCK_STALE_MS;
}

// ---------------------------------------------------------------------------
// Display layer bridge
// ---------------------------------------------------------------------------

char *StockManager::nextStrBuf() {
    char *buf = _strBuf[_strBufIndex];
    _strBufIndex = (uint8_t)((_strBufIndex + 1) % STOCK_STR_BUFFERS);
    buf[0] = '\0';
    return buf;
}

float StockManager::getChangePctFor(int16_t displayIndex) {
    StockIndex idx = getIndex(STOCK_SLOT_OF(displayIndex));
    return idx.valid ? idx.changePct : 0.0f;
}

uint16_t StockManager::getChangeColor(float changePct) {
    StockColorMode mode = getColorMode();
    if (changePct > 0.0001f) {
        return matrixColorManager.getColor(
            mode == STOCK_COLOR_RED_UP ? COLOR_IDX(COLOR_RED)
                                       : COLOR_IDX(COLOR_GREEN));
    }
    if (changePct < -0.0001f) {
        return matrixColorManager.getColor(
            mode == STOCK_COLOR_RED_UP ? COLOR_IDX(COLOR_GREEN)
                                       : COLOR_IDX(COLOR_RED));
    }
    return matrixColorManager.getColor(COLOR_IDX(COLOR_LIGHT_GRAY));
}

int16_t StockManager::getChangeColorIndex(int16_t displayIndex) {
    StockIndex idx = getIndex(STOCK_SLOT_OF(displayIndex));
    if (!idx.valid) {
        return COLOR_IDX(COLOR_GRAY);
    }

    StockColorMode mode = getColorMode();
    if (idx.changePct > 0.0001f) {
        return mode == STOCK_COLOR_RED_UP ? COLOR_IDX(COLOR_RED)
                                          : COLOR_IDX(COLOR_GREEN);
    }
    if (idx.changePct < -0.0001f) {
        return mode == STOCK_COLOR_RED_UP ? COLOR_IDX(COLOR_GREEN)
                                          : COLOR_IDX(COLOR_RED);
    }
    return COLOR_IDX(COLOR_LIGHT_GRAY);
}

const char *StockManager::getStockStr(int16_t displayIndex) {
    StockIndex idx = getIndex(STOCK_SLOT_OF(displayIndex));
    char *buf = nextStrBuf();

    switch (STOCK_FIELD_OF(displayIndex)) {
    case STOCK_FIELD_PRICE:
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%.2f", idx.price);
        } else {
            snprintf(buf, STOCK_STR_LEN, "-------");
        }
        break;

    case STOCK_FIELD_PCT:
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%+.2f%%", idx.changePct);
        } else {
            snprintf(buf, STOCK_STR_LEN, "--.--%%");
        }
        break;

    case STOCK_FIELD_AMOUNT:
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%+.2f", idx.changeAmt);
        } else {
            snprintf(buf, STOCK_STR_LEN, "--.--");
        }
        break;

    case STOCK_FIELD_LABEL:
        snprintf(buf, STOCK_STR_LEN, "%s", idx.label);
        break;

    case STOCK_FIELD_LABEL_PRICE:
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%s %.2f", idx.label, idx.price);
        } else {
            snprintf(buf, STOCK_STR_LEN, "%s -------", idx.label);
        }
        break;

    case STOCK_FIELD_VOLUME:
        // Printed exactly as received. The two exchanges do not agree on the
        // unit in this feed, so scaling it here would be inventing precision.
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%.0f", idx.volume);
        } else {
            snprintf(buf, STOCK_STR_LEN, "--------");
        }
        break;

    case STOCK_FIELD_TURNOVER:
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%.0f", idx.turnover);
        } else {
            snprintf(buf, STOCK_STR_LEN, "--------");
        }
        break;

    case STOCK_FIELD_LABEL_INT:
        // Drops the decimals so a label plus value still fits a 64px row.
        if (idx.valid) {
            snprintf(buf, STOCK_STR_LEN, "%s %d", idx.label,
                     (int)(idx.price + 0.5f));
        } else {
            snprintf(buf, STOCK_STR_LEN, "%s ----", idx.label);
        }
        break;

    default:
        snprintf(buf, STOCK_STR_LEN, "?");
        break;
    }

    return buf;
}

// ---------------------------------------------------------------------------
// Scheduling
// ---------------------------------------------------------------------------

StockPhase StockManager::getPhase() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 20)) {
        return STOCK_PHASE_CLOSED;   // no clock yet, do not guess
    }

    bool weekend = (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6);
    int  minutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;

    if (weekend) {
        // One refresh in the morning is enough: nothing can change until
        // Monday, and it keeps the last trading day on screen.
        return minutes >= STOCK_MIN_WEEKEND_SAMPLE ? STOCK_PHASE_WEEKEND
                                                   : STOCK_PHASE_CLOSED;
    }

    // The 11:30-13:00 break is deliberately inside the session window: the
    // feed keeps serving the morning close through lunch, and treating the
    // break as closed would only change how often that same value is fetched.
    if (minutes >= STOCK_MIN_AM_OPEN && minutes < STOCK_MIN_PM_CLOSE) {
        return STOCK_PHASE_SESSION;
    }
    if (minutes >= STOCK_MIN_PM_CLOSE && minutes < STOCK_MIN_POSTCLOSE_END) {
        return STOCK_PHASE_POSTCLOSE;
    }
    return STOCK_PHASE_CLOSED;
}

// Nothing expires on its own any more - the quote simply gets overwritten by
// the next fetch. Kept as the task loop's housekeeping hook so the day
// counter still rolls even on a day with no successful request.
void StockManager::maintainSession() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 20)) {
        return;
    }
    if (_callDay != (int16_t)timeinfo.tm_yday) {
        _callDay    = (int16_t)timeinfo.tm_yday;
        _callsToday = 0;
    }
}

uint32_t StockManager::getQuoteIntervalMs() {
    StockPhase phase = getPhase();

    if (phase == STOCK_PHASE_POSTCLOSE) {
        return STOCK_POSTCLOSE_MS;
    }
    if (phase != STOCK_PHASE_SESSION) {
        return STOCK_QUOTE_MS_IDLE;
    }

    // Nothing on screen means nothing worth refreshing at full speed.
    if (matrixCoreManager.getCurrentPageIndex() != STOCK_PAGE_INDEX) {
        return STOCK_QUOTE_MS_IDLE;
    }
    return STOCK_QUOTE_MS_SESSION;
}