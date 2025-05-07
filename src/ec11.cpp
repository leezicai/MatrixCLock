#include "ec11.h"

// Initialize static instance pointer
EC11* EC11::instance = nullptr;

// Constructor (private for singleton pattern)
EC11::EC11(int pinA, int pinB, int buttonPin, int vccPin, int encoderSteps) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->buttonPin = buttonPin;
    this->vccPin = vccPin;
    this->encoderSteps = encoderSteps;  // 保存步数
    
    // Initialize rotary encoder instance - 现在将步数作为构造函数参数传入
    rotaryEncoder = new AiEsp32RotaryEncoder(pinA, pinB, buttonPin, vccPin, encoderSteps);
    
    // Initialize button state variables
    isButtonPressedState = false;
    buttonPressedPrevious = false;
    buttonPressTime = 0;
    buttonReleaseTime = 0;
    buttonLongPressDetected = false;
    buttonClickDetected = false;
    buttonDoubleClickDetected = false;
    lastClickTime = 0;
    
    // Set default button timing
    debounceTime = 50;   // 50ms debounce
    longPressTime = 1000; // 1s for long press
    doubleClickTime = 400; // 400ms for double click
    
    lastPosition = 0;
    
    // Flag for rotation detection
    clockwiseRotation = false;
    counterClockwiseRotation = false;
    pressedClockwiseRotation = false;
    pressedCounterClockwiseRotation = false;
}

// Method to check if button is physically pressed
bool EC11::isButtonPressed() {
    // Read the physical button state (assuming LOW means pressed)
    // Adjust logic based on your button setup (pullup/pulldown)
    return digitalRead(buttonPin) == LOW;
}

// Update button state
void EC11::updateButtonState() {
    bool currentState = isButtonPressed();
    unsigned long currentTime = millis();
    
    // Button press detection
    if (currentState && !buttonPressedPrevious) {
        // Button just pressed
        isButtonPressedState = true;
        buttonPressTime = currentTime;
        buttonLongPressDetected = false;
        
        // Debug
        Serial.println("Button Physical Press Detected");
    }
    // Button release detection
    else if (!currentState && buttonPressedPrevious) {
        // Button just released
        isButtonPressedState = false;
        buttonReleaseTime = currentTime;
        
        // Debug
        Serial.println("Button Physical Release Detected");
        
        // Long press already detected
        if (buttonLongPressDetected) {
            buttonLongPressDetected = false;
        }
        // Click detected - after debounce time but before long press time
        else if ((currentTime - buttonPressTime) >= debounceTime && 
                 (currentTime - buttonPressTime) < longPressTime) {
            
            // Detect double-click
            if ((currentTime - lastClickTime) <= doubleClickTime) {
                buttonDoubleClickDetected = true;
                buttonClickDetected = false;  // Consume single click
                Serial.println("Double Click Detected (internal)");
            } else {
                buttonClickDetected = true;
                Serial.println("Click Detected (internal)");
            }
            
            lastClickTime = currentTime;
        }
    }
    // Check for long press while button is held
    else if (currentState && !buttonLongPressDetected && 
             (currentTime - buttonPressTime) >= longPressTime) {
        buttonLongPressDetected = true;
        Serial.println("Long Press Detected (internal)");
    }
    
    buttonPressedPrevious = currentState;
}

// Destructor
EC11::~EC11() {
    delete rotaryEncoder;
}

// Singleton accessor method - 现在也接受步数参数
EC11* EC11::getInstance(int pinA, int pinB, int buttonPin, int vccPin, int encoderSteps) {
    if (instance == nullptr) {
        // Make sure all pins are provided on first call
        if (pinA < 0 || pinB < 0 || buttonPin < 0) {
            Serial.println("Error: Pins must be specified when creating EC11 instance for the first time");
            return nullptr;
        }
        instance = new EC11(pinA, pinB, buttonPin, vccPin, encoderSteps);
    }
    return instance;
}

// Global function to access the singleton instance
EC11* EC11Instance() {
    return EC11::getInstance();
}

// ISR for encoder - correctly defined to match what AiEsp32RotaryEncoder expects
void IRAM_ATTR readEncoderISR() {
    // Get the instance and call the rotaryEncoder's ISR
    EC11* instance = EC11::getInstance();
    if (instance != nullptr && instance->rotaryEncoder != nullptr) {
        instance->rotaryEncoder->readEncoder_ISR();
    }
}

// Setup method - 移除了步数参数
void EC11::begin(bool circleValues, int16_t minValue, int16_t maxValue) {
    // Set config values
    this->circleValues = circleValues;
    this->minValue = minValue;
    this->maxValue = maxValue;
    
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    // Begin encoder
    rotaryEncoder->begin();
    
    // Set up encoder configuration
    rotaryEncoder->setEncoderValue(0);
    rotaryEncoder->setBoundaries(minValue, maxValue, circleValues);
    rotaryEncoder->setup(readEncoderISR); // Use the global ISR function
    rotaryEncoder->setAcceleration(0);  // No acceleration
    
    // Set up button pin as input with pull-up
    pinMode(buttonPin, INPUT_PULLUP);
    
    // Debug
    Serial.println("EC11 initialized");
}

