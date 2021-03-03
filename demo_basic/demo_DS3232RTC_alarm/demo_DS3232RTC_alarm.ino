/**
 * Demo for the DS3232RTC library. using alarm.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * The code includes several examples, to be selected below.
 */

#include <DS3232RTC.h>
#include <Time.h>
#include <Wire.h>

DS3232RTC ds3232;


//*****************************************************************
// Select which example to run (see description below)

const int example = 0;
// const int example = 1;
// const int example = 2;

void initializeAlarm() {
  if (example == 0) {
    // alarm every seconds
    ds3232.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0, 0);

  } else if (example == 1) {
    // alarm once per minute when seconds are equal to 14
    ds3232.setAlarm(ALM1_MATCH_SECONDS, 14, 0, 0, 0);

  } else if (example == 2) {
    // alarm once per minute when seconds are equal to 14, with interrupt
    ds3232.alarmInterrupt(ALARM_1, true);
    ds3232.setAlarm(ALM1_MATCH_SECONDS, 14, 0, 0, 0);
  }

  // initialize wave output
  // ds3232.squareWave(SQWAVE_1_HZ);
}


//*****************************************************************


void printTime(time_t t) {
  Serial.print(year(t));
  Serial.print('/');
  Serial.print(month(t));
  Serial.print('/');
  Serial.print(day(t));
  Serial.print(' ');
  Serial.print(hour(t));
  Serial.print(':');
  Serial.print(minute(t));
  Serial.print(':');
  Serial.print(second(t));
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up");
  initializeAlarm();
}


void loop(void)
{
  Serial.print("Date: ");
  printTime(ds3232.get());

  if (ds3232.alarm(ALARM_1)) {
    Serial.println("Alarm 1 triggered");
  }

  delay(1000);
}
