/**
 * Logger project.
 * Code by Anatole and Arthur Chargueraud
 * This code is GNU GPL v3.
 *
 * Goal is to measure values periodically from sensors,
 * (e.g., temperature/humidity using an SHT1x sensor)
 * log the values on a SD card, and displays the
 * measures on a Nokia5110 screen. 
 *
 *
 * // Not yet implemented:
 *   An DS3232 alarm is used to wake up the Arduino board
 *   when it is time to perform a measure. This allows for
 *   a long-term battery usage of the logger.
 *
 */

// #define ARTHUR
#define USE_SCREEN 

#include <Arduino.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <AC_RAM.h>
#include <SD.h>
#include <SHT1x.h>
#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  
#include <AC_RotatingPot.h>
#include <AC_Button.h>
#include "defs.h"

#ifdef USE_SCREEN
#include <AC_Nokia5100_light.h>
#endif


//*****************************************************************
/* Configuration */

// Debugging parameters
const boolean resetLogOnSetup = true; // true only for debugging
const boolean showSDContent = false;  // true only for debugging

// Log file parameters
char* filename = "logger.txt"; // name of log file (8+3 characters max)
boolean SDused = true; // indicates whether SD card should/can be used

// Serial : for debugging to PC
const long serialBaudRate = 115200; // faster than using 9600
const boolean serialUsed = true;

// Button
// (pins: buttonPin)
AC_Button button(2);
const int buttonSensitivity = 300;

// Rotating potentiometer
byte rotPin = A0;
const int rotSensitivity = 20; 
const boolean rotInverted = true; 
AC_RotatingPot rot(rotPin, rotSensitivity, rotInverted);

// Nokia5100 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
#ifdef USE_SCREEN
#ifdef ARTHUR
const int blPin = 8;
#else 
const int blPin = 7;
#endif
AC_Nokia5100_light screen(3, 4, 5, 11, 13, blPin);
#endif

// SHT1x : for measuring temperature and humidity
// (pins: dataPin, clockPin)
SHT1x sht1x(9, 10);

// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// SD card : for writing log file
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;

// Measure configuration
// see nbMeasures in defs.h
const char measureNames[nbMeasures][7] = { "humid.", "t_SHT1", "t_3232" }; // 6 chars exactly for each
// const char measureNames[nbMeasures][7] = { "h", "t", "t" }; // 6 chars exactly for each
const int floatPrecision = 3; // nb digits after decimal point


//*****************************************************************
/* Program variables */

// Current measure data structure
Record currentMeasure;

// Control of actions
long dateLastRecord = 0;
long dateLastScreen = 0;


//*****************************************************************
/* Performing measures */

void makeMeasures(Record& r) {
  r.date = ds3232.get(); 
  float* v = r.values;
#ifdef ARTHUR
  v[0] = 0;
  v[1] = 1;
  v[2] = 2;
#else 
  v[0] = sht1x.readHumidity();
  v[1] = sht1x.readTemperatureC();
  v[2] = ds3232.temperature() / 4.0; // - the ds3232 returns an int equal to 4 times the temperature.
#endif
}

//*****************************************************************
/* Debugging functions */

void reportSRAM() {
  if (!serialUsed) 
    return;
  Serial.print(F("SRAM free: "));
  Serial.println(AC_RAM::getFree());
}

void readFileSerialPrint() { // TODO: move to library
  if (! SDused)
    return;
  if (SD.exists(filename)) {
    File file = SD.open(filename, FILE_READ);
    if (! file) {
      Serial.println(F("readFileSerialPrint: error opening file"));
    } else {
      Serial.println(F("=========begin contents========="));
      while (file.available()) {
        byte v = file.read();
        Serial.write(v);
      }
      Serial.println(F("=========end contents========="));
      file.close();
    }
  } else {
    Serial.println(F("readFileSerialPrint: file not found"));
  }
}


//*****************************************************************
/* Printing function */

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
  Serial.println("---------------------------");
  printTime(r.date);
  for (int m = 0; m < nbMeasures; m++) {
    Serial.print(measureNames[m]);
    Serial.print(" ");
    Serial.println(r.values[m], floatPrecision);  
  }
  Serial.println("---------------------------");
}


//*****************************************************************
/* LCD functions */

#ifdef USE_SCREEN
const int screenNbRows = AC_Nokia5100_light::LCD_ROWS;
const int screenNbCols = AC_Nokia5100_light::LCD_COLS; 
#endif
const int bufferRowLength = 30; // = screenNbCols+1 (but using more characters for safety)

