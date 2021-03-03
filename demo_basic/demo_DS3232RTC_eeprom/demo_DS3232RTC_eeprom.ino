/**
 * Demo code for reading and writing in the EEPROM of the DS3232
 * Valid address range is 0x00 - 0xFF, no checking done by library.
 *
 * Limitation: for array operations, the number of bytes must be
 * between 1 and 32 (Wire library limitation).
 *
 * Code by Arthur Chargueraud.
 * Copyleft code.
 */

#include <DS3232RTC.h>

DS3232RTC ds3232;

#define NB 5; // no more than 0xFF-1

byte addr0 = 0x00; // first address in EEPROM
byte addr1 = 0x01;

void printOne(byte b) {
  Serial.print(b);
  Serial.println();
}

void demoRead() {
  Serial.print("DS3232 read one byte: ");
  printOne(ds3232.readRTC(addr0));

  Serial.print("DS3232 read NB bytes: ");
  byte data[NB];
  ds3232.readRTC(addr1, data, NB);
  for (int i = 0; i < NB; i++) {
    printOne(data[i]);
  }
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

// puts to zero the NB+1 bytes
void demoReset() {
  ds3232.writeRTC(addr0, 0);
  byte data[NB];
  for (int i = 0; i < NB; i++) {
    data[i] = 0;
  }
  ds3232.writeRTC(addr1, data, NB);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up");

  Serial.print("DS3232 time: ");
  printTime(ds3232.get());

  demoReset();
  demoRead();
  demoWrite();
  demoRead();
}

void loop(void)
{
  delay(1000);
}
