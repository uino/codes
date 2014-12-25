/**
 * Demo for the AC_HMC5883 library.
 * Code by Arthur Chargueraud.
 * This code is GPL.
 *
 * Use a magnetormeter to measure the strength of the magnetic field,
 * and to obtain the direction of the North.
 * For the latter, we assume magnetic declination to be registered 
 * and the compass device to be horizontal.
 *
 */

#include <AC_HMC5883.h>

AC_HMC5883 compass;

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
}

void displayFloat(float value) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 6;
  char buffer[nbDigits];
  dtostrf(value, 6, 1, buffer);
  Serial.print(buffer);
}

void loop()
{
  compass.makeMeasure();
  float heading = compass.getHeadingDegrees();
  float norm = compass.getNorm() * 100.; // in microtesla
  float normXY = compass.getNormXY() * 100.; // in microtesla

  Serial.print("Heading: ");
  displayFloat(heading);
  Serial.print("\t\tNormXY: "); 
  displayFloat(normXY); 
  Serial.print("\t\tNorm: "); 
  displayFloat(norm); 
  Serial.println("");

  delay(2000);
}
