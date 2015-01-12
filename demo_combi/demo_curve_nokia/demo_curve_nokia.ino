/**
 * Demo for the AC_Button library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 * 
 * Show in real time the state of a button on a Nokia5110 display.
 *
 */


#include <AC_Button.h>
#include <AC_Nokia5110.h>

// AC_Nokia5110 pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin
AC_Nokia5110 screen(3, 4, 5, 11, 13, 9);

// Potentiometer pin:
int potentiometerPin = A0;

void setup()
{
  pinMode(potentiometerPin, INPUT);

  screen.begin();
  screen.updateDisplay(); 
  screen.setContrast(60); 
}

const int nbValues = screen.LCD_WIDTH;
int values[nbValues];
long idValue = 0;
int maxValue = 1024;

int delay_between_refresh = 100; // milliseconds
long last_refresh = 0;


void loop()
{
  int potentiometerValue = analogRead(potentiometerPin);    
  values[idValue % nbValues] = potentiometerValue;
  idValue++;

  int date = millis();
  if (date - last_refresh >= delay_between_refresh) {
    last_refresh = date;

    screen.clearDisplay(screen.WHITE);
    for (int x = 0; x < nbValues; x++) {
      float r = (((float) values[x]) * screen.LCD_HEIGHT) / maxValue; // value between 0 and screen.HEIGHT-1
      int y = screen.LCD_HEIGHT - 1 - (int) r; // value between 0 and screen.HEIGHT-1
      screen.setPixel(x, y);
    }
    screen.setString(String(potentiometerValue), 0, 0);
    screen.updateDisplay();
  }

  delay(20);
}
