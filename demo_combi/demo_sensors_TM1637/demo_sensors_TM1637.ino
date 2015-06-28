/**
 * Demo for displaying several values on a TM1637 4-digit display
 * and use a button to switch between different sensors, or to
 * activate an autoscroll mode.
 *
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 */

//****************************************************************
// Code for custom display on TM1637

/*
 *   --A--
 *   F   B
 *   --G--
 *   E   C
 *   --D-- (X)       (X=decimal point)
 * for a code, start with "Ob" then control segments: XGFEDCBA
 * e.g. E is written 0b01111001.
 */


//****************************************************************

#include <AC_TM1637.h>
#include <AC_Button.h>



//****************************************************************
// Configuration

// Button
const int longClickDuration = 1000; // milliseconds
const int buttonPin = 6;
AC_Button button(buttonPin);

// Display
const int displayPinCLK = 11;
const int displayPinDIO = 12;
AC_TM1637 display(displayPinCLK, displayPinDIO);

// Panels
const int panelAutoScrollDelay = 700; // milliseconds
const boolean panelAutoScrollInitial = true;


//****************************************************************
// Sensors

int counter = 0;
float temperature = 19.4;
float voltage = 14.5;
long startDate = 0;


//****************************************************************
// Display panel sensors

int panel = 0;
boolean panelAutoScroll = panelAutoScrollInitial;
long dateLastChange = 0;

const int nbPanels = 8; // nb different panels to show

void panelDisplay() {
  if (panel == 0) {
    byte segs[4] = { 0b01000000, 0b01000000, 0b01000000, 0b01000000 };
    display.setSegments(segs);

  } else if (panel == 1) {
    display.showFloat(temperature, 2); 

  } else if (panel == 2) {
    display.showFloat(voltage, 1, true); // with leading zero; display 014.5

  } else if (panel == 3) {
    display.setDigits(0, 0, 0, 1);

  } else if (panel == 4) {
    display.showTime(14, 32); // display 14:32

  } else if (panel == 5) {
    display.showInt(counter % 10000);

  } else if (panel == 6) {
    int duration = (millis() - startDate) / 1000; // seconds
    display.showInt(duration);

  } else if (panel == 7) {
    byte segs[4] = { 0b01110111, 0b01111001, 0b10111111, 0b00000110 };
    display.setSegments(segs);

  } else if (panel == 8) {
    display.setDigits(0,0,0,0);
  }
}


//****************************************************************
// Display panel generic

void panelNext() {
  panel = (panel+1) % nbPanels;
  dateLastChange = millis(); 
}

void panelUpdate() {
  if (   panelAutoScroll
      && (millis() - dateLastChange > panelAutoScrollDelay)) {
    panelNext();
  }
  panelDisplay();
}

void panelToggleAutoScroll() {
  panelAutoScroll = ! panelAutoScroll;
}


//****************************************************************
// Click Handlers

void upEvent() {
  Serial.print("up short: ");
  Serial.println(button.downDuration());
  panelNext();
  panelUpdate();
}

void upLongEvent() {
  Serial.print("up long: ");
  Serial.println(button.downDuration());
  panelToggleAutoScroll();
}


//****************************************************************
// setup

void setup()
{
  // serial
  Serial.begin(9600);   

  // display
  display.begin();
  display.setDigits(0,0,0,0);

  // button
  button.begin();
  button.setLongPeriodDuration(longClickDuration);
  button.onUp(upEvent);
  button.onUpLong(upLongEvent);

  // sensors
  startDate = millis();

  // ready
  Serial.println("Starting up");
}


//****************************************************************
// loop

void loop()
{
  button.poll();
  panelUpdate();

  // sensors
  counter++;

  delay(50);
}
