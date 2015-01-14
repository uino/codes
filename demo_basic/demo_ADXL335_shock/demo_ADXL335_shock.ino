/**
 * Demo for the ADXL335 accelerometer, to measure acceleration 
 * in any direction.
 * Code by Arthur Chargueraud.
 * This code is GNU GPL v3.
 *
 * Reports how much acceleration (in any direction) is measured.
 *
 * Wiring: 
 * - connect x-output to pinX  (A0 by default)
 * - connect y-output to pinY  (A1 by default)
 * - connect z-output to pinZ  (A1 by default)
 * - connect VCC to +3.3V or +5V 
 */
 
#include <Arduino.h>
#include <AC_Vector.h>

const int pinX = A0;
const int pinY = A1;
const int pinZ = A2;

// calibration.
const int minX = 270;
const int maxX = 408;
const int minY = 261;
const int maxY = 402;
const int minZ = 261;
const int maxZ = 397;

// float version of the Arduino "map" function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup(){
  Serial.begin(115200); 
  Serial.println("Starting up"); 
}

Vector low, high;
const int delayBetweenReports = 20; // millis
long dateLastReport = 0;

void loop(){

  int x = analogRead(pinX);
  int y = analogRead(pinY);
  int z = analogRead(pinZ);

  float xg = mapFloat(x, minX, maxX, 1., -1);
  float yg = mapFloat(y, minY, maxY, 1., -1);
  float zg = mapFloat(z, minZ, maxZ, 1., -1);

  low.x = min(low.x, xg);
  low.y = min(low.y, yg);
  low.z = min(low.z, zg);

  high.x = max(high.x, xg);
  high.y = max(high.y, yg);
  high.z = max(high.z, zg);

  long dateNow = millis();
  if (dateNow - dateLastReport > delayBetweenReports) {
    dateLastReport = dateNow;
    Vector move = high - low;  // each direction between +/- 3.2 roughly
    float strength = move.norm(); // between 0 and 12 roughtly
    /*
    Serial.print((int) (move.x * 100));
    Serial.print("\t");
    Serial.print((int) (move.y * 100));
    Serial.print("\t");
    Serial.print((int) (move.z * 100));
    Serial.print("\t");
    Serial.println((int) (move.norm() * 100));
    */
    Serial.println((int) (move.norm() * 1000) ); 
    /*
    int nb = 1 + (int) (3 * strength); // at most 36 X's
    for (int i = 0; i < nb; i++) {
      Serial.print("X");
    }
    Serial.println("");
    */

    low = Vector(xg, yg, zg);
    high = Vector(xg, yg, zg);
  }

  delay(1);
}

