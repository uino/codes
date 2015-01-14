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
