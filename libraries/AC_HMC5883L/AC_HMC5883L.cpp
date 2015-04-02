/**
 * AC_Button Library. See "AC_Button.h" for information.
 */

#include "AC_HMC5883L.h"

AC_HMC5883L::AC_HMC5883L()
{
  declination = 0.;
  offset = Vector(0., 0., 0.);
  scale = Vector(1., 1., 1.);
  measure = Vector(0., 0., 0.);
}

void AC_HMC5883L::setOffset(Vector offset) {
  this->offset = offset;
}

void AC_HMC5883L::setOffset(float x, float y, float z) {
  setOffset(Vector(x, y, z));
}

void AC_HMC5883L::setScale(Vector scale) {
  this->scale = scale;
}

Vector AC_HMC5883L::getOffset() {
  return offset;
}

Vector AC_HMC5883L::getScale() {
  return scale;
}

void AC_HMC5883L::setScale(float x, float y, float z) {
  setScale(Vector(x, y, z));
}

void AC_HMC5883L::setDeclination(float angle) {
  declination = angle;
}

void AC_HMC5883L::setDeclinationDegrees(float angleInDegrees) {
  setDeclination(angleInDegrees / 180. * M_PI);
}

void AC_HMC5883L::setDeclinationDegrees(float angleInDegrees, float minutes) {
  setDeclinationDegrees(angleInDegrees + (minutes / 60.));
}

void AC_HMC5883L::begin(GainMode gainMode) {
  Wire.begin();
  write8(REGISTER_MR_REG_M, 0x00);
  setGainMode(gainMode);
}

void AC_HMC5883L::begin() {
  begin(GAIN_1_3);
}

void AC_HMC5883L::write8(byte reg, byte value) {
  Wire.beginTransmission(deviceAddress);
  Wire.write((uint8_t) reg);
  Wire.write((uint8_t) value);
  Wire.endTransmission();
}

void AC_HMC5883L::setGainMode(GainMode gainMode) {
  this->gainMode = gainMode;
  gain = gainForMode(gainMode);
  write8(REGISTER_CRB_REG_M, (byte) gainMode);
}

AC_HMC5883L::GainMode AC_HMC5883L::getGainMode() {
  return gainMode;
}

float AC_HMC5883L::gainForMode(GainMode gainMode) {
  switch (gainMode) {
    case GAIN_0_8:
      return 1370;
    case GAIN_1_3:
      return 1090;
    case GAIN_1_9:
      return 820;
    case GAIN_2_5:
      return 660;
    case GAIN_4_0:
      return 440;
    case GAIN_4_7:
      return 390;
    case GAIN_5_6:
      return 330;
    case GAIN_8_1:
      return 230;
  } 
  return 0; // assert false
}

Vector AC_HMC5883L::measureRawVector() {
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
  return Vector(x, y, z);
}

Vector AC_HMC5883L::measureCalibratedVector() {
  Vector v = measureRawVector();
  v.x = scale.x * (offset.x + v.x);
  v.y = scale.y * (offset.y + v.y);
  v.z = scale.z * (offset.z + v.z);
  return v;
}

Vector AC_HMC5883L::measureGainAdjustedVector() {
  Vector v = measureCalibratedVector();
  v.x /= gain;
  v.y /= gain;
  v.z /= gain;
  return v;
}

void AC_HMC5883L::update() {
  measure = measureGainAdjustedVector();
}

Vector AC_HMC5883L::getVector() {
  return measure;
}

float AC_HMC5883L::getX() {
  return measure.x;
}

float AC_HMC5883L::getY() {
  return measure.y;
}

float AC_HMC5883L::getZ() {
  return measure.z;
}

float AC_HMC5883L::getNorm() {
  return measure.norm();
}

float AC_HMC5883L::getNormXY() {
  Vector& m = measure;
  return sqrt(m.x * m.x + m.y * m.y);
}

float AC_HMC5883L::getHeading() {
  float heading = measure.phi(); // in range (-PI,+PI]
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

void AC_HMC5883L::beginCalibration() {
  minv = Vector(1000, 1000, 1000);
  maxv = Vector(-1000, -1000, -1000);
}

void AC_HMC5883L::endCalibration() {
  Vector offset, scale;
  offset.x = - (maxv.x + minv.x) / 2;
  offset.y = - (maxv.y + minv.y) / 2;
  offset.z = - (maxv.z + minv.z) / 2;
  float rangex = maxv.x - minv.x;
  float rangey = maxv.y - minv.y;
  float rangez = maxv.z - minv.z;
  scale.x = 1.;
  /*
  scale.y = 1.;
  scale.z = 1.;
  */
  scale.y = rangex / rangey;
  scale.z = rangex / rangez;
  setOffset(offset);
  setScale(scale);
}

void AC_HMC5883L::stepCalibration() {
  Vector v = measureRawVector();
  minv.x = min(minv.x, v.x);
  maxv.x = max(maxv.x, v.x);
  minv.y = min(minv.y, v.y);
  maxv.y = max(maxv.y, v.y);
  minv.z = min(minv.z, v.z);
  maxv.z = max(maxv.z, v.z);
}


