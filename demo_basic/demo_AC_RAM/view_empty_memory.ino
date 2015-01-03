/**
 * Demo for the AC_RAM library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * TODO: add comments
 *
 * Tested on Uno
 * Code that only blinks: 1835 (+/- 4) bytes free
 * Code that only uses serial: 1829 bytes free
 * Code that only uses serial and includes Arduino.h: 1793 bytes free.
 * Nokia5100 Screen: consumes 530 bytes (84*48/8 = 504 bytes; optimize: 14*6=94 bytes)
 * RotatingPot: consumes 54
 * SHT1x: consumes 52
 * DS3232: consumes 252
 *  
 */

#define SERIAL_REPORT 
// #define DEL_REPORT 
#define COUNT_ARDUINO

// #define COUNT_SHT1x
//#define COUNT_SCREEN  
//#define COUNT_ROTATING_POT
#define COUNT_DS3232

#include <AC_RAM.h>

const int baseline = 1793; // free with only count-arduino and serial-report

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
  #include <AC_Nokia5100.h>
  AC_Nokia5100 screen(3, 4, 5, 11, 13, 7);
#endif

#ifdef COUNT_ROTATING_POT
  #include <AC_RotatingPot.h>
  byte rotPin = A0;
  const int rotSensitivity = 20; 
  const boolean rotInverted = true; 
  AC_RotatingPot rot(rotPin, rotSensitivity, rotInverted);
#endif


void setup()
{
/* raw values
  extern int __heap_start, *__brkval; 
  int v;
  int w;
  Serial.println((int) &v);   
  Serial.println((int) &w);   
  Serial.println((int) &__heap_start);   
*/

#ifdef SERIAL_REPORT
  Serial.begin(9600);   
  Serial.print("SRAM free setup begin: ");
  Serial.println(AC_RAM::getFree());
#endif

#ifdef COUNT_SCREEN
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay(screen.WHITE);
  screen.setString("X", 0, 0);
  screen.updateDisplay();
  Serial.print("Screen consumes: ");
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef DEL_REPORT
  pinMode(pinReport, OUTPUT);
  int nb = (AC_RAM::getFree() - 1800) / 3; // 1835
  for (int i = 0; i < nb; i++) {
    digitalWrite(pinReport, HIGH);
    delay(200);
    digitalWrite(pinReport, LOW);
    delay(200);
  }
#endif

#ifdef COUNT_ROTATING_POT
  rot.begin();
  Serial.print("Rot value: ");
  Serial.println(rot.getValue());
  Serial.print("RotatingPot consumes: ");
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef COUNT_SHT1x
  Serial.print("SHT1x value: ");
  Serial.println(sht1x.readTemperatureC(), 2);
  Serial.print("SHT1x consumes: ");
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef COUNT_DS3232
  Serial.print("DS3232 value: ");
  Serial.println(ds3232.get());
  Serial.print("DS3232 consumes: ");
  Serial.println(baseline - AC_RAM::getFree());
#endif

#ifdef SERIAL_REPORT
  Serial.print("SRAM free setup end: ");
  Serial.println(AC_RAM::getFree());
#endif
}

void loop()
{
  // do nothing
  delay(1000);
}


