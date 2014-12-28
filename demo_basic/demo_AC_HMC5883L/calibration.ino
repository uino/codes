/**
 * Demo for the AC_HMC5883L library.
 * Code by Arthur Chargueraud.
 * This code is GPL.



 *  ****************** TODO: udpate...




 *
 * Use a magnetometer to measure the strength of the magnetic field,
 * and to obtain the direction of the North.
 * For the latter, we assume magnetic declination to be registered 
 * and the compass device to be horizontal.
 *
 * The demo begins with a calibration phase during which the sensor
 * should be rotated in the plane in all directions for 10 seconds.
 *
 */

#include <AC_HMC5883L.h>

AC_HMC5883L compass;

// 1 degree 14 minutes of magnetic declination
const int declinationDegrees = 1;
const int declinationMinutes = 24;

const long durationCalibration = 10000; // milliseconds
long dateCalibrationStart;
boolean isCalibrating;

// values for calibration phase
Vector minv = { 1000, 1000, 1000 };
Vector maxv = { -1000, -1000, -1000 };
Vector scale = {0, 0, 0} ;
Vector offset = {1, 1, 1} ;

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
compass.setGainMode(AC_HMC5883L::GAIN_8_1); // todo : TEMPORARY

  compass.setDeclinationDegrees(declinationDegrees, declinationMinutes); 

  isCalibrating = true;
  dateCalibrationStart = millis();
  Serial.println("Starting calibration: rotate slowly all around for 10 seconds.");
  delay(1000);
}
  

void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}

void loop()
{
  compass.update();

  // display of raw values
  if (true) { // use "true" to see raw measures
    AC_HMC5883L::Vector v = compass.getRawVector();
    Serial.print("Raw values: ");
    Serial.print("\t");
    displayFloat(v.x, 7, 3);
    Serial.print("\t");
    displayFloat(v.y, 7, 3);
    Serial.print("\t");
    displayFloat(v.z, 7, 3);
    Serial.println("");
    delay(500);
    return;
  }

  if (isCalibrating) {
    AC_HMC5883L::Vector v = compass.getVector();
    minv.x = min(minv.x, v.x);
    maxv.x = max(maxv.x, v.x);
    minv.y = min(minv.y, v.y);
    maxv.y = max(maxv.y, v.y);
    minv.z = min(minv.z, v.z);
    maxv.z = max(maxv.z, v.z);
    
    offset.x = - (maxv.x + minv.x) / 2;
    offset.y = - (maxv.y + minv.y) / 2;
    offset.z = - (maxv.z + minv.z) / 2;
    scale.x = 1 / ((maxv.x - minv.x) / 2);
    scale.y = 1 / ((maxv.y - minv.y) / 2);
    scale.z = 1 / ((maxv.z - minv.z) / 2);

    Serial.print("Calibration offset: ");
    displayFloat(offset.x, 7, 3);
    Serial.print("\t");
    displayFloat(offset.y, 7, 3);
    Serial.print("\t");
    displayFloat(offset.z, 7, 3);
    Serial.println("");
    Serial.print("Calibration scale: ");
    displayFloat(scale.x, 7, 3);
    Serial.print("\t");
    displayFloat(scale.y, 7, 3);
    Serial.print("\t");
    displayFloat(scale.z, 7, 3);
    Serial.println("");
  }

  if (isCalibrating && millis() - dateCalibrationStart > durationCalibration) {
    Serial.print("Setting offset: ");
    displayFloat(offset.x, 7, 3);
    Serial.print(", ");
    displayFloat(offset.y, 7, 3);
    Serial.print(", ");
    displayFloat(offset.z, 7, 3);
    Serial.println("");
    Serial.print("Setting scale: ");
    displayFloat(scale.x, 7, 3);
    Serial.print(", ");
    displayFloat(scale.y, 7, 3);
    Serial.print(", ");
    displayFloat(scale.z, 7, 3);
    Serial.println("");

    compass.setOffset(offset);
    compass.setScale(scale);
    isCalibrating = false;
  }

  if (! isCalibrating) {
    float heading = RAD_TO_DEG * compass.getHeading();
    float norm = compass.getNorm() * 100.; // in microtesla
    float normXY = compass.getNormXY() * 100.; // in microtesla
    Serial.print("Heading: ");
    displayFloat(heading, 7, 1);
    Serial.print("\t\tNormXY: "); 
    displayFloat(normXY, 7, 1); 
    Serial.print("\t\tNorm: "); 
    displayFloat(norm, 7, 1); 
    Serial.println("");
  }

  delay(500);
}
