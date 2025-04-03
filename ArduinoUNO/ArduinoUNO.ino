#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "bitmaps.h"   //Store the oled bitmaps


//AD8232 Heartrate Monitor
#define ECG_ANALOG = 1

//TTP223B Capative Touch Sensor
#define TOUCH_PIN = 2

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

// Declaration for SSD1306 display connected using software SPI：
#define OLED_D0    18  // SCLK (SPI Clock)
#define OLED_D1    23  // MOSI (SPI Data)
#define OLED_RES   4   // Reset
#define OLED_DC    2   // Data/Command
#define OLED_CS    5   // Chip Select

// Initialize SSD1306 with SPI
Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_D1, OLED_D0, OLED_DC, OLED_RES, OLED_CS
);

//MPU6050 Gyroscope
//Adafruit_MPU6050 mpu


enum PetState { IDLE, HAPPY };
//PetState currentState = IDLE;
bool isPetted = false;

//The setup function
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Display bitmap
  display.clearDisplay();
  display.drawBitmap(0, 0, testBitmap, 128, 64, 1);
  display.display();
}

void loop() {
  // Nothing here (static display)
}