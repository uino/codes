// out of memory !!


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
 *
 * TODO: currently the SD card conflicts with the LCD display
 */

// #include <ACbuttonLong.h>
// #include <SD.h>
#include <ACmemory.h>
#include <avr/pgmspace.h>
#include <SHT1x.h>
#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  
#include <ACnokia5100.h>
#include <ACrotatingPot.h>
#include <ACbuttonLong.h>

//*****************************************************************
/* Configuration */

// Log file parameters
const boolean resetLogOnSetup = true;
char* filename = "logger.txt"; // name of log file (8+3 characters max)
boolean SDused = false; // indicates whether SD card should/can be used

// Serial : for debugging to PC
const long serialBaudRate = 115200; // faster than using 9600
const boolean serialUsed = true;

// Button
// (pins: buttonPin)
ACbuttonLong button(2);
const int buttonSensitivity = 300;

// Rotating potentiometer
int rotPin = A0;
const int rotSensitivity = 20; 
const boolean rotInverted = true; 
ACrotatingPot rot(rotPin, rotSensitivity, rotInverted);

// Nokia5100 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
ACnokia5100 screen(3, 4, 5, 11, 13, 7);

// SHT1x : for measuring temperature and humidity
// (pins: dataPin, clockPin)
SHT1x sht1x(9, 10);

// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// SD card : for writing log file
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;

// Measure configuration
const int nbMeasures = 3; // --currently, at most 5 measures
const String measureNames[] = { "humid.", "t_SHT1", "t_3232" }; // at most 6 chars  // TODO: move to flash
const int floatPrecision = 3; // nb digits after decimal point

// History configuration
const int maxHistory = 1; 


//*****************************************************************
/* Types of measures */

typedef struct {
  time_t date;
  float values[nbMeasures];
} Record;

// History data
int nbHistory = 0;
Record history[maxHistory];


//*****************************************************************
/* History function */

void clearHistory() {
  nbHistory = 0;
  history[0].date = 0;
}

void addRecordToHistory(Record& r) {
  // when history becomes full, start over
  if (nbHistory == maxHistory)
    clearHistory();
  history[nbHistory] = r;
  nbHistory++;
}

//*****************************************************************
/* Memory functions */

void reportSRAM() {
  if (!serialUsed) 
    return;
  Serial.print(F("SRAM free: "));
  Serial.println(getFreeSRAM());
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
    Serial.print(measureNames[m] + " ");
    Serial.println(r.values[m], floatPrecision);  
  }
  Serial.println("---------------------------");
}

/*
void printHistory() {
  Serial.println("===========================");
  for (int h = 0; h < nbHistory; h++) {
    printMeasureOnSerial(history[h]);
  }
  Serial.println("===========================");
}
*/

//*****************************************************************
/* LCD functions */

void screenClear() {
  screen.clearDisplay(screen.WHITE);
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
/* Menu */

void rotChange() {
  Serial.println(F("rot change"));
  Serial.println(rot.getValue());
  // (change action is implicit on most panels)
}

void shortClick() {
  Serial.println(F("short click"));
  /*
  if (panel == 0) {
    changePanel(10 * panel + rot.getValue());
  } else {
    // go back
    changePanel(panel / 10);
  }
  Serial.println(panel);
  */
}

void longClick() {
  Serial.println(F("long click"));
  /*
  // return to previous menu
  changePanel(panel / 10);
  Serial.println(panel);
  */
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
  Serial.println(F("Time initialized"));
}

void setup()
{
  // Serial port
  if (serialUsed) {
    Serial.begin(serialBaudRate); 
    Serial.println(F("Starting"));
  }
  reportSRAM();

  int line = 0; // Note: use at most 6 lines on screen

  // LCD screen
  screen.setup();
  screen.setContrast(60);
  screen.setString("Loading...", line++, 0);
  screen.updateDisplay(); 

  rot.setup();
  rot.onChange(rotChange);

  // Button
  button.setup();
  button.setLongPeriodDuration(buttonSensitivity);
  button.onUp(shortClick);
  button.onUpAfterLong(longClick);

  // DS3232 set initial time
  initializeTime();

  // Time.h library (adjust from ds3232 every 300 seconds)
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 

  // History 
  clearHistory();

  // Ready for action
  if (serialUsed)
    Serial.println(F("Ready"));
  delay(1000);
}


//*****************************************************************
/* Main loop */

long dateLastRecord = 0;

void loop()
{
  long dateNow = now(); 
  if (dateNow - dateLastRecord > 3) { // record every 3 seconds
    dateLastRecord = dateNow;
    Serial.println(F("record"));

    screenClear();
    screen.setString("record...", 0, 0);
    screen.updateDisplay(); 

    Record r;
    makeMeasures(r);
    if (serialUsed) {
      printMeasureOnSerial(r);
    }
    
    // addRecordToHistory(r);

    screenClear();
    screen.setString("done...", 0, 0);
    screen.updateDisplay(); 

    // printHistory();
  }
  Serial.println(F("wait"));
  // displayPanel();
  button.poll();
  rot.poll();
  delay(200);
}




// marche pas : displayMeasuresOnScreen(current);

//*****************************************************************


