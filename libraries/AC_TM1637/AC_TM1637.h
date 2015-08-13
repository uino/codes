/**
 * AC_TM1637 library for controlling the TM1637 display device.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Inspired from the TM1637Display library (LGPL) by avishorp@gmail.com.
 *
 * Name of the segments:
 *
 *   --A--
 *   F   B
 *   --G--
 *   E   C
 *   --D-- (DP)       (DP=decimal point)
 *
 * Note that the device comes in two versions: one with a double dot in
 * the middle for displaying a clock, and one with decimal points after
 * every digit. The showFloat function only works with the latter device.
 *
 */

#ifndef AC_TM1637_h
#define AC_TM1637_h

#include <Arduino.h>

// #include <inttypes.h>

class AC_TM1637 {

public:

  /** 
    * Construct a new TM1637 device given its CLK and DIO pins;
    * Optionally provide the brightness (see setBrightness).
    */
  // AC_TM1637(byte pinCLK, byte pinDIO);
  AC_TM1637(byte pinCLK, byte pinDIO, byte brightness = 8);

  /** 
    * Initialize the device
    */
  void begin();

  /** 
    * Set the brightness of the device. Too low values may result
    * in no display at all. 
    */
  void setBrightness(byte brightness);

  /** 
    * Display the segments for a specified number of consecutive digits,
    * starting at a given position. The display of each digit is specified
    * by a byte, whose bits correspond to the segments.
    */
  void setSegments(int pos, int length, const byte segments[]);

  /** 
    * Display all four segments.
    */  
  void setSegments(const byte segments[]);
 
  /** 
    * Display a segment at a given position.
    */
  void setSegmentAt(int pos, byte segment);

  /** 
    * Display a digit (value between 0 and 9) at a given position.
    * Optionally specify to show the decimal point to the right of this digit.
    */
  void setDigitAt(int pos, int digit, boolean decimalPoint = false);

  /** 
    * Display 4 digits (value between 0 and 9).  
    */
  void setDigits(int digit1, int digit2, int digit3, int digit4);

  /** 
    * Display a int between -999 and +9999.
    * The second argument can be set to force leading zeros.
    */
  void showInt(int value, boolean leadingZero = false);

  /** 
    * Display a float, for a specific precision (number of digits
    * after the decimal point). For example, with precision 2, the range 
    * of values that can be displayed is (-9.99, +99.99).
    * Precision must be in the range [0,3].
    */
  void showFloat(float value, int precision, boolean leadingZero = false);

  /** 
    * Display a time, in the format "10:25".
    * Leading zero is only applied to the minutes.
    */
  void showTime(int hours, int minutes);

  /** 
    * Display "----" on the screen, to indicate invalid display request.
    *
    */
  void showInvalid();

  /** 
    * Given a digit value 0-15,
    * return the segments associated with it.
    * Values in the range 10-15 are displayed as letters.
    */
  byte segmentOfDigit(int digit);

/*
  // TODO: deprecated...
  //! Displayes a decimal number
  //!
  //! Dispalyes the given argument as a decimal number
  //!
  //! @param num The number to be shown
  //! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
  //!        blank
  //! @param length The number of digits to set. The user must ensure that the number to be shown
  //!        fits to the number of digits requested (for example, if two digits are to be displayed,
  //!        the number must be between 0 to 99)
  //! @param pos The position least significant digit (0 - leftmost, 3 - rightmost)
  //! @param decimal_dot_place - show decimal dot on selected segment
  void showNumberDecDot(int num, bool leading_zero = false, byte length = 4, byte pos = 0, int decimal_dot_place = 5);
*/
  /** 
    * Return the bit masks associated with each of the segments.
    * Add these bitmask using the "|" symbol to compose values
    * that can bet provided to the setSegments function.
    */
  static const int SEG_A = 0b00000001;
  static const int SEG_B = 0b00000010;
  static const int SEG_C = 0b00000100;
  static const int SEG_D = 0b00001000;
  static const int SEG_E = 0b00010000;
  static const int SEG_F = 0b00100000;
  static const int SEG_G = 0b01000000;
  static const int POINT = 0b10000000;


//---------------------------------------------------------------------
private:
	byte pinCLK;
  byte pinDIO;
  byte brightness;

  void smallDelay();
  void start();
  void stop();
  bool writeByte(byte b);
 
  void segmentsForInt(int value, boolean leadingZero, byte* segments);
  void segmentsForInvalid(byte* segments);

  static const int COMM1 = 0x40;
  static const int COMM2 = 0xC0;
  static const int COMM3 = 0x80;

  static const byte segmentOfDigitData[];

};

#endif // __TM1637DISPLAY__
