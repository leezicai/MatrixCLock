#ifndef MAX98357_H
#define MAX98357_H

#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "matrixData.h"

// I2S Pin Definitions
#define I2S_DIN   8
#define I2S_LRC   9
#define I2S_BCLK  17

// Volume settings (percentage 0-100)
#define DEFAULT_VOLUME_PERCENT 75
#define MIN_VOLUME_PERCENT 5
#define MAX_VOLUME_PERCENT 100
#define VOLUME_STEP 1

class Max98357Manager {
private:
    AudioGeneratorWAV* wav;
    AudioFileSourcePROGMEM* file;
    AudioOutputI2S* out;
    
    bool isInitialized;
    bool isPlaying;
    int currentVolumePercent;  // Volume as percentage (0-100)
    
    // For repeat playback
    const unsigned char* repeatAudioData;
    unsigned int repeatAudioLength;
    unsigned int repeatCount;
    unsigned int currentRepeat;
    
    // Convert percentage (0-100) to gain (0.0-4.0)
    float percentToGain(int percent);
    
public:
    Max98357Manager();
    ~Max98357Manager();
    
    // Initialize the I2S audio output
    bool begin();
    
    // Play alarm sound from PROGMEM once
    bool playAlarm(const unsigned char* audioData, unsigned int audioLength);
    
    // Play alarm sound from PROGMEM with specified repeat count
    bool playAlarm(const unsigned char* audioData, unsigned int audioLength, unsigned int num);
    
    // Stop current playback
    void stop();
    
    // Check if audio is currently playing
    bool isAudioPlaying();
    
    // Must be called in loop() to handle audio streaming
    void loop();
    
    // Set volume by percentage (5-100)
    void setVolume(int percent);
    
    // Get current volume as percentage (5-100)
    int getVolume();
    
    // Increase volume by VOLUME_STEP (max: MAX_VOLUME_PERCENT)
    void addVolume();
    
    // Decrease volume by VOLUME_STEP (min: MIN_VOLUME_PERCENT)
    void subVolume();
    
    // Check if initialized
    bool isReady();
};

// Global instance for external access
extern Max98357Manager max98357Manager;

#endif // MAX98357_H