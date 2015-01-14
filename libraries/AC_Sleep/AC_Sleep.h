/**
 * AC_Sleep library for controlling sleeping modes.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 * 
 * Code adapted from: http://www.gammon.com.au/forum/?id=11497 (sketch J).
 *
 */
 
#ifndef AC_SLEEP_h
#define AC_SLEEP_h

#include <avr/sleep.h>

class AC_Sleep {
public:

  /*
   * Call this function to enter deep sleeping mode. 
   * Wake up is only possible through interrupts (or reset).
   */
  static void enterSleepOnInterrupt(byte idInterrupt, void (*wake)(void), byte triggerState) {  
    // prevent interrupts during the critical section that follows
    noInterrupts();
    attachInterrupt(idInterrupt, wake, triggerState);  
    // disable analogic to digital conversion
    ADCSRA = 0;  
    // enable sleep mode
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
    sleep_enable();
    // Turn off brown-out enable in software
    // BODS must be set to one and BODSE must be set to zero within four clock cycles
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS); 
    interrupts(); 
    // no interrupts can happen between the instruction above and the one below
    sleep_cpu();
  }

  /*
   * Call this function immediately after exiting sleep mode
   * (e.g. on the first line of the interrupt handler)
   * to prevent going back to sleep just afterwards.
   */
  static void terminateSleep() {  
    // disable the sleeping flag
    sleep_disable();
  }

};


#endif