// prints a two-digit nonnegative int into a given target string (of length >= 2)
void timeItemIntoString(int v, char* str) {
  str[0] = v / 10;
  str[1] = v % 10;
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

void displayMeasures(Record r) {
#ifdef USE_SCREEN
  char buffer[bufferRowLength];
  screen.clearDisplay(); 
  timeIntoString(r.date, buffer, false);
  int line = 0;
  screen.setString(buffer, 0, 0);
  for (int m = 0; m < nbMeasures; m++) {
    string_of_float(r.values[m], 5, floatPrecision, buffer);
    screen.setString(measureNames[m], m+1, 0);
    screen.setString(":", m+1, 7);
    screen.setString(buffer, m+1, 9);
  }
  screen.updateDisplay(); 
#endif
}


//*****************************************************************
/* File functions */

void writeTime(File file, time_t t) {
  file.print(year(t)); 
  file.print('/');  
  file.print(month(t));
  file.print('/');
  file.print(day(t));
  file.print('\t');
  file.print(hour(t));
  file.print(':');
  file.print(minute(t));
  file.print(':');
  file.print(second(t));
}

File openLog(byte mode) {
  if (! SDused) 
    return File();
  File file = SD.open(filename, mode);
  if (! file) {
    Serial.println(F("Error opening file"));
    SDused = false;
  }
  return file;
}

void writeLogHeader() {
  File file = openLog(FILE_WRITE);
  if (! file)
    return;
  file.print(F("#date\ttime\tunixtime\t"));
  for (int m = 0; m < nbMeasures; m++) {
    file.print(measureNames[m]);  
    file.print("\t");
  }
  file.println("");
  file.close();
}

void writeRecordToFile(File file, Record& r) {
  time_t t = r.date;
  writeTime(file, t);
  file.print("\t");
  file.print(r.date);
  file.print("\t");
  for (int m = 0; m < nbMeasures; m++) {
    file.print(r.values[m], floatPrecision);  
    file.print("\t");
  }
  file.println("");
}

void writeRecordToLog(Record& r) {
  File file = openLog(FILE_WRITE);
  if (! file)
    return;
  writeRecordToFile(file, r);
  file.close();
}

void resetLog() {
  if (! SDused)
    return;
  SD.remove(filename);
  writeLogHeader();
}


//*****************************************************************
/* Control handlers */

void shortClick() {
  Serial.println(F("short click"));
}

void longClick() {
  Serial.println(F("long click"));
}

void rotChange() {
  Serial.println(F("rot change"));
}


//*****************************************************************
/* Setup */

void setup()
{
  // Serial port
  if (serialUsed) {
    Serial.begin(serialBaudRate); 
    Serial.println(F("Starting"));
  }
  reportSRAM();

#ifdef USE_SCREEN
  // LCD screen
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay(); 
  screen.setString("Load", 0, 0);
  screen.updateDisplay(); 
  delay(500);
#endif

  // SD
  if (SDused) {
    pinMode(SDhardwareCSPin, OUTPUT); 
    if (! SD.begin(SDselectPin)) {
      if (serialUsed) {
        Serial.println(F("Card failed or missing"));
      }
      SDused = false;
    } else {
      if (resetLogOnSetup) {
        Serial.println(F("Card reset"));
        File myFile = SD.open("example.txt", FILE_WRITE);
        if (myFile) {
          #ifdef USE_SCREEN
          screen.setString("SD ok", 0, 0);
          screen.updateDisplay(); 
          #endif
          Serial.println(F("ok"));
        } else {
          #ifdef USE_SCREEN
          screen.setString("SD bug", 0, 0);
          screen.updateDisplay(); 
          #endif
          Serial.println(F("bug"));
        }
        myFile.close();
        
        resetLog();
      }
    }
  }

  // RotatingPot
  rot.begin();
  rot.onChange(rotChange);

  // Button
  button.begin();
  button.setLongPeriodDuration(buttonSensitivity);
  button.onUp(shortClick);
  button.onUpLong(longClick);

  // Adjust from time from ds3232 every 300 seconds
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  // Ready for action
  if (serialUsed) {
    Serial.println(F("Ready"));
  }

  // Menu
  reportSRAM();
}


//*****************************************************************
/* Main loop */

void loop()
{
  long dateNow = now(); // in seconds

  // update screen if time to do so
  if (dateNow - dateLastScreen > 1) { // measure and display every 1 second
    dateLastScreen = dateNow;
    Serial.println(F("Measurement performed"));
    makeMeasures(currentMeasure);
    displayMeasures(currentMeasure);
  }

  // make measure if time to do so
  if (dateNow - dateLastRecord > 3) { // record to SD every 3 seconds
    dateLastRecord = dateNow;
    if (serialUsed) {
      printMeasureOnSerial(currentMeasure);
    }
    if (SDused) {
      writeRecordToLog(currentMeasure); 
      Serial.println(F("Measurement saved to SD"));
      if (showSDContent) {
        readFileSerialPrint();
      }
    }
  }

  // Serial.println(F("wait"));
  button.poll();
  rot.poll();
  delay(50);
}



//*****************************************************************


