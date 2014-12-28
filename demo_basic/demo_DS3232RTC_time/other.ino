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
  // alternative: ds3232.set(makeTime(initTime));
  Serial.println("Time initialized");
}

void printTime() {
  Serial.print("Date:    ");
  Serial.print(year()); 
  Serial.print('/');
  Serial.print(day());
  Serial.print('/');
  Serial.print(month());
  Serial.print(' ');
  Serial.print(hour());
  Serial.print(':');
  Serial.print(minute());
  Serial.print(':');
  Serial.print(second());
  Serial.println(); 

  /* Note the alternative:
  time_t myTime = now();
  Serial.print(second(myTime)); 
  Serial.println(); 
  */
}

void printTimeRTC() { 
  Serial.print("DateRTC: ");
  time_t myTime = ds3232.get();
  Serial.print(year(myTime)); 
  Serial.print('/');
  Serial.print(day(myTime));
  Serial.print('/');
  Serial.print(month(myTime));
  Serial.print(' ');
  Serial.print(hour(myTime));
  Serial.print(':');
  Serial.print(minute(myTime));
  Serial.print(':');
  Serial.print(second(myTime));
  Serial.println(); 
}

void printTemperature() {
  int celsiusTimes4 = ds3232.temperature();
  float celsius = celsiusTimes4 / 4.0;
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(); 
}

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  initializeTime();

  setSyncProvider(ds3232.get); 
  // setSyncInterval(300); // nb seconds between synchronizations 
  if (timeStatus() != timeSet) 
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");      

}

void loop(void)
{
  printTimeRTC();
  printTime();
  printTemperature();
  delay(2000);
}


