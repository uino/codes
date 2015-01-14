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
 * To add a measure, modify "defs.h" and then modify
 * the functions: writeMeasuresToScreen, writeMeasuresToSerial, 
 * and writeMeasuresToLog.
 *
 *
 * // Not yet implemented:
 *   An DS3232 alarm is used to wake up the Arduino board
 *   when it is time to perform a measure. This allows for
 *   a long-term battery usage of the logger.
 *
 */

#define ARTHUR 1 // à commenter pour utilisation sur le vrai logger

//*****************************************************************
/* Includes */

#include <Arduino.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <AC_RAM.h>
#include <SD.h>
#include <SHT1x.h>
#include <Time.h>
#include <Wire.h>  
#include <AC_DS3232.h> 
#include <AC_RotatingPot.h>
#include <AC_Nokia5110_light.h>
#include <AC_Sleep.h>
#include <AC_Button.h>
#include <avr/sleep.h>
#include "defs.h"


//*****************************************************************
/* Software Configuration */

// See defs.h for list of measures

// Log file reset on startup
const boolean resetLogOnSetup = false; // use "true" only for debugging

// Log file parameters
char* filename = "logger.txt"; // name of log file (8+3 characters max)

// Delay between two measurements
const long delayBetweenRecordsOnScreen = 2; // seconds

// Period at which to wake up the board
const int delayBetweenRecordsOnSD = 6; // seconds

// Whether to use Serial
const boolean useSerial = true; // use "true" only for debugging



// Whether to go to sleep after measure
boolean useSleep = true; 

// Whether to show measures on screen
boolean useScreen = false;


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

// ds3232 : for alarm, clock and temperature
AC_DS3232 ds3232;
const int pinDS3232vcc = 7;
const byte interruptPinWakeUp = 0; // actual pin2

// Interrupt pins
const byte idInterrupt = 0; 
const byte pinInterrupt = 2; 

// SD card : for writing log file
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;


//*****************************************************************
/* Program variables */

// Current measure data structure
Record currentMeasure;

// Control of actions
boolean isSleeping = false;
time_t dateNextRecordOnScreen = 0;
time_t dateLastRecordOnSD = 0;
time_t dateNextRecordOnSD = 0;


//*****************************************************************
/* Performing measures */

void makeMeasures(Record& r) {
  r.date = ds3232.get();
  #ifdef ARTHUR
  r.humidity = 1;
  r.tempSHT1x = 2;
  #else
  r.humidity = sht1x.readHumidity();
  r.tempSHT1x = sht1x.readTemperatureC();
  #endif
  r.tempDS3232 = ds3232.temperature() / 4.0; // - the ds3232 returns an int equal to 4   times the temperature.
  r.potar = analogRead(rotPin);
}


//*****************************************************************
/* Serial reporting */

