//****************************************************************

// Wiring:
// 5,6 for TM1637; 
// A5,14 for HMC5883; 
// 8 for button

//****************************************************************

void beginCalibration();

//****************************************************************

#include <AC_Button.h>

// Button
const int longClickDuration = 1000; // milliseconds
const int buttonPin = 8;
AC_Button button(buttonPin);

void upEvent() {
  beginCalibration();
}

void buttonSetup() {
  button.begin();
  button.onUp(upEvent);
  // button.setLongPeriodDuration(longClickDuration);
  // button.onUpLong(upLongEvent);
}

/*
  void upEvent() {
    Serial.print("up short: ");
    Serial.println(button.downDuration());
  }

  void upLongEvent() {
    Serial.print("up long: ");
    Serial.println(button.downDuration());
  }
*/

//****************************************************************

#include <AC_TM1637.h>

const int displayPinCLK = 6;
const int displayPinDIO = 5;
const int displayBrightness = 10; // between 8 and 15
AC_TM1637 display(displayPinCLK, displayPinDIO);

void displaySetup() {
  display.setBrightness(displayBrightness);
  display.begin();
  display.setDigits(0,0,0,0);
}

//****************************************************************

#include <AC_HMC5883L.h>

// Magnetic declination equal to 1 degree 14 minutes
const int declinationDegrees = 1;
const int declinationMinutes = 24;

AC_HMC5883L compass;

const long durationCalibration = 12000; // milliseconds

const boolean useManualCalibration = true;
long dateCalibrationStart = 0;
boolean isCalibrating = false;
long dateLastShow = 0;  

void compassSetup() { 
  compass.begin(); 
  compass.setGainMode(AC_HMC5883L::GAIN_0_8);
  compass.setDeclinationDegrees(declinationDegrees, declinationMinutes);
  // manual calibration // TODO: declare vectors
  compass.setOffset(-12, 771.000, 574);
  compass.setScale(1.000,   0.874,   7.195);
}

void beginCalibration() {
  isCalibrating = true;
  dateCalibrationStart = millis();
  compass.beginCalibration();
  Serial.println("Starting calibration: rotate slowly all around for 12 seconds.");
}

void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}

void reportCalibration() {
  Vector offset = compass.getOffset();
  Vector scale = compass.getScale();
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

void checkCalibration() {
  if (isCalibrating) {
    compass.stepCalibration();
    // test for end
    if (millis() - dateCalibrationStart > durationCalibration) { 
      compass.endCalibration();
      isCalibrating = false;
      reportCalibration();
    }

    float remain = (durationCalibration - (millis() - dateCalibrationStart)) / 1000;
    display.showFloat(remain, 1);
    // display remaining calibration time every second
    if (millis() - dateLastShow > 1000) {
      dateLastShow = millis();
      Serial.print("Calibration remaining seconds: ");
      Serial.println((long) remain);
    } 
  }
}


//****************************************************************
// Serial

void serialSetup() {
  Serial.begin(9600);   
}

//****************************************************************
// setup

void setup()
{
  serialSetup();
  displaySetup();
  buttonSetup();
  compassSetup();
  Serial.println("Starting up");
}


//****************************************************************
// loop

void loop()
{
  button.poll();
  checkCalibration();

  if (! isCalibrating && millis() - dateLastShow > 200) {
    dateLastShow = millis();
    compass.update();

    float headRad = compass.getHeading();
    float headDeg = headRad * RAD_TO_DEG;

    display.showFloat(headDeg, 1);

    Serial.print("XY-heading: ");
    displayFloat(headDeg, 7, 1);
    Serial.println("");
  }

  delay(20);
}


