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

#include <avr/pgmspace.h>
#include <ACmemory.h>
#include <SD.h>
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
const boolean showSDContent = true;
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
const char measureNames[nbMeasures][7] = { "humid.", "t_SHT1", "t_3232" }; // 6 chars exactly for each
const int floatPrecision = 3; // nb digits after decimal point



//*****************************************************************
/* Types of measures */

typedef struct {
  time_t date;
  float values[nbMeasures];
} Record;

// current measure
Record currentMeasure;


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
    Serial.print(measureNames[m]);
    Serial.print(" ");
    Serial.println(r.values[m], floatPrecision);  
  }
  Serial.println("---------------------------");
}


//*****************************************************************
/* LCD functions */

const int screenNbRows = ACnokia5100::LCD_ROWS;
const int screenNbCols = ACnokia5100::LCD_COLS; 
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
  // TODO: fix bugs
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
}


//*****************************************************************
/* File functions */

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

void resetLog() {
  if (! SDused)
    return;
  SD.remove(filename);
  writeLogHeader();
}

void readRecordFromFile(File file, Record& r) {
  // read and ignore the human readable date
  String date = file.readStringUntil('\t');
  String time = file.readStringUntil('\t');
  // read the date and the measures into the record
  long unixtime = file.parseInt();
  r.date = unixtime;
  for (int m = 0; m < nbMeasures; m++) {
    float value = file.parseFloat();
    r.values[m] = value;
  }
  // read what remains of the line
  file.readStringUntil('\n');
}

// returns the number effectively read
int readRecordsFromFile(File file, int maxNbRecords, Record* rs) {
  int count = 0;
  while (file.available()) {
    // ignore lines that start with a '#' symbol
    if (file.peek() == '#') {
      file.readStringUntil('\n');
      continue;
    }
    // other lines are parsed as records
    readRecordFromFile(file, rs[count]);
    // for debug: printMeasureOnSerial(r);
    count++;
    if (count >= maxNbRecords)
      return count;
  }
  return count;
}

/*
void readRecordsFromLog() {
  File file = openLog(FILE_READ);
  if (! file)
    return;
  // nbHistory = readRecordsFromFile(file, maxHistory, history);
  file.close();
}
*/

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
/* Menu definition */


const int panelItems0_size = 16;
const char panelItems0_0[] PROGMEM = "disp. curve";
const char panelItems0_1[] PROGMEM = "disp. submenu";
const char panelItems0_2[] PROGMEM = "disp. all";
const char panelItems0_3[] PROGMEM = "set a";
const char panelItems0_4[] PROGMEM = "set b";
const char panelItems0_5[] PROGMEM = "set c";
const char panelItems0_6[] PROGMEM = "set b";
const char panelItems0_7[] PROGMEM = "set d";
const char panelItems0_8[] PROGMEM = "set e";
const char panelItems0_9[] PROGMEM = "set f";
const char panelItems0_10[] PROGMEM = "set g";
const char panelItems0_11[] PROGMEM = "set h";
const char panelItems0_12[] PROGMEM = "set i";
const char panelItems0_13[] PROGMEM = "set j";
const char panelItems0_14[] PROGMEM = "set k";
const char panelItems0_15[] PROGMEM = "set l";

const int panelItems1_size = 3;
const char panelItems1_0[] PROGMEM = "set x-scale";
const char panelItems1_1[] PROGMEM = "set y-scale";
const char panelItems1_2[] PROGMEM = "set log params";

typedef const char* fstring;

const fstring panelItems[] PROGMEM = { 
  panelItems0_0, 
  panelItems0_1, 
  panelItems0_2, 
  panelItems0_3, 
  panelItems0_4,  
  panelItems0_5, 
  panelItems0_6, 
  panelItems0_7, 
  panelItems0_8, 
  panelItems0_9, 
  panelItems0_10, 
  panelItems0_11, 
  panelItems0_12, 
  panelItems0_13, 
  panelItems0_14, 
  panelItems0_15,

  panelItems1_0, 
  panelItems1_1, 
  panelItems1_2, 
  };

