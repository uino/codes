/**
 * AC_Button Library. See "AC_Button.h" for information.
 */

#include "AC_Button.h"

#define DebugPrint(msg)

AC_Button::AC_Button(int inputPin)
{
  this->inputPin = inputPin;
  longPeriodDuration = 200;
  downSinceValue = NEVER;
  downDurationValue = NEVER;
  status = UP;
  downHandler = NULL;
  upHandler = NULL;
  upLongHandler = NULL;
}

void AC_Button::setLongPeriodDuration(long longPeriodDuration) {
  this->longPeriodDuration = longPeriodDuration;
}

void AC_Button::begin() {
  pinMode(inputPin, INPUT);
}

void AC_Button::poll() {
  boolean isDown = (digitalRead(inputPin) == LOW);
  if (isDown) { 
    if (status == UP) { // from up to down
      status = DOWN;
      downSinceValue = millis();
      if (downHandler != NULL) {
        downHandler();
      }
    } else if (status != LONGDOWN && downLongHandler != NULL) {
      long downDuration = millis() - downSinceValue;
      if (downDuration >= longPeriodDuration) {
        status = LONGDOWN;
        downDurationValue = downDuration;
        downLongHandler();
      }
    }
  } else {
    if (status != UP) { // from down or longdown to up
      status = UP;
      downDurationValue = millis() - downSinceValue;
      if (upLongHandler != NULL 
          && downDurationValue >= longPeriodDuration) {
        upLongHandler();
      } else if (upHandler != NULL) {
        upHandler();
      }
    }
  }
}

boolean AC_Button::AC_Button::isDown() {
  return (status != UP);
}

long AC_Button::downSince() {
  return downSinceValue;
}

long AC_Button::downDuration() {
  if (status != UP) {
    return millis() - downSinceValue;
  }
  return downDurationValue;
}

void AC_Button::onDown(eventHandler handler) {
  downHandler = handler;
}

void AC_Button::onDownLong(eventHandler handler) {
  downLongHandler = handler;
}

void AC_Button::onUp(eventHandler handler) {
  upHandler = handler;
}

void AC_Button::onUpLong(eventHandler handler) {
  upLongHandler = handler;
}

