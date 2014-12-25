/**
 * Demo for the AC_UT390B library, for controlling a telemeter.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * The demo reports measures on every click of a button.
 *
 * Assumes Serial to be connected to a computer for reporting values.
 * Assumes Serial1 to be connected to the telemeter.
 *
 */

#include <AC_UT390B.h>
#include <ACbuttonLong.h>

const int buttonPin = 6;

ACbuttonLong button(buttonPin);


typedef enum { IDLE, ACQUIRE, READY, ERROR } Status;
Status status = IDLE;

float measureValue = 0;
const int bufferMaxLength = 16;
char buffer[bufferMaxLength+1];
int bufferPos = 0;
boolean parsingData = false;

void analyseBuffer() {
  buffer[bufferPos] = '\0';
  // for debug: Serial.println(buffer);
  int mul = 1;
  long res = 0;
  // read the value between digits at index 9 and 13
  for (int i = 13; i >= 9; i--) {
    res += mul * (buffer[i] - '0');
    mul *= 10;
  }
  measureValue = res / 1000.0;
  status = READY;
}

void measureProgress() {
  while (Serial1.available() > 0) {
    char c = (char) Serial1.read();
    // for debugging: Serial.print(c);
    if (c == '*') { // data start
      parsingData = true;
      bufferPos = 0;
    }
    if (c == '#') { // data end
      parsingData = false;
      if (bufferPos == bufferMaxLength) { // expecting a 16-char message
        analyseBuffer();
      }
    }
    if (parsingData == true && bufferPos < bufferMaxLength && c >= '0') { 
      buffer[bufferPos++] = c;
    }
  }
}

void measureRequest() {
  status = ACQUIRE;
  bufferPos = 0;
  parsingData = false;
  Serial1.write("*00004#");
}

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  // telemeter.begin();
  Serial1.begin(115200);
  Serial1.write("*100515#");
  Serial.println("Laser on");

  button.begin();
  button.onUp(measureRequest);
}

void loop()
{
  button.poll();

  if (status == ACQUIRE) {
    measureProgress();
  }
  if (status == ERROR) {
    Serial.println("Measure: error");
    status = IDLE;
//status = ACQUIRE;
  }
  if (status == READY) {
    Serial.print("Measure: ");
    Serial.println(measureValue, 3);
//Serial.println(buffer);
    status = IDLE;
//status = ACQUIRE;
  }

  delay(10);
}




/*
void readAndReport() {   
  while (Serial1.available() > 0) {
    byte c = Serial1.read();
    Serial.write(c);
  }
}

// AC_UT390B telemeter(&Serial1);

*/