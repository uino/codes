/**
 * Code by Arthur Chargueraud.
 *
 */

#include <AC_HMC5883L.h>
#include <AC_UT390B.h>
#include <AC_Button.h>

const int compassPowerPin = 4;

const int buttonMeasurePin = 2;
const int buttonZeroPin = 6;
AC_Button buttonMeasure(buttonMeasurePin);
AC_Button buttonZero(buttonZeroPin);
AC_UT390B telemeter(&Serial1);
AC_HMC5883L compass;


const byte rotPin1 = A6; // lower
const byte rotPin2 = A7; // upper

float compassOffset = 0.;
float accelOffset = 0.;

const int accelPinX = A0;
const int accelPinY = A1;
const int accelPinZ = A2;

// calibration = TODO rename.
const int minX = 260;
const int maxX = 396;
const int minY = 260;
const int maxY = 396;
const int minZ = 280;
const int maxZ = 415;

// float version of the Arduino "map" function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


boolean ongoingMeasure = false;
int idMeasure = 0;
float phi;
float theta;
float dist;
Vector points[2];
int nbPointsTargeted = 2;

// TODO: move to lib vector
Vector vectorOfSpherical(float dist, float phi, float theta) {
  float z = dist * sin(theta);
  float p = dist * cos(theta);
  float y = p * sin(phi);
  float x = p * cos(phi);
  return Vector(x, y, z);
}

void printVector(Vector v) {
  Serial.print("X: ");
  Serial.print(v.x, 2);
  Serial.print("\t Y: ");
  Serial.print(v.y, 2);
  Serial.print("\t Z: ");
  Serial.print(v.z, 2);
}

// TODO: libe
Vector vectorSubstract(Vector v1, Vector v2) {
  return Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

float vectorNorm(Vector v) {
  return sqrt(sq(v.x) + sq(v.y) + sq(v.z));
}

float vectorDist(Vector v1, Vector v2) {
  return vectorNorm(vectorSubstract(v1, v2));
}

float getAccelAngleRaw() {
  int x = analogRead(accelPinX);
  int y = analogRead(accelPinY);
  int z = analogRead(accelPinZ);
  float xg = mapFloat(x, minX, maxX, 1., -1);
  float yg = mapFloat(y, minY, maxY, 1., -1);
  float zg = mapFloat(z, minZ, maxZ, 1., -1);
  float a = atan2(yg, xg) + M_PI/2;
  return a;
}

float getAccelAngle() {
   return getAccelAngleRaw(); // + accelOffset
}

float getCompassAngleRaw() {
  compass.update();
  return compass.getHeading();
}

float getCompassAngle() {
  float a = getCompassAngleRaw() + compassOffset;
  a = fmod(a + 6 * M_PI, 2 * M_PI);
  return a;
}

void rotZero() {
  compassOffset = - getCompassAngleRaw();
  // accelOffset = - getAccelAngleRaw();
}

void startMeasure() {
  telemeter.requestMeasure();
  float anglePhi = getCompassAngle();
  phi = anglePhi;
  float angleTheta = getAccelAngle();
  theta = angleTheta;
  if (nbPointsTargeted == 2) {
    Serial.print("Point ");
    Serial.print(idMeasure+1);
    Serial.print("\t  Phi: ");
    Serial.print(RAD_TO_DEG * anglePhi, 1);
    Serial.print("\t  Theta: ");
    Serial.print(RAD_TO_DEG * angleTheta, 1);
    Serial.print("\t");
  }
}

void queryMeasure() {
  ongoingMeasure = true;
  nbPointsTargeted = 2;
  startMeasure();
}

void reportPoint() {
  ongoingMeasure = true;
  nbPointsTargeted = 1;
  startMeasure();
}

void completeMeasure() {
  if (ongoingMeasure) {
    telemeter.processMeasure();
    if (telemeter.isMeasureComplete()) {
      if (telemeter.isMeasureSuccessful()) {
        dist = telemeter.getMeasure();
        Vector v = vectorOfSpherical(dist, phi, theta);
        if (nbPointsTargeted == 2) {
          Serial.print("\t Dist: ");
          Serial.println(dist, 3);
          Serial.print("  ==> ");
          printVector(v);
          Serial.println("");
        } else if (nbPointsTargeted == 1) {
          Serial.print("# ");
          Serial.print(v.x, 2);
          Serial.print(" ");
          Serial.print(v.y, 2);
          Serial.print(" ");
          Serial.print(v.z, 2);
          Serial.println("");
        }
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


void setup()
{

  // TODO --TEMPORARY, due to the wiring of the compas
  {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }

  digitalWrite(compassPowerPin, HIGH);

  Serial.begin(9600);   
  Serial.println("Starting up");

  compass.begin();
  /* earth
  compass.setGainMode(AC_HMC5883L::GAIN_0_8);
  compass.setOffset(144.500, 155.000, 349.000);
  compass.setScale(1.000, 0.866, 9.264);
  compass.setDeclinationDegrees(1, 24);
  */

  // magnet
  compass.setGainMode(AC_HMC5883L::GAIN_4_0);
  compass.setOffset(-221.500, 100.500, -46.500);
  compass.setScale(1.000,   1.015,   5.102);

  // flip x-axis of compass
  /*
  AC_HMC5883L::Vector scale = compass.getScale();
  scale.x = - scale.x;
  compass.setScale(scale);
  */

  telemeter.begin();
  telemeter.setOffset(-0.05);
  telemeter.setLaserVisible(true);
  Serial.println("Laser on");

  buttonMeasure.begin();
  buttonMeasure.onUp(queryMeasure);

  buttonZero.begin();
  buttonZero.onUp(rotZero);
  // buttonZero.onUp(reportPoint);
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
    float accelAngle = RAD_TO_DEG * getAccelAngle();
    float compassAngle = RAD_TO_DEG * getCompassAngle();
    Serial.print("Phi: ");
    displayFloat(compassAngle, 6, 1);
    Serial.print("   Theta: ");
    displayFloat(accelAngle, 6, 1);
    Serial.println();
  }
*/
  if (idMeasure == nbPointsTargeted) {

    if (nbPointsTargeted == 2) {
      float distance = vectorDist(points[0], points[1]);
      Serial.print("Distance: ");
      Serial.println(distance, 3);
      Serial.println("");
    } 
    idMeasure = 0;
  }

  delay(10);
}



    /* code for 2D
    // Loi des cosinus:  c^2 = a^2 + b^2 - 2*a*b*cos(alpha)
    float a = dist[0];
    float b = dist[1];
    float alpha = phi[1] - phi[0];
    float distance = sqrt(sq(a) + sq(b) - 2 * a * b * cos(alpha));
    */


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

  /*
  int v1 = analogRead(rotPin1);
  int v2 = 1023 - analogRead(rotPin2);
  float d1 = 300.0;
  float d2 = 300.0;
  float d = (d1 * v1 / 1023.0) + (d2 * v2 / 1023.0);
  float a = d / 180 * M_PI;
  return a;
  
  */