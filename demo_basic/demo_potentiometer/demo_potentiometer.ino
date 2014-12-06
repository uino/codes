/*
 * Demo for reading value on a analog input and 
 * reporting values to the serial port.
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

int potentiometerPin = A0;

void setup() {
  pinMode(potentiometerPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(potentiometerPin);    
  Serial.println(sensorValue);
  delay(100);
}
