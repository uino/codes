/**
 * AC_UT390B library.
 * Code by Arthur Chargueraud
 * Adpated from http://youtu.be/qUgFji4Figw.
 *
 * This library can be used to control a UT390B telemeter.
 *
 */

#ifndef AC_UT390B_h
#define AC_UT390B_h

#include <Arduino.h>

class AC_UT390B
{
  public:

    /** 
      * Construct a new device given the 
      */
    AC_UT390B(HardwareSerial* serialPort);

    /** 
      * To be called from the main setup before the device can be used.
      */
    void begin();

    /** 
      * Read the distance into the float address, if successful measure;
      * and returns a boolean value indicating whether the read succeeded.
      */
    boolean readDistance(float* distance);

  private:
    HardwareSerial* serialPort;

};

#endif
