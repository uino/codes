/**
 * Demo for the ADXL335 accelerometer, to measure acceleration 
 * in any direction.
 * Code by Arthur Chargueraud.
 * This code is GNU GPL v3.
 *
 * Reports how much acceleration (in any direction) is measured:
 * - on a logarithmic scale, with a value between 0 and 15 (always)
 * - on a linear scale, with a value between 30 and 16k (approximately)
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

Vector low, high;

void setup(){
  Serial.begin(115200); 
  Serial.println("Starting up"); 

  low = Vector();
  high = Vector();
}

const int delayBetweenReports = 200; // millis
long dateLastReport = 0;

// converts the norm of the acceleration vector (from normalized measures)
// into a value from 0 to 8 //--was: to 15.
int shock(float v) {
  /*const int nbBoundries = 15;
  float boundries[nbBoundries] = {  // approximately pow(1.4,x)/25
    0.05, 0.07, 0.10, 0.15, 0.22, 
    0.30, 0.42, 0.60, 0.82, 1.15, 
    1.60, 2.30, 3.10, 4.40, 6.20 };*/
  const int nbBoundries = 8;
  float boundries[nbBoundries] = {  
    0.038, 0.046, 0.052, 0.06, 0.08, 0.11, 0.22, 0.5 };
  for (int i = 0; i < nbBoundries; i++) {
    if (v < boundries[i])
      return i;
  }
  return nbBoundries;
}

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
    float strength = move.norm(); // between 0 and 16 roughtly
    /*
    Serial.print((int) (move.x * 100));
    Serial.print("\t");
    Serial.print((int) (move.y * 100));
    Serial.print("\t");
    Serial.print((int) (move.z * 100));
    Serial.print("\t");
    Serial.println((int) (move.norm() * 100));
    */
    // 
    Serial.print(shock(move.norm()));  
    Serial.print("\t");  
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

  delay(10);
}

