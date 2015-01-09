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

  screen.begin();
  screen.setContrast(60); // good values range from 40-60
  screen.clearDisplay();

  Serial.println(6);
  for (int i = 0; i < nbRows; i++) {
    String line = String("line ") + String(i);
    Serial.println(i);
    Serial.println(line);
    screen.setString(line, i, 0);  
  }
  screen.updateDisplay();
}

void loop()
{
  // do nothing
  delay(1000);
}

