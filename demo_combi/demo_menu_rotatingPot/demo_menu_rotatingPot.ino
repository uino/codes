/* 
 * Arthur Chargueraud
 * This code is in the public domain.
 *
 * Demo associated with library "ACnokia5100".
 */

#include <ACnokia5100.h>

// ACnokia5100 pins: scePin(3), rstPin(4), dcPin(5), sdinPin(6), sclkPin, blPin
//ACnokia5100 screen(3, 4, 5, 11, 13, 9);
// ACnokia5100 screen(3, 4, 5, 6, 7, 0);
ACnokia5100 screen(3, 4, 5, 11, 13, 7);

const int WHITE = ACnokia5100::WHITE;
const int BLACK = ACnokia5100::BLACK;
const int nbRows = ACnokia5100::LCD_ROWS;

String spaces[8] = {
  "",
  " ",
  "  ",
  "   ",
  "    ",
  "     ",
  "      ",
  "       ",
};

String StringCompleteTo14(String base) {
  int nb = 14;
  int len = base.length();
  if (len >= nb)
    return base;
  else if (len >= 7)
    return base + spaces[nb-len];
  else
    return base + spaces[7] + spaces[nb-len-7];
}

void setup()
{
  Serial.begin(9600);

  screen.setup();
  screen.setContrast(60); // good values range from 40-60
  screen.clearDisplay(WHITE);

  Serial.println(6);
  for (int i = 0; i < nbRows; i++) {
    String line = StringCompleteTo14(String(spaces[i]) + "line " + String(i));
    // String line = String("line ") + String(i);
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






const int nbChoices0 = 6; // at most 6
String choices0[nbChoices0] = {
  "disp. current",
  "disp. curve",
  "disp. measure",
  "set x-scale",
  "set y-scale",
  "set log params" };

void showChoices(int nbChoices, String* choices, int selected) {
  for (int i = 0; i < nbChoices; i++) {
    screen.setString(choices[i], i, 0, (i==selected));
  }
}
    showChoices(nbChoices0, choices0, rot.getValue());
  rot.setModulo(modulo);
  rot.resetValue(value);
    modulo = nbChoices0;
    value = oldPanel;


String spaces[8] = {
  "",
  " ",
  "  ",
  "   ",
  "    ",
  "     ",
  "      ",
  "       ",
};

String StringCompleteTo14(String base) {
  int nb = 14;
  int len = base.length();
  if (len >= nb)
    return base;
  else if (len >= 7)
    return base + spaces[nb-len];
  else
    return base + spaces[7] + spaces[nb-len-7];
}
