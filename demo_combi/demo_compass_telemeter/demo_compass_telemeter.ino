/**
 * Demo for using the AC_HMC5883L library to calibrate the magneto-meter.
 * Code by Arthur Chargueraud.
 * This code is GNU GPL v3.
 *
 * The demo begins with a calibration phase during which the sensor
 * should be rotated in all directions for 15 seconds.
 * During this phase, raw values and their extrema are reported.
 * Then the calibration settings are printed out.
 * Then, calibrated values are reported out, both as raw and as Gauss values.
 *
 */

#include <AC_HMC5883L.h>
#include <AC_UT390B.h>
#include <AC_Button.h>
const int buttonZeroPin = 2;


int pinCompass[2] = { 3, 4 };

AC_Button buttonZero(buttonZeroPin);
AC_UT390B telemeter(&Serial1);
AC_HMC5883L magneto[2];

float angleOffset[2] = { 0., 0.};

float getRotAngleRaw(int i) {
  digitalWrite(pinCompass[i], HIGH);
  delay(50);
  magneto[i].update();
  digitalWrite(pinCompass[i], LOW);
  delay(50);
  Vector v = magneto[i].getVector();
  float a = v.phi();
  /*if (a > M_PI)
    a = a - 2*M_PI; */
  return a;
}

void rotZero() {
  Serial.println("ok");
  for (int i = 0; i < 2; i++) {
    angleOffset[i] = - getRotAngleRaw(i);
  }
}

float getRotAngle(int i) {
  float a = getRotAngleRaw(i) + angleOffset[i];
  a = fmod(a + 6 * M_PI, 2 * M_PI);
  if (a > M_PI) 
    a -= 2 * M_PI;
  return a;
}



void setup()
{
  // TODO --TEMPORARY, due to the current wiring of the device
  {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }

  Serial.begin(9600);   
  Serial.println("Starting up");

  telemeter.begin();
  telemeter.setOffset(-0.05);
  telemeter.setLaserVisible(true);
  Serial.println("Laser on");

  buttonZero.begin();
  buttonZero.onUp(rotZero);

  magneto[0].begin(); 
  magneto[0].setGainMode(AC_HMC5883L::GAIN_0_8);
  /* #4
  magneto[0].setOffset(-132.500, 291.500, 452.000);
  magneto[0].setScale( -1.000, 0.869, 9.271);
  */
  /*
  // #3
  magneto[0].setOffset(132.500, 233.500, -91.000);
  magneto[0].setScale(1.000,   0.910,   1.095);
  */
  // #3 hori
  magneto[0].setOffset(256.500, 451.000, 589.500);
  magneto[0].setScale(- 1.000,   0.791,   4.248);


  // #5
  magneto[1].begin(); 
  magneto[1].setGainMode(AC_HMC5883L::GAIN_2_5);
  magneto[1].setOffset(-346.000,  69.500,  13.000);
  magneto[1].setScale(- 1.000,   1.009,   5.220);

  Serial.println("begin");
}

void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}

long dateLastShow = 0;

void loop()
{
  buttonZero.poll();

  if (millis() - dateLastShow > 200) {
    if (true) {
      int id = 1;
      digitalWrite(pinCompass[id], HIGH);
      delay(20);
      Vector v = magneto[id].measureRawVector();
      digitalWrite(pinCompass[id], LOW);
      delay(20);
      Serial.print("XY-heading: ");
      float d = getRotAngle(id) * RAD_TO_DEG;
      displayFloat(d, 7, 1);
      Serial.print(" Raw: ");
      Serial.print("\t");
      Serial.print((int) v.x);
      Serial.print("\t");
      Serial.print((int) v.y);
      Serial.print("\t");
      Serial.print((int) v.z);
      Serial.println("");

    } else {
      dateLastShow = millis();
      Serial.print("XY-heading; Compass");
      float d0 = getRotAngle(0) * RAD_TO_DEG;
      float d1 = getRotAngle(1) * RAD_TO_DEG;
      displayFloat(d0, 7, 1);
      Serial.print("\tMagnet:");
      displayFloat(d1, 7, 1);
      Serial.print("\tDiff:");
      displayFloat(abs(d1-d0), 7, 1);
      Serial.println("");
    }
  }

  delay(20);
}
