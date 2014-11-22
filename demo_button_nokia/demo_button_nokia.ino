/**
 * Demo for the ACbuttonLong library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 * 
 * Show in real time the state of a button on a Nokia5100 display.
 *
 */


#include <ACbuttonLong.h>
#include <ACnokia5100.h>

// ACnokia5100 pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin
ACnokia5100 screen(3, 4, 5, 11, 13, 9);

// ACbuttonLong pins: buttonPin
ACbuttonLong button(1);

void setup()
{
  screen.setup();
  screen.updateDisplay(); 
  screen.setContrast(60); 
  screen.setString("coucou", screen.LCD_ROWS-1, 0);
  delay(1200);
}

void loop()
{
  button.poll();

  screen.clearDisplay(screen.WHITE);

  if (button.isDown()) 
    screen.setString("down", screen.LCD_ROWS-1, 0);
  else
    screen.setString("up", screen.LCD_ROWS-1, 0);

  screen.updateDisplay();

  delay(200);
}
