/**
 * ACbuttonLong Library. See "ACbuttonLong.h" for information.
 */

#include "AC_UT390B.h"

AC_UT390B::AC_UT390B(HardwareSerial* serialPort)
{
  this->serialPort = serialPort;
  timeout = 0;
  status = IDLE;
}

void AC_UT390B::setOffset(float offset) {
  this->offset = offset;
}

void AC_UT390B::setTimeout(int timeout) {
  this->timeout = timeout;
}

void AC_UT390B::setLaserVisible(boolean visible) {
  if (visible) {
    serialPort->write("*100515#");
  }
}

void AC_UT390B::begin() {
  serialPort->begin(115200);
  status = IDLE;
  serialPort->write("*100515#");
}

void AC_UT390B::emptySerial() {
  while (serialPort->available() > 0) {
    char c = (char) serialPort->read();
  }
}

void AC_UT390B::requestMeasure() {
  if (timeout != 0) {
    dateRequest = millis();
  }
  emptySerial();
  bufferPos = 0;
  parsingData = false;
  status = ACQUIRE;
  serialPort->write("*00004#"); // code for a single measure
}

float AC_UT390B::getMeasureFromBuffer() {
  // for debugging: 
  //   buffer[bufferPos] = '\0';
  //   Serial.println(buffer);
  int mul = 1;
  long res = 0;
  // read the value between digits at index 9 and 13
  for (int i = 13; i >= 9; i--) {
    res += mul * (buffer[i] - '0');
    mul *= 10;
  }
  return res / 1000.0;
}

void AC_UT390B::processMeasure() {
  while (serialPort->available() > 0) {
    char c = (char) serialPort->read();
    // for debugging: Serial.print(c);
    if (c == '*') { // data start
      parsingData = true;
      bufferPos = 0;
    }
    if (c == '#') { // data end
      parsingData = false;
      if (bufferPos == bufferMaxLength) { // expecting a 16-char message
        measure = offset + getMeasureFromBuffer();
        status = READY;
      }
    }
    if (parsingData == true && bufferPos < bufferMaxLength && c >= '0') { 
      buffer[bufferPos++] = c;
    }
  }
  if ((timeout != 0) && (millis() - dateRequest > timeout)) {
    status = ERROR;
  }
}

boolean AC_UT390B::isMeasureComplete() {
  return (status == READY) || (status == ERROR);
}

boolean AC_UT390B::isMeasureSuccessful() {
  return (status == READY);
}

float AC_UT390B::getMeasure() {
  return measure;  
}

