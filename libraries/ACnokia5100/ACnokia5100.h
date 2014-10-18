/**
   Nokia 5100 LCD Library

   Adapted by Arthur Charguéraud from code by!

   ---
     Graphics driver and PCD8544 interface code for SparkFun's
     84x48 Graphic LCD.
     https://www.sparkfun.com/products/10168
   
      by: Jim Lindblom
        adapted from code by Nathan Seidle and mish-mashed with
        code from the ColorLCDShield.
      date: October 10, 2013
      license: Beerware. Feel free to use, reuse, and modify this
      code as you see fit. If you find it useful, and we meet someday,
      you can buy me a beer.
    ---

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


#ifndef ACnokia5100_h
#define ACnokia5100_h

#include <Arduino.h>
#include <SPI.h>


class ACnokia5100
{
  public:

    static const int LCD_WIDTH = 84; // Note: x-coordinates go wide
    static const int LCD_HEIGHT = 48; // Note: y-coordinates go high
    static const int LCD_COLS = LCD_WIDTH/6;
    static const int LCD_ROWS = LCD_HEIGHT/8; 

    static const int WHITE = 0;  // For drawing pixels. A 0 draws white.
    static const int BLACK = 1;  // A 1 draws black.

    ACnokia5100(int scePin, int rstPin, int dcPin, int sdinPin, int sclkPin, int blPin);

    /** 
      * Must be called before using the screen
      */
    void setup();

    /** 
      * Must be called to propagate any change to the screen
      * (it writes the content of the array displayMap to the device)
      */
    void updateDisplay();

    /** 
      * Set contrast can set the LCD Vop to a value between 0 and 127.
      * 40-60 is usually a pretty good range.
      */
    void setContrast(byte contrast);

    /** 
      * Inverts the display by swapping all bits in the displayMap.
      */
    void invertDisplay();

    /** 
      * This function clears the entire display either white (0) or black (1). 
      * The screen won't actually clear until you call updateDisplay().
      */
    void clearDisplay(boolean st);

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



  private:

    int scePin;
    int rstPin;
    int dcPin;
    int sdinPin;
    int sclkPin;
    int blPin;
     
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

    /* Font table:
    This table contains the hex values that represent pixels for a
    font that is 5 pixels wide and 8 pixels high. Each byte in a row
    represents one, 8-pixel, vertical column of a character. 5 bytes
    per character. */
    static const byte ASCII[][5];
  
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
