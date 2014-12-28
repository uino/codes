/**
 * AC_RAM library for getting information on the SRAM available.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Code adapted from:
 * https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
 *
 */
 
#ifndef AC_RAM_h
#define AC_RAM_h

class AC_RAM {
public:
  // return free SRAM
  static int getFree() {
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }
};

#endif
