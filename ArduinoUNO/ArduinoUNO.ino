#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Adafruit_MPU6050.h>


//AD8232
#define ECG_ANALOG = 1

//TTP223B Capative Touch Sensor
#define TOUCH_PIN = 2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//MPU6050 Gyroscope
Adafruit_MPU6050 mpu



enum PetState { IDLE, HAPPY };
PetState currentState = IDLE;
bool isPetted = false;

//Bitmaps
const unsigned char idlePet[] PROGMEM = {};
const unsigned char happyPet[] PROGMEM = {};

void setup(){
        Serial.begin(115200);

        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        if(!display.begin(SSD1306_SWITCHCAPVCC)) {
                Serial.println(F("SSD1306 allocation failed"));
                for(;;); // Don't proceed, loop forever
        }

        display.clearDisplay();
        //display.setTextColor(BLUE);



}

void update(){

}