typedef struct {
  int nbItems;
  const fstring* items;
} PanelDescr;


// TODO: could be in flash memory as well if needed
const int nbPanels = 2;
const PanelDescr panelDescrs[nbPanels] = { 
  { panelItems0_size, panelItems+0 },
  { panelItems1_size, panelItems+panelItems0_size },
  // { panelItems2_size, panelItems+panelItems0_size+panelItems1_size },
  };

//*****************************************************************
/* Menu implementation */

// TODO: move to library

// assumes buffer length >= bufferRowLength, and storing null-terminated string.
void completeLineWithSpaces(char* buffer) {
  int len = strlen(buffer);
  for (int k = len; k < screenNbCols; k++) {
    buffer[k] = ' ';
  }
  buffer[screenNbCols] = '\0';
}

void displayChoices(const fstring* choices, int firstChoice, int nbChoices, int selected) {
  char buffer[bufferRowLength];
  screen.clearDisplay();
  for (int line = 0; line < nbChoices; line++) {
    int item = firstChoice + line;
    boolean whiteBackground = (item != selected);
    fstring fline = ((fstring) pgm_read_word(& choices[item]));
    strcpy_P(buffer, fline);
    completeLineWithSpaces(buffer);
    screen.setString(buffer, line, 0, whiteBackground);
  }
  screen.updateDisplay();
}

const int panelRoot = 0;
int panel = panelRoot;

int menuCurrent = 0; // should be between 0 and nbPanelItems[panel]
int menuPevious = -1;

long dateLastDisplayMeasures = 0;


void displayPanel() {
  // Serial.println(F("display menu"));
  // Serial.println(menuCurrent);
  if (panel == 0 || panel == 1) {
    if (menuCurrent == menuPevious)
      return; // if no change, nothing to do
    int idPage = menuCurrent / screenNbRows;
    int idFirst = idPage * screenNbRows;
    PanelDescr descr = panelDescrs[panel];
    int nbShown = min(screenNbRows, descr.nbItems - idFirst);
    displayChoices(descr.items, idFirst, nbShown, menuCurrent);
    menuPevious = menuCurrent;
  } else if (panel == 100) {
    long timeNow = millis(); 
    if (timeNow - dateLastDisplayMeasures > 1000) { // display every 1 second
      displayMeasures(currentMeasure);
    }
  }
}

void enterPanel(int idPanel) {
  Serial.println(F("enter panel"));
  panel = idPanel;
  menuPevious = -1;
  rot.setModulo(panelDescrs[panel].nbItems);
  rot.resetValue(0);
}

void shortClick() {
  Serial.println(F("short click"));
  if (panel == 0) {
    if (menuCurrent == 0) {
      enterPanel(100);
    } else if (menuCurrent == 1) {
      enterPanel(1);
    }
  }
}

void longClick() {
  Serial.println(F("long click"));
  if (panel == 1 || panel == 100) {
    enterPanel(0);
  }
}

void rotChange() {
  menuCurrent = rot.getValue();
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

  // LCD screen
  screen.setup();
  screen.setContrast(60);
  screen.setString("Loading...", 0, 0);
  screen.updateDisplay(); 

  if (SDused) {
    pinMode(SDhardwareCSPin, OUTPUT); 
    if (! SD.begin(SDselectPin)) {
      if (serialUsed)
        Serial.println("Card failed or missing");
      SDused = false;
    }
  }

  // RotatingPot
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

  // Ready for action
  if (serialUsed) {
    Serial.println(F("Ready"));
  }

  // Menu
  enterPanel(panelRoot);
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
    makeMeasures(currentMeasure);
    if (serialUsed) {
      printMeasureOnSerial(currentMeasure);
    }
    if (SDused) {
      writeRecordToLog(currentMeasure); 
      /*if (showSDContent) {
        readFileSerialPrint();
      }*/
    }
  }

  Serial.println(F("wait"));
  button.poll();
  rot.poll();
  displayPanel();
  delay(50);
}



//*****************************************************************


