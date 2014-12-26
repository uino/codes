/**
 * Code by Arthur Chargueraud.
 *
 */

#include <AC_HMC5883L.h>
#include <AC_UT390B.h>
#include <ACbuttonLong.h>

const int buttonMeasurePin = 6;
const int buttonZeroPin = 2;
ACbuttonLong buttonMeasure(buttonMeasurePin);
ACbuttonLong buttonZero(buttonZeroPin);
AC_UT390B telemeter(&Serial1);
AC_HMC5883L compass;

const byte rotPin1 = A6; // lower
const byte rotPin2 = A7; // upper

float angleOffset = 0.;

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

float getRotAngleRaw() {
  int v1 = analogRead(rotPin1);
  int v2 = 1023 - analogRead(rotPin2);
  float d1 = 300.0;
  float d2 = 300.0;
  float d = (d1 * v1 / 1023.0) + (d2 * v2 / 1023.0);
  float a = d / 180 * M_PI;
  return a;
}

void rotZero() {
  angleOffset = - getRotAngleRaw();
}

float getRotAngle() {
  float a = getRotAngleRaw() + angleOffset;
  a = fmod(a + 6 * M_PI, 2 * M_PI);
  return a;
}

float getRotAngleDegree() {
  return getRotAngle() / M_PI * 180;
}

void setup()
{
  pinMode(rotPin1, INPUT);
  pinMode(rotPin2, INPUT);


  // TODO --TEMPORARY, due to the wiring of the compas
  {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
  }

  Serial.begin(9600);   
  Serial.println("Starting up");

  compass.begin();
/*
  compass.setOffset(- ( -0.060), - (-0.178), 0);
  compass.setScale(1/0.233, 1/0.280, 0);
  compass.setOffset(- ( -0.047), - (-0.195), 0);
  compass.setScale(1/0.180, 1/0.213, 0);
  compass.setOffset(- ( -0.045), - (-0.194), 0);
  compass.setScale(1/0.181, 1/0.211, 0);
*/
  compass.setOffset(0.057, 0.177, 0);
  compass.setScale(4.129, 3.603, 0);

  // flip x-axis of compass
  AC_HMC5883L::Vector scale = compass.getScale();
  scale.x = - scale.x;
  compass.setScale(scale);


  compass.setDeclinationDegrees(1, 24);

  telemeter.begin();
  telemeter.setOffset(-0.05);
  telemeter.setLaserVisible(true);
  Serial.println("Laser on");

  buttonMeasure.begin();
  buttonMeasure.onUp(queryMeasure);

  buttonZero.begin();
  buttonZero.onUp(rotZero);
}

void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}


/*
float sq(float x) {
  return x * x;
}*/

long lastDisp = 0;


void loop()
{
  buttonMeasure.poll();
  buttonZero.poll();

  completeMeasure();

  if (millis() - lastDisp > 500) {
    compass.makeMeasure();
    float angleCompass = compass.getHeadingDegrees();
    float angleRot = getRotAngleDegree();
    float diff = fmod(angleCompass - angleRot + 360, 360);
    diff = min(diff, 360-diff);
    Serial.print("Potar: ");
    displayFloat(angleRot, 6, 1);
    Serial.print("\tCompass: ");
    displayFloat(angleCompass, 6, 1);
    Serial.print("\tDiff: ");
    displayFloat(diff, 6, 1);
    Serial.println();
  }


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



/*
     lastDisp = millis();
     int v1 = analogRead(rotPin1);
     int v2 = 1023 - analogRead(rotPin2);
     Serial.print("R1: ");
     Serial.print(v1);
     Serial.print("\tR2: ");
     Serial.print(v2);
     int r = v1 + v2;
     Serial.print("\tR: ");
     Serial.print(r);
     Serial.print("\tA: ");
     float a = r / 2046.0 * 590.0;
     Serial.println(a);

*/