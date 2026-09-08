#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>
#include <cstdint>

#define ANIMATION_0 0  // static
#define ANIMATION_1 1  // vertical roll
#define ANIMATION_2 2  // cross fade
#define ANIMATION_3 3  // CRT scanline wipe   (per-pixel)
#define ANIMATION_4 4  // dissolve / noise    (per-pixel)
#define ANIMATION_5 5  // particle burst      (per-pixel)

// ---- Transition windows inside the one-second tick, in milliseconds ----
// elapsed comes from main.cpp and runs 0 .. ~1000 for every second.
// Duration = END - START. A longer window means a slower transition; for the
// per-pixel types it also means more frames on the expensive path, so watch
// frame time when widening those.
// 一秒周期内的动画时间窗，时长 = END - START。窗口越长动画越慢；
// 逐像素类型还会占用更多昂贵帧，调宽时留意帧时间。

// ANIMATION_1 - vertical roll 上下滚动 (300ms)
// Outside the window this type draws nowStr, which is already the new value
// once the second has ticked, so ending at 999 is harmless here.
// 窗口外画的是 nowStr（进位后已是新值），所以结束点贴着 999 没问题。
#define ROLL_START_MS      699
#define ROLL_END_MS        999

// ANIMATION_2 - cross fade 淡出淡入 (650ms, split at FADE_MID_MS)
// START..MID fades the old value out, MID..END fades the new one in.
#define FADE_START_MS      350
#define FADE_MID_MS        650
#define FADE_END_MS       1000

// ANIMATION_3 / 4 / 5 - per-pixel types 逐像素类型
// These end at 980 rather than 1000 on purpose: they draw the incoming glyph
// once progress reaches 1, so the window has to close before the second ticks
// or the settle frame would show the wrong digit.
// 这三个结束点是 980 而不是 1000：进度到 1 时画的是即将到来的字形，
// 窗口必须在秒进位之前关掉，否则落定的那一帧会显示错误数字。
#define SCAN_START_MS      350   // 630ms
#define SCAN_END_MS        980
#define DISSOLVE_START_MS  530   // 450ms
#define DISSOLVE_END_MS    980
#define PARTICLE_START_MS  480   // 500ms
#define PARTICLE_END_MS    980

// ---- Scanline tuning 扫描线参数 ----
// How far the beam row is mixed toward white (0 = base colour, 1 = white).
#define SCAN_LINE_MIX      0.60f
// Rows just below the beam that get sheared sideways by one pixel.
// Set to 0 to disable the tearing.
#define SCAN_TEAR_ROWS     3

// ---- Dissolve tuning 溶解参数 ----
// Width of the bright "noise front" around the current threshold, as a
// fraction of the whole transition. Larger = a wider band of sparkle.
#define DISSOLVE_FRONT_BAND 0.12f
#define DISSOLVE_FRONT_MIX  0.55f

// ---- Particle tuning 粒子参数 ----
// Radial speed: how far a pixel travels relative to its distance from the
// glyph centre. 0 = no spread, 1 = doubles its radius by the end.
#define PARTICLE_SPREAD    1.15f
// Random sideways scatter in pixels, on top of the radial motion.
#define PARTICLE_JITTER    3.0f
// Downward drift in pixels over the whole burst.
#define PARTICLE_GRAVITY   5.0f
// Length of the out / in phases as a fraction of the window. Above 0.5 the
// two phases overlap so the character is never fully blank.
#define PARTICLE_PHASE     0.70f

class AnimationManager {
private:
    std::vector<int16_t> animationTypes;
public:
    // Constructor
    AnimationManager();
    
    // Initialize the animation types vector
    void init();
    
    // Get next animation type based on current index
    int16_t nextAnimationType(int16_t index);
    
    // Get previous animation type based on current index
    int16_t preAnimationType(int16_t index);
    
    // Get the size of animation types vector
    size_t getSize() const;
};

// Global instance declaration for external use
extern AnimationManager animationManager;

#endif // ANIMATION_H