/**
 * Demo for the ACbuttonLong library.
 * Code by Arthur Charguéraud.
 * This code is in the public domain.
 *
 *
 * Show in real time the state of a collection of sensors on a Nokia5100 display.
 *
 * - a button (digital input)
 * - a potentiometer (analog input)
 * - a temperature/humidity sensor (SHT1x sensor)
 * - a clock (DS1307)
 * - the duration since beginning
 *
 */


#include <ACnokia5100.h>
#include <RealTimeClockDS1307.h>
#include <ACbuttonLong.h>
#include <SHT1x.h>


// ACnokia5100 pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin
ACnokia5100 screen(3, 4, 5, 11, 13, 9);

// Clock:
RealTimeClockDS1307 clock;  

// Potentiometer pin:
int potentiometerPin = A0;

// ACbuttonLong pins: buttonPin
ACbuttonLong button(1);

// SHT1x pins: dataPin, clockPin
SHT1x sht1x(9, 10);


void setup()
{
  pinMode(potentiometerPin, INPUT);

  screen.setup();
  screen.updateDisplay(); 
  screen.setContrast(60); 
}

String String_of_float(float value, int nbChars, int precision) {
  char chars[] = "                           ";
  dtostrf(value, nbChars, precision, chars); 
  return String(chars);
}

int delay_between_SHT1x_measures = 2000; // milliseconds
long lastSHT1x_measure = - delay_between_SHT1x_measures;
float temperature = 0;
float humidity = 0;

// http://www.arduino-projects4u.com/sht11/

void loop()
{
  // periodic actions
  button.poll();

  // read sensors

  clock.readClock();
  int year = clock.getYear();
  int month = clock.getMonth();
  int day = clock.getDay();
  int hour = clock.getHours();
  int minute = clock.getMinutes();
  int second = clock.getSeconds();
  String dateStr = String(year) + "/" + String(month) + "/" + String(day);
  String clockStr = String(hour) + ":" + String(minute) + ":" + String(second);

  String buttonStr = (button.isDown()) ? "down" : "up";

  int potentiometerValue = analogRead(potentiometerPin);    
  String potentiometerStr = String(potentiometerValue);

  int date = millis();
  if (date - lastSHT1x_measure >= delay_between_SHT1x_measures) {
    lastSHT1x_measure = date;
    temperature = sht1x.readTemperatureC();
    humidity = sht1x.readHumidity();
  }

  // print on 7 characters, with 2 digit after decimal points
  String temperatureStr = String_of_float(temperature, 7, 2);
  String humidityStr = String_of_float(humidity, 7, 2);

  // print measures

  screen.clearDisplay(screen.WHITE);
  int line = 0;
  screen.setString(dateStr, line++, 0);
  screen.setString(clockStr, line++, 0);
  screen.setString("button:" + buttonStr, line++, 0);
  screen.setString("poten.:" + potentiometerStr, line++, 0);
  screen.setString("temp. :" + temperatureStr, line++, 0);
  screen.setString("humid.:" + humidityStr, line++, 0);
  screen.updateDisplay();

  delay(100);
}



/*
// Simplified version for button

void loop()
{
  button.poll();
  String buttonStr = (button.isDown()) ? "down" : "up";
  screen.clearDisplay(screen.WHITE);
  screen.setString("button:" + buttonStr, 1, 0);
  screen.updateDisplay();
  delay(500);
}

// Simplified version for potentiometer

void loop()
{
  int potentiometerValue = analogRead(potentiometerPin);    
  String potentiometerStr = String(potentiometerValue);
  screen.clearDisplay(screen.WHITE);
  screen.setString("poten.:" + potentiometerStr, 0, 0);
  screen.updateDisplay();
  delay(100);
}


*/


