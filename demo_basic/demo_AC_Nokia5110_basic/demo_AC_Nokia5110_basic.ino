/* 
 * Arthur Chargueraud
 * This code is in the public domain.
 *
 * Demo associated with library "AC_Nokia5110".
 */

#include <AC_Nokia5110.h>

// Nokia5110 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
AC_Nokia5110 screen(3, 4, 5, 11, 13, 7);

const int nbRows = AC_Nokia5110::LCD_ROWS;

void setup()
{
  Serial.begin(9600);
  Serial.println("Start up");

  screen.begin();
  screen.setContrast(60); // good values range from 40-60
  screen.clearDisplay();

  for (int i = 0; i < nbRows; i++) {
    String line = String("line ") + String(i);
    boolean bw = ((i%2)==1);
    screen.setString(line, i, 0, bw);  
    Serial.println(line);
  }
  screen.updateDisplay();
}

void loop()
{
  // do nothing
  delay(1000);
}

