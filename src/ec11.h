#ifndef EC11_H
#define EC11_H
#include <Arduino.h>
#include <AiEsp32RotaryEncoder.h>

// Pins definition for EC11
#define ROTARY_ENCODER_A_PIN 35
#define ROTARY_ENCODER_B_PIN 45
#define ROTARY_ENCODER_BUTTON_PIN 48
#define ROTARY_ENCODER_VCC_PIN -1  // Set to -1 if directly connected to 3.3V
#define ROTARY_ENCODER_STEPS 1     // 默认步数为1

// Forward declare ISR function
void IRAM_ATTR readEncoderISR();

class EC11 {
private:
    // Pins for EC11 rotary encoder
    int pinA;
    int pinB;
    int buttonPin;
    int vccPin;
    int encoderSteps;  // 步数作为成员变量
    
    // Instance of rotary encoder
    AiEsp32RotaryEncoder* rotaryEncoder;
    
    // Button state tracking - renamed isButtonPressedState to avoid conflict
    bool isButtonPressedState;  // True when button is physically pressed
    bool buttonPressedPrevious;
    unsigned long buttonPressTime;
    unsigned long buttonReleaseTime;
    bool buttonLongPressDetected;
    bool buttonClickDetected;
    bool buttonDoubleClickDetected;
    unsigned long lastClickTime;
    
    // Rotation detection flags
    bool clockwiseRotation;
    bool counterClockwiseRotation;
    bool pressedClockwiseRotation;
    bool pressedCounterClockwiseRotation;
    
    // Previous state for button press + rotation detection
    int32_t lastPosition;
    
    // Configuration
    bool circleValues;
    int16_t minValue;
    int16_t maxValue;
    
    // Button timing configuration
    unsigned long debounceTime;
    unsigned long longPressTime;
    unsigned long doubleClickTime;
    
    // Singleton instance
    static EC11* instance;
    
    // Private constructor for singleton pattern
    EC11(int pinA, int pinB, int buttonPin, int vccPin = -1, int encoderSteps = ROTARY_ENCODER_STEPS);
    
    // Private helper methods for button handling
    bool isButtonPressed();
    void updateButtonState();
    
public:
    // Make rotaryEncoder accessible to ISR function
    friend void IRAM_ATTR readEncoderISR();
    
    // Destructor
    ~EC11();
    
    // Get singleton instance
    static EC11* getInstance(int pinA = -1, int pinB = -1, int buttonPin = -1, int vccPin = -1, int encoderSteps = ROTARY_ENCODER_STEPS);
    
    // Setup method
    void begin(bool circleValues = false, int16_t minValue = 0, int16_t maxValue = 1000);
    
    // Set button timing parameters
    void setButtonTiming(unsigned long debounceMs = 50, 
                         unsigned long longPressMs = 1000, 
                         unsigned long doubleClickMs = 400);
    
    // Update method - should be called regularly in loop()
    void update();
    
    // Event detection methods
    bool rotatedClockwise();
    bool rotatedCounterClockwise();
    bool buttonPressed();
    bool buttonClicked();
    bool buttonDoubleClicked();
    bool buttonLongPressed();
    bool buttonPressedAndRotatedClockwise();
    bool buttonPressedAndRotatedCounterClockwise();
    
    // Utility methods
    int32_t getPosition();
    void setPosition(int32_t position);
    void reset();
};

// Global instance accessor function
EC11* EC11Instance();

#endif // EC11_H