/**
 * ...
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */

// #include <Arduino>

 #define TESTING 1

//*****************************************************************

#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>
#include <AC_Nokia5110_light.h>
#include <AC_RAM.h>
#include <AC_Vector.h>

//*****************************************************************

const byte pinGreenLED = 6;
const byte pinTorch = 10;
const byte pinBuzzer = 12;
const byte pinPhotoresistor = A3; 
const byte pinDS3232vcc = 7;
const byte pinNokiaBacklight = 9;
const byte interruptPinWakeUp = 0; // actual pin2
// pin2 is interrupt 0
// pin3 is interrupt 1

//*****************************************************************

// Nokia5110 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
AC_Nokia5110_light screen(3, 4, 5, 11, 13, 9);

DS3232RTC ds3232;

time_t currentTime = 0;

//*****************************************************************

//boolean screenActive = false;
const long activityTimeout = 12 * 1000; // ms
const long buttonLongClickDuration = 300; // ms
long activityLastDate = 0;

boolean wakeActive = false;
time_t wakeHour = 0;
time_t wakeMinute = 0;

long buttonDateDown = 0;
long editorStatus = 0; // 0: inactive,  1: alarmstatus, 2: hour+, 3: hour-, 4: minutes+, 5: minutes-,  
int editorLastChange = 0;



//*****************************************************************

const int alarmNbPhases = 5;
const int alarmPhases[alarmNbPhases] = {  
   10,  // LED on
   5,  // LED blink
   5,  // Torch
   5,  // Bib
   5,  // Bips
   };

long alarmLastProcessDate = 0;
boolean alarmActive = false;
long alarmStartDate = 0;
int alarmPhase = 0;
int alarmDurationInPhase = 0;
int alarmSteps = 0;

void alarmStart() {
  alarmActive = true;
  alarmLastProcessDate = millis();
  alarmStartDate = millis();
  alarmPhase = 0;
  alarmDurationInPhase = 0;
}

void alarmOff() {
  digitalWrite(pinGreenLED, LOW);
  digitalWrite(pinTorch, LOW);
  digitalWrite(pinBuzzer, LOW);
}

void alarmStop() {
  // if (! alarmActive) // optional
  //  return;
  alarmActive = false;
  alarmOff();
}

void shortBip() {
  digitalWrite(pinBuzzer, HIGH);
  delay(50);
  digitalWrite(pinBuzzer, LOW);
}

int alarmSelectPhase() {
  long currentDate = millis();
  long duration = currentDate - alarmStartDate;
  int newPhase = 0;
  int seconds = 0;
  for (newPhase = 0; newPhase < alarmNbPhases; newPhase++) {
    alarmDurationInPhase = duration - seconds;
    seconds += 1000 * alarmPhases[newPhase];
    if (duration <= seconds)
       break;
  }
  return newPhase;
}

void alarmProcess() {
  if (! alarmActive)
    return;
  long now = millis();
  if (now - alarmLastProcessDate < 200)
    return;
  alarmLastProcessDate = now;

  int oldPhase = alarmPhase;
  int newPhase = alarmSelectPhase();
  alarmPhase = newPhase;
  // Serial.print("Phase: ");
  // Serial.println(alarmPhase);
  switch (newPhase) {
    case 0: {
      digitalWrite(pinGreenLED, HIGH);
      break; }
    case 1: {
      if (newPhase != oldPhase)
        alarmSteps = 0;
      if (alarmSteps % 2 == 0)
        digitalWrite(pinGreenLED, HIGH);
      else
        digitalWrite(pinGreenLED, LOW);
      alarmSteps++;
      /* 
      if (alarmDurationInPhase > alarmSteps * 200) {
      }
      */
      break; }
    case 2: {
      if (newPhase != oldPhase) {
        digitalWrite(pinGreenLED, LOW);
        digitalWrite(pinTorch, HIGH);
      }
      else if (alarmDurationInPhase > 4000)
        digitalWrite(pinTorch, LOW);
      break; }
    case 3: {
      if (newPhase != oldPhase) {
        digitalWrite(pinTorch, LOW);
        shortBip();
      }
      break; }
    case 4: {
      /*if (newPhase != oldPhase)
        alarmSteps = 0;
      if (alarmDurationInPhase > alarmSteps * 200) {
        alarmSteps++;
        shortBip();
      }*/
      shortBip();
      break; }
    default: {
      alarmStop();
      break; }
  }
}



