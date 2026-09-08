#include "glyphBitmap.h"

GlyphBitmap::GlyphBitmap()
    : canvas_(nullptr), cachedFont_(nullptr), cachedChar_(0), w_(0), h_(0),
      ascent_(0) {
}

GlyphBitmap::~GlyphBitmap() {
    if (canvas_ != nullptr) {
        delete canvas_;
        canvas_ = nullptr;
    }
}

// Lazily allocated so a device that never selects a pixel animation pays
// nothing for it.
// 懒加载：不用逐像素特效的设备完全不占这份内存。
bool GlyphBitmap::ensureCanvas() {
    if (canvas_ != nullptr) {
        return true;
    }
    canvas_ = new GFXcanvas1(GLYPH_BITMAP_MAX_W, GLYPH_BITMAP_MAX_H);
    if (canvas_ == nullptr || canvas_->getBuffer() == nullptr) {
        if (canvas_ != nullptr) {
            delete canvas_;
            canvas_ = nullptr;
        }
        return false;
    }
    u8g2_.begin(*canvas_);
    return true;
}

bool GlyphBitmap::render(const uint8_t *font, char ch) {
    if (!ensureCanvas()) {
        return false;
    }

    // Cache hit - the canvas already holds this glyph.
    if (font == cachedFont_ && ch == cachedChar_ && w_ > 0) {
        return true;
    }

    // Invalidate first: any early return below must not leave a stale cache.
    cachedFont_ = nullptr;
    cachedChar_ = 0;

    u8g2_.setFont(font);
    const int16_t ascent = u8g2_.getFontAscent();
    const int16_t descent = u8g2_.getFontDescent();
    const int16_t h = ascent - descent;

    char one[2] = {ch, '\0'};
    const int16_t w = u8g2_.getUTF8Width(one);

    if (w <= 0 || h <= 0 || w > GLYPH_BITMAP_MAX_W || h > GLYPH_BITMAP_MAX_H) {
        w_ = 0;
        h_ = 0;
        return false;
    }

    canvas_->fillScreen(0);
    u8g2_.setFontMode(1);         // transparent - keep the cleared background
    u8g2_.setFontDirection(0);
    u8g2_.setForegroundColor(1);  // 1bpp canvas: 1 means lit
    u8g2_.setCursor(0, ascent);   // glyph top edge lands on canvas row 0
    u8g2_.print(ch);

    w_ = w;
    h_ = h;
    ascent_ = ascent;
    cachedFont_ = font;
    cachedChar_ = ch;
    return true;
}

bool GlyphBitmap::rowSpanLit(int16_t x, int16_t yStart, int16_t yEnd) const {
    if (yStart < 0) {
        yStart = 0;
    }
    if (yEnd > h_) {
        yEnd = h_;
    }
    for (int16_t y = yStart; y < yEnd; ++y) {
        if (pixelAt(x, y)) {
            return true;
        }
    }
    return false;
}