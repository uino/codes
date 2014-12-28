/**
 * Demo for the AC_RAM library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * TODO: add comments
 */

// #define COUNT_SCREEN  // (530 bytes!)

#include <AC_RAM.h>


#ifdef COUNT_SCREEN
#include <AC_Nokia5100.h>
AC_Nokia5100 screen(3, 4, 5, 11, 13, 7);
#endif


void report() {
  Serial.print("SRAM free: ");
  Serial.println(AC_RAM::getFree());
}

void setup()
{
  Serial.begin(9600);   
  report();

#ifdef COUNT_SCREEN
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay(screen.WHITE);
  screen.setString("Loading...", 0, 0);
  screen.updateDisplay(); 
#endif

}

void loop()
{
  // do nothing
  delay(1000);
  report();
}

