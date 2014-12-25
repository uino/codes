/**
 * ACbuttonLong Library. See "ACbuttonLong.h" for information.
 */

#include "AC_UT390B.h"

AC_UT390B::AC_UT390B(HardwareSerial* serialPort)
{
  this->serialPort = serialPort;
}

void AC_UT390B::begin() {
  serialPort->begin(115200);
}


boolean AC_UT390B::readDistance(float* distance) {
  return false;
}
