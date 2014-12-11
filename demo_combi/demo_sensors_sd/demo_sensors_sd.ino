/**
 * Demo for the SHT1x with SD card.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 *
 * Show in real time the state of a SHT1x sensor on
 * a Nokia5100 display. 
 *
 */
#include <SPI.h>
#include <Time.h>
#include <Wire.h>  
#include <DS3232RTC.h> 
#include <SD.h>
#include <SHT1x.h>
#include <avr/pgmspace.h>
#include "defs.h"


//*****************************************************************
/* Configuration */

// Log file parameters
const boolean resetLogOnSetup = true; // TODO!!!
const boolean showSDContent = true;
char filename[] = "logger.txt"; // name of log file (8+3 characters max)

// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// SHT1x pins: dataPin, clockPin
SHT1x sht1x(9, 10);

// SD card : for writing log file
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;

// Current measure
const int measurePeriod = 2000; // milliseconds
long dateLastMeasure = 0;
Record currentMeasure;


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
/* SD functions */

void readFileSerialPrint() { // TODO: move to library
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
  File file = SD.open(filename, mode);
  if (! file) {
    Serial.println(F("Error opening file"));
  }
  return file;
}

void writeLogHeader() {
  File file = openLog(FILE_WRITE);
  if (! file)
    return;
  file.print(F("#date\ttime\tunixtime\ttemperature\thumidity\t"));
  file.println("");
  file.close();
}

void resetLog() {
  SD.remove(filename);
  writeLogHeader();
}

void writeRecordToFile(File file, Record r) {
  int floatPrecision = 2;
  writeTime(file, r.date);
  file.print("\t");
  file.print(r.date);
  file.print("\t");
  file.print(r.temperature, floatPrecision);
  file.print("\t");
  file.print(r.humidity, floatPrecision);
  file.println("");
}

void writeRecordToLog(Record r) {
  File file = openLog(FILE_WRITE);
  if (! file)
    return;
  writeRecordToFile(file, r);
  file.close();
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

  // DS3232 set initial time
  initializeTime();

  // Time.h library (adjust from ds3232 every 300 seconds)
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  // SD
  pinMode(SDhardwareCSPin, OUTPUT); 
  if (! SD.begin(SDselectPin)) {
    Serial.println("Card failed or missing");
    while(1);
  }

  // Reset
  if (resetLogOnSetup) {
    resetLog();
  }

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
    writeRecordToLog(currentMeasure);
    if (showSDContent) {
      readFileSerialPrint();
    }
  }

  // periodic actions
  delay(50);
}


