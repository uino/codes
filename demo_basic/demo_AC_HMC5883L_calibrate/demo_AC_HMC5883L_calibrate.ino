/**
 * Demo for using the AC_HMC5883L library to calibrate the magneto-meter.
 * Code by Arthur Chargueraud.
 * This code is GPL.
 *
 * The demo begins with a calibration phase during which the sensor
 * should be rotated in all directions for 15 seconds.
 * During this phase, raw values and their extrema are reported.
 * Then the calibration settings are printed out.
 * Then, calibrated values are reported out, both as raw and as Gauss values.
 *
 */

#include <AC_HMC5883L.h>

AC_HMC5883L magneto;

const long durationCalibration = 15000; // milliseconds
long dateCalibrationStart;
boolean isCalibrating;

/*
Setting offset: -49.000, -109.000, 1578.500
Setting scale:   1.000,   0.941,   0.612

*/


void setup()
{
  // TODO --TEMPORARY, due to the current wiring of the device
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

  magneto.begin(); 
   //magneto.setGainMode(AC_HMC5883L::GAIN_8_1); // todo : TEMPORARY
  magneto.setGainMode(AC_HMC5883L::GAIN_0_8);

  isCalibrating = true;
  dateCalibrationStart = millis();

  Serial.println("Starting calibration: rotate slowly all around for 15 seconds.");
  magneto.beginCalibration();
  delay(1000);
}
  
void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}

long dateLastShow = 0;

int nb = 0;
Vector sum = Vector();

void loop()
{
  if (isCalibrating) {
    magneto.stepCalibration();
  }

  nb++;
  sum = sum + magneto.measureGainAdjustedVector();


  if (isCalibrating && millis() - dateLastShow > 200) {
    dateLastShow = millis();

    // display of raw values
    Vector v = magneto.measureRawVector();
    Serial.print("Raw: ");
    Serial.print("\t");
    Serial.print((int) v.x);
    Serial.print("\t");
    Serial.print((int) v.y);
    Serial.print("\t");
    Serial.print((int) v.z);
    Serial.print("\t");

    // display of extremum values
    Serial.print("Extrem: ");
    Serial.print((int) magneto.minv.x);
    Serial.print(" / ");
    Serial.print((int) magneto.maxv.x);
    Serial.print("  ");
    Serial.print((int) magneto.minv.y);
    Serial.print(" / ");
    Serial.print((int) magneto.maxv.y);
    Serial.print("  ");
    Serial.print((int) magneto.minv.z);
    Serial.print(" / ");
    Serial.print((int) magneto.maxv.z);
    Serial.println("");
  } 

  if (isCalibrating && millis() - dateCalibrationStart > durationCalibration) {
    magneto.endCalibration();
    isCalibrating = false;

    Vector offset = magneto.getOffset();
    Vector scale = magneto.getScale();
    Serial.print("Setting offset: ");
    displayFloat(offset.x, 7, 3);
    Serial.print(", ");
    displayFloat(offset.y, 7, 3);
    Serial.print(", ");
    displayFloat(offset.z, 7, 3);
    Serial.println("");
    /*
     */
   Serial.print("Setting scale: ");
    displayFloat(scale.x, 7, 3);
    Serial.print(", ");
    displayFloat(scale.y, 7, 3);
    Serial.print(", ");
    displayFloat(scale.z, 7, 3);
    Serial.println("");
  }

  if (! isCalibrating && millis() - dateLastShow > 200) {
    dateLastShow = millis();

    Vector m = magneto.measureCalibratedVector();
    Serial.print("Calibrated: ");
    Serial.print("\t");
    Serial.print((int) m.x);
    Serial.print("\t");
    Serial.print((int) m.y);
    Serial.print("\t");
    Serial.print((int) m.z);
    Serial.println("");

  /*
 */


/*
    magneto.update();
    Vector v = magneto.getVector();
    Serial.print("XY-heading: ");
    displayFloat(v.phi() * RAD_TO_DEG, 7, 1);
    Serial.print("\t");

    Serial.print("average: ");
    displayFloat(sum.phi() * RAD_TO_DEG, 7, 1);
    Serial.println("");

    sum = Vector();
    nb = 0;
*/


/*
        // display of raw values
    Vector r = magneto.measureRawVector();
    Serial.print("Raw: ");
    Serial.print("\t");
    Serial.print((int) r.x);
    Serial.print("\t");
    Serial.print((int) r.y);
    Serial.print("\t");
    Serial.print((int) r.z);
    Serial.println("");

*/
         /*

    Serial.print(" \tField: ");
    displayFloat(v.x, 7, 3);
    Serial.print(" ");
    displayFloat(v.y, 7, 3);
    Serial.print(" ");
    displayFloat(v.z, 7, 3);
    Serial.println("");
   */
  }

  delay(20);
}