//*****************************************************************

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

//*****************************************************************
// TODO: factorize into library

// Wiring of accelerometer
const int pinX = A0;
const int pinY = A1;
const int pinZ = A2;

// calibration.
const int minX = 270;
const int maxX = 408;
const int minY = 261;
const int maxY = 402;
const int minZ = 261;
const int maxZ = 397;

// local logging
Vector accelLow, accelHigh;

// global logging, every 6 seconds for 10 hours (every 2 seconds in testing mode)
int accelNbReports = 0;

const int accelNbBitsPerValue = 3;
#ifdef TESTING
int accelReportPeriod = 2; // seconds
const int accelMaxNbReports = 200;
const int accelNbValuesPerLong = 4;
#else
int accelReportPeriod = 6; // seconds
const int accelMaxNbReports = 6000;
const int accelNbValuesPerLong = 21;
#endif
unsigned long accelReports[accelMaxNbReports/accelNbValuesPerLong]; // fits 21  3-bit values per long value

int accelReportsGet(int i) {
  unsigned long v = accelReports[i / accelNbValuesPerLong];
  int s = accelNbBitsPerValue * (i % accelNbValuesPerLong); 
  return (int) ((v >> s) % (1 << accelNbBitsPerValue)); 
}

void accelReportsDump() {
  Serial.print("Dump: ");
  int n = 0;
  int r = (1 << accelNbBitsPerValue);
  for (int k = 0; true; k++) {
    unsigned long v = accelReports[k];
    for (int p = 0; p < accelNbValuesPerLong; p++) {
      if (n == accelNbReports)
        goto out;
      n++;
      Serial.print((int) (v % r));
      Serial.print(" ");
      v /= r;   
    }
  }
 out:
  Serial.println("");
}


void accelReportsSet(int i, int v) {
#ifdef TESTING
  accelReportsDump();
#endif
  if (v < 0 || v >= (1 << accelNbBitsPerValue))
    return; // error
  unsigned long oldv = accelReports[i / accelNbValuesPerLong];
  int s = accelNbBitsPerValue * (i % accelNbValuesPerLong);
  int snext = s + accelNbBitsPerValue;
  unsigned long high = (oldv >> snext) << snext;
  unsigned long low = oldv % (1 << s);
  unsigned long newv = high + (v << s) + low;
  accelReports[i / accelNbValuesPerLong] = newv;
}

// float version of the Arduino "map" function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// const int accelLoggingPeriod = 5; // seconds
long accelLastProcessDate = 0; // milliseconds 
time_t accelStartReportDate = 0; //seconds
time_t accelLastReportDate = 0; //seconds

// converts the norm of the acceleration vector (from normalized measures)
// into a value from 0 to 7.
int shock(float v) {
  const int nbBoundries = 7;
  float boundries[nbBoundries] = {  
    0.038, 0.046, 0.052, 0.06, 0.08, 0.12, 0.25 };
  for (int i = 0; i < nbBoundries; i++) {
    if (v < boundries[i])
      return i;
  }
  return nbBoundries;
}

void accelDump() {
  Serial.begin(9600);
  Serial.print("MeasuresStart:\t");
  Serial.print(accelStartReportDate);
  Serial.print("\t");
  printTime(accelStartReportDate);
  Serial.print("NbMeasure:\t");
  Serial.println(accelNbReports);
  for (int i = 0; i < accelNbReports; i++) {
    Serial.println(accelReportsGet(i));
  }
}

void accelSetup() {
  Serial.println("MeasuresReset");
  accelLow = Vector();
  accelHigh = Vector();
  accelStartReportDate = ds3232.get(); 
  accelNbReports = 0;
}

