#include <Arduino>

/*
 * Demo for printing values to the serial port.
 * Code by Arthur Chargueraud.
 * This example code is in the public domain.
 */

void setup() {
  Serial.begin(9600);
  Serial.println("Start up x");
}

int counter = 0;

void loop() {
  Serial.println(counter);
  delay(200);
  counter++;
}
