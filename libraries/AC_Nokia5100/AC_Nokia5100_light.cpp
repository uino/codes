
/**
 * AC_Nokia5100 Library -- see "AC_Nokia5100.h" for information.
 *
 */ 

#include "AC_Nokia5100_light.h"

AC_Nokia5100_light::AC_Nokia5100_light(byte scePin, byte rstPin, byte dcPin, byte sdinPin, byte sclkPin, byte blPin) 
  : AC_Nokia5100_common::AC_Nokia5100_common(scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
{ }

void AC_Nokia5100_light::setChar(char character, int row, int col)
{
  // First, double check that the coordinate is in range.
  if ((row >= 0) && (row < LCD_ROWS) && (col >= 0) && (col < LCD_COLS)) {
    charMap[col + LCD_COLS * row] = character;
  }
}

void AC_Nokia5100_light::setString(char* dString, int row, int col)
{
  while (*dString != 0x00) // loop until null terminator
  {
    setChar(*dString, row, col);
    dString++;
    col++;
    if (col >= LCD_COLS) { // Enables wrap around
      col = 0;
      row++;
    }
  }
}

// adapated from above // TODO: factorize
void AC_Nokia5100_light::setString(String str, int row, int col)
{
  int nb = str.length();
  for (int k = 0; k < nb; k++) {
    char c = str[k];
    setChar(c, row, col);
    col++;
    if (col >= LCD_COLS) { // Enables wrap around
      col = 0;
      row++;
    }
  }
}

void AC_Nokia5100_light::clearDisplay()
{
  for (int i = 0; i < (LCD_COLS * LCD_ROWS); i++) {
    charMap[i] = ' '; // space
  }
}

void AC_Nokia5100_light::updateDisplay()
{
  gotoXY(0, 0);
  for (int row = 0; row < LCD_ROWS; row++) {
    for (int col = 0; col < LCD_COLS; col++) {
      for (int i = 0; i < 5; i++) { // 5 columns (x) per character
        char c = charMap[col + LCD_COLS * row];
        byte bits = getASCII(c, i);
        writeCmd(LCD_DATA, bits);
      }
      // plus one column of spacing
      writeCmd(LCD_DATA, 0x00);
    }
  }
}

