#include "animations.h"

// IDLE
unsigned long lastIdleAnimTime = 0;
int idleFrame = 0;

//HAPPY

// DIZZY
unsigned long lastDizzyAnimTime = 0;
int dizzyFrame = 0;

// CATCH
extern unsigned long catchAnimTime;
int catchFrame = 0; 
extern int dotX;
extern int dotY;
extern int catX;
extern int catY;

// IDLE
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

// HAPPY
void showHappyAnimation(bool isBeingPetted) {
  if(isBeingPetted) {
    display.drawBitmap(32, 0, petPet, 64, 64, WHITE);
  }
  else {
    display.drawBitmap(32, 0, happyPet, 64, 64, WHITE);
  }
}

// DIZZY
void showDizzyAnimation(bool shouldDisplay) {
  unsigned long currentMillis = millis();

  // Update frame index every 200ms
    if (currentMillis - lastDizzyAnimTime >= 200) {
      lastDizzyAnimTime = currentMillis;
      dizzyFrame = (dizzyFrame + 1) % 4;
    }

  // Always draw the current frame  
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


  if (shouldDisplay) display.display();
}

// VIGIL
void showVigilantAnimation(bool shouldDisplay) {
  display.drawBitmap(32, 0, vigilantPet, 64, 64, WHITE);
  if (shouldDisplay) display.display();
}

//CATCH
void showCatchLeftFollowingDot() {
  if (millis() - catchAnimTime >= 500) {
    catchFrame = (catchFrame + 1) % 2;
    catchAnimTime = millis();
  }

  const unsigned char* frame = (catchFrame == 0) ? catchLeftCat_1 : catchLeftCat_2;

  display.drawBitmap(catX, catY, frame, 64, 32, WHITE);
}


void showCatchRightFollowingDot() {
  if (millis() - catchAnimTime >= 500) {
    catchFrame = (catchFrame + 1) % 2;
    catchAnimTime = millis();
  }

  const unsigned char* frame = (catchFrame == 0) ? catchRightCat_1 : catchRightCat_2;

  int drawX = (catX - 64 + 4);
  int drawY = catY;

  // fix laggy
  drawX = max(drawX, -63);  // only draw what can be seen

  display.drawBitmap(drawX, drawY, frame, 64, 32, WHITE);
}

