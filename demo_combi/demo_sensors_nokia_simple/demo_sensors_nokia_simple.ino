/**
 * Demo for loggin on SD card, with display on Nokia screen.
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
#include <AC_RAM.h>
#include <avr/pgmspace.h>
#include "defs.h"


//*****************************************************************
/* Configuration */

// Log file parameters
const boolean resetLogOnSetup = false;
const boolean showSDContent = true;
char filename[] = "logger.txt"; // name of log file (8+3 characters max)

// SD card : for writing log file
const int SDselectPin = 10;
const int SDhardwareCSPin = 10;

// AC_Nokia5110 pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin
// AC_Nokia5110 screen(3, 4, 5, 11, 13, 8);
AC_Nokia5110_light screen(8, 7, 6, 11, 13, 0);
boolean needRefresh = true;


// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// SHT1x pins: dataPin, clockPin
// SHT1x sht1x(9, 10);

// Current measure
const int measurePeriod = 3000; // milliseconds
long dateLastMeasure = 0;
bool lastMeasureWriteSuccessful = true;
long lastMeasureWriteSize = 0;
Record currentMeasure;   //include def.h


//*****************************************************************
/* Report */

void report() {
  Serial.print(F("SRAM free: "));
  Serial.println(AC_RAM::getFree());
}

//*****************************************************************
/* Measure */


void makeMeasures(Record& r) {
  r.date = ds3232.get();
  r.temperature = ds3232.temperature() / 4.0;
  r.humidity = 218.3;  // dummy value
} 

/* original:

void makeMeasures(Record& r) {
  r.date = ds3232.get(); 
  r.temperature = sht1x.readTemperatureC();
  r.humidity = sht1x.readHumidity();
}
*/


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
  Serial.println(F("Call to writeRecordToFile"));
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

  // success of last write
  line++;
  screen.setString("Save.:", line, 0);
  if (lastMeasureWriteSuccessful) {
    screen.setString("ok", line, 9);
  } else {
    screen.setString("fail", line, 9);
  }

  // success of last write
  line++;
  screen.setString("Size :", line, 0);
  screen.setString("Temp.:", line, 0);
  string_of_float((float) lastMeasureWriteSize, floatNbChars, 0, buffer);
  screen.setString(buffer, line, 9);

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
  // DEACTIVATED:  initializeTime();

  // Time.h library (adjust from ds3232 every 300 seconds)
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  // SD
  pinMode(SDhardwareCSPin, OUTPUT); 
  if (! SD.begin(SDselectPin)) {
    Serial.println(F("Card failed or missing"));
  }

  // Reset
  if (resetLogOnSetup) {
    resetLog();
  }

  Serial.println(F("Starting")); 
}


//*****************************************************************
/* Main */

void loop()
{
  long now = millis();
  if (now - dateLastMeasure > measurePeriod) {
    dateLastMeasure = now;
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
    if (showSDContent) {
      readFileSerialPrint();
    }
    report();
  }

  displayPanel();
  delay(50);
}
