/**
 * Demo for the AC_UT390B library, for controlling a telemeter.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * The demo requires:
 * - a computer connected on Serial
 * - a UT390B telemeter connected on Serial1
 * - a button for launching measures.
 * 
 * It reports a measure when the button is clicked.
 * It reports failed measurements.
 *
 * The demo can be easily adapted to work without a computer
 * and/or without a button.
 *
 */

#include <AC_UT390B.h>
#include <ACbuttonLong.h>

const int buttonPin = 6;

ACbuttonLong button(buttonPin);

AC_UT390B telemeter(&Serial1);

boolean ongoingMeasure = false;

void queryMeasure() {
  ongoingMeasure = true;
  telemeter.requestMeasure();
}

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  telemeter.begin();
  telemeter.setLaserVisible(true);
  Serial.println("Laser on");

  button.begin();
  button.onUp(queryMeasure);
}

void loop()
{
  button.poll();

  if (ongoingMeasure) {
    telemeter.processMeasure();
    if (telemeter.isMeasureComplete()) {
       Serial.print("Measure: ");
      if (telemeter.isMeasureSuccessful()) {
        float value = telemeter.getMeasure();
        Serial.println(value, 3);
      } else {
        Serial.println("error");
      }
      ongoingMeasure = false;
    }
  }

  delay(10);
}
