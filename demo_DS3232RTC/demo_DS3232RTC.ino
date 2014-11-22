/**
 * Demo for the DS3232RTC library.
 * Adapted from the examples associated with the library.
 */

#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  

DS3232RTC ds3232;

void initializeTime() {
  tmElements_t initTime;
  initTime.Second = 9;
  initTime.Minute = 9;
  initTime.Hour = 9;
  initTime.Wday = 1; // day of week, sunday is day 1
  initTime.Day = 11;
  initTime.Month = 4;
  initTime.Year = 44;
  ds3232.write(initTime);
  // alternative for passing directly a time_t
  // ds3232.set(makeTime(initTime));
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

void printTemperature() {
  int celsiusTimes4 = ds3232.temperature();
  float celsius = celsiusTimes4 / 4.0;
  Serial.print(celsius);
  Serial.println(); 
}

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  // set a given time into the ds3232
  initializeTime();

  // calls to ensure that the value now() is read from the
  // ds3232 every 300 seconds, and ajusted using millis()
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 
}

void loop(void)
{
  // print time read on the DS3232 directly
  Serial.print("Date ds3232.get():  ");
  printTime(ds3232.get());
  
  // print time computed from millis() with synchro every 5 minutes on the RTC
  Serial.print("Date now():         ");
  printTime(now()); 

  // print temperature
  Serial.print("Temperature: ");
  printTemperature();

  delay(2000);
}