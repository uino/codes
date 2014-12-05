/* 
 * Arthur Chargueraud
 * This code is in the public domain.
 *
 * Demo associated with library "ACnokia5100".
 */

#include <ACnokia5100.h>

// ACnokia5100 pins: scePin(3), rstPin(4), dcPin(5), sdinPin(6), sclkPin, blPin
ACnokia5100 screen(3, 4, 5, 11, 13, 7);

const int WHITE = ACnokia5100::WHITE;
const int BLACK = ACnokia5100::BLACK;
const int nbRows = ACnokia5100::LCD_ROWS;

void setup()
{
  Serial.begin(9600);

  screen.setup();
  screen.setContrast(60); // good values range from 40-60
  screen.clearDisplay(WHITE);

  Serial.println(6);
  for (int i = 0; i < nbRows; i++) {
    String line = String("line ") + String(i);
    boolean bw = ((i%2)==1);
    Serial.println(i);
    Serial.println(line);
    screen.setString(line, i, 0, bw);  
  }
  screen.updateDisplay();
}

void loop()
{
  // do nothing
  delay(1000);
}

