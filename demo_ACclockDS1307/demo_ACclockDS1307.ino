/**
 * Demo for the ACclock library.
 * Code by Arthur Charguéraud.
 * This code is in the public domain.
 */


#include <ACclockDS1307.h>

ACclockDS1307 myclock;

void setup()
{
  myclock.setup();

  Serial.begin(9600);   
  Serial.println("Starting up");
  
  // Change these values to what you want to set your clock to.
  // You probably only want to set your clock once and then remove
  // the setDateDs1307 call.

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  second = 45;
  minute = 3;
  hour = 7;
  dayOfWeek = 5;
  dayOfMonth = 17;
  month = 4;
  year = 8;
  myclock.setDate(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
}

void loop()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  myclock.getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print("  ");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print("  Day_of_week:");
  Serial.println(dayOfWeek, DEC);

  delay(1000);
}


