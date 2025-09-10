#ifndef SHT30_H
#define SHT30_H

#include <Arduino.h>
#include <Wire.h>

// Pin definitions
#define SDA_2 1
#define SCL_2 2

// SHT30 I2C address
#define SHT30_ADDRESS 0x44

// SHT30 commands
#define SHT30_CMD_MEAS_HIGHREP 0x2C06  // High repeatability measurement
#define SHT30_CMD_SOFT_RESET   0x30A2  // Soft reset

class SHT30 {
public:
    SHT30();
    
    // Initialize the sensor
    bool begin();
    
    // Read temperature and humidity data
    bool readData();
    
    // Get temperature in Celsius
    float getTemperature() const;
    
    // Get humidity in percentage
    float getHumidity() const;
    
    // Get temperature in Fahrenheit
    float getTemperatureF() const;
    
    // Check if sensor is connected
    bool isConnected();
    
    // Reset sensor
    void reset();

private:
    float temperature;  // Temperature in Celsius
    float humidity;     // Humidity in percentage
    bool initialized;
    
    // Send command to sensor
    bool sendCommand(uint16_t command);
    
    // Calculate CRC checksum
    uint8_t calculateCRC(uint8_t data[], uint8_t length);
    
    // Verify CRC checksum
    bool verifyCRC(uint8_t data[], uint8_t checksum);
};

// Global instance declaration
extern SHT30 sht30;

#endif