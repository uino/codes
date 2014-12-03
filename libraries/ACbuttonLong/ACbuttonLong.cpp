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

void ACbuttonLong::setup() {
  pinMode(buttonPin, INPUT);
}

void ACbuttonLong::debug(String msg) {
  if (false)
    Serial.println(msg);
}

void ACbuttonLong::poll() {
  byte st = digitalRead(buttonPin);
  if (st == LOW) {
    debug("low");
    if (dateLastDown == NEVER) {
      debug("high to low");
      dateLastDown = millis();
      if (downHandler != NULL) {
        downHandler();
      }
    } 
  } else { // (st == HIGH)
    debug("high");
    if (dateLastDown != NEVER) {
      debug("low to high");
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