void accelProcess() {
  int x = analogRead(pinX);
  int y = analogRead(pinY);
  int z = analogRead(pinZ);

  // TODO: introduce min/max/normalize functions on vectors
  float xg = mapFloat(x, minX, maxX, 1., -1);
  float yg = mapFloat(y, minY, maxY, 1., -1);
  float zg = mapFloat(z, minZ, maxZ, 1., -1);
  accelLow.x = min(accelLow.x, xg);
  accelLow.y = min(accelLow.y, yg);
  accelLow.z = min(accelLow.z, zg);
  accelHigh.x = max(accelHigh.x, xg);
  accelHigh.y = max(accelHigh.y, yg);
  accelHigh.z = max(accelHigh.z, zg);

  if (accelNbReports >= accelMaxNbReports)
    return;
  /*
  long now = millis();
  if (now - accelLastProcessDate < 300 || accelNbReports >= accelMaxNbReports)
    return;
  accelLastProcessDate = now + 300; // wait a bit before next time measure
  currentTime = ds3232.get();
  */
  currentTime = now();
  if (currentTime > accelLastProcessDate && 
      currentTime % accelReportPeriod == 0) { 
    accelLastReportDate = currentTime;
    Vector move = accelHigh - accelLow;  // each direction between +/- 3.2 roughly
    float strength = move.norm(); // between 0 and 16 roughtly
    int value = shock(move.norm());
    accelReportsSet(accelNbReports++, value);
    Serial.print("Measure:\t");
    Serial.println(value);
    accelLow = Vector(xg, yg, zg);
    accelHigh = Vector(xg, yg, zg);
  }
}



//*****************************************************************

const long screenKeepLightDuration = 4 * 1000; // ms
long screenLastProcessDate = 0;
long screenKeepLightSinceDate = 0;

void showTime(int line, int col, int hour, int minute, int seconds = 0, boolean showSeconds = false) {
  screen.setInt(hour, line, col, 2, false);
  col += 2;
  screen.setString(":", line, col);
  col += 1;
  screen.setInt(minute, line, col, 2, true);
  col += 2;
  if (showSeconds) {
    screen.setString(":", line, col);
    col += 1;
    screen.setInt(seconds, line, col, 2, true);
    col += 2;
  }
}

void screenEditor(int line, int colInfos) {
  switch (editorStatus) {
    case 0: { 
      break; }
    case 1: { 
      if (wakeActive)
        screen.setString("activated", line, 0);
      else
        screen.setString("deactivated", line, 0);
      break; }
    case 2: { 
      screen.setString("+", line, colInfos+1);
      break; }
    case 3: { 
      screen.setString("-", line, colInfos+1);
      break; }
    case 4: { 
      screen.setString("+", line, colInfos+4);
      break; }
    case 5: { 
      screen.setString("-", line, colInfos+4);
      break; }
    default: { 
      break; }
  }
}

void screenUpdate() {
  currentTime = ds3232.get(); // might cache this value
  int line = 0;
  int colInfos = 6;
  screen.clearDisplay(); 
  screen.setString("time :", line, 0);
  showTime(line, colInfos, hour(currentTime), minute(currentTime), second(currentTime), true);
  line++;
  screen.setString("alarm:", line, 0);
  if (wakeActive)
    showTime(line, colInfos, wakeHour, wakeMinute);
  else
    screen.setString("off", line, colInfos);
  line++;
  screenEditor(line, colInfos);
  line++;
  screen.setString("meas:", line, 0);
  screen.setInt(accelNbReports, line, colInfos, 5, false);
  line++;
  if (accelNbReports > 0) {
    screen.setString("last:", line, 0);
    screen.setInt(accelReportsGet(accelNbReports-1), line, colInfos, 2, false);
  }
  /*
  line++;
  screen.setString("cur:", line, 0);
  screen.setInt(cur, line, colInfos, 2, false);
  */
  screen.updateDisplay(); 
}

void screenKeepLight() {
  screenKeepLightSinceDate = millis();
  digitalWrite(pinNokiaBacklight, HIGH);
  //screenActive = true;
}

void screenProcess() {
  //if (! screenActive)
  //  return;
  long now = millis();
  if (now - screenLastProcessDate < 300)
    return;
  screenLastProcessDate = now;
  if (now - screenKeepLightSinceDate > screenKeepLightDuration) {
    digitalWrite(pinNokiaBacklight, LOW);
  }
  screenUpdate();
}


//*****************************************************************


void buttonAnyClick() {
  activityLastDate = millis();
  screenKeepLight();
  alarmStop();
}

