/**
 * Logger project.
 * Code by Anatole and Arthur Chargueraud
 * GNU GPL code.
 *
 * Goal is to measure values periodically from sensors,
 * (e.g., temperature/humidity using an SHT1x sensor)
 * log the values on a SD card, and provide an interface
 * for viewing results on a Nokia5110 screen. Navigation in
 * the menus is performed using a button and a potentiometer.
 *
 * An DS3232 alarm is used to wake up the Arduino board
 * when it is time to perform a measure. This allows for
 * a long-term battery usage of the logger.
 *
 */

#include <SD.h>
#include <SHT1x.h>
#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  

//*****************************************************************
/* Configuration */

// Serial
const int serialBaudRate = 115200; // faster than using 9600

// SHT1x : temperature and humidity
const int dataPin = 9;
const int clockPin = 10;
SHT1x sht1x(dataPin, clockPin);

// ds3232 : clock and temperature
DS3232RTC ds3232;

// SD card : for file writing
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;
char* filename = "logger.txt"; // name of log file (8+3 characters max)
boolean usingSD = true; // indicates whether SD card can be used


//*****************************************************************
/* Types of measures */

const int nbMeasures = 3;
const String measureNames[] = { "Humidity", "Temp_SHT1", "Temp_DS3232" };

typedef struct {
  time_t date;
  float values[nbMeasures];
} Record;

const int floatPrecision = 4; // nb digits after decimal point recorded in log


//*****************************************************************
/* Printing function */

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

void printMeasure(Record& r) {
  Serial.println("---------------------------");
  printTime(r.date);
  for (int m = 0; m < nbMeasures; m++) {
    Serial.print(measureNames[m] + " ");
    Serial.println(r.values[m], floatPrecision);  
  }
  Serial.println("---------------------------");
}

void writeTime(File file, time_t t) {
  file.print(year(t)); 
  file.print('/');  
  file.print(day(t));
  file.print('/');
  file.print(month(t));
  file.print('\t');
  file.print(hour(t));
  file.print(':');
  file.print(minute(t));
  file.print(':');
  file.print(second(t));
}


//*****************************************************************
/* File functions */

void readFileSerialPrint() {
  if (! usingSD)
    return;
  if (SD.exists(filename)) {
    File file = SD.open(filename, FILE_READ);
    if (! file) {
      Serial.println("readFileSerialPrint: error opening file");
    } else {
      Serial.println("=========begin contents=========");
      while (file.available()) {
        byte v = file.read();
        Serial.write(v);
      }
      Serial.println("=========end contents=========");
      file.close();
    }
  } else {
    Serial.println("readFileSerialPrint: file not found");
  }
}

File openLog() {
  if (! usingSD) 
    return File();
  File file = SD.open(filename, FILE_WRITE);
  if (! file) {
    Serial.println("Error opening file");
    usingSD = false;
  }
  return file;
}

void writeLogHeader() {
  File file = openLog();
  if (! file)
    return;
  file.print("#date\ttime\tunixtime\t");
  for (int m = 0; m < nbMeasures; m++) {
    file.print(measureNames[m]);  
    file.print("\t");
  }
  file.println("");
  file.close();
}

void resetLog() {
  if (! usingSD)
    return;
  SD.remove(filename);
  writeLogHeader();
}

void writeMeasuresToLog(Record& r) {
  File file = openLog();
  if (! file)
    return;
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
  file.close();
}


//*****************************************************************
/* Performing measures */

// Remarks:
// - date is read from the ds3232 time (without interpolation from "millis")
// - the ds3232 returns an int equal to 4 times the temperature.

void makeMeasures(Record& r) {
  r.date = ds3232.get(); 
  float* v = r.values;
  v[0] = sht1x.readHumidity();
  v[1] = sht1x.readTemperatureC();
  v[2] = ds3232.temperature() / 4.0; 
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
  // alternative for passing directly a time_t
  // ds3232.set(makeTime(initTime));
  Serial.println("Time initialized");
}

void setup()
{
  // initialize serial
  Serial.begin(serialBaudRate); 

  // initialize SD
  pinMode(SDhardwareCSPin, OUTPUT); 
  if (! SD.begin(SDselectPin)) {
    Serial.println("Card failed or missing");
    usingSD = false;
  }

  // set a given time into the ds3232
  initializeTime();

  // calls to ensure that the value now() is read from the
  // ds3232 every 300 seconds, and ajusted using millis()
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  resetLog();
  Serial.println("Reset log");

  Serial.println("Starting up");
}


//*****************************************************************
/* Main loop */

void loop()
{
  Record current;
  makeMeasures(current);
  printMeasure(current);
  writeMeasuresToLog(current); 
  readFileSerialPrint();
  delay(5000);
}

//*****************************************************************


