#include <unistd.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#define byte unsigned char

/**
 * Code for software-correction of the drift of a clock (e.g. the DS3232)
 * (The amount of correction could be read from two EEPROM fields)
 *
 * The correction consists of adding/subtracting 1 second every X seconds.
 * Where X is a long value.
 * This sign of X indicates increase or decrease the value.
 * The value of X can be stored internally on 4 bytes.
 * Use the special value X=0 for "no correction".
 *
 * Code by Arthur Chargueraud.
 * Copyleft code.
 *
*/


#define uint unsigned int

long correction = -4; // subtract one second every 4 seconds, for demo

/*************************************************/

long correctionFromBytes(byte* data) { // data array of 4 bytes
  return (long) ((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
}

void correctionToBytes(long n, byte* data) { // data array of 4 bytes
  data[0] = (byte) n;
  data[1] = (byte) (n >> 8);
  data[2] = (byte) (n >> 16);
  data[3] = (byte) (n >> 24);
  // printf("%d %d %d %d\n", (int) data[0], (int) data[1], (int) data[2], (int) data[3]);
}

void checkConversion(long x) {
  byte data[4];
  correctionToBytes(x, data);
  long y = correctionFromBytes(data);
  /*
  Serial.print("Conversion: ");
  Serial.print(x);
  Serial.print(" --> ");
  Serial.println(y);
  printf("%ld --> %ld\n", x, y);
  */
}

void checkConversions() {
  checkConversion(0);
  checkConversion(1);
  checkConversion(-1);
  checkConversion(800);
  checkConversion(-800);
  checkConversion(18000000);
  checkConversion(-18000000);
}


/*************************************************/
/* Storing a correction on 3 bytes. */

// use Arduino clock for the demo
int getSecondsSinceReference() {
  // return millis() / 1000;

  struct timeval time;
  double secs = 0;
  gettimeofday(&time, NULL);
  return (int) (time.tv_sec);
}

int nextTimeToCorrect;

long abs(long v) {
  return (v >= 0) ? v : -v;
}

int main() {
  // checkConversions();

  nextTimeToCorrect = getSecondsSinceReference() + abs(correction);

  while (1) {
    usleep(200000);

    int now = getSecondsSinceReference();
    printf("ok %d\n", now);

    if (correction != 0 && now >= nextTimeToCorrect) {
      int delta = (correction > 0) ? 1 : -1;
      printf("change time by %d second\n", delta);
      nextTimeToCorrect = now + abs(correction);
    }
  }
}


/*
void setup() {

}

void loop() {
  delay(1000);
}



*/

/*************************************************/

/* DEPRECATED NOT TESTED Storing a correction on 3 bytes.

long correctionFromBytes(byte* data) { // data array of 3 bytes
  byte v = data[0];
  long sign = 1;
  if (v >= 128) {
    sign = -1;
    v -= 256;
  }
  return sign * (v * 256 * 256 + data[1] * 256 + data[2]);
}

void correctionToBytes(long x, byte* data) { // data array of 3 bytes
  int v = x / (256 * 256);
  if (x < 0) {
    x = - x;
    v += 256; // here v should go from [-128,-1] to [128,255]
  }
  data[0] = v;
  data[1] = (x / 256) % 256;
  data[2] = x % 256;
}

*/
