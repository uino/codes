/**
 * Demo for the AC_Button library, for basic clicks.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * The demo reports click events with their duration on the Serial port.
 */

#include <AC_Button.h>

const int buttonPin = 6;

AC_Button button(buttonPin);

void click() {
  Serial.print("click: ");
  Serial.println(button.downDuration());
}

void setup()
{
  button.begin();
  button.onUp(click);

  Serial.begin(9600);   
  Serial.println("Starting up");
}

void loop()
{
  button.poll();
  delay(10);
}
