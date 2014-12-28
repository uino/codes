/*
 * Demo for library AC_RotatingPot
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

#include <AC_RotatingPot.h>

int rotPin = A0;
const int rotSensitivity = 20; 
const boolean rotInverted = true; 

AC_RotatingPot rot(rotPin, rotSensitivity, rotInverted);

void rotChange() {
  Serial.println(rot.getValue());
}

void setup() {
  Serial.begin(9600);
  rot.begin();
  rot.onChange(rotChange);
}

void loop() {
  rot.poll();
  delay(50);
}


