/**
 * AC_DS1307 library.
 * Code by Arthur Chargueraud
 * Copyleft code.
 */

#ifndef AC_DS1307_h
#define AC_DS1307_h

#include <Arduino.h>
#include <Wire.h>

//#include <ArduinoRobot.h>

typedef struct {
  byte second;
  byte minute;
  byte hour;
  byte dayOfWeek;
  byte dayOfMonth;
  byte month;
  byte year; } AC_DS1307_str;

class AC_DS1307
{
public:

  AC_DS1307();

  /** 
    * This function must be called to initialize the device.
    */
  void begin();

  /** 
    * Sets the date and time on the ds1307, 
    * sets hour mode to 24 hour clock,
    * and starts the clock. 
    * Assumes valid arguments.
    */
  void setDate(
      byte second,        // 0-59
      byte minute,        // 0-59
      byte hour,          // 1-23
      byte dayOfWeek,     // 1-7
      byte dayOfMonth,    // 1-28/29/30/31
      byte month,         // 1-12
      byte year);         // 0-99

  void setDate(AC_DS1307_str date); 

  /** 
    * Configure the threshold delay for clicks considered long,
    * to be expressed in milliseconds. Default value is 200ms.
    */
  void getDate(
      byte *second,
      byte *minute,
      byte *hour,
      byte *dayOfWeek,
      byte *dayOfMonth,
      byte *month,
      byte *year);

  void getDate(AC_DS1307_str* date); 

//---------------------------------------------------------------------
private:
  static const int DS1307_I2C_ADDRESS = 0x68;
  byte decToBcd(byte val);
  byte bcdToDec(byte val); 

};

#endif
