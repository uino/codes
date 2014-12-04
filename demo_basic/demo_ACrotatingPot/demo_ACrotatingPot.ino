/*
 * Demo for library ACrotatingPot
 * Code by Arthur Chargueraud.
 * This example code is in the public domain.
 * 
 *
 * Potentiometer:
 * - center pin of the potentiometer attached to the A0 pin,
 * - one side pin (either one) attached to ground,
 * - the other side pin attached to +5V.
 *
 * Prints value 10 times per second on the serial output.
 * Values are between 0 and 1023 inclusive.
 */

#include "ACrotatingPot.h";

int potPin = A0;
const int potSensitivity = 20; 
const boolean potInverted = true; 

ACrotatingPot rot(potPin, potSensitivity, potInverted);

void rotChange() {
  Serial.println(rot.getValue());
}

void setup() {
  Serial.begin(9600);
  rot.setup();
  rot.onChange(rotChange);
}

void loop() {
  rot.poll();
  delay(50);
}


