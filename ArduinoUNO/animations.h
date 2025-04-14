#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#include "bitmaps.h"

void showIdleAnimation  (bool shouldDisplay = true);
void showHappyAnimation (bool shouldDisplay = true);
void showDizzyAnimation (bool shouldDisplay = true);

#endif