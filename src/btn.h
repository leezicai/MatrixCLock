#ifndef BTN_H
#define BTN_H

#include <OneButton.h>
#include "common_define.h"

// Button pins
// ∫#define BUTTON1_PIN 46
// #define BUTTON2_PIN 16
// #define BUTTON3_PIN 15

// Button timing parameters (in milliseconds)
#define DEBOUNCE_TIME_MS 50      // Time for debounce
#define CLICK_TIME_MS 600        // Time to detect a single click
#define DOUBLE_CLICK_TIME_MS 300 // Time between clicks for double click detection
#define LONG_PRESS_TIME_MS 1200  // Time to detect a long press

class ButtonManager {
private:
    OneButton button1;
    OneButton button2;
    OneButton button3;

    // Callback functions prototypes
    static void handleButton1Click();
    static void handleButton1DoubleClick();
    static void handleButton1LongPressStart();
    static void handleButton1LongPress();
    static void handleButton1LongPressStop();

    static void handleButton2Click();
    static void handleButton2DoubleClick();
    static void handleButton2LongPressStart();
    static void handleButton2LongPress();
    static void handleButton2LongPressStop();

    static void handleButton3Click();
    static void handleButton3DoubleClick();
    static void handleButton3LongPressStart();
    static void handleButton3LongPress();
    static void handleButton3LongPressStop();

public:
    // Constructor
    ButtonManager();

    // Initialize buttons
    void begin();

    // Update all buttons (to be called in the loop)
    void tick();
};

extern ButtonManager buttonManager;

#endif // BTN_H