#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

#include "bitmaps.h"      //Store the oled bitmaps
#include "animations.h"   //Animations


//AD8232 Heartrate Monitor
//#define ECG_ANALOG = 1

//TTP223B Capative Touch Sensor
#define TOUCH_PIN     13

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
//Adafruit_MPU6050 gyro

//Button
const int buttonPin = 7;
bool buttonPressed = false;


//Pet state vars
enum PetState { IDLE, HAPPY, DIZZY };
PetState currentState = IDLE;

//HAPPY
unsigned long lastSpawn = 0; //Heart spawn
bool isBeingPetted = false;
unsigned long happyTimer = 0;
const unsigned long happyDuration = 1500;  // 1.5 seconds of happiness
//DIZZY
unsigned long dizzyStartTime = 0;
const unsigned long dizzyDuration = 600;



//The setup function
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //Set button pin to input mode
  pinMode(buttonPin, INPUT_PULLUP); 

  // Display clear
  display.clearDisplay();
}


void loop() {
  readSensors();
  updatePetState();
}


void readSensors (){
  // Read touch sensor
  isBeingPetted = (digitalRead(TOUCH_PIN) ? true : false); // Low signal = touched

  // Read button (active LOW)
  if (digitalRead(buttonPin) == LOW && currentState != DIZZY) {
    currentState = DIZZY;
    dizzyStartTime = millis();
    clearHearts();  // Clear all existing hearts
  }
}



void updatePetState() {
  // Clear once per frame
  display.clearDisplay();

  //DIZZY > HAPPY > IDLE

  //DIZZY
  if (currentState == DIZZY) {
    showDizzyAnimation(false);

    // Exit dizzy after timeout
    if (millis() - dizzyStartTime >= dizzyDuration) {
      currentState = IDLE;
    }

    //Remove all heart particles


    display.display();  // Early return so no other drawing happens
    return;
  }


  // Check for petting and update timer
  if (isBeingPetted) {
    happyTimer = millis();  // Reset happy timer
  }


  // If happy timer still running, display happy animation
  if (millis() - happyTimer < happyDuration) {
    showHappyAnimation(false);
  } else {
    showIdleAnimation(false);
  }

  

  // Spawn hearts if currently being petted
  if (isBeingPetted && millis() - lastSpawn >= 200) {
    lastSpawn = millis();
    spawnHearts(1 + random(3)); // spawn 1–3 new hearts
  }

  // Update and render all hearts
  updateHearts();
  drawHearts();

  // Final display
  display.display();
}





// Heart Codes

#define MAX_HEARTS 10 //Max amount of hearts on screen

struct Heart {
  int x, y;
  int dy;
  bool active;
};

Heart hearts[MAX_HEARTS];

void spawnHearts(int count) {
  for (int i = 0; i < MAX_HEARTS && count > 0; i++) {
    if (!hearts[i].active) {
      hearts[i].x = random(0, display.width() - HEART_WIDTH);
      hearts[i].y = display.height(); // bottom

      //randomize dy with skew to 1
      int r = random(0, 100);
      if (r < 60) hearts[i].dy = 1;
      else if (r < 90) hearts[i].dy = 2;
      else hearts[i].dy = 3;

      hearts[i].active = true;
      count--;
    }
  }
}

void updateHearts() {
  for (int i = 0; i < MAX_HEARTS; i++) {
    if (hearts[i].active) {
      hearts[i].y -= hearts[i].dy;
      if (hearts[i].y < -HEART_HEIGHT) {
        hearts[i].active = false; // de-spawn
      }
    }
  }
}

void drawHearts() {
  for (int i = 0; i < MAX_HEARTS; i++) {
    if (hearts[i].active) {
      display.drawBitmap(hearts[i].x, hearts[i].y, heartBitmap, HEART_WIDTH, HEART_HEIGHT, SSD1306_WHITE);
    }
  }
  display.display();
}

void clearHearts() {
  for (int i = 0; i < MAX_HEARTS; i++) {
    hearts[i].active = false;
  }
}

