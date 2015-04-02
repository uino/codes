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
boolean wakeActive = false;
time_t wakeHour = 0;
time_t wakeMinute = 0;


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
  screen.updateDisplay(); 
}

void screenKeepLight() {
  screenKeepLightSinceDate = millis();
  digitalWrite(pinNokiaBacklight, HIGH);
}

void screenProcess() {
  long now = millis();
  if (now - screenLastProcessDate < 300)
    return;
  if (now - screenKeepLightSinceDate() > screenKeepLightDuration) {
    digitalWrite(pinNokiaBacklight, LOW);
  }
  screenLastProcessDate = now;
  screenUpdate();
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

void alarmStop() {
  if (! alarmActive)
    return;
  alarmActive = false;
  digitalWrite(pinGreenLED, LOW);
  digitalWrite(pinTorch, LOW);
  digitalWrite(pinBuzzer, LOW);
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
      else if (alarmDurationInPhase > 1000)
        digitalWrite(pinTorch, LOW);
      break; }
    case 3: {
      if (newPhase != oldPhase)
        shortBip();
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

const long buttonLongClickDuration = 300; // ms
long buttonDateDown = 0;

void buttonAnyClick() {
  screenKeepLight();
  alarmStop();
}

void buttonShortClick() {
  // Serial.println("Short click");
}

void buttonLongClick() {
  // Serial.println("Long click");
}

void buttonProcess() {
  int v = analogRead(pinPhotoresistor);
  boolean isDown = (v < 10);
  if (isDown && buttonDateDown == 0) { // goes down
    buttonDateDown = millis();
  } else if (!isDown && buttonDateDown != 0) { // goes up
    long duration = millis() - buttonDateDown;
    buttonDateDown = 0;
    buttonAnyClick();
    if (duration < buttonLongClickDuration)
      buttonShortClick();
    else 
      buttonLongClick();
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
  pinMode(pinTorch, OUTPUT);
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
  alarmStop();

  // Demo:
  // alarmStart();

  wakeActive = true;
  wakeHour = 8;
  wakeMinute = 10;

}

//*****************************************************************

void loop() 
{
  //alarmProcess();
  // buttonProcess();
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

