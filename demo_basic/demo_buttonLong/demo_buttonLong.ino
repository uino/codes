/**
 * Demo for the ACbuttonLong library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */


#include <ACbuttonLong.h>

const int buttonPin = 1;
const long durationLong = 200; // milliseconds

ACbuttonLong button(buttonPin);

void setup()
{
  button.setup();
  button.setLongPeriodDuration(durationLong);

  Serial.begin(9600);   
  Serial.println("Starting up");

  // pinMode(buttonPin, INPUT);
}

void loop()
{
  /*
  pinMode(buttonPin, INPUT);
  byte st = digitalRead(buttonPin);
  if (st == LOW) {
      Serial.println("low");
  } else { 
      Serial.println("high");
  }
  */
  button.poll();
  if (button.isDown())
    Serial.println("down");
  else
    Serial.println("none");
  delay(200);
}
