/**
 * ACmemory library.
 * Code by Arthur Chargueraud
 * Copyleft code.
 *
 * This library contains functions that can be used to interact
 * with the various memories of the arduino: flash, sram, eeprom.
 *
 * [under construction]
 */
 

#ifndef ACmemory_h
#define ACmemory_h

int getFreeSRAM() {
  // code adpated from:
  // https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

#endif
