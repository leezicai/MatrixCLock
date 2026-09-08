#include "sht30.h"
#include <math.h>  // For round() function

// Global instance definition
SHT30 sht30;

SHT30::SHT30() : temperature(0.0), humidity(0.0), initialized(false) {
}

bool SHT30::begin() {
    // Initialize I2C with custom pins
    Wire.begin(SDA, SCL);
    Wire.setClock(100000); // Set I2C frequency to 100kHz
    
    delay(10); // Wait for sensor to be ready
    
    // Test if sensor is connected
    if (!isConnected()) {
        return false;
    }
    
    // Perform soft reset
    reset();
    delay(10);
    
    initialized = true;
    return true;
}

bool SHT30::readData() {
    if (!initialized) {
        return false;
    }
    
    // Send measurement command
    if (!sendCommand(SHT30_CMD_MEAS_HIGHREP)) {
        return false;
    }
    
    // Wait for measurement to complete (typical 15ms for high repeatability)
    delay(20);
    
    // Read 6 bytes (2 bytes temp + 1 byte CRC + 2 bytes humidity + 1 byte CRC)
    Wire.requestFrom(SHT30_ADDRESS, 6);
    
    if (Wire.available() != 6) {
        return false;
    }
    
    uint8_t data[6];
    for (int i = 0; i < 6; i++) {
        data[i] = Wire.read();
    }
    
    // Verify CRC for temperature data
    if (!verifyCRC(&data[0], data[2])) {
        return false;
    }
    
    // Verify CRC for humidity data
    if (!verifyCRC(&data[3], data[5])) {
        return false;
    }
    
    // Calculate temperature in Celsius and round to 1 decimal place
    uint16_t tempRaw = (data[0] << 8) | data[1];
    float tempCalc = -45.0 + (175.0 * tempRaw / 65535.0);
    temperature = round(tempCalc * 10.0) / 10.0;  // Round to 1 decimal place
    
    // Calculate humidity in percentage and round to 1 decimal place
    uint16_t humRaw = (data[3] << 8) | data[4];
    float humCalc = 100.0 * humRaw / 65535.0;
    humidity = round(humCalc * 10.0) / 10.0;  // Round to 1 decimal place
    
    return true;
}

float SHT30::getTemperature() const {
    return temperature;
}

float SHT30::getHumidity() const {
    return humidity;
}

float SHT30::getTemperatureF() const {
    // Convert to Fahrenheit and round to 1 decimal place
    float tempF = temperature * 9.0 / 5.0 + 32.0;
    return round(tempF * 10.0) / 10.0;
}

bool SHT30::isConnected() {
    Wire.beginTransmission(SHT30_ADDRESS);
    return (Wire.endTransmission() == 0);
}

void SHT30::reset() {
    sendCommand(SHT30_CMD_SOFT_RESET);
    delay(10);
}

bool SHT30::sendCommand(uint16_t command) {
    Wire.beginTransmission(SHT30_ADDRESS);
    Wire.write(command >> 8);    // Send high byte
    Wire.write(command & 0xFF);  // Send low byte
    return (Wire.endTransmission() == 0);
}

uint8_t SHT30::calculateCRC(uint8_t data[], uint8_t length) {
    uint8_t crc = 0xFF;
    
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }
    
    return crc;
}

bool SHT30::verifyCRC(uint8_t data[], uint8_t checksum) {
    return (calculateCRC(data, 2) == checksum);
}