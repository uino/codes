/**
 * AC_Nokia_5100 library for controlling a Nokia 5100 LCD display device.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Adapted from code by https://www.sparkfun.com/products/10168
 * Adapted from code by Jim Lindblom,
 * Adapted from code by Nathan Seidle and mish-mashed with ColorLCDShield code.
 *
 *
 */

/*
   Hardware: (Note most of these pins can be swapped)
    Graphic LCD Pin ---------- Arduino Pin
       1-VCC       ----------------  5V
       2-GND       ----------------  GND
       3-SCE       ----------------  7
       4-RST       ----------------  6
       5-D/C       ----------------  5
       6-DN(MOSI)  ----------------  11
       7-SCLK      ----------------  13
       8-LED       - 330 Ohm res --  9
   Most of these pins can be moved to any digital or analog pin.
   The SCLK, DN(MOSI), must remain where they are, but the other 
   pins can be swapped. The LED pin should remain a PWM-capable
   pin. Don't forget to stick a current-limiting resistor in line
   between the LCD's LED pin and Arduino pin 9! 

*/


#ifndef AC_Nokia5100_h
#define AC_Nokia5100_h

#include "AC_Nokia5100_common.h"

// Note: see AC_Nokia5100_IN_SRAM in file AC_Nokia5100_common

class AC_Nokia5100 : public AC_Nokia5100_common
{
public:

  AC_Nokia5100(byte scePin, byte rstPin, byte dcPin, byte sdinPin, byte sclkPin, byte blPin);

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
    * This function clears the entire display either white (0) or black (1). 
    * The screen won't actually clear until you call updateDisplay().
    */
  void clearDisplay(boolean st);

  /** 
    * Same as clearDisplay(WHITE);
    */
  void clearDisplay();

  /** 
    * Set the state of a given pixel; true=Black and false=white
    */
  void setPixel(int x, int y, boolean st);
  void setPixel(int x, int y);  // same with st=BLACK

  /** 
    * clearPixel(x, y) is the same as setPixel(x, y, WHITE)
    */
  void clearPixel(int x, int y);

  /** 
    * setLine draws a line from x0,y0 to x1,y1 with the set color.
    */
  void setLine(int x0, int y0, int x1, int y1, boolean st);

  /** 
    * setRect will draw a rectangle from x0,y0 top-left corner to
    * a x1,y1 bottom-right corner. Can be filled with the fill
    * parameter, and colored with bw.
    */
  void setRect(int x0, int y0, int x1, int y1, boolean fill, boolean st);

  /** 
    * setCircle draws a circle centered around x0,y0 with a defined
    * radius. The circle can be black or white. And have a line
    * thickness ranging from 1 to the radius of the circle.
    */
  void setCircle(int x0, int y0, int radius, boolean st, int thickness);

  /** 
    * This function will draw a char (defined in the ASCII table
    * near the beginning of this sketch) at a defined row and col).
    * The color can be either black (1) or white (0).
    */
  void setChar(char c, int row, int col); // same with st=BLACK
  void setChar(char c, int row, int col, boolean st);

  /** 
    * This function will draw a char (defined in the ASCII table
    * near the beginning of this sketch) at a defined x and y).
    * The color can be either black (1) or white (0).
    */
  void setCharXY(char c, int x, int y, boolean st);

  /** 
    * setString draws a string of characters, calling setChar with
    * progressive coordinates until it's done.
    * If at the end of a line, continue on the following one
    */
  void setString(char* str, int row, int col); // same with st=BLACK
  void setString(char* str, int row, int col, boolean st);
  void setString(String str, int row, int col); // same with st=BLACK
  void setString(String str, int row, int col, boolean st);

  /** 
    * setStringXY draws a string of characters, calling setChar with
    * progressive coordinates until it's done.
    * If at the end of a line, continue on the following one
    */
  void setStringXY(char* str, int x, int y, boolean st);
  void setStringXY(String str, int x, int y, boolean st);

  /** 
    * This function will draw an array over the screen. The
    * array must be the same size as the screen, covering the entirety
    * of the display.
    */
  void setBitmap(char* bitArray);


//---------------------------------------------------------------------

private:

  /* The displayMap variable stores a buffer representation of the
  pixels on our display. There are 504 total bits in this array,
  same as how many pixels there are on a 84 x 48 display.

  Each byte in this array covers a 8-pixel vertical block on the 
  display. Each successive byte covers the next 8-pixel column over
  until you reach the right-edge of the display and step down 8 rows.

  To update the display, we first have to write to this array, then
  call the updateDisplay() function, which sends this whole array
  to the PCD8544.

  Because the PCD8544 won't let us write individual pixels at a 
  time, this is how we can make targeted changes to the display. */
  byte displayMap[LCD_WIDTH * LCD_HEIGHT / 8];

};

#endif
