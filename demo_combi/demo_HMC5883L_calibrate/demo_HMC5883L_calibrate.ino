/**
 * Demo for calibration of a HMC5883L magnetometer.
 * Code by Arthur Chargueraud.
 * This code is GPL.
 *
 * The demo begins with a calibration phase during which the sensor
 * should be rotated in all directions for 15 seconds.
 *
 * During this phase, raw values and their extrema are reported.
 * If values exceed +/- 4096, it means that the gain is to small
 * and overflow occurs; use a higher gain value and try again.
 *
 * Then the calibration settings are printed out.
 * Then, XY-heading and calibrated measures are reported.
 *
 * At this point, a button can be pressed to assign an offset so
 * that the current direction is reported as 0 degrees.
 * 
 * The powering of the magnetometer may be controlled by a particular
 * digital pin, whose value is set to HIGH in order to turn on the 
 * magnetometer device.
 *
 * Make sure to select the gain to be used
 */

// comment out the line below if no power-control pin is used.
#define POWER_CONTROL_PIN 4


#include <AC_HMC5883L.h>
#include <AC_Button.h>

const int buttonZeroPin = 2;
AC_Button buttonZero(buttonZeroPin);

// AC_HMC5883L::GainMode gain = AC_HMC5883L::GAIN_0_8; // use for north-pointing compass
AC_HMC5883L::GainMode gain = AC_HMC5883L::GAIN_4_0; // higher gain needed for a magnet
AC_HMC5883L magneto;

const long durationCalibration = 15000; // milliseconds
const boolean skipCalibration = false; // set true to skip calibration

float angleOffset = 0.;
long dateCalibrationStart;
boolean isCalibrating;
long dateLastShow = 0;

float getAngleRaw() {
  Vector v = magneto.getVector();
  return v.phi();
}

void rotZero() {
  Serial.println("Assign to zero");
  angleOffset = - getAngleRaw();
}

float getAngle() {
  float a = getAngleRaw() + angleOffset;
  a = fmod(a + 6 * M_PI, 2 * M_PI);
  if (a > M_PI) 
    a -= 2 * M_PI;
  return a;
}

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

#ifdef POWER_CONTROL_PIN
    
    // TEMPORARY (only because other devices need to be off)
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    /**/
    pinMode(POWER_CONTROL_PIN, OUTPUT);
    digitalWrite(POWER_CONTROL_PIN, HIGH);
    delay(50);
#endif

  buttonZero.begin();
  buttonZero.onUp(rotZero);

  magneto.begin(); 
  magneto.setGainMode(gain);
  
  if (skipCalibration) {
    isCalibrating = false;
    // for debugging, these values can be used instead of calibration
    magneto.setOffset(132.500, 233.500, -91.000);
    magneto.setScale(1.000,   0.910,   1.095);
  } else {
    isCalibrating = true;
    dateCalibrationStart = millis();
    Serial.println("Starting calibration: rotate slowly all around for 15 seconds.");
    magneto.beginCalibration();
  }

  delay(50);
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
  buttonZero.poll();

  if (isCalibrating) {
    magneto.stepCalibration();
  }

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

    magneto.update();
    float a = getAngle();
    Serial.print("XY-heading: ");
    displayFloat(a * RAD_TO_DEG, 7, 1);
    Serial.print("\t");

    Vector m = magneto.measureCalibratedVector();
    Serial.print("Calibrated: ");
    Serial.print("\t");
    Serial.print((int) m.x);
    Serial.print("\t");
    Serial.print((int) m.y);
    Serial.print("\t");
    Serial.print((int) m.z);
    Serial.println("");
  }

  delay(20);
}
