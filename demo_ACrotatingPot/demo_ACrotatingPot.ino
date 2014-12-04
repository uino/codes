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


/*
int reported = 0;
int ignoring = 0; // 0, +1, or -1 for ignore direction
int lastIgnored = 0;
int sensitivity = 20; // how much to turn potentiometer before next value
int lastReference = -1;  // position of potentiometer at last restart
int reportedOffset = 0; // how much was accumulated in the past

void loop() {
  int current = analogRead(potentiometerPin);
  if (lastReference == -1) {
    lastReference = current;
  }
  if (ignoring != 0) {
    if ((ignoring == 1 && current >= lastIgnored)
     || (ignoring == -1 && current <= lastIgnored)) {
      lastIgnored = current;
    } else {
      ignoring = 0;
      lastReference = current;
    }
  }
  if (ignoring == 0) {
    int reportedDelta = (current - lastReference) / sensitivity;
    int newReported = reportedOffset + reportedDelta;
    if (reported != newReported) {
      reported = newReported;
      Serial.println("New value = " + String(reported));
    }
    if (current == 0 || current == 1023) {
      ignoring = (current == 0) ? +1 : -1;
      lastIgnored = current;
      reportedOffset += reportedDelta;
    }
  }
  delay(50);
}
*/
