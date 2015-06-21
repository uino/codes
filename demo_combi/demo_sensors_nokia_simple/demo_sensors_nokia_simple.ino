/**
 * Demo for loggin on SD card, with display on Nokia screen;
 * and deep spleeping mode (see demo_DS3232_AC_Sleep).
 *
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * Important: to support hot swapping of the SD card, you need to 
 * patch the file "libraries/SD.cpp" from your distribution,
 * by adding the line:
 *    if (root.isOpen()) root.close(); // allows repeated calls 
 * immediately after the line:
 *    boolean SDClass::begin(uint8_t csPin) {
 *
 *
 * Show in real time the state of a sensor on
 * a Nokia5110 display. 
 * (In comments in the code: code to obtain data from SHT1x device)
 *
 * A button can used to go from a panel to the next.
 *
 */

#include <SPI.h>
#include <Wire.h>
// #include <SHT1x.h>
#include <SD.h>
#include <Time.h>
#include <AC_Nokia5110_light.h>
#include <DS3232RTC.h> 
// #include <AC_RAM.h>
#include <avr/pgmspace.h>
#include "defs.h"
#include <AC_Sleep.h>
#include <AC_DS3232.h>


//*****************************************************************
/* Configuration */

// Alimentation
const byte pinAlimClock = 4;
const byte pinAlimSPI = 9;

// Log file parameters
// const boolean resetLogOnSetup = false;
const boolean showSDContent = false;
char filename[] = "logger.txt"; // name of log file (8+3 characters max)

// SD card : for writing log file
const int SDselectPin = 10;
const int SDhardwareCSPin = 10;

// AC_Nokia5110 pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin
// AC_Nokia5110 screen(3, 4, 5, 11, 13, 8);
AC_Nokia5110_light screen(8, 7, 6, 11, 13, 0);
boolean needRefresh = true;
const long delayScreenRead = 2000; // milliseconds

// ds3232 : for measuring clock and temperature, and alarm
// DS3232RTC ds3232;
AC_DS3232 ds3232;  // extends the original module
const byte idInterrupt = 0; // corresponds to pinInterrupt
const byte pinInterrupt = 2; 

// alarm
const int periodWakeUp = 15 * 60; // seconds
boolean isSleeping = false;
long dateLastAlarm = 0;

// SHT1x pins: dataPin, clockPin
// SHT1x sht1x(9, 10);

// Current measure
bool lastMeasureWriteSuccessful = true;
long lastMeasureWriteSize = 0;
Record currentMeasure;   //include def.h


//*****************************************************************
/* Report

void report() {
  Serial.print(F("SRAM free: "));
  Serial.println(AC_RAM::getFree());
}

*/

//*****************************************************************
/* Measure */

