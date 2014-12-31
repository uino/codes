/**
 * Demo for the ADXL335 accelerometer.
 * Code by Arthur Chargueraud.
 * This code is GPL.
 *
 * Display raw values (as reported by the device)
 * and calibrated values (expressed relative to local gravity).
 * It also reports the angle of the rotation around the Z-axis,
 * (assumging that Z-arrow always points horizontally, and that
 * initially X-arrow points horizontally and Y-arrow points vertically).
 *
 * To calibrate the device, place each of the 3 axis of the 
 * accelerometer facing downwards and then upwards, and observe
 * the minimum and maximum raw values reported (when the device
 * is still). These values should be registered below.
 * The values depend both on your device and on the local gravity.
 *
 * Note that some of the calibration code appears in the 
 * AC_ADXL335 library.
 *
 * Wiring: 
 * - connect x-output to pinX  (A0 by default)
 * - connect y-output to pinY  (A1 by default)
 * - connect z-output to pinZ  (A1 by default)
 * - connect VCC to +3.3V or +5V 
 */
 
#include <Arduino.h>

const int pinX = A0;
const int pinY = A1;
const int pinZ = A2;

// calibration.
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

void setup(){
  Serial.begin(9600); 
  Serial.println("Starting up"); 
}

void loop(){
  
  int x = analogRead(pinX);
  int y = analogRead(pinY);
  int z = analogRead(pinZ);

  Serial.print("Raw: ");
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(z);
  Serial.print(" ");

  float xg = mapFloat(x, minX, maxX, 1., -1);
  float yg = mapFloat(y, minY, maxY, 1., -1);
  float zg = mapFloat(z, minZ, maxZ, 1., -1);

  Serial.print("  Calibrated:\t");
  Serial.print(xg, 3);
  Serial.print("\t");
  Serial.print(yg, 3);
  Serial.print("\t");
  Serial.print(zg, 3);
  Serial.print("\t");

  Serial.print("XY-angle: ");
  float a = atan2(yg, xg) + M_PI/2;
  Serial.print(RAD_TO_DEG * a, 1);
  Serial.println("");

  delay(200);
}

