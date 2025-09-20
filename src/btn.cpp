#include "btn.h"
#include <Arduino.h>
#include "data.h"
#include "ds3231.h"
#include "page.h"

// Create a global instance
ButtonManager buttonManager;

// Constructor
ButtonManager::ButtonManager() : 
    button1(BUTTON1_PIN, true), // true = INPUT_PULLUP
    button2(BUTTON2_PIN, true),
    button3(BUTTON3_PIN, true) {
}

void ButtonManager::begin() {
    // Configure timing parameters for button1
    button1.setDebounceMs(DEBOUNCE_TIME_MS);
    button1.setClickMs(CLICK_TIME_MS);
    button1.setPressMs(LONG_PRESS_TIME_MS);
    // Configure timing parameters for button2
    button2.setDebounceMs(DEBOUNCE_TIME_MS);
    button2.setClickMs(CLICK_TIME_MS);
    button2.setPressMs(LONG_PRESS_TIME_MS);
    // Configure timing parameters for button3
    button3.setDebounceMs(DEBOUNCE_TIME_MS);
    button3.setClickMs(CLICK_TIME_MS);
    button3.setPressMs(LONG_PRESS_TIME_MS);

    // Attach callback functions for button1
    button1.attachClick(handleButton1Click);
    button1.attachDoubleClick(handleButton1DoubleClick);
    button1.attachLongPressStart(handleButton1LongPressStart);
    button1.attachDuringLongPress(handleButton1LongPress);
    button1.attachLongPressStop(handleButton1LongPressStop);

    // Attach callback functions for button2
    button2.attachClick(handleButton2Click);
    button2.attachDoubleClick(handleButton2DoubleClick);
    button2.attachLongPressStart(handleButton2LongPressStart);
    button2.attachDuringLongPress(handleButton2LongPress);
    button2.attachLongPressStop(handleButton2LongPressStop);

    // Attach callback functions for button3
    button3.attachClick(handleButton3Click);
    button3.attachDoubleClick(handleButton3DoubleClick);
    button3.attachLongPressStart(handleButton3LongPressStart);
    button3.attachDuringLongPress(handleButton3LongPress);
    button3.attachLongPressStop(handleButton3LongPressStop);
}

// Update all buttons (to be called in the loop)
void ButtonManager::tick() {
    // Check all buttons in one function call
    button1.tick();
    button2.tick();
    button3.tick();
}

// Button 1 callbacks
void ButtonManager::handleButton1Click() {
    matrixCoreManager.nextPrimaryPage();
  page.increaseFirstClassPage();
  Serial.println(appData.getWifiConfigured());
  Serial.println("Button 1 clicked");
  // Your code for button 1 click here
}

void ButtonManager::handleButton1DoubleClick() {
    matrixCoreManager.nextPrimaryPage();
  page.decreaseFirstClassPage();
  Serial.println("Button 1 double-clicked");
  // Your code for button 1 double-click here
}

void ButtonManager::handleButton1LongPressStart() {
    Serial.println(appData.getWifiConfigured());
    appData.setWifiConfigured(!appData.getWifiConfigured());
    Serial.println(appData.getWifiConfigured());
    ESP.restart();
    Serial.println("Button 1 long press started");
    // Your code for button 1 long press start here
}

void ButtonManager::handleButton1LongPress() {
    Serial.println("Button 1 long press ongoing");
    // Your code for button 1 during long press here
}

void ButtonManager::handleButton1LongPressStop() {
    Serial.println("Button 1 long press stopped");
    // Your code for button 1 long press stop here
}

// Button 2 callbacks
void ButtonManager::handleButton2Click() {
    Serial.println("Button 2 clicked");
    matrixCoreManager.nextSecondaryPage();
    Serial.println("Button 2 clicked End!");
    // Your code for button 2 click here
}

void ButtonManager::handleButton2DoubleClick() {
    matrixCoreManager.prevSecondaryPage();
    Serial.println("Button 2 double-clicked");
    // Your code for button 2 double-click here
}

void ButtonManager::handleButton2LongPressStart() {
  matrixCoreManager.button2LongPressStart();
  Serial.println("Button 2 long press started");
  // Your code for button 2 long press start here
}

void ButtonManager::handleButton2LongPress() {
    Serial.println("Button 2 long press ongoing");
    // Your code for button 2 during long press here
}

void ButtonManager::handleButton2LongPressStop() {
    matrixCoreManager.button2LongPressEnd();
    Serial.println("Button 2 long press stopped");
    // Your code for button 2 long press stop here
}

// Button 3 callbacks
void ButtonManager::handleButton3Click() {
    MatrixCore mm =  matrixCoreManager.getCurrentMatrixCore();
       Serial.println("----------------s22");
       Serial.println(mm.animationType);
    mm.animationType = animationManager.nextAnimationType(mm.animationType);
       Serial.println(mm.animationType);
       Serial.println("----------------s23");
    matrixCoreManager.modifyElement(matrixCoreManager.getCurrentPageIndex(),
                                  matrixCoreManager.getCurrentSecondaryIndex(),
                                  matrixCoreManager.getCurrentElementGroupIndex(),
                                  mm);
    Serial.println("Button 3 clicked");
    page.increaseAnimationType();
    // Your code for button 3 click here
}

void ButtonManager::handleButton3DoubleClick() {
  MatrixCore mm = matrixCoreManager.getCurrentMatrixCore();
  mm.animationType = animationManager.preAnimationType(mm.animationType);
  matrixCoreManager.modifyElement(matrixCoreManager.getCurrentPageIndex(),
                                matrixCoreManager.getCurrentSecondaryIndex(),
                                matrixCoreManager.getCurrentElementGroupIndex(),
                                mm);
  Serial.println("Button 3 double-clicked");
  page.decreaseAnimationType();
  // Your code for button 3 double-click here
}

void ButtonManager::handleButton3LongPressStart() {
  MatrixCore mm = matrixCoreManager.getCurrentMatrixCore();
  mm.animationType = animationManager.preAnimationType(0);
  matrixCoreManager.modifyElement(matrixCoreManager.getCurrentPageIndex(),
                                matrixCoreManager.getCurrentSecondaryIndex(),
                                matrixCoreManager.getCurrentElementGroupIndex(),
                                mm);
    Serial.println("Button 3 long press started");
    page.setAnimationType(0);
}

void ButtonManager::handleButton3LongPress() {
    Serial.println("Button 3 long press ongoing");
    // Your code for button 3 during long press here
}

void ButtonManager::handleButton3LongPressStop() {
    Serial.println("Button 3 long press stopped");
    Serial.println(appData.getTimezone());
    // Your code for button 3 long press stop here
}