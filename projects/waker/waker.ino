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

//*****************************************************************

const byte pinGreenLED = 6;
const byte pinTorch = 10;
const byte pinBuzzer = 12;
const byte pinPhotoresistor = A3; 
const byte pinDS3232vcc = 7;

const byte interruptPinWakeUp = 0; // actual pin2
// pin2 is interrupt 0
// pin3 is interrupt 1

//*****************************************************************

DS3232RTC ds3232;

time_t dateAlarm = 0;


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
   3,  // LED on
   3,  // LED blink
   3,  // Torch
   3,  // Bib
   3,  // Bips
   };

boolean alarmActive = false;
long alarmStartDate = 0;
int alarmPhase = 0;

void alarmStart() {
  alarmActive = true;
  alarmStartDate = ds3232.get();
  alarmPhase = 0;
}

void alarmStop() {
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
  long currentDate = ds3232.get();
  long duration = currentDate - alarmStartDate;
  int newPhase = 0;
  int seconds = 0;
  for (newPhase = 0; newPhase < alarmNbPhases; newPhase++) {
    seconds += alarmPhases[newPhase];
    if (duration <= seconds)
       break;
  }
  return newPhase;
}

void alarmProcess() {
  if (! alarmActive)
    return;
  int oldPhase = alarmPhase;
  int newPhase = alarmSelectPhase();
  alarmPhase = newPhase;
  Serial.print("Phase: ");
  Serial.println(alarmPhase);
  switch (newPhase) {
    case 0: {
      digitalWrite(pinGreenLED, HIGH);
      break; }
    case 1: {
      digitalWrite(pinGreenLED, HIGH);
      delay(100);
      digitalWrite(pinGreenLED, LOW);
      break; }
    case 2: {
      if (newPhase != oldPhase)
        digitalWrite(pinTorch, HIGH);
        delay(100);
        digitalWrite(pinTorch, LOW);
      break; }
    case 3: {
      if (newPhase != oldPhase)
        shortBip();
      break; }
    case 4: {
      if (newPhase != oldPhase) {
        for (int i = 0; i < 10; i++) {
          shortBip();
          delay(200);
        }
      }
      break; }
    default: {
      alarmStop();
      break; }
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

  // All output off
  alarmStop();

  // Serial
  Serial.begin(9600);   
  Serial.println("Starting up");
  delay(100);

  // Display date
  Serial.print("Date: ");
  printTime(ds3232.get());
  delay(100);

  // Display alarm
  Serial.print("Alarm: ");
  printTime(dateAlarm);
  delay(100);

  alarmStart();
}

//*****************************************************************

void loop() 
{
  alarmProcess();
  delay(200);
}


//*****************************************************************

#ifdef TEST_PHOTO_RESISTOR
  pinMode(pinPhotoresistor, INPUT); 
  int v = analogRead(pinPhotoresistor);
  Serial.println(v);
  delay(300);
#endif

