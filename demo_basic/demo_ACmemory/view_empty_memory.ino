/**
 * Demo for the ACmemory library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * TODO: add comments
 */

// #define COUNT_SCREEN  // (530 bytes!)

#include <ACmemory.h>


#ifdef COUNT_SCREEN
#include <ACnokia5100.h>
ACnokia5100 screen(3, 4, 5, 11, 13, 7);
#endif


void report() {
  Serial.print("SRAM free: ");
  Serial.println(getFreeSRAM());
}

void setup()
{
  Serial.begin(9600);   
  report();

#ifdef COUNT_SCREEN
  screen.setup();
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