// Set button timing parameters
void EC11::setButtonTiming(unsigned long debounceMs, unsigned long longPressMs, unsigned long doubleClickMs) {
    this->debounceTime = debounceMs;
    this->longPressTime = longPressMs;
    this->doubleClickTime = doubleClickMs;
}

// Update method - should be called regularly in loop()
void EC11::update() {
    // First update button states
    updateButtonState();
    
    // Check for encoder rotation
    if (rotaryEncoder->encoderChanged()) {
        int32_t currentPosition = rotaryEncoder->readEncoder();
        int32_t lastPosValue = lastPosition;  // Store for comparison
        
        // Update last position
        lastPosition = currentPosition;
        
        // Check rotation direction
        if (currentPosition > lastPosValue) {
            // Clockwise rotation
            if (isButtonPressedState) {
                pressedClockwiseRotation = true;
                Serial.println("Pressed Clockwise rotation detected (internal)");
            } else {
                clockwiseRotation = true;
                Serial.println("Clockwise rotation detected (internal)");
            }
        } else if (currentPosition < lastPosValue) {
            // Counter-clockwise rotation
            if (isButtonPressedState) {
                pressedCounterClockwiseRotation = true;
                Serial.println("Pressed Counter-clockwise rotation detected (internal)");
            } else {
                counterClockwiseRotation = true;
                Serial.println("Counter-clockwise rotation detected (internal)");
            }
        }
    }
}

// Check if rotated clockwise
bool EC11::rotatedClockwise() {
    if (clockwiseRotation) {
        clockwiseRotation = false;  // Reset the flag
        Serial.println("Method 1: Rotated Clockwise");
        return true;
    }
    return false;
}

// Check if rotated counter-clockwise
bool EC11::rotatedCounterClockwise() {
    if (counterClockwiseRotation) {
        counterClockwiseRotation = false;  // Reset the flag
        Serial.println("Method 2: Rotated Counter-Clockwise");
        return true;
    }
    return false;
}

// Check if button is pressed
bool EC11::buttonPressed() {
    static bool lastIsButtonPressed = false;
    bool currentIsButtonPressed = isButtonPressedState;
    
    // Button press transition (from not pressed to pressed)
    if (currentIsButtonPressed && !lastIsButtonPressed) {
        lastIsButtonPressed = currentIsButtonPressed;
        Serial.println("Method 3: Button Pressed");
        return true;
    }
    
    lastIsButtonPressed = currentIsButtonPressed;
    return false;
}

// Check if button is clicked (short press)
bool EC11::buttonClicked() {
    if (buttonClickDetected) {
        buttonClickDetected = false;  // Reset the flag
        Serial.println("Method 4: Button Clicked");
        return true;
    }
    return false;
}

// Check if button is double clicked
bool EC11::buttonDoubleClicked() {
    if (buttonDoubleClickDetected) {
        buttonDoubleClickDetected = false;  // Reset the flag
        Serial.println("Method 5: Button Double Clicked");
        return true;
    }
    return false;
}

// Check if button is long pressed
bool EC11::buttonLongPressed() {
    static bool reportedLongPress = false;
    
    if (buttonLongPressDetected && !reportedLongPress) {
        reportedLongPress = true;
        Serial.println("Method 6: Button Long Pressed");
        return true;
    } else if (!buttonLongPressDetected) {
        reportedLongPress = false;
    }
    
    return false;
}

// Check if button is pressed and rotated clockwise
bool EC11::buttonPressedAndRotatedClockwise() {
    if (pressedClockwiseRotation) {
        pressedClockwiseRotation = false;  // Reset the flag
        Serial.println("Method 7: Button Pressed and Rotated Clockwise");
        return true;
    }
    return false;
}

// Check if button is pressed and rotated counter-clockwise
bool EC11::buttonPressedAndRotatedCounterClockwise() {
    if (pressedCounterClockwiseRotation) {
        pressedCounterClockwiseRotation = false;  // Reset the flag
        Serial.println("Method 8: Button Pressed and Rotated Counter-Clockwise");
        return true;
    }
    return false;
}

// Get current position value
int32_t EC11::getPosition() {
    return rotaryEncoder->readEncoder();
}

// Set position value
void EC11::setPosition(int32_t position) {
    rotaryEncoder->setEncoderValue(position);
    lastPosition = position;
}

// Reset position to zero
void EC11::reset() {
    rotaryEncoder->setEncoderValue(0);
    lastPosition = 0;
}