/**
 * Demo for the AC_HMC5883 library.
 * Code by Arthur Chargueraud.
 * This code is GNU GPL v3.
 *
 * Use 3 magnetormeters from the same board
 *
 */

#include <AC_HMC5883.h>


const int pinOffset = 3;
AC_HMC5883 compass[3];
float shift[3] = { 0, 0, 0 }; // can use offsets here.


void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  for (int i = 0; i < 3; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
    compass[i].setDeclinationDegrees(1, 24);
  }

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
  for (int i = 0; i < 3; i++) {
    //Serial.println("m");
    delay(400);
    digitalWrite(pinOffset+i, HIGH);
    delay(100);
    compass[i].begin();
    delay(20);

    compass[i].makeMeasure();
    float heading = compass[i].getHeadingDegrees();
    Serial.print("H");
    Serial.print(i+1);
    Serial.print(":");
    displayFloat(heading - shift[i]);
    Serial.print("\t");
    Serial.println("");
    digitalWrite(pinOffset+i, LOW);
  }
  Serial.println("");

  delay(300);
}
