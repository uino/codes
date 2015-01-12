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
#include <AC_Nokia5110_light.h>
#include <AC_Button.h>
#include "defs.h"


//*****************************************************************
/* Software Configuration */

// See defs.h for list of measures

// Log file reset on startup
const boolean resetLogOnSetup = false; // use "true" only for debugging

// Log file parameters
char* filename = "logger.txt"; // name of log file (8+3 characters max)

// Delay between two measurements
const long delayBetweenRecords = 2; // seconds

// Whether to use Serial
const boolean serialUsed = true; // use "true" only for debugging


//*****************************************************************
/* Harware Configuration */

// Serial : for debugging to PC
const long serialBaudRate = 115200; // faster than using 9600

// Button
// (pins: buttonPin)
AC_Button button(2);
const int buttonSensitivity = 300;

// Rotating potentiometer
byte rotPin = A0;
const int rotSensitivity = 20; 
const boolean rotInverted = true; 
AC_RotatingPot rot(rotPin, rotSensitivity, rotInverted);

// Nokia5110 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
AC_Nokia5110_light screen(3, 4, 5, 11, 13, 
  #ifdef ARTHUR
  8
  #else
  7
  #endif
  );

// SHT1x : for measuring temperature and humidity
// (pins: dataPin, clockPin)
SHT1x sht1x(9, 10);

// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// SD card : for writing log file
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;


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
#ifdef ARTHUR
  r.date = 161813518 + (millis()/1000);
  r.humidity = 0;
  r.tempSHT1x = 0;
  r.tempDS3232 = 1; // - the ds3232 returns an int equal to 4 times the temperature.
  r.potar = analogRead(rotPin);
  return;
#endif
  r.date = ds3232.get();
  r.humidity = sht1x.readHumidity();
  r.tempSHT1x = sht1x.readTemperatureC();
  r.tempDS3232 = ds3232.temperature() / 4.0; // - the ds3232 returns an int equal to 4 times the temperature.
  r.potar = analogRead(rotPin);
}


//*****************************************************************
/* Serial reporting */

void printTimeOnSerial(time_t t) {
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
  printTimeOnSerial(r.date);
  Serial.print(names[0]);
  Serial.print(": ");
  Serial.println(r.humidity, 3);
  Serial.print(names[1]);
  Serial.print(": ");
  Serial.println(r.tempSHT1x, 3);
  Serial.print(names[2]);
  Serial.print(": ");
  Serial.println(r.tempDS3232, 3);
  Serial.print(names[3]);
  Serial.print(": ");
  Serial.println(r.potar);
  Serial.println("---------------------------");
}


//*****************************************************************
/* LCD reporting */

void displayMeasures(Record r) {
  const int separatorCol = 7;
  const int valueCol = 9;
  const int valueNbChars = 5;
  int line = 0;
  screen.clearDisplay(); 
  // date
  time_t t = r.date;
  // no space to display year; so we display "month/day"
  // screen.setInt((year(t) % 100), line, 0, 2);
  screen.setInt(month(t), line, 0, 2, true); // true for leadingZero
  screen.setString("/", line, 2);
  screen.setInt(day(t), line, 3, 2, true);
  screen.setString(" ", line, 5);
  screen.setInt(hour(t), line, 6, 2, true);
  screen.setString(":", line, 8);
  screen.setInt(minute(t), line, 9, 2, true);
  screen.setString(":", line, 11);
  screen.setInt(second(t), line, 12, 2, true);
  line++;
  // data1
  screen.setString(names[0], line, 0);
  screen.setString(":", line, separatorCol);
  screen.setFloat(r.humidity, 3, line, valueCol, valueNbChars); // 3 is precision
  line++;
  // data2
  screen.setString(names[1], line, 0);
  screen.setString(":", line, separatorCol);
  screen.setFloat(r.tempSHT1x, 3, line, valueCol, valueNbChars);
  line++;
  // data3
  screen.setString(names[2], line, 0);
  screen.setString(":", line, separatorCol);
  screen.setFloat(r.tempDS3232, 3, line, valueCol, valueNbChars);
  // data4
  line++;
  screen.setString(names[3], line, 0);
  screen.setString(":", line, separatorCol);
  screen.setInt(r.potar, line, valueCol, valueNbChars);
  // end
  screen.updateDisplay(); 
}


//*****************************************************************
/* SD reporting */

File openLog() {
  File file = SD.open(filename, FILE_WRITE);
  if (!file && serialUsed) {
    Serial.println(F("Error opening file"));
  }
  return file;
}

void writeLogHeader() {
  File file = openLog();
  if (! file)
    return;
  // write 
  file.print(F("date\t"));
  file.print(F("time\t"));
  file.print(F("unixtime\t"));
  for (int m = 0; m < nbMeasures; m++) {
    file.print(names[m]);  
    file.print("\t");
  }
  file.println("");
  file.close();
}

void resetLog() {
  SD.remove(filename);
  writeLogHeader();
}

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

// return whether the operation was successful
boolean writeRecordToLog(Record& r) {
  File file = openLog();
  if (! file)
    return false;
  // begin writing
  writeTime(file, r.date); // human-readable time
  file.print("\t");
  file.print(r.date); // unix time
  file.print("\t");
  file.print(r.humidity, 3); // 3 is precision
  file.print("\t");
  file.print(r.tempSHT1x, 3);  
  file.print("\t");
  file.print(r.tempDS3232, 3);  
  file.print("\t");
  file.print(r.potar);  
  file.println("");
  // end writing
  file.close();
  return true;
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

  // LCD screen
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay(); 
  screen.setString("Load", 0, 0);
  screen.updateDisplay(); 
    // delay(500);

  // SD
  pinMode(SDhardwareCSPin, OUTPUT); 
  if (! SD.begin(SDselectPin)) {
    if (serialUsed) {
      Serial.println(F("Card failed or missing"));
    }
  } else {
    // check that the card appears to be working
    {
      File myFile = SD.open("example.txt", FILE_WRITE);
      if (myFile) {
        Serial.println(F("SD ok"));
        screen.setString("SD ok", 0, 0);
      } else {
        Serial.println(F("SD bug"));
        screen.setString("SD bug", 0, 0);
      }
      screen.updateDisplay(); 
      myFile.close();
    }

    if (resetLogOnSetup) {
      Serial.println(F("Card reset"));
      screen.setString("SD log reset", 0, 0);
      resetLog(); // reset file if configuration says so
    } else if (! SD.exists(filename)) {
      resetLog(); // create file if it does not exist
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

  // Ready for action
  if (serialUsed) {
    Serial.print(F("SRAM free: "));
    Serial.println(AC_RAM::getFree());
    Serial.println(F("Ready"));
  }
}


//*****************************************************************
/* Main loop */

void loop()
{
  #ifdef ARTHUR
  long dateNow = millis() / 1000;
  #else
  long dateNow = now(); // in seconds
  #endif

  // make measure if time to do so
  if (dateNow - dateLastRecord > delayBetweenRecords) {
    dateLastRecord = dateNow;
    if (serialUsed) {
      Serial.println(F("Starting measurement"));
    }
    makeMeasures(currentMeasure);
    displayMeasures(currentMeasure);
    boolean saveSuccessful = writeRecordToLog(currentMeasure); 
    if (serialUsed) {
      printMeasureOnSerial(currentMeasure);
      if (saveSuccessful) {
        Serial.println(F("Measurement saved to SD"));
     }
    }
  }

  // Serial.println(F("wait"));
  button.poll();
  rot.poll();
  delay(50);
}



//*****************************************************************


