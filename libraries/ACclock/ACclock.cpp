/**
 * ACclock Library. See "ACclock.h" for information.
 */

#include "ACclock.h"

ACclock::ACclock()
{
}

void ACclock::setup() {
  Wire.begin();
}

// Convert normal decimal numbers to binary coded decimal
byte ACclock::decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte ACclock::bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void ACclock::setDate(
    byte second,        // 0-59
    byte minute,        // 0-59
    byte hour,          // 1-23
    byte dayOfWeek,     // 1-7
    byte dayOfMonth,    // 1-28/29/30/31
    byte month,         // 1-12
    byte year)          // 0-99
{
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.write(0);
   Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
   Wire.write(decToBcd(minute));
   Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                   // bit 6 (also need to change readDateDs1307)
   Wire.write(decToBcd(dayOfWeek));
   Wire.write(decToBcd(dayOfMonth));
   Wire.write(decToBcd(month));
   Wire.write(decToBcd(year));
   Wire.endTransmission();
}

void ACclock::setDate(ACclock_str date) {
  setDate(date.second,
    date.minute,
    date.hour,
    date.dayOfWeek,
    date.dayOfMonth,
    date.month,
    date.year);
}

void ACclock::getDate(
    byte *second,
    byte *minute,
    byte *hour,
    byte *dayOfWeek,
    byte *dayOfMonth,
    byte *month,
    byte *year)
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

  // A few of these need masks because certain bits are control bits
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

void ACclock::getDate(ACclock_str* date) {
  getDate(&(date->second),
    &(date->minute),
    &(date->hour),
    &(date->dayOfWeek),
    &(date->dayOfMonth),
    &(date->month),
    &(date->year));
}

