/**
 * ACbuttonLong Library. See "ACbuttonLong.h" for information.
 */

#include "ACbuttonLong.h"

#define DebugPrint(msg)
// use command below for debugging
// #define DebugPrint(msg) Serial.println(msg)

ACbuttonLong::ACbuttonLong(int inputPin)
{
  this->inputPin = inputPin;
  longPeriodDuration = 200;
  dateLastDown = NEVER;
  downHandler = NULL;
  upHandler = NULL;
  upAfterLongHandler = NULL;
}

void ACbuttonLong::setup() {
  pinMode(inputPin, INPUT);
}

void ACbuttonLong::poll() {
  byte st = digitalRead(inputPin);
  if (st == LOW) {
    DebugPrint("low");
    if (dateLastDown == NEVER) {
      DebugPrint("high to low");
      dateLastDown = millis();
      if (downHandler != NULL) {
        downHandler();
      }
    } 
  } else { // (st == HIGH)
    DebugPrint("high");
    if (dateLastDown != NEVER) {
      DebugPrint("low to high");
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