void makeMeasures(Record& r) {
  r.date = ds3232.get();
  
  r.tempDS3232 = ds3232.temperature() / 4.0;

  float m = analogRead(A0);
  float mbar = (m - 37) / 0.9;
  r.mass = mbar * 1.9 * 2 / 3 / 10;
  
  float v = analogRead(A1);
  v = v*5/1023;
  float Rctn = (10000*v)/(5-v);
  float x = log(Rctn/22000);
  float T = (4220*298.15)/(298.15*x+4220);
  r.temp = T - 273.15;
 
  float w = analogRead(A2);
  float b = -9.96*w+9373 +abs(1000*(w-620))/500+abs(1000*(w-500))/500;
  r.alim = (float)b/1000-0.04;
     
  r.lumi = analogRead(A3);
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
  int floatPrecision = 4;
  Serial.println(F("---------------------------"));
  printTime(r.date);
  Serial.print(F("Mass: "));
  Serial.println(r.mass, floatPrecision);  
  Serial.print(F("Temp: "));
  Serial.println(r.temp, floatPrecision);  
  Serial.print(F("TempDS3232: "));
  Serial.println(r.tempDS3232, floatPrecision);  
  Serial.print(F("Alim: "));
  Serial.println(r.alim, floatPrecision);  
  Serial.print(F("Lumi: "));
  Serial.println(r.lumi, floatPrecision);  
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

/* Currently not used

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
*/

void writeRecordToFile(File file, Record r) {
  int floatPrecision = 4;
  writeTime(file, r.date);
  file.print("\t");
  file.print(r.date);
  file.print("\t");
  file.print(r.mass, floatPrecision);
  file.print("\t");
  file.print(r.temp, floatPrecision);
  file.print("\t");
  file.print(r.tempDS3232, floatPrecision);
  file.print("\t");
  file.print(r.alim, floatPrecision);
  file.print("\t");
  file.print(r.lumi, floatPrecision);
  file.println("");
}


void writeRecordToLog(Record r) {
  File file = openLog(FILE_WRITE);
  if (! file)
    return;
  writeRecordToFile(file, r);
  lastMeasureWriteSize = file.size();
  file.close();
}


//*****************************************************************
/* Screen functions */

// TODO: move some of this to a library

const int screenNbRows = screen.LCD_ROWS;
const int screenNbCols = screen.LCD_COLS; 
const int bufferRowLength = 30; // = screenNbCols+1 (but using more characters for safety)

// prints a two-digit nonnegative int into a given target string (of length >= 2)
void timeItemIntoString(int v, char* str) {
  str[0] = '0' + (v / 10);
  str[1] = '0' + (v % 10);
}

// prints the date into a given target string
// str needs to have a least 18 characters (15 if no year)
void timeIntoString(time_t t, char* str, boolean showYear) {
  char* pos = str;
  if (showYear) {
    timeItemIntoString(year(t) % 100, pos);
    pos += 2;
    *pos = '/';
    pos++;
  }
  timeItemIntoString(month(t), pos);
  pos += 2;
  *pos = '/';
  pos++;
  timeItemIntoString(day(t), pos);
  pos += 2;
  *pos = ' ';
  pos++;
  timeItemIntoString(hour(t), pos);
  pos += 2;
  *pos = ':';
  pos++;
  timeItemIntoString(minute(t), pos);
  pos += 2;
  *pos = ':';
  pos++;
  timeItemIntoString(second(t), pos);
  pos += 2;
  *pos = '\0';
}

void string_of_float(float value, int nbChars, int precision, char* target) {
  dtostrf(value, nbChars, precision, target); 
}

void displayMeasures(Record r) {
  char buffer[bufferRowLength];
  int floatNbChars = 7;
  int floatPrecision = 2;
  int line = 0;
  int colValue = 6;
  screen.clearDisplay(); 

  // mass
  screen.setString("Mass:", line, 0);
  string_of_float(r.mass, floatNbChars, floatPrecision, buffer);
  screen.setString(buffer, line, colValue);
  line++;

  // temp
  screen.setString("Temp:", line, 0);
  string_of_float(r.temp, floatNbChars, floatPrecision, buffer);
  screen.setString(buffer, line, colValue);
  line++;

  // alim
  screen.setString("Alim:", line, 0);
  string_of_float(r.alim, floatNbChars, floatPrecision, buffer);
  screen.setString(buffer, line, colValue);
  line++;

  // lumi
  screen.setString("Lumi:", line, 0);
  string_of_float(r.lumi, floatNbChars, floatPrecision, buffer);
  screen.setString(buffer, line, colValue);
  line++;

  // date
  timeIntoString(r.date, buffer, false);
  screen.setString(buffer, line, 0);
  line++;

  // success of last write and file size
  if (lastMeasureWriteSuccessful) {
    screen.setString("ok", line, 0);
  } else {
    screen.setString("pb", line, 0);
  }
  string_of_float((float) lastMeasureWriteSize, 11, 0, buffer);
  screen.setString(buffer, line, 3);
  line++;

  // finish
  screen.updateDisplay(); 
}


//*****************************************************************
/* Menu */

void displayPanel() {
  if (! needRefresh)
    return;
  displayMeasures(currentMeasure);
  needRefresh = false;
}


//*****************************************************************
/* Alarm */

void wake() {
  AC_Sleep::terminateSleep();
  detachInterrupt(idInterrupt);
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

void alimOn() {
  pinMode(pinAlimClock, OUTPUT);
  pinMode(pinAlimSPI, OUTPUT);
  digitalWrite(pinAlimClock, HIGH);
  digitalWrite(pinAlimSPI, HIGH);
  delay(50);
}

void alimOff() {
  pinMode(pinAlimClock, INPUT);
  pinMode(pinAlimSPI, INPUT);
  digitalWrite(pinAlimClock, LOW);
  digitalWrite(pinAlimSPI, LOW);
}

void setup()
{
  // Serial
  Serial.begin(9600); 

  // Alim
  alimOn();

  // Screen
  screen.begin();
  screen.updateDisplay(); 
  screen.setContrast(60); 

  // DS3232 set initial time
  // DEACTIVATED:  initializeTime();

  // Time.h library (adjust from ds3232 every 300 seconds)
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  // SD
  pinMode(SDhardwareCSPin, OUTPUT); 
  if (! SD.begin(SDselectPin)) {
    Serial.println(F("Card failed or missing"));
  }

  /* Reset
  if (resetLogOnSetup) {
    resetLog();
  }*/

  Serial.println(F("Starting")); 

  // periodic wake up signal
  dateLastAlarm = ds3232.get();
  digitalWrite(pinInterrupt, HIGH); // enable pull up
  ds3232.alarmInterrupt(ALARM_1, true); 
}


//*****************************************************************
/* Main */

long nbAlarmCycles = 0;

void loop()
{
  if (isSleeping) {
    isSleeping = false;
    Serial.println(F("Exit sleep"));
    alimOn();
  }

  // for debugging only:
  printTime(ds3232.get());
  nbAlarmCycles++;
  Serial.print(F("Nb alarm cycles: "));
  Serial.println(nbAlarmCycles);
    // report();

  // perform measures
  makeMeasures(currentMeasure);
  if (SD.begin(SDselectPin)) {
    printMeasureOnSerial(currentMeasure);
    writeRecordToLog(currentMeasure);
    lastMeasureWriteSuccessful = true;
  } else {
    Serial.println(F("Card failed or missing"));
    lastMeasureWriteSuccessful = false;
  }
  needRefresh = true;

  // for debugging:
  if (showSDContent) {
    readFileSerialPrint();
  }

  // Display on screen
  displayPanel();
  delay(delayScreenRead);

  // Go back to sleep
  Serial.println(F("Enter sleep"));
  delay(100);

  ds3232.alarm1(); // reset alarm
  dateLastAlarm += periodWakeUp;
  time_t timeNow = ds3232.get();
  if (dateLastAlarm <= timeNow) { // if target in the past, catch up
    dateLastAlarm = timeNow + periodWakeUp;
  }
  ds3232.setAlarm1(dateLastAlarm);
  alimOff();
  isSleeping = true;
  AC_Sleep::enterSleepOnInterrupt(idInterrupt, wake, LOW);

}
