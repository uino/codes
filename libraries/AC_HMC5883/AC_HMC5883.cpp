/**
 * ACbuttonLong Library. See "ACbuttonLong.h" for information.
 */

#include "AC_HMC5883.h"

AC_HMC5883::AC_HMC5883()
{
  declination = 0.;
  measure = { 0., 0., 0. };
}

void AC_HMC5883::setDeclination(float angle) {
  declination = angle;
}

void AC_HMC5883::setDeclinationDegrees(float angleInDegrees) {
  setDeclination(angleInDegrees / 180. * M_PI);
}

void AC_HMC5883::setDeclinationDegrees(float angleInDegrees, float minutes) {
  setDeclinationDegrees(angleInDegrees + (minutes / 60.));
}

void AC_HMC5883::begin(GainMode gainMode) {
  Wire.begin();
  write8(REGISTER_MR_REG_M, 0x00);
  setGainMode(gainMode);
}

void AC_HMC5883::begin() {
  begin(GAIN_1_3);
}

void AC_HMC5883::write8(byte reg, byte value) {
  Wire.beginTransmission(deviceAddress);
  Wire.write((uint8_t) reg);
  Wire.write((uint8_t) value);
  Wire.endTransmission();
}

void AC_HMC5883::setGainMode(GainMode gainMode) {
  this->gainMode = gainMode;
  setGainXYZ();
  write8(REGISTER_CRB_REG_M, (byte) gainMode);
}

void AC_HMC5883::setGainXYZ() {
  switch (gainMode) {
    case GAIN_1_3:
      gainXY = 1100;
      gainZ  = 980;
      break;
    case GAIN_1_9:
      gainXY = 855;
      gainZ  = 760;
      break;
    case GAIN_2_5:
      gainXY = 670;
      gainZ  = 600;
      break;
    case GAIN_4_0:
      gainXY = 450;
      gainZ  = 400;
      break;
    case GAIN_4_7:
      gainXY = 400;
      gainZ  = 255;
      break;
    case GAIN_5_6:
      gainXY = 330;
      gainZ  = 295;
      break;
    case GAIN_8_1:
      gainXY = 230;
      gainZ  = 205;
      break;
  } 
}

void AC_HMC5883::makeMeasure() {
  // Request 6 bytes (2 bytes for each of the 3 axes), and wait for data
  Wire.beginTransmission(deviceAddress);
  Wire.write(REGISTER_OUT_X_H_M);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t) 6);
  while (Wire.available() < 6); // wait for data
  int x = Wire.read() << 8; 
  x |= Wire.read(); 
  int z = Wire.read() << 8; 
  z |= Wire.read(); 
  int y = Wire.read() << 8; 
  y |= Wire.read();
  measure.x = x / gainXY;
  measure.y = y / gainXY;
  measure.z = z / gainZ;
}

AC_HMC5883::Vector AC_HMC5883::getMeasure() {
  return measure;
}

float AC_HMC5883::getX() {
  return measure.x;
}

float AC_HMC5883::getY() {
  return measure.y;
}

float AC_HMC5883::getZ() {
  return measure.z;
}

float AC_HMC5883::getNorm() {
  Vector& m = measure;
  return sqrt(m.x * m.x + m.y * m.y + m.z * m.z);
}

float AC_HMC5883::getNormXY() {
  Vector& m = measure;
  return sqrt(m.x * m.x + m.y * m.y);
}

float AC_HMC5883::getHeading() {
  float heading = atan2(measure.y, measure.x); // in range (-PI,+PI]
  heading += declination;
  // put value in the range [0,2PI)
  if (heading < 0) {
    heading += 2*PI;
  }
  if (heading > 2*PI) { // might not be needed, but added for safety
    heading -= 2*PI;
  }
  return heading;
}

float AC_HMC5883::getHeadingDegrees() {
  return getHeading() / M_PI * 180.; 
}


