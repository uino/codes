/**
 * Demo for the ACbuttonLong library.
 * Code by Arthur Charguéraud.
 * This code is in the public domain.
 */


#include <ACbuttonLong.h>
#include <ACnokia5100x.h>

ACnokia5100x screen(3, 4, 5, 11, 13, 9);

const int buttonPin = 2;
const long durationLong = 200; // milliseconds

ACbuttonLong button(buttonPin, durationLong);

void setup()
{
  screen.setup();
  screen.updateDisplay(); 
  screen.setContrast(60); 
  screen.setString("coucou", screen.LCD_ROWS-1, 0, screen.BLACK);
  delay(1200);
}

void loop()
{
  button.poll();

  screen.clearDisplay(screen.WHITE);

  if (button.isDown()) 
    screen.setString("down", screen.LCD_ROWS-1, 0, screen.BLACK);
  else
    screen.setString("up", screen.LCD_ROWS-1, 0, screen.BLACK);

  screen.updateDisplay();

  delay(200);
}
