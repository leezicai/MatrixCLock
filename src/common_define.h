#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

// Digital I/O Pins
#define GPIO_0    0   // Boot button, pull-up, can be used as input/output
#define GPIO_1    1   // Available for general use
#define GPIO_2    2   // Available for general use

#define GPIO_4    4   // Available for general use
#define GPIO_5    5   // Available for general use
#define GPIO_6    6   // Available for general use
#define GPIO_7    7   // Available for general use
#define GPIO_8    8   // Available for general use
#define GPIO_9    9   // Available for general use
#define GPIO_10   10  // Available for general use
#define GPIO_11   11  // Available for general use
#define GPIO_12   12  // Available for general use
#define GPIO_13   13  // Available for general use
#define GPIO_14   14  // Available for general use
#define GPIO_15   15  // Available for general use
#define GPIO_16   16  // Available for general use
#define GPIO_17   17  // Available for general use

#define GPIO_21   21  // Available for general use

// GPIO 22-32 are not available on ESP32-S3

#define GPIO_33   33  // Available for general use
#define GPIO_34   34  // Available for general use
#define GPIO_35   35  // Available for general use
#define GPIO_36   36  // Available for general use
#define GPIO_37   37  // Available for general use
#define GPIO_38   38  // Available for general use
#define GPIO_39   39  // Available for general use
#define GPIO_40   40  // Available for general use
#define GPIO_41   41  // Available for general use
#define GPIO_42   42  // Available for general use

#define GPIO_45   45  // Available for general use
#define GPIO_46   46  // Available for general use
#define GPIO_47   47  // Available for general use
#define GPIO_48   48  // Available for general use


// button
#define BUTTON1_PIN 46
#define BUTTON2_PIN 16
#define BUTTON3_PIN 15

// light
#define LIGHT_ADC 4   

// I2C
#define SDA 1
#define SCL 2

// ec11
#define ROTARY_ENCODER_A_PIN 11
#define ROTARY_ENCODER_B_PIN 10
#define ROTARY_ENCODER_BUTTON_PIN 12

// display hub75 dma

#define LAT_PIN 45
#define D_PIN 48
#define B_PIN 47
#define E_PIN 21
#define G2_PIN 14
#define G1_PIN 13

#define OE_PIN 42
#define CLK_PIN 41
#define C_PIN 40
#define A_PIN 39
#define B2_PIN 38
#define R2_PIN 37
#define B1_PIN 36
#define R1_PIN 35

#define PANEL_WIDTH 128
#define PANEL_HEIGHT 64
#define PANEL_CHAIN 1
#define PANEL_TYPE 0

#define PANEL_NAME "ICN2012"
#define PANEL_NAME_ICN2012 "ICN2012"
#define PANEL_NAME_TC7528 "TC7528D"

// loading
#define DURATION 3600;
// loading error message
#define LOADING_ERR_MSG_FAIL_WIFI "WiFi Connect Failed"
#define LOADING_ERR_MSG_FAIL_NETWORK "Net SyncTime Failed"

// Setup Msg
#define SETUP_WIFI_MSG "WiFi Setup:"
#define SETUP_WIFI_NAME "Connect MatrixClock"
#define SETUP_WIFI_ADDRESS "http://10.0.0.1"




#endif // ESP32S3_PINS_H