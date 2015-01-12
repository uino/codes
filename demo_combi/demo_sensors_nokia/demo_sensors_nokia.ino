/**
 * Demo for the SHT1x with Nokia screen.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 *
 * Show in real time the state of a SHT1x sensor on
 * a Nokia5110 display. 
 *
 * A button can used to go from a panel to the next.
 *
 */

#include <SPI.h>
#include <Wire.h>
#include <SHT1x.h>
#include <Time.h>
#include <AC_Nokia5110.h>
#include <DS3232RTC.h> 
#include <AC_Button.h>
#include <avr/pgmspace.h>
#include "defs.h"


//*****************************************************************
/* Configuration */


// AC_Nokia5110 pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin
// AC_Nokia5110 screen(3, 4, 5, 11, 13, 9);
AC_Nokia5110 screen(3, 4, 5, 11, 13, 8);

// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// AC_Button pins: buttonPin
AC_Button button(2);

// SHT1x pins: dataPin, clockPin
SHT1x sht1x(9, 10);

// Menu panels
const int nbPanels = 1;
int currentPannel = 0;
boolean needRefresh = true;

// Current measure
const int measurePeriod = 2000; // milliseconds
long dateLastMeasure = 0;
Record currentMeasure;   //includ def.h


//*****************************************************************
/* Measure */

void makeMeasures(Record& r) {
  r.date = ds3232.get(); 
  r.temperature = sht1x.readTemperatureC();
  r.humidity = sht1x.readHumidity();
}


//*****************************************************************
/* Serial functions */

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

void printMeasureOnSerial(Record& r) {
  Serial.println(F("---------------------------"));
  printTime(r.date);
  Serial.print(F("Temperature: "));
  Serial.println(r.temperature, 2);  
  Serial.print(F("Humidity: "));
  Serial.println(r.humidity, 2);  
  Serial.println(F("---------------------------"));
}


//*****************************************************************
/* Screen functions */

// TODO: move some of this to a library

const int screenNbRows = AC_Nokia5110::LCD_ROWS;
const int screenNbCols = AC_Nokia5110::LCD_COLS; 
const int bufferRowLength = 30; // = screenNbCols+1 (but using more characters for safety)

// prints a two-digit nonnegative int into a given target string (of length >= 2)
void timeItemIntoString(int v, char* str) {
  str[0] = '0' + v / 10;
  str[1] = '0' + v % 10;
}

// prints the date into a given target string
// str needs to have a least 18 characters (15 if no year)
void timeIntoString(time_t t, char* str, boolean showYear) {
  char* pos = str;
  if (showYear) {
    timeItemIntoString(year(t), pos);
    *pos = '/';
    pos += 3;
  }
  timeItemIntoString(month(t), pos);
  *pos = '/';
  pos += 3;
  timeItemIntoString(day(t), pos);
  *pos = ' ';
  pos += 3;
  timeItemIntoString(hour(t), pos);
  *pos = ':';
  pos += 3;
  timeItemIntoString(minute(t), pos);
  *pos = ':';
  pos += 3;
  timeItemIntoString(second(t), pos);
  pos += 2;
  *pos = '\0';
}

void string_of_float(float value, int nbChars, int precision, char* target) {
  dtostrf(value, nbChars, precision, target); 
}

void displayMeasures1(Record r) {
  char buffer[bufferRowLength];
  int floatNbChars = 5;
  int floatPrecision = 2;
  int line = 0;
  screen.clearDisplay(); 

  // date
  timeIntoString(r.date, buffer, false);
  screen.setString(buffer, line, 0);

  // temperature
  line++;
  screen.setString("Temp.:", line, 0);
  string_of_float(r.temperature, floatNbChars, floatPrecision, buffer);
  screen.setString(buffer, line, 9);

  // humidity
  line++;
  screen.setString("Humi.:", line, 0);
  string_of_float(r.humidity, floatNbChars, floatPrecision, buffer);
  screen.setString(buffer, line, 9);

  screen.updateDisplay(); 
}


//*****************************************************************
/* Menu */

void shortClick() {
  currentPannel = (currentPannel + 1) % nbPanels;
  needRefresh = true;
}

void displayPanel() {
  if (! needRefresh)
    return;
  if (currentPannel == 0) {
    displayMeasures1(currentMeasure);
  } // else .. // to handle more panels
  needRefresh = false;
}


//*****************************************************************
/* Setup */

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
}

void setup()
{
  // Serial
  Serial.begin(9600); 

  // Screen
  screen.begin();
  screen.updateDisplay(); 
  screen.setContrast(60); 

  // DS3232 set initial time
  initializeTime();

  // Time.h library (adjust from ds3232 every 300 seconds)
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  // Button
  button.begin();
  button.onUp(shortClick);

  Serial.print("Starting"); 
}



//*****************************************************************
/* Main */

void loop()
{
  long now = millis();
  if (dateLastMeasure - now > measurePeriod) {
    dateLastMeasure = now;
    makeMeasures(currentMeasure);
    printMeasureOnSerial(currentMeasure);
    needRefresh = true;
  }

  // periodic actions
  button.poll();
 displayPanel();
  delay(50);
}