void writeTimeToSerial(time_t t) {
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

void writeMeasuresToSerial(Record& r) {
  Serial.println("---------------------------");
  writeTimeToSerial(r.date);
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

void writeTimeToScreen(int line, time_t t) {
  // no space to display year; so we display "month/day"
  // screen.setInt((year(t) % 100), line, 0, 2);
  int col = 0;
  screen.setInt(month(t), line, col, 2, true); // true for leadingZero
  col += 2;
  screen.setString("/", line, col);
  col += 1;
  screen.setInt(day(t), line, col, 2, true);
  col += 2;
  screen.setString(" ", line, col);
  col += 1;
  screen.setInt(hour(t), line, col, 2, true);
  col += 2;
  screen.setString(":", line, col);
  col += 1;
  screen.setInt(minute(t), line, col, 2, true);
  col += 2;
  screen.setString(":", line, col);
  col += 1;
  screen.setInt(second(t), line, col, 2, true);
}


void writeMeasuresToScreen(Record r) {
  const int separatorCol = 7;
  const int valueCol = 9;
  const int valueNbChars = 5;
  int line = 0;
  screen.clearDisplay(); 
  // date
  writeTimeToScreen(line, r.date);
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
  if (!file && useSerial) {
    Serial.println(F("=== Error opening file"));
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

void writeTimeToLog(File file, time_t t) {
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
boolean writeMeasuresToLog(Record& r) {
  File file = openLog();
  if (! file)
    return false;
  // begin writing
  writeTimeToLog(file, r.date); // human-readable time
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
/* Debugging */

// Auxiliary function to check that the card appears to be working
void SDcheckWorking() {
  File myFile = SD.open("example.txt", FILE_WRITE);
  if (myFile) {
    screen.setString("SD ok", 0, 0);
    if (useSerial) {
      Serial.println(F("SD ok"));
    }
  } else {
    screen.setString("SD bug", 0, 0);
    if (useSerial) {
      Serial.println(F("SD bug"));
    }
  }
  screen.updateDisplay(); 
  myFile.close();
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
/* Performing measures */

void performMeasures(boolean saveToSD) {
  if (useSerial) {
    Serial.println(F("Starting measurements"));
  }
  makeMeasures(currentMeasure);
  if (useScreen) {
    writeMeasuresToScreen(currentMeasure);
  }
  if (useSerial) {
    writeMeasuresToSerial(currentMeasure);
  }
  if (saveToSD) {
    boolean saveSuccessful = writeMeasuresToLog(currentMeasure); 
    if (useSerial) {
      if (saveSuccessful) {
        Serial.println(F("Measurements saved to SD"));
      }
    }
  }
}


//*****************************************************************
/* Sleep function and wake up interrupt handler */


void wake() {
  AC_Sleep::terminateSleep();
  detachInterrupt(idInterrupt);
}

/*
  // TODO: est-ce qu'il faut faire ça ?
  pinMode(pinDS3232vcc, OUTPUT);
  digitalWrite(pinDS3232vcc, HIGH);
  // TODO: est-ce qu'il faut faire ça ?
  digitalWrite(pinDS3232vcc, LOW);
  pinMode (pinDS3232vcc, INPUT);
*/

//*****************************************************************
/* Setup */

void setup()
{
  // Serial port
  if (useSerial) {
    Serial.begin(serialBaudRate); 
    Serial.println(F("Starting"));
  }

  // DS3232 power
  pinMode(pinDS3232vcc, OUTPUT);
  digitalWrite(pinDS3232vcc, HIGH);

  // LCD screen
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay(); 
  screen.setString("Load", 0, 0);
  screen.updateDisplay(); 

  // SD
  pinMode(SDhardwareCSPin, OUTPUT); 
  if (! SD.begin(SDselectPin)) {
    if (useSerial) {
      Serial.println(F("Card failed or missing"));
    }
  } else {
    SDcheckWorking();
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
  if (useSerial) {
    Serial.print(F("SRAM free: "));
    Serial.println(AC_RAM::getFree());
    Serial.println(F("Ready"));
  }

  // Sleeping
  if (useSleep) {
    dateNextRecordOnSD = ds3232.get();
    digitalWrite(pinInterrupt, HIGH); // enable pull up
    ds3232.alarmInterrupt(ALARM_1, true);
    isSleeping = false;
  }
}



//*****************************************************************
/* Main loop */

void loop()
{
  button.poll();
  rot.poll();

  if (isSleeping) {
    isSleeping = false;
    if (useSerial) {
      Serial.println("Exit sleep");
    }
  }

  if (useSleep) {

    performMeasures(true);

    Serial.println("Enter sleep");
    delay(100);

    ds3232.alarm1(); // reset alarm
    dateNextRecordOnSD += delayBetweenRecordsOnSD;
    time_t dateNow = ds3232.get();
    if (dateNextRecordOnSD <= dateNow) { // if target date is in the past, catch up
      dateNextRecordOnSD = dateNow + delayBetweenRecordsOnSD;
    }
    ds3232.setAlarm1(dateNextRecordOnSD);
    isSleeping = true;
    AC_Sleep::enterSleepOnInterrupt(idInterrupt, wake, LOW);

  } else {

    time_t dateNow = now(); // in seconds
    if (dateNow > dateNextRecordOnSD) {
      dateNextRecordOnSD += delayBetweenRecordsOnSD;
      dateNextRecordOnScreen = dateNow + delayBetweenRecordsOnScreen;
      performMeasures(true); // save on SD

      if (dateNextRecordOnSD <= dateNow) { // if target date is in the past, catch up
        dateNextRecordOnSD = dateNow + delayBetweenRecordsOnSD;
      }
    } else if (dateNow > dateNextRecordOnScreen) {
      dateNextRecordOnScreen += delayBetweenRecordsOnScreen;
      performMeasures(false); // don't save on SD
    }
  }

  delay(50);
}



//*****************************************************************


