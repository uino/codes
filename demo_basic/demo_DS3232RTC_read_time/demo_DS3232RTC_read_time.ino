/**
 * Reading time from DS3232.
 * Code by Arthur Chargueraud.
 * Copyleft code.
 */

#include <DS3232RTC.h> 

DS3232RTC ds3232;

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
}

void loop(void)
{
  Serial.print("DS3232 time: ");
  printTime(ds3232.get());
  delay(2000);
}
