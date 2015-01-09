/**
 * Same as AC_Nokia5100 library except that it only supports printing
 * of ACSII characters on the screen and consumes 5 times less memory.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Adapted from code by https://www.sparkfun.com/products/10168
 * Adapted from code by Jim Lindblom,
 * Adapted from code by Nathan Seidle and mish-mashed with ColorLCDShield code.
 *
 */

#ifndef AC_Nokia5100_light_h
#define AC_Nokia5100_light_h

#include "AC_Nokia5100_common.h"

// Note: see AC_Nokia5100_IN_SRAM in file AC_Nokia5100_common


class AC_Nokia5100_light : public AC_Nokia5100_common
{
public:

  AC_Nokia5100_light(byte scePin, byte rstPin, byte dcPin, byte sdinPin, byte sclkPin, byte blPin);

  // inherited from AC_Nokia5100_common:
  //   void begin();
  //   void setContrast(byte contrast);
  //   void invertDisplay();


  /** 
    * Must be called to propagate any change to the screen
    * (it writes the content of the array displayMap to the device)
    */
  void updateDisplay();

  /** 
    * Same as clearDisplay(WHITE);
    */
  void clearDisplay();

  /** 
    * This function will draw a char (defined in the ASCII table
    * near the beginning of this sketch) at a defined row and col).
    */
  void setChar(char c, int row, int col); 

  /** 
    * setString draws a string of characters, calling setChar with
    * progressive coordinates until it's done.
    * If at the end of a line, continue on the following one
    */
  void setString(char* str, int row, int col);
  void setString(String str, int row, int col); 

//---------------------------------------------------------------------

private:

  /* The charMap variable stores a buffer representation of the
  characters on our display. There are LCD_COLS x LCD_ROWS
  (84/6 * 48/8 = 14 * 6 = 84) characters on a display,
  each requiring one byte of storage.

  To update the display, we first have to write to this array, then
  call the updateDisplay() function, which sends this whole array
  to the PCD8544.

  We use charMap[col + LCD_COLS * row] to denote the char at pos (row,col).
  */

  byte charMap[LCD_COLS * LCD_ROWS];

};

#endif
