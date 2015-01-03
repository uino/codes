/**
 *  * AC_RotatingPot Library. See "AC_RotatingPot.h" for information.
 */

#include "AC_RotatingPot.h"

AC_RotatingPot::AC_RotatingPot(byte inputPin)
{
  this->inputPin = inputPin;
  changeHandler = NULL;
  sensitivity = 20;
  inverted = false;
  modulo = 0;
  // code below not needed, but added for safety if forgetting to call "setup"
  valueOffset = 0;
  value = 0;
  reference = 0;
  ignoring = 0;
}

AC_RotatingPot::AC_RotatingPot(byte inputPin, int sensitivity, boolean inverted) : AC_RotatingPot(inputPin)
{
  setSensitivity(sensitivity);
  rotationInverted(inverted);
}

void AC_RotatingPot::begin() {
  pinMode(inputPin, INPUT);
  resetValue(0);
}

void AC_RotatingPot::setSensitivity(int sensitivity) {
  this->sensitivity = sensitivity;
}

void AC_RotatingPot::setModulo(int modulo) {
  this->modulo = modulo;
}

void AC_RotatingPot::rotationInverted(boolean inverted) {
  this->inverted = inverted;
}

void AC_RotatingPot::onChange(eventHandler handler) {
  changeHandler = handler;
}

int AC_RotatingPot::getModuloValue(int v) {
  if (modulo == 0)
    return v;
  if (0 <= v && v < modulo)
    return v;
  v = v % modulo;
  if (v >= 0)
    return v;
  else
    return v + modulo;
}

int AC_RotatingPot::readSensor()
{
  return analogRead(inputPin);
}

void AC_RotatingPot::resetValue(int newValue) {
  valueOffset = newValue;
  value = valueOffset;
  reference = readSensor();
  ignoring = 0;
}

void AC_RotatingPot::resetValue() {
  resetValue(0);
}

int AC_RotatingPot::getValue() {
  return getModuloValue(value);
}

void AC_RotatingPot::poll() {
  int sensor = readSensor();
  if (ignoring != 0) {
    if ((ignoring == +1 && sensor < reference)
     || (ignoring == -1 && sensor > reference)) {
      ignoring = 0;
    }
    reference = sensor;
  }
  if (ignoring == 0) {
    int valueDelta = (sensor - reference) / sensitivity;
    if (inverted) {
      valueDelta = -valueDelta;
    }
    int valueNew = valueOffset + valueDelta;
    if (value != valueNew) {
      value = valueNew;
      if (changeHandler != NULL) {
        changeHandler();
      }
    }
    if (sensor == 0 || sensor == 1023) {
      valueOffset += valueDelta;
      // prevent long-term overflows by taking modulo if needed
      valueOffset = getModuloValue(valueOffset);
      ignoring = (sensor == 0) ? +1 : -1;
      reference = sensor;
    }
  }
}

/*
void debug() {
  Serial.println("value = " + String(value));
  Serial.println("valueOffset = " + String(valueOffset));
  Serial.println("ignoring = " + String(ignoring));
  Serial.println("reference = " + String(reference));
  Serial.println("=================");
}
*/

