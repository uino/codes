/**
 *  * ACrotatingPot Library. See "ACrotatingPot.h" for information.
 */

#include "ACrotatingPot.h"

ACrotatingPot::ACrotatingPot(int inputPin)
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

ACrotatingPot::ACrotatingPot(int inputPin, int sensitivity, boolean inverted) : ACrotatingPot(inputPin)
{
  setSensitivity(sensitivity);
  rotationInverted(inverted);
}

void ACrotatingPot::setup() {
  pinMode(inputPin, INPUT);
  resetValue(0);
}

void ACrotatingPot::setSensitivity(int sensitivity) {
  this->sensitivity = sensitivity;
}

void ACrotatingPot::setModulo(int modulo) {
  this->modulo = modulo;
}

void ACrotatingPot::rotationInverted(boolean inverted) {
  this->inverted = inverted;
}

void ACrotatingPot::onChange(eventHandler handler) {
  changeHandler = handler;
}

int ACrotatingPot::getModuloValue(int v) {
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

int ACrotatingPot::readSensor()
{
  return analogRead(inputPin);
}

void ACrotatingPot::resetValue(int newValue) {
  valueOffset = newValue;
  value = valueOffset;
  reference = readSensor();
  ignoring = 0;
}

void ACrotatingPot::resetValue() {
  resetValue(0);
}

int ACrotatingPot::getValue() {
  return getModuloValue(value);
}

void ACrotatingPot::poll() {
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

void ACrotatingPot::debug() {
  if (true) {
    Serial.println("value = " + String(value));
    Serial.println("valueOffset = " + String(valueOffset));
    Serial.println("ignoring = " + String(ignoring));
    Serial.println("reference = " + String(reference));
    Serial.println("=================");
  }
}


