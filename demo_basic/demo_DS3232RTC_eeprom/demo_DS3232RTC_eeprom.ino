// WORK IN PROGRESS

/**
 * Demo code for reading and writing in the EEPROM of the DS3232
 *
 * Code by Arthur Chargueraud.
 * Copyleft code.
 */

/**
 * Demo for the AC_DS3232 library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */

#include <AC_DS3232.h>

AC_DS3232 ds3232;

#defin NB 5

void printTime(time_t t) {
  Serial.print("Date: ");
  Serial.print(year(t));
  Serial.print('/');
  Serial.print(day(t));
  Serial.print('/');
  Serial.print(month(t));
  Serial.print(' ');
  Serial.print(hour(t));
  Serial.print(':');
  Serial.print(minute(t));
  Serial.print(':');
  Serial.print(second(t));
  Serial.println();
}

void printOne(byte b) {
  Serial.print(b);
  Serial.println();
}

void demoRead() {
  Serial.print("DS3232 read NB bytes: ");
  for (int i = 0; i < NB; i++) {
    byte addr = i;
    printOne(ds3232.EERead(addr));
  }
}

void demoWrite(byte value) {
  Serial.print("DS3232 write NB bytes: ");
  for (int i = 0; i < NB; i++) {
    byte addr = i;
    ds3232.EERead(addr, value);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up");

  printTime(ds3232.get());

  demoWrite(0);
  demoRead();
  demoWrite(9);
  demoRead();
}

void loop() {

  delay(1000);
}









// write: 9 10 11 12 13 14 ... 10+NB-1 in memory
void demoWrite() {
  Serial.print("DS3232 write one byte.");
  ds3232.writeRTC(addr0, 9);

  Serial.print("DS3232 write NB bytes.");
  byte data[NB];
  for (int i = 0; i < NB; i++) {
    data[i] = 10+i;
  }
  ds3232.writeRTC(addr1, data, NB);
}
