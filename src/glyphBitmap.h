#ifndef GLYPH_BITMAP_H
#define GLYPH_BITMAP_H

#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>

// Largest glyph the off-screen rasteriser can hold.
// One canvas costs ((W + 7) / 8) * H bytes -> 48 x 64 = 384 bytes.
#define GLYPH_BITMAP_MAX_W 48
#define GLYPH_BITMAP_MAX_H 64

/**
 * @brief Off-screen 1bpp rasteriser for a single u8g2 glyph.
 *
 * u8g2 can only print a glyph onto a GFX target - it cannot report which
 * pixels are lit. Any per-pixel transition (flip / dissolve / scanline /
 * particles) needs that information, so the glyph is first drawn into a small
 * private GFXcanvas1 and then read back pixel by pixel.
 *
 * u8g2 字库只能"打印"字形，拿不到点阵数据。翻页/溶解/扫描线/粒子这类
 * 逐像素特效都需要先把字形离屏渲染到 1bpp 画布上，再逐点读回。
 */
class GlyphBitmap {
public:
    GlyphBitmap();
    ~GlyphBitmap();

    /**
     * @brief Rasterise one character into the private canvas.
     * @return false when the canvas could not be allocated or the glyph does
     *         not fit - the caller must then fall back to a plain u8g2 print.
     */
    bool render(const uint8_t *font, char ch);

    /** @brief Is this glyph pixel lit? Out-of-range reads return false. */
    inline bool pixelAt(int16_t x, int16_t y) const {
        if (canvas_ == nullptr) {
            return false;
        }
        if (x < 0 || y < 0 || x >= w_ || y >= h_) {
            return false;
        }
        return canvas_->getPixel(x, y);
    }

    /**
     * @brief True when any pixel in column x between rows [yStart, yEnd) is lit.
     *
     * Used as a max filter when the glyph is squashed: without it thin strokes
     * disappear as soon as several source rows collapse into one output row.
     * 竖向压缩时用它做"最大值采样"，否则细笔画会被采样丢掉。
     */
    bool rowSpanLit(int16_t x, int16_t yStart, int16_t yEnd) const;

    int16_t width() const { return w_; }
    int16_t height() const { return h_; }
    int16_t ascent() const { return ascent_; }

private:
    bool ensureCanvas();

    GFXcanvas1 *canvas_;
    U8G2_FOR_ADAFRUIT_GFX u8g2_;  // bound to canvas_, never to the panel

    // Skip re-rasterising when the same glyph is requested again.
    const uint8_t *cachedFont_;
    char cachedChar_;

    int16_t w_;
    int16_t h_;
    int16_t ascent_;
};

#endif // GLYPH_BITMAP_H