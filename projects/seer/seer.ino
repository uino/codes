/**
 * Code by Arthur Chargueraud.
 *
 */

#include <AC_HMC5883.h>
#include <AC_UT390B.h>
#include <ACbuttonLong.h>

const int buttonPin = 6;
ACbuttonLong button(buttonPin);
AC_UT390B telemeter(&Serial1);
AC_HMC5883 compass;

boolean ongoingMeasure = false;
int idMeasure = 0;
float heading[2];
float dist[2];

void queryMeasure() {
  ongoingMeasure = true;
  telemeter.requestMeasure();
  compass.makeMeasure();
  float angle = compass.getHeading();
  float angleDegree = angle / M_PI * 180.0;
  heading[idMeasure] = angle;
  Serial.print("Point ");
  Serial.print(idMeasure+1);
  Serial.print("\t Angle: ");
  Serial.print(angleDegree, 1);
}

void completeMeasure() {
  if (ongoingMeasure) {
    telemeter.processMeasure();
    if (telemeter.isMeasureComplete()) {
      if (telemeter.isMeasureSuccessful()) {
        float value = telemeter.getMeasure();
        Serial.print("\t Dist: ");
        Serial.println(value, 3);
        dist[idMeasure] = value;
        idMeasure++;
      } else {
        Serial.println("Error");
        idMeasure = 0;
      }
      ongoingMeasure = false;
    }
  }
}

void setup()
{
  // TODO --TEMPORARY, due to the wiring of the compas
  {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
  }

  Serial.begin(9600);   
  Serial.println("Starting up");

  compass.begin();
  compass.setDeclinationDegrees(1, 24);

  telemeter.begin();
  telemeter.setLaserVisible(true);
  Serial.println("Laser on");

  button.begin();
  button.onUp(queryMeasure);
}

void displayFloat(float value) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 6;
  char buffer[nbDigits];
  dtostrf(value, 6, 1, buffer);
  Serial.print(buffer);
}
/*
float sq(float x) {
  return x * x;
}*/

void loop()
{
  button.poll();
  completeMeasure();

  if (idMeasure == 2) {
    // Loi des cosinus:  c^2 = a^2 + b^2 - 2*a*b*cos(alpha)
    float a = dist[0];
    float b = dist[1];
    float alpha = heading[1] - heading[0];
    float distance = sqrt(sq(a) + sq(b) - 2 * a * b * cos(alpha));
    Serial.print("Distance: ");
    Serial.println(distance, 3);
    Serial.println("");
    idMeasure = 0;
  }

  delay(10);
}



