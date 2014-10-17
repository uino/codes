/**
 * Demo for the ACbuttonLong library.
 * Code by Arthur Charguéraud
 * Copyleft code.
 */


#include <ACbuttonLong.h>
#include <ACnokia5100x.h>

ACnokia5100x screen(3, 4, 5, 11, 13, 9);

const int buttonPin = 1;
const long durationLong = 200; // milliseconds

ACbuttonLong button(buttonPin, durationLong);

void setup()
{
  screen.setup(); // This will setup our pins, and initialize the LCD
  screen.updateDisplay(); // with displayMap untouched, blank screen
  screen.setContrast(60); // Good values range from 40-60

  // Serial.begin(9600);   
  // Serial.println("Starting up");
  screen.setString("coucou", 0, screen.LCD_HEIGHT-8, screen.BLACK);

}

void loop()
{
  button.poll();

  screen.clearDisplay(screen.WHITE);

  if (button.isDown()) 
    screen.setString("down", 0, screen.LCD_HEIGHT-8, screen.BLACK);
  else
    screen.setString("up", 0, screen.LCD_HEIGHT-8, screen.BLACK);

  screen.updateDisplay();

  delay(200);
}
