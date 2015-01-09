/**
 * Code for setting the time of the DS3232 based on the 
 * computer's time.
 *
 * Code by Arthur Chargueraud.
 * Copyleft code.
 *
 * This code is intended to be used on linux, with the "ino"
 * toolchain. Launch the script "set_time.sh" for generating
 * the file "local_time.h" storing the current unix timestamp,
 * and then compile and upload the present program.
 *
 * For accurate result, set the delayOfCompilationAndUpload 
 * to the right date (trial-and-error process).
 */

#include <DS3232RTC.h> 
#include "local_time.h"

// To be adjusted depending on your hardware
const long delayOfCompilationAndUpload = 5; // in seconds

DS3232RTC ds3232;

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

  ds3232.set(CURRENT_TIME + delayOfCompilationAndUpload);
  Serial.println("Time initialized");
}

void loop(void)
{
  Serial.print("DS3232 time:  ");
  printTime(ds3232.get());
  delay(2000);
}