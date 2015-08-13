/**
 * Demo for the AC_HMC5883L library to control a magnetometer.
 * Code by Arthur Chargueraud.
 * This code is GNU GPL v3.
 *
 * This demo only reports raw values as reported by the sensor.
 * Make sure to use the appopriate gain value. GAIN_0_8 works
 * for earth magnetic field, but higher values should be used
 * with magnets.
 * 
 * The demo_HMC5883L_calibrate contains code for calibration
 * and use as a compass.
 *
 * Wiring:
 * - GND
 * - VCC +5V
 * - SCL : pin A5
 * - SDA : pin A4
 */

#include <AC_HMC5883L.h>

AC_HMC5883L::GainMode gain = AC_HMC5883L::GAIN_0_8; // use for north-pointing compass
// AC_HMC5883L::GainMode gain = AC_HMC5883L::GAIN_4_0; // higher gain needed for a magnet
AC_HMC5883L magneto;

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  magneto.begin(); 
}

void loop()
{
  Vector v = magneto.measureRawVector();
  Serial.print("Raw: ");
  Serial.print("\t");
  Serial.print((int) v.x);
  Serial.print("\t");
  Serial.print((int) v.y);
  Serial.print("\t");
  Serial.print((int) v.z);
  Serial.println("");
  delay(200);
}



