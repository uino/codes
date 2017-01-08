/**
 *
 * Demo for the AC_RAM library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * TODO: add update and add comments
 */

#include <AC_RAM.h>

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

  Serial.print("SRAM free: ");
  Serial.println(AC_RAM::getFree());
}

void loop()
{
  // do nothing
  delay(1000);
}


