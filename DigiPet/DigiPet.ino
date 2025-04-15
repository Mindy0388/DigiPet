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
  VIGIL,
  CATCH_LEFT,
  CATCH_RIGHT
};
PetState currentState = IDLE;

//HAPPY
unsigned long lastSpawn = 0; //Heart spawn
bool isBeingPetted = false;
unsigned long happyStartTime = 0;
const unsigned long happyDuration = 1500;  // 1.5 seconds of happiness

//DIZZY
unsigned long dizzyStartTime = 0;
const unsigned long dizzyDuration = 3000;

// Joystick pins
const int joystickX = 32;
const int joystickY = 33;
const int joyButton = 25;
// Joystick default values
const int joystickMidX = 1776;
const int joystickMidY = 1797;
// Threshold to ignore tiny movement (deadzone)
const int deadzone = 100;
// Dot position
int dotX = 64; // Start center
int dotY = 32;
const int dotSize = 4;
const int dotMoveSpeed = 400; //bigger = slower movement
bool dotVisible = false;
// Response delay
unsigned long lastJoystickMoveTime = 0;

// VIGIL
unsigned long vigilantStartTime = 0;
const unsigned long joystickVigilantDuration = 1000;
bool joystickTriggeredVigilance = false;

//CATCH
unsigned long catchAnimTime = 0;
int catX = 64;
int catY = 32;
const float followSpeed = 0.05;  // bigger = faster, max = 1
const int maxSpeed = 4;


//The setup function
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Setup pinmodes
  pinMode(joyButton, INPUT_PULLUP); 

  // Display clear
  display.clearDisplay();
}

//Oled Frame rate
const unsigned long displayFrameInterval = 33; // ~30fps
unsigned long lastFrameTime = 0;

void loop() {
  if (millis() - lastFrameTime >= displayFrameInterval) {
    display.clearDisplay();
    lastFrameTime = millis();
    updatePetState();
    display.display();
  }
}




void updatePetState() {

  // Update dot
  updateJoystickDot();

  // DIZZY > CATCH > HAPPY > IDLE
  // Read touch sensor (active LOW)
  isBeingPetted = (digitalRead(TOUCH_PIN) ? true : false);

  // Read button (active LOW)
  if (digitalRead(joyButton) == LOW && currentState != DIZZY) {
    currentState = DIZZY;
    dizzyStartTime = millis();
    clearHearts();  // Clear all existing hearts
  } 
  // See if joystick edits the VIGIL state
  else if (currentState == VIGIL) {
    if (millis() - vigilantStartTime >= joystickVigilantDuration) {
      currentState = (dotX < 64) ? CATCH_LEFT : CATCH_RIGHT;
      catchAnimTime = millis();  // reset animation timing
      joystickTriggeredVigilance = false;
    }
  }
  else if (currentState == CATCH_LEFT && dotX > 70) {
    currentState = CATCH_RIGHT;
  }
  else if (currentState == CATCH_RIGHT && dotX < 64) {
    currentState = CATCH_LEFT;
  }
  //Read Touch Sensor
  else if (isBeingPetted) {
    currentState = HAPPY;
    happyStartTime = millis(); // 1.5s of happiness
  }


  // Spawn hearts while being petted & update existing Heart positions
  drawUpdatedHearts(isBeingPetted);

  // Dot Drawing  
  if (dotVisible) {
    drawDot();
  }


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

    case VIGIL:
      showVigilantAnimation(false);

      break;

    case CATCH_LEFT:
      updateCatPositionTowardDot();
      showCatchLeftFollowingDot();
      break;

    case CATCH_RIGHT:
      updateCatPositionTowardDot();
      showCatchRightFollowingDot();
      break;


    case HAPPY:
      showHappyAnimation(isBeingPetted);
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


// CATCH
void updateCatPositionTowardDot() {
  // Follow point just above the dot
  int targetX = dotX;
  int targetY = dotY - 32;

  int dx = targetX - catX;
  int dy = targetY - catY;

  catX += (int)dx * followSpeed;
  catY += (int)dy * followSpeed;

  catX += constrain(dx * followSpeed, -maxSpeed, maxSpeed);
  catY += constrain(dy * followSpeed, -maxSpeed, maxSpeed);
  Serial.print(catX + " x/y " + catY) ;

}


// DOT codes

// Update dot position from joystick
void updateJoystickDot() {
  int xVal = analogRead(joystickX);
  int yVal = analogRead(joystickY);

  bool moved = false; // Stores whether a valid input is detected from joystick

  // Move on X and Y based on joystick deviation from center
  if (abs(xVal - joystickMidX) > deadzone) {
    dotX += (xVal - joystickMidX) / dotMoveSpeed;
    moved = true;
  }
  if (abs(yVal - joystickMidY) > deadzone) {
    dotY += (yVal - joystickMidY) / dotMoveSpeed;
    moved = true;
  }


  // If moved, enter VIGIL state and draw Dot
  if (moved) {
  lastJoystickMoveTime = millis();
  
    if (!dotVisible) {
      dotVisible = true;

      // Trigger vigilance once
      if (!joystickTriggeredVigilance) {
        joystickTriggeredVigilance = true;
        currentState = VIGIL;
        vigilantStartTime = millis();
      }
    }

    // Clamp inside screen
    dotX = constrain(dotX, 0, 128 - dotSize);
    dotY = constrain(dotY, 0, 64 - dotSize);


  }
  else {
    // Auto-hide dot if idle for too long (3s)
    // Also change cat state to IDLE
    if (dotVisible && millis() - lastJoystickMoveTime > 3000) {
      dotVisible = false;
      dotX = 64;
      dotY = 32;
      currentState = IDLE;
      joystickTriggeredVigilance = false;
    } 
  }


}

// Draw the dot
void drawDot() {
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

