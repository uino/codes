/**
 * AC_DS3232 library.
 * Code by Arthur Chargueraud
 * This code is GNU LGPL v3.
 *
 * -------- In construction -------
 *
 * todo: alarm2 does not seem to work currently
 *
 */

#ifndef AC_DS3232_h
#define AC_DS3232_h

#include <DS3232RTC.h>
#include <Arduino.h>
#include <Wire.h>

// EEPROM address // On-board 32k byte EEPROM; 128 pages of 32 bytes each (4096 bytes)
#define I2C24C32Add 0x57

class AC_DS3232 : public DS3232RTC
{
public:

  /*
   * Construct a new ds3232 object.
   * Inherits all methods from DS3232RTC.
   */
  AC_DS3232() : DS3232RTC() {}

// TODO: comment and move to cpp file

  void setAlarm1(time_t unixTime) {
    setAbsoluteAlarm(ALM1_MATCH_DATE, unixTime);
  }

  void setAlarm2(time_t unixTime) {
    setAbsoluteAlarm(ALM2_MATCH_DATE, unixTime);
  }

  void setRelativeAlarm1(long nbSecondsFromNow) {
    setRelativeAlarm(ALM1_MATCH_DATE, nbSecondsFromNow);
  }

  // warning: precision of alarm2 is to the minute only
  void setRelativeAlarm2(long nbSecondsFromNow) {
    setRelativeAlarm(ALM2_MATCH_DATE, nbSecondsFromNow);
  }

  void setRelativeAlarm1(int nbDays, int nbHours, int nbMinutes, int nbSeconds) {
    setRelativeAlarm1(nbSecondsIn(nbDays, nbHours, nbMinutes, nbSeconds));
  }

  void setRelativeAlarm2(int nbDays, int nbHours, int nbMinutes, int nbSeconds) {
    setRelativeAlarm2(nbSecondsIn(nbDays, nbHours, nbMinutes, nbSeconds));
  }

  boolean alarm1() {
    return alarm(ALARM_1);
  }

  boolean alarm2() {
    return alarm(ALARM_2);
  }

  /* EEPROM accesses, adapted from
     https://forum.hobbycomponents.com/viewtopic.php?f=80&t=1946
     https://raw.githubusercontent.com/dtu-mekatronik/
   */

  void EEStartWrite(unsigned int addr) {
    Wire.beginTransmission(I2C24C32Add);
    Wire.write((byte)(addr >> 8));
    Wire.write((byte)(addr & 0xFF));
  }

  void EEWriteByte(byte data) {
    Wire.write(data);
  }

  void EEEndWrite() {
    Wire.endTransmission();
  }

  void EEWrite(unsigned int addr, byte data) {
    EEStartWrite(addr);
    EEWriteByte(data);
    EEEndWrite();
  }

  void EEStartRead(unsigned int addr) {
    Wire.beginTransmission(I2C24C32Add);
    Wire.write((byte)(addr >> 8));
    Wire.write((byte)(addr & 0xFF));
    Wire.endTransmission();
  }

  byte EEReadByte() {
    Wire.requestFrom(I2C24C32Add, 1);
    if (Wire.available()) {
      return Wire.read();
    } else {
      return 0xFF; // ERROR!
    }
  }

  byte EERead(unsigned int addr) {
    EEStartRead(addr);
    return EEReadByte();
  }

  /* Access to the aging register */

  // Offset register is at 0x10
  // valid values for the input parameter offset ranges from -128 to 127
  void setAgingOffset(int offset) {
      if (offset < 0) offset += 256;
      Wire.beginTransmission(RTC_ADDR); // DS3232 I2C Address is 0x68
      Wire.write(0x10);
      Wire.write(offset);
      Wire.endTransmission();
  }

private:

  long nbSecondsIn(int nbDays, int nbHours, int nbMinutes, int nbSeconds) {
    return nbSeconds + 60 * (nbMinutes + 60 * (nbHours + 24 * nbDays));
  }

  void setAbsoluteAlarm(ALARM_TYPES_t alarmType, time_t unixTime) {
    time_t t = unixTime;
    byte sec = second(t);
    if (alarmType == ALARM_2) {
      sec = 0;
    }
    setAlarm(alarmType, sec, minute(t), hour(t), day(t));
  }

  void setRelativeAlarm(ALARM_TYPES_t alarmType, long nbSecondsFromNow) {
    setAbsoluteAlarm(alarmType, get() + nbSecondsFromNow);
  }


};

#endif
