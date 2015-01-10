/* 
 * Arthur Chargueraud
 * This code is in the public domain.
 *
 * Demo associated with library "AC_Nokia5100_light".
 */

#include <AC_Nokia5100_light.h>

// Nokia5100 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
AC_Nokia5100_light screen(3, 4, 5, 11, 13, 7);

const int nbRows = AC_Nokia5100_light::LCD_ROWS;

void setup()
{
  Serial.begin(9600);
  Serial.println("Start up");

  screen.begin();
  screen.setContrast(60); // good values range from 40-60
  screen.clearDisplay();

  screen.setString("This is the demo of a multiline string that spans over many lines", 0, 0); screen.updateDisplay();
  delay(5000);
  screen.clearDisplay();

  for (int i = 0; i < nbRows; i++) {
    String line = String("line ") + String(i);
    screen.setString(line, i, 0);  
    Serial.println(line);
  }
  screen.updateDisplay();

}

void loop()
{
  // do nothing
  delay(1000);
}

