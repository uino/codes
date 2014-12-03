/**
 * Demo for the ACbuttonLong library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 * 
 * Show in real time the state of a button and a potentiometer
 * on a Nokia5100 display. Also show how the two inputs can be 
 * used to efficiently set values.
 * 
 */


#include <ACbuttonLong.h>
#include <ACnokia5100.h>
#include <math.h>

// ACnokia5100 pins: scePin(3), rstPin(4), dcPin(5), sdinPin(6), sclkPin, blPin
// ACnokia5100 screen(3, 4, 5, 11, 13, 9);
ACnokia5100 screen(3, 4, 5, 6, 7, 0);

// ACbuttonLong pins: buttonPin
ACbuttonLong button(2);

// Pin potentiometer
int potentiometerPin = A0;

// Value to be controlled
int value = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("start");

  pinMode(potentiometerPin, INPUT);

  screen.setup();
  screen.updateDisplay(); 
  screen.setContrast(60); 
  screen.setString("coucou", screen.LCD_ROWS-1, 0);

  // delay(1000);
}

long lastDisplay = 0;
long lastModif = 0;

void loop()
{
  long now = millis();
  if (now - lastDisplay > 200) {    
    lastDisplay = now;

    Serial.println(value);

    button.poll();

    screen.clearDisplay(screen.WHITE);

    if (button.isDown()) {
      screen.setString("down", screen.LCD_ROWS-1, 0);
      Serial.println("down");
    } else {
      screen.setString("up", screen.LCD_ROWS-1, 0);
      Serial.println("up");
    }

    int sensorValue = analogRead(potentiometerPin);    
    Serial.println("pot = " + String(sensorValue));
    screen.setString(String(sensorValue), screen.LCD_ROWS-2, 0);

    screen.updateDisplay();
    /*
    */
  }

  now = millis();
  if (now - lastModif >= 250) {
    lastModif = now;
    int controllerValue = 1023 - analogRead(potentiometerPin);
    int middle = 500;
    int neutralZone = 100;
    int widthZone = 50;
    float diff = abs(controllerValue - middle);
    if (diff > neutralZone) {
      int change = (int) (1 + log((diff - neutralZone) / widthZone));
      int sign = (controllerValue > middle) ? 1 : - 1;
      value += sign * change;
    }
  }

  delay(0);
}
