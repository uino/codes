/**
 * ACbuttonLong Library. See "ACbuttonLong.h" for information.
 */

#include "ACbuttonLong.h"

ACbuttonLong::ACbuttonLong(int buttonPin)
{
  this->buttonPin = buttonPin;
  longPeriodDuration = 200;
  dateLastDown = NEVER;
  downHandler = NULL;
  upHandler = NULL;
  upAfterLongHandler = NULL;
}

ACbuttonLong::ACbuttonLong(int buttonPin, long longPeriodInMillis) : ACbuttonLong(buttonPin)
{
  setLongPeriodDuration(longPeriodInMillis);
}

void ACbuttonLong::setup() {
  pinMode(buttonPin, INPUT);
}

void ACbuttonLong::poll() {
  byte st = digitalRead(buttonPin);
  if (st == LOW) {
      Serial.println("low");
    if (dateLastDown == NEVER) {
       Serial.println("high to low");
      // transition from HIGH to LOW
      dateLastDown = millis();
      if (downHandler != NULL) {
        downHandler();
      }
    } 
  } else { // (st == HIGH)
      Serial.println("high");
    if (dateLastDown != NEVER) {
      Serial.println("low to high");
     // transition from LOW to HIGH
      long duration = millis() - dateLastDown;
      dateLastDown = NEVER;
      if (upAfterLongHandler != NULL 
          && duration > longPeriodDuration) {
        upAfterLongHandler();
      } else if (upHandler != NULL) {
        upHandler();
      }
    }
  }
}

void ACbuttonLong::setLongPeriodDuration(long longPeriodInMillis) {
  longPeriodDuration = longPeriodInMillis;
}

boolean ACbuttonLong::ACbuttonLong::isDown() {
  return dateLastDown != NEVER;
}

long ACbuttonLong::downSince() {
  return dateLastDown;
}

long ACbuttonLong::downDuration() {
  return millis() - dateLastDown;
}

void ACbuttonLong::onDown(eventHandler handler) {
  downHandler = handler;
}

void ACbuttonLong::onUp(eventHandler handler) {
  upHandler = handler;
}

void ACbuttonLong::onUpAfterLong(eventHandler handler) {
  upAfterLongHandler = handler;
}

