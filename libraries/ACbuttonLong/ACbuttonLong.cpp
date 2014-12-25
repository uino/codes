/**
 * ACbuttonLong Library. See "ACbuttonLong.h" for information.
 */

#include "ACbuttonLong.h"

#define DebugPrint(msg)

ACbuttonLong::ACbuttonLong(int inputPin)
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

void ACbuttonLong::setLongPeriodDuration(long longPeriodInMillis) {
  longPeriodDuration = longPeriodInMillis;
}

void ACbuttonLong::begin() {
  pinMode(inputPin, INPUT);
}

void ACbuttonLong::poll() {
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

boolean ACbuttonLong::ACbuttonLong::isDown() {
  return (status != UP);
}

long ACbuttonLong::downSince() {
  return downSinceValue;
}

long ACbuttonLong::downDuration() {
  if (status != UP) {
    return millis() - downSinceValue;
  }
  return downDurationValue;
}

void ACbuttonLong::onDown(eventHandler handler) {
  downHandler = handler;
}

void ACbuttonLong::onDownLong(eventHandler handler) {
  downLongHandler = handler;
}

void ACbuttonLong::onUp(eventHandler handler) {
  upHandler = handler;
}

void ACbuttonLong::onUpLong(eventHandler handler) {
  upLongHandler = handler;
}

