/**
 * Demo for the ACbuttonLong library, including short and long events.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */

#include <ACbuttonLong.h>

const int buttonPin = 6;
const long durationLong = 1000; // milliseconds

ACbuttonLong button(buttonPin);

void downEvent() {
  Serial.println("down");
}

void downLongEvent() {
  Serial.print("long down: ");
  Serial.println(button.downDuration());
}

void upEvent() {
  Serial.print("up short: ");
  Serial.println(button.downDuration());
}

void upLongEvent() {
  Serial.print("up long: ");
  Serial.println(button.downDuration());
}

void setup()
{
  button.begin();
  button.setLongPeriodDuration(durationLong);
  
  button.onDown(downEvent);
  button.onDownLong(downLongEvent);
  button.onUp(upEvent);
  button.onUpLong(upLongEvent);

  Serial.begin(9600);   
  Serial.println("Starting up");
}

void loop()
{
  button.poll();
  delay(10);
}
