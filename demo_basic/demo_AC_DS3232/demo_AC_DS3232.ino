/**
 * Demo for the AC_DS3232 library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */


#include <AC_DS3232.h>

AC_DS3232 ds3232;

const int delayAlarm1 = 5; // seconds

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

void setup() {
  Serial.begin(9600);   
  Serial.println("Starting up");

  ds3232.setRelativeAlarm1(delayAlarm1);
}

void loop() {
  printTime(ds3232.get());

  if (ds3232.alarm1()) {
    Serial.println("Alarm 1");
    ds3232.setRelativeAlarm1(delayAlarm1);
  }

  delay(1000);
}


