#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <MPU6050.h>

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
// const int buttonPin = 7;

//Pet state
enum PetState {
  IDLE,
  HAPPY,
  DIZZY,
  CHASE,
  VIGILANT,
};
PetState currentState = IDLE;

//HAPPY
unsigned long lastSpawn = 0; //Heart spawn
bool isBeingPetted = false;
unsigned long happyStartTime = 0;
const unsigned long happyDuration = 1500;  // 1.5 seconds of happiness

//DIZZY
unsigned long dizzyStartTime = 0;
const unsigned long dizzyDuration = 600;

// CHASE
// Joystick pins
const int joystickX = 32;
const int joystickY = 33;
const int joyButton = 25;
// Joystick default values
const int joystickMidX = 1776;
const int joystickMidY = 1797;
// Threshold to ignore tiny movement (deadzone)
const int deadzone = 100;
bool isLaserMode = false;
// Dot position
int dotX = 64; // Start center
int dotY = 32;
const int dotSize = 4;
const int dotMoveSpeed = 400; //bigger = slower movement
// Response delay
unsigned long lastJoystickResponse = 0;
const unsigned long responseInterval = 1500;
// For vigilance delay
unsigned long vigilantStartTime = 0;
bool dotTriggeredVigilant = false;



//The setup function
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // //Set button pin to input mode
  // pinMode(buttonPin, INPUT_PULLUP); 

  // Display clear
  display.clearDisplay();
}


void loop() {
  display.clearDisplay();

  updatePetState();

  display.display();
  delay(20); 
}




void updatePetState() {

  // Update dot
  updateJoystickDot();

  // DIZZY > CHASE > HAPPY > IDLE
  // Read touch sensor (active LOW)
  isBeingPetted = (digitalRead(TOUCH_PIN) ? true : false);

  // Read button (active HIGH)
  if (digitalRead(joyButton) == HIGH) {
    currentState = DIZZY;
    dizzyStartTime = millis();
    clearHearts();  // Clear all existing hearts
  } 
  // Read Joystick
  else if (isLaserMode) {
    currentState = CHASE;
  }
  //Read Touch Sensor
  else if (isBeingPetted) {
    currentState = HAPPY;
    happyStartTime = millis(); // 1.5s of happiness
  }


  // Spawn hearts while being petted & update existing Heart positions
  drawUpdatedHearts(isBeingPetted);


  switchStateAnimations();

}


// High-priority state logic
void switchStateAnimations() {
  switch (currentState) {
    case DIZZY:
      showDizzyAnimation(false);

      if (millis() - dizzyStartTime >= dizzyDuration) {
        currentState = IDLE;
      }

      break;

    case VIGILANT:
      showVigilantAnimation(false);

      // if (millis() - vigilantStartTime >= 1500) {
      //   switch (dotQuadrant) {
      //     case 1: currentState = CATCHING_Q1; break;
      //     case 2: currentState = CATCHING_Q2; break;
      //     case 3: currentState = CATCHING_Q3; break;
      //     case 4: currentState = CATCHING_Q4; break;
      //   }
      //   lastJoystickResponse = millis();
      //   dotTriggeredVigilant = false;
      // }
      break;

    case HAPPY:
      showHappyAnimation(false);
      if (millis() - happyStartTime >= happyDuration) {
        currentState = IDLE;
      }
      break;

    case IDLE:
      showIdleAnimation(false);
      break;
    
    default:
      break;
  }
}




// DOT codes

// Update dot position from joystick
void updateJoystickDot() {
  int xVal = analogRead(joystickX);
  int yVal = analogRead(joystickY);

  // Move based on joystick deviation from center
  if (abs(xVal - joystickMidX) > deadzone) {
    dotX += (xVal - joystickMidX) / dotMoveSpeed;
    isLaserMode = true;
  }
  if (abs(yVal - joystickMidY) > deadzone) {
    dotY += (yVal - joystickMidY) / dotMoveSpeed;
    isLaserMode = true;
  }
  else {
    isLaserMode = false;
  }

  // Clamp inside screen
  dotX = constrain(dotX, 0, 128 - dotSize);
  dotY = constrain(dotY, 0, 64 - dotSize);

  // Draw the dot
  display.fillRect(dotX, dotY, dotSize, dotSize, WHITE);
}






// Heart Codes

#define MAX_HEARTS 10 // Max amount of hearts on screen

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

void drawUpdatedHearts(bool isBeingPetted) {
  // Spawn new Hearts
  if (isBeingPetted && millis() - lastSpawn >= 200) {
    lastSpawn = millis();
    spawnHearts(1 + random(3));
  }

  // Update positions
  for (int i = 0; i < MAX_HEARTS; i++) {
    if (hearts[i].active) {
      hearts[i].y -= hearts[i].dy;
      if (hearts[i].y < -HEART_HEIGHT) {
        hearts[i].active = false; // de-spawn
      }
    }
  }

  // Draw each Heart
  for (int i = 0; i < MAX_HEARTS; i++) {
    if (hearts[i].active) {
      display.drawBitmap(hearts[i].x, hearts[i].y, heartBitmap, HEART_WIDTH, HEART_HEIGHT, SSD1306_WHITE);
    }
  }
}

void clearHearts() {
  for (int i = 0; i < MAX_HEARTS; i++) {
    hearts[i].active = false;
  }
}

