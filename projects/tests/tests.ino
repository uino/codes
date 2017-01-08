
#include <Arduino.h>

//----------------------------------------------------------------------------

/**
 * Led
 */

int led1 = 6;
int led2 = 7;

void ledSetup() {                
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     
}


//----------------------------------------------------------------------------

/**
 * Serial
 */

void serialSetup() {
  Serial.begin(115200); 
}

//----------------------------------------------------------------------------

/**
 * Potar
 */

byte pinPotar = A0;

//----------------------------------------------------------------------------

void setup() {
  serialSetup();

  pinMode(pinPotar, INPUT);     
  
  ledSetup();
  Serial.println("Starting up");

      digitalWrite(led1, HIGH); 
    digitalWrite(led2, LOW); 

}

//----------------------------------------------------------------------------


/*
// independent motor and analog read

boolean direction = true;
long dateLastChange = 0;
long duration = 1000;

// the loop routine runs over and over again forever:
void loop() {
  long now = millis();
  if (now - dateLastChange > duration) {
    dateLastChange = now;
    direction = !direction;
    digitalWrite(led1, ( direction) ? HIGH : LOW); 
    digitalWrite(led2, (!direction) ? HIGH : LOW); 
  }

  int b = analogRead(pinPotar);
  Serial.println(b);

  delay(200);
}
*/

// moteur pas à pas, independent read

/*

long dateLastChange = 0;
boolean active = true;
long durationActive = 25;
long durationInactive = 30;

void loop() {
  long now = millis();
  long duration = now - dateLastChange;
  if (active && duration > durationActive) {
    dateLastChange = now;
    active = false;
    digitalWrite(led1, LOW); 
    digitalWrite(led2, LOW); 
  } else if (!active && duration > durationInactive) {
    dateLastChange = now;
    active = true;
    digitalWrite(led1, HIGH); 
    digitalWrite(led2, LOW); 
  }

  int b = analogRead(pinPotar);
  Serial.println(b);

  delay(10);
}
*/




long dateLastReport = 0;
boolean direction = true;
long valueLow = 300;
long valueHigh = 550;

void loop() {
  int b = analogRead(pinPotar);

  long now = millis();
  if (now - dateLastReport > 200) {
    dateLastReport = now;
    Serial.println(b);
  }

  if (direction && b >= valueHigh) {
    direction = false;
    digitalWrite(led1, LOW); 
    digitalWrite(led2, HIGH); 
  } else if (!direction && b <= valueLow) {
    direction = true;
    digitalWrite(led1, HIGH); 
    digitalWrite(led2, LOW); 
  }

  delay(10);
}

/* 

http://blog.3sigma.fr/category/arduino/
*/
