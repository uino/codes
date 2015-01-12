/**
 * Demo for reporting the SRAM consumption of various devices.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * Uncomment a #define line in the first section below
 * in order to measure the associated memory consumption.
 * 
 * By default, the serial port is used to report information. However,
 * in order to measure the memory without taking into account
 * the overhead of Serial connection, there is a mode for using
 * the builtin LED for reporting the amount of free memory.
 * 
 * Measures obtained on an Arduino Uno:
 * Code that only uses serial and Arduino.h: 1839 bytes free (serves as baseline).
 * Adding "const int" defitions: consume 0 bytes.
 * Nokia5110 Screen: consumes 512 bytes (large but expected: 84x48 bits = 504 bytes).
 * Nokia5110_light Screen: consumes 92 byes.
 * RotatingPot: consumes 18.
 * SHT1x: consumes 20.
 * DS3232: consumes 221. (why so much?)
 *  
 */


//*****************************************************************
// Uncomment one of these lines to measure the memory consumption

//#define COUNT_ARDUINO
//#define COUNT_CONSTINT
//#define COUNT_SHT1x
//#define COUNT_SCREEN_LIGHT
//#define COUNT_SCREEN  
//#define COUNT_ROTATING_POT
//#define COUNT_DS3232

//*****************************************************************

#include <AC_RAM.h>

// Define the amount of free memory on a minimal program.
// This value may need to be adjusted on different board.
// To obtain the value, run this program with all #define
// from the section above commented out.
const int baseline = 1839; 


//*****************************************************************
// Select how the amount of free memory should be reported.
// Serial is the default.

#define SERIAL_REPORT 
//#define DEL_REPORT 


//*****************************************************************

#ifdef COUNT_CONSTINT
  const int x = 0;
  const int y = 1;
  const int z = 2;
#endif

#ifdef COUNT_ARDUINO
  #include <Arduino.h>
#endif

#ifdef DEL_REPORT
  const int pinReport = 13;
#endif

#ifdef COUNT_DS3232
  #include <DS3232RTC.h>
  DS3232RTC ds3232;
#endif


#ifdef COUNT_SHT1x
  #include <SHT1x.h>
  SHT1x sht1x(9, 10);
#endif


#ifdef COUNT_SCREEN
  #include <AC_Nokia5110.h>
  AC_Nokia5110 screen(3, 4, 5, 11, 13, 7);
#endif

#ifdef COUNT_SCREEN_LIGHT
  #include <AC_Nokia5110_light.h>
  AC_Nokia5110_light screen(3, 4, 5, 11, 13, 7);
#endif

#ifdef COUNT_ROTATING_POT
  #include <AC_RotatingPot.h>
  byte rotPin = A0;
  const int rotSensitivity = 20; 
  const boolean rotInverted = true; 
  AC_RotatingPot rot(rotPin, rotSensitivity, rotInverted);
#endif

//*****************************************************************

void setup()
{
#ifdef SERIAL_REPORT
  Serial.begin(9600);   
  Serial.print(F("SRAM free: "));
  Serial.println(AC_RAM::getFree());
#endif

#if defined(COUNT_SCREEN) || defined(COUNT_SCREEN_LIGHT)
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay();
  screen.setString("X", 0, 0);
  screen.updateDisplay();
  Serial.print(F("Screen consumes: "));
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef DEL_REPORT
  pinMode(pinReport, OUTPUT);
  int nb = (AC_RAM::getFree() - 1800) / 3; // 1839
  for (int i = 0; i < nb; i++) {
    digitalWrite(pinReport, HIGH);
    delay(200);
    digitalWrite(pinReport, LOW);
    delay(200);
  }
#endif

#ifdef COUNT_CONSTINT
  Serial.println(x);
  Serial.println(y);
  Serial.println(z);
  Serial.print(F("ConstInt consumes: "));
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef COUNT_ROTATING_POT
  rot.begin();
  Serial.print(F("Rot value: "));
  Serial.println(rot.getValue());
  Serial.print(F("RotatingPot consumes: "));
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef COUNT_SHT1x
  Serial.print(F("SHT1x value: "));
  Serial.println(sht1x.readTemperatureC(), 2);
  Serial.print(F("SHT1x consumes: "));
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef COUNT_DS3232
  Serial.print(F("DS3232 consumes: "));
  Serial.println(baseline - AC_RAM::getFree());
  Serial.print(F("DS3232 value: "));
  Serial.println(ds3232.get());
#endif


  /* Code for reporting raw values used by AC_RAM::getFree()
  extern int __heap_start, *__brkval; 
  int v;
  int w;
  Serial.println((int) &v);   
  Serial.println((int) &w);   
  Serial.println((int) &__heap_start);   
  */
}

//*****************************************************************

void loop()
{
  // do nothing
  delay(1000);
}


