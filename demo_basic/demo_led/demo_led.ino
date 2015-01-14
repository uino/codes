/**
 * Demo for getting the builtin led to blink.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 */

const int pinLED = 13;

void setup()
{
  pinMode(pinLED, OUTPUT);
}

void loop()
{
  digitalWrite(pinLED, HIGH);
  delay(300);
  digitalWrite(pinLED, LOW);
  delay(300);
}
