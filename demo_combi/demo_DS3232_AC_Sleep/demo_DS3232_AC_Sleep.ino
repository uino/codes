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


const byte idInterrupt = 0; // corresponds to pinInterrupt
const byte pinInterrupt = 2; 

const int periodWakeUp = 5; // seconds

int counter = 0;
int totalCounter = 0;
boolean isSleeping = false;
long dateLastAlarm = 0;



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
}

void setup() {
  pinMode(7, OUTPUT); digitalWrite(7, HIGH);  // TEMP: power the ds3232
  
  Serial.begin(9600);   
  Serial.println("Starting up");

  // init date
  dateLastAlarm = ds3232.get();

  // periodic wake up signal
  digitalWrite(pinInterrupt, HIGH); // enable pull up
  ds3232.alarmInterrupt(ALARM_1, true); 
}

void loop(void)
{
  if (isSleeping) {
    isSleeping = false;
    Serial.println("Exit sleep");
  }

  counter++;
  totalCounter++;
  Serial.print("Total counter: ");
  Serial.println(totalCounter);
  printTime(ds3232.get());
  delay(300);

  // goes to sleep after 5 iterations
  if (counter == 5) {
    counter = 0;
    Serial.println("Enter sleep");
    delay(100);

    ds3232.alarm1(); // reset alarm
    dateLastAlarm += periodWakeUp;
    time_t timeNow = ds3232.get();
    if (dateLastAlarm <= timeNow) { // if target in the past, catch up
      dateLastAlarm = timeNow + periodWakeUp;
    }
    ds3232.setAlarm1(dateLastAlarm);
    isSleeping = true;
    AC_Sleep::enterSleepOnInterrupt(idInterrupt, wake, LOW);
  }

}

