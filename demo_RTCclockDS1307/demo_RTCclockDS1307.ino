/**
 * Demo for the RTC clock library.
 * Code by Arthur Charguéraud.
 * This code is in the public domain.
 */

#include <RealTimeClockDS1307.h>

RealTimeClockDS1307 clock;  

void setup() {
  Serial.begin(9600);   
  Serial.println("Starting up");
  /* 
  byte v = clock.readData(4);//read a single value from a register
  Serial.println(v);
  clock.writeData(4, v+1);//write a single value to a register
  */
}

void loop() {
  clock.readClock();
  int year = clock.getYear();
  int month = clock.getMonth();
  int dayOfMonth = clock.getDay();
  int dayOfWeek = clock.getDayOfWeek();
  int hour = clock.getHours();
  int minute = clock.getMinutes();
  int second = clock.getSeconds();

  Serial.print(year, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("  ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print("  Day_of_week:");
  Serial.println(dayOfWeek, DEC);

  delay(1000);
}


