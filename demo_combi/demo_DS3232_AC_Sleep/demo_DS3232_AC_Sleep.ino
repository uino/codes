/**
 * Demo for the AC_Sleep library, for use with DS3232.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * Code adapted from: http://www.gammon.com.au/forum/?id=11497 (sketch J).
 *
 * The purpose of this code is to power-off the arduino and wake it up
 * at regular intervals.
 *
 */

#include <Time.h>
#include <Wire.h>
#include <AC_Sleep.h> 
#include <AC_DS3232.h> 


const byte pinLED = 13;

const byte idInterrupt = 0; // corresponds to pinInterrupt
const byte pinInterrupt = 2; 

const int periodWakeUp = 5; // seconds

AC_DS3232 ds3232;

void printTime(time_t t) {
  Serial.print("Date: ");
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

void wake() {
  AC_Sleep::terminateSleep();
  detachInterrupt(idInterrupt);
  Serial.println("Exit sleep");
}

void setup() {
  digitalWrite(pinInterrupt, HIGH); // enable pull up

  pinMode(7, OUTPUT); digitalWrite(7, HIGH);  // TEMP: power the ds3232

  pinMode(pinLED, OUTPUT);
  
  Serial.begin(9600);   
  Serial.println("Starting up");

  // periodic alarm
  ds3232.setRelativeAlarm1(periodWakeUp);
  ds3232.alarmInterrupt(ALARM_1, true); 

  /* alternative code:
  // alarm once per minute when seconds are equal to 5 more than now
  int sec = ds3232.get() % 60 + 5;
  ds3232.alarmInterrupt(ALARM_1, true); 
  ds3232.setAlarm(ALM1_MATCH_SECONDS, sec, 0, 0, 0); 
  */
}

int counter = 0;
int totalCounter = 0;

void loop(void)
{
  counter++;
  totalCounter++;
  Serial.print("Total counter: ");
  Serial.println(totalCounter);
  printTime(ds3232.get());

  // blink pin
  digitalWrite(pinLED, HIGH);
  delay(100);
  digitalWrite(pinLED, LOW);
  delay(100);

  // goes to sleep after 5 iterations
  if (counter == 5) {
    Serial.print("Enter sleep");
    delay(100);
    counter = 0;

    ds3232.alarm(ALARM_1); // reset alarm
    ds3232.setRelativeAlarm1(periodWakeUp);
    AC_Sleep::enterSleepOnInterrupt(idInterrupt, wake, LOW);
  }

}

