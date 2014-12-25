/**
 * Demo for polling the state of a button. 
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * Note: this is only a demo to check button wiring.
 * For processing button events, use the library ACbuttonLong.
 */

const int buttonPin = 6;

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");
  pinMode(buttonPin, INPUT);
}

void loop()
{
  boolean isDown = (digitalRead(buttonPin) == LOW);
  if (isDown) {
    Serial.println("down");
  } else {
    Serial.println("up");
  }
  delay(200);
}
