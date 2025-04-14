#include "animations.h"

//IDLE
unsigned long lastIdleAnimTime = 0;
int idleFrame = 0;

//DIZZY
unsigned long lastDizzyAnimTime;
int dizzyFrame;

//IDLE
void showIdleAnimation(bool shouldDisplay) {
    
    unsigned long currentMillis = millis();

    // Update frame index every 300ms
    if (currentMillis - lastIdleAnimTime >= 300) {
        lastIdleAnimTime = currentMillis;
        idleFrame = (idleFrame + 1) % 3;
    }

    // Always draw the current frame
    switch (idleFrame) {
        case 0:
            display.drawBitmap(32, 0, idlePet_1, 64, 64, WHITE); break;
        case 1:
            display.drawBitmap(32, 0, idlePet_2, 64, 64, WHITE); break;
        case 2:
            display.drawBitmap(32, 0, idlePet_3, 64, 64, WHITE); break;
    }

    if (shouldDisplay) display.display();
}

//HAPPY
void showHappyAnimation(bool shouldDisplay) {
     display.drawBitmap(32, 0, happyPet, 64, 64, WHITE);
     if (shouldDisplay) display.display();
}

//DIZZY
void showDizzyAnimation(bool shouldDisplay) {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDizzyAnimTime >= 150) {
    lastDizzyAnimTime = currentMillis;

    switch (dizzyFrame) {
      case 0:
        display.drawBitmap(32, 0, dizzyPet_1, 64, 64, WHITE); break;
      case 1:
        display.drawBitmap(32, 0, dizzyPet_2, 64, 64, WHITE); break;
      case 2:
        display.drawBitmap(32, 0, dizzyPet_3, 64, 64, WHITE); break;
      case 3:
        display.drawBitmap(32, 0, dizzyPet_4, 64, 64, WHITE); break;
    }

    dizzyFrame = (dizzyFrame + 1) % 4;
  }

  if (shouldDisplay) display.display();
}