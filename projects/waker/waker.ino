/**
 * ...
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */

// #include <Arduino>

//*****************************************************************

#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>
#include <AC_Nokia5110_light.h>

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
    if (duration < buttonLongClickDuration)
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
}


//*****************************************************************


void loop() 
{
  wakeProcess();
  alarmProcess();
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

