#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "bitmaps.h"   //Store the oled bitmaps


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
//Adafruit_MPU6050 mpu


//Pet state vars
enum PetState { IDLE, HAPPY, DIZZY };
PetState currentState = IDLE;
//IDLE
unsigned long lastIdleAnimTime = 0;
int idleFrame = 0;
//HAPPY
unsigned long lastSpawn = 0; //Heart spawn
bool isBeingPetted = false;
//DIZZY
unsigned long dizzyStartTime = 0;
bool isDizzy = false;

uint32_t now = 0; //Timer

//The setup function
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

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
}



void updatePetState() {

  //If being petted, update the state
  if (isBeingPetted){
    currentState = HAPPY;
  }

  //If IDLE, just update the animation and skip everything else
  if (currentState == IDLE) {
    showIdleAnimation();
    return;
  }

  //Generate heart particles while being petted
  if (isBeingPetted && millis() - lastSpawn >= 500) {
    lastSpawn = millis();
    spawnHearts(1 + random(3)); // spawn 1–3 new hearts
  }

  updateHearts();
  drawHearts();

}

//IDLE
void showIdleAnimation() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastIdleAnimTime >= 300) {  // Change frame every 300ms
    lastIdleAnimTime = currentMillis;

    display.clearDisplay();
    switch (idleFrame) {
      case 0:
        display.drawBitmap(32, 0, idlePet_1, 64, 64, WHITE);
        break;
      case 1:
        display.drawBitmap(32, 0, idlePet_2, 64, 64, WHITE);
        break;
      case 2:
        display.drawBitmap(32, 0, idlePet_3, 64, 64, WHITE);
        break;
    }
    display.display();
    idleFrame = (idleFrame + 1) % 3;
  }
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
  display.clearDisplay();
  for (int i = 0; i < MAX_HEARTS; i++) {
    if (hearts[i].active) {
      display.drawBitmap(hearts[i].x, hearts[i].y, heartBitmap, HEART_WIDTH, HEART_HEIGHT, SSD1306_WHITE);
    }
  }
  display.display();
}

