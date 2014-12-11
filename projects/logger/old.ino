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
#include <SHT1x.h>
#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  
#include <ACnokia5100.h>
#include <ACrotatingPot.h>


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
// ACbuttonLong button(2);
const int buttonSensitivity = 300;

// Rotating potentiometer
int rotPin = A0;
const int rotSensitivity = 20; 
const boolean rotInverted = true; 
ACrotatingPot rot(rotPin, rotSensitivity, rotInverted);

// SHT1x : for measuring temperature and humidity
// (pins: dataPin, clockPin)
SHT1x sht1x(9, 10);

// ds3232 : for measuring clock and temperature
DS3232RTC ds3232;

// SD card : for writing log file
const int SDselectPin = 8;
const int SDhardwareCSPin = 10;

// Nokia5100 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
ACnokia5100 screen(3, 4, 5, 11, 13, 7);

// Measure configuration
const int nbMeasures = 3; // --currently, at most 5 measures
const String measureNames[] = { "humid.", "t_SHT1", "t_3232" }; // at most 6 chars
const int floatPrecision = 3; // nb digits after decimal point

// History configuration
const int maxHistory = 500; 


//*****************************************************************
/* Types of measures */

typedef struct {
  time_t date;
  float values[nbMeasures];
} Record;

// History data
int nbHistory;
Record history[maxHistory];


//*****************************************************************
/* History function */

void clearHistory() {
  nbHistory = 0;
}

void addRecordToHistory(Record& r) {
  // when history becomes full, start over
  if (nbHistory == maxHistory)
    clearHistory();
  history[nbHistory] = r;
  nbHistory++;
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

void printHistory() {
  Serial.println("===========================");
  for (int h = 0; h < nbHistory; h++) {
    printMeasureOnSerial(history[h]);
  }
  Serial.println("===========================");
}


//*****************************************************************
/* File functions */

/*

void readFileSerialPrint() { // TODO: move to library
  if (! SDused)
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
    Serial.println("Error opening file");
    SDused = false;
  }
  return file;
}

void writeLogHeader() {
  File file = openLog(FILE_WRITE);
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

void readRecordsFromLog() {
  File file = openLog(FILE_READ);
  if (! file)
    return;
  nbHistory = readRecordsFromFile(file, maxHistory, history);
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

*/

void readFileSerialPrint() {}
void resetLog() {}
void writeRecordToLog(Record& r) {}


//*****************************************************************
/* LCD functions */

String String_of_float(float value, int nbChars, int precision) {
  char chars[] = "                           "; // longer than screenWidth
  dtostrf(value, nbChars, precision, chars); 
  return "" + String(chars);
}

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

void screenClear() {
  screen.clearDisplay(screen.WHITE);
}

void displayMeasuresOnScreen(Record& r) {
  // TODO: ça ne marche pas...
  screenClear();
  char chrDate[20] = "\0";
  timeIntoString(r.date, chrDate, false);
  int line = 0;
  String strDate = String(chrDate);
  screen.setString(strDate, line++, 0);
  /*
  String str[nbMeasures];
  for (int m = 0; m < nbMeasures; m++) {
    str[m] = String_of_float(r.values[m], 5, floatPrecision);
  }
  for (int m = 0; m < nbMeasures; m++) {
    // float v = r.values[m];
    // Serial.println(v);
    // String strValue = String_of_float(v, 7, floatPrecision);
    // String str = measureNames[m] + ":" + strValue;
    String s = measureNames[m] + ":" + str[m];
    screen.setString(s, m, 0);
  }
  */
  screen.updateDisplay(); 
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
/* Menu 

int panel = 0;   // current panel being shown
int select = 0;  // current menu item selected
int measure = 0; // current measure viewed

const int nbChoices0 = 6; // at most 6
String choices0[nbChoices0] = {
  "disp. current",
  "disp. curve",
  "disp. measure",
  "set x-scale",
  "set y-scale",
  "set log params" };

void showChoices(int nbChoices, String* choices, int selected) {
  for (int i = 0; i < nbChoices; i++) {
    screen.setString(choices[i], i, 0, (i==selected));
  }
}

void changePanel(int newPanel) {
  int oldPanel = panel;
  panel = newPanel;
  int modulo = 0;
  int value = 0;
  if (panel == 0) {
    modulo = nbChoices0;
    value = oldPanel;
  } 
  rot.setModulo(modulo);
  rot.resetValue(value);
}

void displayPanel() {
  screenClear();
  if (panel == 0) {
    showChoices(nbChoices0, choices0, rot.getValue());
  } else {
    screen.setString("a panel", 0, 0);
  }
  screen.updateDisplay();
}

void shortClick() {
  Serial.println("short click");
  if (panel == 0) {
    changePanel(10 * panel + rot.getValue());
  } else {
    // go back
    changePanel(panel / 10);
  }
  Serial.println(panel);
}

void longClick() {
  Serial.println("long click");
  // return to previous menu
  changePanel(panel / 10);
  Serial.println(panel);
}
*/

void rotChange() {
  Serial.println("rot change");
  Serial.println(rot.getValue());
  // (change action is implicit on most panels)
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
  int line = 0; // Note: use at most 6 lines on screen

  // LCD screen
  screen.begin();
  screen.updateDisplay(); 
  screen.setContrast(60);
  screen.setString("Loading...", line++, 0);
  screen.updateDisplay(); 

  // Serial port
  if (serialUsed) {
    Serial.begin(serialBaudRate); 
    Serial.println("Starting");
    screen.setString("  Serial", line++, 0);
  }

  // SD card
  /*
  if (SDused) {
    pinMode(SDhardwareCSPin, OUTPUT); 
    if (! SD.begin(SDselectPin)) {
      if (serialUsed)
        Serial.println("Card failed or missing");
      SDused = false;
    }
  }
  */

  screen.setString("  SD:", line, 0);
  screen.setString(((SDused) ? "yes" : "no"), line, 5);
  line++;
  
  /*/ Button
  button.begin();
  button.setLongPeriodDuration(buttonSensitivity);
  button.onUp(shortClick);
  button.onUpAfterLong(longClick);
  */

  // Rotating potentiometer
  rot.begin();
  rot.onChange(rotChange);

  // DS3232 set initial time
  initializeTime();

  // Time.h library (adjust from ds3232 every 300 seconds)
  setSyncProvider(ds3232.get); 
  setSyncInterval(300); 
  screen.setString("  DS3232", line++, 0);

  // Reset log and start
  if (resetLogOnSetup) {
    resetLog();
    screen.setString("  Reset log", line++, 0);
    if (serialUsed)
      Serial.println("Reset log");
  }

  // Ready for action
  screen.setString("Ready...", line++, 0);
  screen.updateDisplay();
  if (serialUsed)
    Serial.println("Ready");
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
    Serial.println("record");
    Record current;
    makeMeasures(current);
    addRecordToHistory(current);
    if (serialUsed) {
      printMeasureOnSerial(current);
    }
    if (SDused) {
      writeRecordToLog(current); 
      readFileSerialPrint();
    }
    printHistory();
  }
  Serial.println("display");
  // displayPanel();
  // button.poll();
  rot.poll();
  delay(200);
}




// marche pas : displayMeasuresOnScreen(current);

//*****************************************************************


