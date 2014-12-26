/**
 * Demo for the AC_HMC5883L library.
 * Code by Arthur Chargueraud.
 * This code is GPL.
 *
 * Use a magnetormeter to measure the strength of the magnetic field,
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

const long durationCalibration = 12000; // milliseconds
long dateCalibrationStart;
boolean isCalibrating;


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
  compass.setDeclinationDegrees(declinationDegrees, declinationMinutes); 

  isCalibrating = true;
  dateCalibrationStart = millis();
  Serial.println("Starting calibration: rotate slowly all around for 12 seconds.");
  compass.beginCalibration();
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
  if (false) { // use "true" to see raw measures
    float x = compass.getX();
    float y = compass.getY();
    float z = compass.getZ();
    Serial.print("Raw values: ");
    Serial.print("\t");
    displayFloat(x, 7, 3);
    Serial.print("\t");
    displayFloat(y, 7, 3);
    Serial.print("\t");
    displayFloat(z, 7, 3);
    Serial.println("");
    delay(500);
    return;
  }

  if (isCalibrating) {
    compass.stepCalibration();
  }

  if (isCalibrating && millis() - dateCalibrationStart > durationCalibration) {
    compass.endCalibration();
    isCalibrating = false;

    AC_HMC5883L::Vector offset = compass.getOffset();
    AC_HMC5883L::Vector scale = compass.getScale();
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