void buttonShortClick() {
  // Serial.println("Short click");
  buttonAnyClick();
  if (editorStatus != 0)
    editorLastChange = editorStatus;
  switch (editorStatus) {
    case 0: { 
      break; }
    case 1: { 
      wakeActive = ! wakeActive;
      break; }
    case 2: { 
      wakeHour = (wakeHour+1) % 24;
      break; }
    case 3: { 
      wakeHour = (wakeHour+23) % 24;
      break; }
    case 4: { 
      wakeMinute = (wakeMinute+5) % 60; // +5
      break; }
    case 5: { 
      wakeMinute = (wakeMinute+55) % 60; // +55
      break; }
    default: { 
      break; }
  }
  screenUpdate();
}

void buttonLongClick() {
  // Serial.println("Long click");
  buttonAnyClick();
  if (editorStatus == 0 && ! wakeActive) { // enter automatically activates
    wakeActive = true;
    editorStatus = 2;
  } else if (editorStatus == 1 && ! wakeActive) { // exit if deactivated
    editorStatus = 0;
  } else if (editorStatus == 2 && editorLastChange == 2) { // changed on hour+
    editorStatus = 4;
  } else if (editorStatus == 4 && editorLastChange == 4) { // changed on minutes+
    editorStatus = 0;
  } else { // all other cases
    editorStatus = (editorStatus+1) % 6;
  }
  editorLastChange = 0;
  screenUpdate();
  // Serial.print("Status: "); Serial.println(editorStatus);
}

void buttonProcess() {
  int v = analogRead(pinPhotoresistor);
  boolean isDown = (v < 10);
  if (isDown && buttonDateDown == 0) { // goes down
    buttonDateDown = millis();
  } else if (!isDown && buttonDateDown != 0) { // goes up
    long duration = millis() - buttonDateDown;
    buttonDateDown = 0;
    if (duration > 6000) // reset
      accelSetup();
    else if (duration > 3000) // dump
      accelDump();
    else if (duration < buttonLongClickDuration)
      buttonShortClick();
    else 
      buttonLongClick();
  }
  if (millis() - activityLastDate > activityTimeout) {
    if (editorStatus != 0)
      editorStatus = 0;
    /*screenActive = false;
    screen.clearDisplay(); 
    screen.updateDisplay(); */
    // Serial.print("Activity timeout");
  }
}


//*****************************************************************

// TODO: a check function to factorize the pattern of checking the time

//*****************************************************************

long wakeLastProcessDate = 0;

void wakeProcess() {
  if (! wakeActive)
    return;
  long now = millis();
  if (now - wakeLastProcessDate < 300)
    return;
  wakeLastProcessDate = now;
  currentTime = ds3232.get();
  if ( hour(currentTime) == wakeHour
    && minute(currentTime) == wakeMinute
    && second(currentTime) < 5) {
    wakeActive = false; // stop an alarm once it has been triggered
    alarmStart();
  }
}


//*****************************************************************

void setup()
{
  // --needed??
  digitalWrite (2, HIGH);

  // DS3232 alimentation on
  pinMode(pinDS3232vcc, OUTPUT);
  digitalWrite(pinDS3232vcc, HIGH);

  // Pin modes
  pinMode(pinGreenLED, OUTPUT);
  digitalWrite(pinGreenLED, LOW);
  pinMode(pinTorch, OUTPUT);
  digitalWrite(pinTorch, LOW);
  pinMode(pinPhotoresistor, INPUT); 

  // Serial
  Serial.begin(9600);   
  Serial.println("Starting up");
  delay(100);

  // Time
  setSyncProvider(ds3232.get);
  if (timeStatus() != timeSet) 
     Serial.println("Time.h: unable to sync with the RTC");

  // Screen
  screen.begin();
  screen.setContrast(60); // good values range from 40-60
  screen.clearDisplay();

  // Display date
  Serial.print("Date: ");
  printTime(ds3232.get());
  delay(100);

  // All output off
  alarmOff();

  // Default
  wakeActive = false;
  wakeHour = 7; 
  wakeMinute = 30;

  // Accelerometer
  accelSetup();

  // Debug
  Serial.print(F("SRAM free: "));
  Serial.println(AC_RAM::getFree());
}


//*****************************************************************


void loop() 
{
  wakeProcess();
  alarmProcess();
  accelProcess();
  buttonProcess();
  screenProcess();
  delay(30);
}


//*****************************************************************

#ifdef TEST_PHOTO_RESISTOR
  pinMode(pinPhotoresistor, INPUT); 
  int v = analogRead(pinPhotoresistor);
  Serial.println(v);
  delay(300);
#endif

