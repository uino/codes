/**
 * Code by Anatole Chargueraud and Arthur Chargueraud.
 * This code is GNU GPL v3.
 */

#include <Arduino.h> 

// #define Min * 60
//#define Min * 2

const int pinControl = 5;

const int nbDelays = 20;

int delays[nbDelays] = { // off,on  in seconds
  0, 15, 
  5, 15,
  5, 15,
  90, 5,
  0, 10, 
  10, 10,
  10, 10,
  60, 5
  };  

long dateLastEvent = 0; // in seconds
int idDelays = 0;
boolean state = 1; // initially on

void setup()
{
  /*
  Serial.begin(9600);   
  Serial.println("Starting up");
  */
  pinMode(pinControl, OUTPUT);
  digitalWrite(pinControl, state);
 
  dateLastEvent = millis();
}

void loop() 
{
  long dateNow = millis() / 1000; // in seconds
  if (dateNow - dateLastEvent >= delays[idDelays]) {
    dateLastEvent = dateNow;
    idDelays = (idDelays + 1) % nbDelays;
    state = !state;
    digitalWrite(pinControl, state);
    /*
    Serial.print("dateNow ");
    Serial.println(dateNow);
    */
  } 
  delay(500);
}

