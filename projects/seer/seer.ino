/**
 * Code by Arthur Chargueraud.
 *
 */

#include <AC_HMC5883L.h>
#include <AC_UT390B.h>
#include <ACbuttonLong.h>

typedef AC_HMC5883L::Vector Vector;

const int buttonMeasurePin = 6;
const int buttonZeroPin = 2;
ACbuttonLong buttonMeasure(buttonMeasurePin);
ACbuttonLong buttonZero(buttonZeroPin);
AC_UT390B telemeter(&Serial1);
AC_HMC5883L compass;

const byte rotPin1 = A6; // lower
const byte rotPin2 = A7; // upper

float angleOffset = 0.;
float accelOffset = 0.;

const int accelXPin = A0;
const int accelYPin = A1;
const int accelZPin = A2;


boolean ongoingMeasure = false;
int idMeasure = 0;
float phi;
float theta;
float dist;
Vector points[2];

Vector vectorOfSpherical(float dist, float phi, float theta) {
  float z = dist * sin(theta);
  float p = dist * cos(theta);
  float y = p * sin(phi);
  float x = p * cos(phi);
  Vector v = { x, y, z };
  return v;
}

void printVector(Vector v) {
  Serial.print("X: ");
  Serial.print(v.x, 2);
  Serial.print("\t Y: ");
  Serial.print(v.y, 2);
  Serial.print("\t Z: ");
  Serial.print(v.z, 2);
}

Vector vectorSubstract(Vector v1, Vector v2) {
  Vector v = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
  return v;
}

float vectorNorm(Vector v) {
  return sqrt(sq(v.x) + sq(v.y) + sq(v.z));
}

float vectorDist(Vector v1, Vector v2) {
  return vectorNorm(vectorSubstract(v1, v2));
}



float getAccelAngleRaw() {
  int xRead = analogRead(accelXPin);
  int yRead = analogRead(accelYPin);
  // not used int zRead = analogRead(zPin);
  int vmin = 257;
  int vmax = 397;
  float x = map(xRead, vmin, vmax, -1000, 1000);
  float y = map(yRead, vmin, vmax, -1000, 1000);
  return - atan2(x, y); // due to placement of accel
}

float getAccelAngle() {
   return getAccelAngleRaw(); 
}

void queryMeasure() {
  ongoingMeasure = true;
  telemeter.requestMeasure();
  compass.update();
  float anglePhi = compass.getHeading();
  phi = anglePhi;
  float angleTheta = getAccelAngle();
  theta = angleTheta;
  Serial.print("Point ");
  Serial.print(idMeasure+1);
  Serial.print("\t  Phi: ");
  Serial.print(RAD_TO_DEG * anglePhi, 1);
  Serial.print("\t  Theta: ");
  Serial.print(RAD_TO_DEG * angleTheta, 1);
}

void completeMeasure() {
  if (ongoingMeasure) {
    telemeter.processMeasure();
    if (telemeter.isMeasureComplete()) {
      if (telemeter.isMeasureSuccessful()) {
        dist = telemeter.getMeasure();
        Serial.print("\t Dist: ");
        Serial.println(dist, 3);
        Vector v = vectorOfSpherical(dist, phi, theta);
        Serial.print("  ==> ");
        printVector(v);
        Serial.println("");

        points[idMeasure] = v;
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
  // accelOffset = - getAccelAngleRaw();
}

float getRotAngle() {
  float a = getRotAngleRaw() + angleOffset;
  a = fmod(a + 6 * M_PI, 2 * M_PI);
  return a;
}

void setup()
{

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
  /*
  AC_HMC5883L::Vector scale = compass.getScale();
  scale.x = - scale.x;
  compass.setScale(scale);
  */


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

  /*
  if (millis() - lastDisp > 500) {
    compass.update();
    float accel = RAD_TO_DEG * getAccelAngle();
    float angleCompass = RAD_TO_DEG * compass.getHeading();
    float angleRot = RAD_TO_DEG * getRotAngle();
    float diff = fmod(angleCompass - angleRot + 360, 360);
    diff = min(diff, 360-diff);
    Serial.print("Accel: ");
    displayFloat(accel, 6, 1);
    Serial.print("\tPotar: ");
    displayFloat(angleRot, 6, 1);
    Serial.print("\tCompass: ");
    displayFloat(angleCompass, 6, 1);
    Serial.print("\tDiff: ");
    displayFloat(diff, 6, 1);
    Serial.println();
  }
  */

  if (idMeasure == 2) {

    float distance = vectorDist(points[0], points[1]);

    /* code for 2D
    // Loi des cosinus:  c^2 = a^2 + b^2 - 2*a*b*cos(alpha)
    float a = dist[0];
    float b = dist[1];
    float alpha = phi[1] - phi[0];
    float distance = sqrt(sq(a) + sq(b) - 2 * a * b * cos(alpha));
    */
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