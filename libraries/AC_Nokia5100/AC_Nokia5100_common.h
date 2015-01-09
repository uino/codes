/**
 * Common definitions for the AC_Nokia5100 and AC_Nokia5100_light library.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Adapted from code by https://www.sparkfun.com/products/10168
 * Adapted from code by Jim Lindblom,
 * Adapted from code by Nathan Seidle and mish-mashed with ColorLCDShield code.
 *
 */

#ifndef AC_Nokia5100_common_h
#define AC_Nokia5100_common_h

#include <Arduino.h>
#include <SPI.h>

// Place the following line uncommented in your code, before the include of AC_Nokia5100.h
// to allocate the character table in Flash instead of SRAM
#define AC_Nokia5100_IN_SRAM

#ifdef AC_Nokia5100_IN_SRAM
#include <avr/pgmspace.h>
typedef const byte PROGMEM prog_byte;
#endif

class AC_Nokia5100_common
{
public:

  static const int LCD_WIDTH = 84; // Note: x-coordinates go wide
  static const int LCD_HEIGHT = 48; // Note: y-coordinates go high
  static const int LCD_COLS = LCD_WIDTH/6;
  static const int LCD_ROWS = LCD_HEIGHT/8; 

  static const int WHITE = 0;  // For drawing pixels. A 0 draws white.
  static const int BLACK = 1;  // A 1 draws black.

  AC_Nokia5100_common(byte scePin, byte rstPin, byte dcPin, byte sdinPin, byte sclkPin, byte blPin);

  /** 
    * Must be called before using the screen
    */
  void begin();

  /** 
    * Set contrast can set the LCD Vop to a value between 0 and 127.
    * 40-60 is usually a pretty good range.
    */
  void setContrast(byte contrast);

  /** 
    * Inverts the display by swapping all bits in the displayMap.
    */
  void invertDisplay();

//---------------------------------------------------------------------

protected:

  byte scePin;
  byte rstPin;
  byte dcPin;
  byte sdinPin;
  byte sclkPin;
  byte blPin;
   
  /* PCD8544-specific definitions */
  static const int LCD_COMMAND = 0;
  static const int LCD_DATA = 1;

  /**
   * Jump directlyto a specific x,y coordinate.
   */
  void gotoXY(int x, int y);

  /**
   * Write a command to the screen
   */
  void writeCmd(byte data_or_command, byte data);

  /**
   * Get the bit pattern of a given column (indexed from 0 to 4) 
   * of a given ASCII character.
   */
  byte getASCII(char k, int column);

  /* Font table:
  This table contains the hex values that represent pixels for a
  font that is 5 pixels wide and 8 pixels high. Each byte in a row
  represents one, 8-pixel, vertical column of a character. 5 bytes
  per character. */

#ifndef AC_Nokia5100_IN_SRAM
  static const byte ASCII[];
#else
  PROGMEM static const prog_byte ASCII[];
#endif

};

#endif
