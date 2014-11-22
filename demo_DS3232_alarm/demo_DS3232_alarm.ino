/**
 * Demo for the DS3232RTC library. using alarm.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */

#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  

DS3232RTC ds3232;

tmElements_t initTime;

void initializeTime() {
  initTime.Second = 9;
  initTime.Minute = 9;
  initTime.Hour = 9;
  initTime.Wday = 1; // day of week, sunday is day 1
  initTime.Day = 9;
  initTime.Month = 9;
  initTime.Year = 9;
  ds3232.write(initTime);
  Serial.println("Time initialized");
}

void printTime(time_t t) {
  Serial.print(year(t)); 
  Serial.print('/');
  Serial.print(day(t));
  Serial.print('/');
  Serial.print(month(t));
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

  // set a given time into the ds3232
  initializeTime();
  
  int example = 1;
  if (example == 0) { 
    // alarm every seconds
    ds3232.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0, 0);  
  } else if (example == 1) {
    // alarm once per minute 5 seconds after start
    ds3232.setAlarm(ALM1_MATCH_SECONDS, initTime.Second+5, 0, 0, 0);  
  } else if (example == 2) {
    // alarm once per minute 5 seconds after start, with interrupt
    ds3232.alarmInterrupt(ALARM_1, true); 
    ds3232.setAlarm(ALM1_MATCH_SECONDS, initTime.Second+5, 0, 0, 0);  
  }
  // initialize wave output
  // ds3232.squareWave(SQWAVE_1_HZ);
}


void loop(void)
{
  Serial.print("Date: ");
  printTime(ds3232.get());

  if (ds3232.alarm(ALARM_1)) {
    Serial.println("Alarm 1 triggered");
  }

  delay(300);
}