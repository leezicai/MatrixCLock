#include "max98357.h"

// Sample alarm sound data (short beep - you can replace with actual alarm audio)
// This is a simple 1kHz tone WAV file header + data
const unsigned char defaultAlarmSound[] PROGMEM = {
    0x52, 0x49, 0x46, 0x46, 0x26, 0x08, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45,
    0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x44, 0xAC, 0x00, 0x00, 0x88, 0x58, 0x01, 0x00, 0x02, 0x00, 0x10, 0x00,
    0x64, 0x61, 0x74, 0x61, 0x00, 0x08, 0x00, 0x00
    // Add more audio data here
};

// Create global instance
Max98357Manager max98357Manager;

Max98357Manager::Max98357Manager() {
    wav = nullptr;
    file = nullptr;
    out = nullptr;
    isInitialized = false;
    isPlaying = false;
    currentVolumePercent = DEFAULT_VOLUME_PERCENT;
    repeatAudioData = nullptr;
    repeatAudioLength = 0;
    repeatCount = 0;
    currentRepeat = 0;
}

Max98357Manager::~Max98357Manager() {
    stop();
    if (out) delete out;
}

float Max98357Manager::percentToGain(int percent) {
    // Clamp percent to valid range
    percent = constrain(percent, MIN_VOLUME_PERCENT, MAX_VOLUME_PERCENT);
    
    // Convert 0-100% to 0.0-4.0 gain
    // 100% = 4.0 gain (maximum amplification)
    // 50% = 2.0 gain
    // 25% = 1.0 gain (original volume)
    // 5% = 0.2 gain (minimum)
    return (percent / 100.0) * 4.0;
}

bool Max98357Manager::begin() {
    if (isInitialized) {
        Serial.println("MAX98357 already initialized");
        return true;
    }
    
    Serial.println("Initializing MAX98357...");
    
    // Create I2S output with ESP32-S3 pins
    out = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S);  // Use I2S port 0, external DAC mode
    
    if (!out) {
        Serial.println("Failed to create AudioOutputI2S instance");
        return false;
    }
    
    // Set I2S pins for ESP32-S3
    out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DIN);
    
    // Set output mode for 16-bit samples
    out->SetOutputModeMono(true);  // MAX98357 is mono
    
    // Set default volume (this will also set the gain)

    currentVolumePercent = matrixDataManager.loadVolume();

    setVolume(currentVolumePercent);
    
    isInitialized = true;
    
    Serial.println("===============================");
    Serial.println("MAX98357 I2S initialized successfully");
    Serial.printf("I2S Pins - BCLK: %d, LRC: %d, DIN: %d\n", I2S_BCLK, I2S_LRC, I2S_DIN);
    Serial.printf("Default Volume: %d%% (Gain: %.2f)\n", currentVolumePercent, percentToGain(currentVolumePercent));
    Serial.printf("Volume Range: %d%% - %d%%\n", MIN_VOLUME_PERCENT, MAX_VOLUME_PERCENT);
    Serial.println("===============================");

    return true;
}

bool Max98357Manager::playAlarm(const unsigned char* audioData, unsigned int audioLength) {
    // Call the overloaded version with num = 1
    return playAlarm(audioData, audioLength, 1);
}

bool Max98357Manager::playAlarm(const unsigned char* audioData, unsigned int audioLength, unsigned int num) {
    if (!isInitialized) {
        Serial.println("MAX98357 not initialized! Call begin() first.");
        return false;
    }
    
    if (num == 0) {
        Serial.println("Invalid repeat count: must be >= 1");
        return false;
    }
    
    // Stop any current playback
    stop();
    
    // Store repeat parameters
    repeatAudioData = audioData;
    repeatAudioLength = audioLength;
    repeatCount = num;
    currentRepeat = 0;
    
    Serial.printf("Loading audio data: %d bytes, repeat %d times\n", audioLength, num);
    
    // Create new audio source from PROGMEM
    file = new AudioFileSourcePROGMEM(audioData, audioLength);
    
    // Create WAV generator
    wav = new AudioGeneratorWAV();
    
    // Start playback
    if (wav->begin(file, out)) {
        isPlaying = true;
        currentRepeat = 1;
        Serial.printf("Alarm playback started (1/%d)\n", repeatCount);
        return true;
    } else {
        Serial.println("Failed to start alarm playback");
        delete wav;
        delete file;
        wav = nullptr;
        file = nullptr;
        repeatAudioData = nullptr;
        return false;
    }
}

void Max98357Manager::stop() {
    if (wav && wav->isRunning()) {
        wav->stop();
    }
    
    if (wav) {
        delete wav;
        wav = nullptr;
    }
    
    if (file) {
        delete file;
        file = nullptr;
    }
    
    isPlaying = false;
    repeatAudioData = nullptr;
    repeatCount = 0;
    currentRepeat = 0;
}

bool Max98357Manager::isAudioPlaying() {
    if (wav && wav->isRunning()) {
        return true;
    }
    
    // Auto-cleanup when done
    if (isPlaying && wav && !wav->isRunning()) {
        // Check if we need to repeat
        if (repeatAudioData != nullptr && currentRepeat < repeatCount) {
            // Clean up current playback
            delete wav;
            delete file;
            
            // Start next repeat
            file = new AudioFileSourcePROGMEM(repeatAudioData, repeatAudioLength);
            wav = new AudioGeneratorWAV();
            
            if (wav->begin(file, out)) {
                currentRepeat++;
                Serial.printf("Repeating alarm (%d/%d)\n", currentRepeat, repeatCount);
            } else {
                Serial.println("Failed to restart alarm playback");
                stop();
            }
        } else {
            // All repeats done
            stop();
            Serial.println("Alarm playback finished");
        }
    }
    
    return isPlaying;
}

void Max98357Manager::loop() {
    if (wav && wav->isRunning()) {
        if (!wav->loop()) {
            // Current playback iteration finished
            // The isAudioPlaying() function will handle repeats
            isAudioPlaying();
        }
    }
}

void Max98357Manager::setVolume(int percent) {
    // Clamp to valid range
    currentVolumePercent = constrain(percent, MIN_VOLUME_PERCENT, MAX_VOLUME_PERCENT);
    
    if (out) {
        float gain = percentToGain(currentVolumePercent);
        out->SetGain(gain);
        Serial.printf("Volume set to: %d%% (Gain: %.2f)\n", currentVolumePercent, gain);
    }
}

int Max98357Manager::getVolume() {
    return currentVolumePercent;
}

void Max98357Manager::addVolume() {
    int newVolume = currentVolumePercent + VOLUME_STEP;
    
    // Don't exceed maximum
    if (newVolume > MAX_VOLUME_PERCENT) {
        newVolume = MAX_VOLUME_PERCENT;
        Serial.println("Volume already at maximum");
    }
    
    setVolume(newVolume);
    matrixDataManager.saveVolume(newVolume);
}

void Max98357Manager::subVolume() {
    int newVolume = currentVolumePercent - VOLUME_STEP;
    
    // Don't go below minimum
    if (newVolume < MIN_VOLUME_PERCENT) {
        newVolume = MIN_VOLUME_PERCENT;
        Serial.println("Volume already at minimum");
    }
    
    setVolume(newVolume);
    matrixDataManager.saveVolume(newVolume);
}

bool Max98357Manager::isReady() {
    return isInitialized;
}