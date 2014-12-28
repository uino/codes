
void setup() {
}
void loop() {
}
/*
// TODO

#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

File file;  // test file
const uint8_t SD_CS = 10; // SD chip select
RTC_DS1307 RTC;  // define the Real Time Clock object
//------------------------------------------------------------------------------
// call back for file timestamps
void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = RTC.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!RTC.begin()) {
    Serial.println("RTC failed");
    while(1);
  };
  // set date time callback function
  SdFile::dateTimeCallback(dateTime); 

  if (!SD.begin(SD_CS)) {
    Serial.println("SD.begin failed");
    while(1);
  }
  file = SD.open("TEST_SD.TXT", FILE_WRITE);
  file.close();
  Serial.println("Done");
}
//------------------------------------------------------------------------------
void loop() {}


















bool SdBaseFile::timestamp    (    uint8_t     flags,
      uint16_t     year,
      uint8_t     month,
      uint8_t     day,
      uint8_t     hour,
      uint8_t     minute,
      uint8_t     second 
   )       

Set a file's timestamps in its directory entry.

Parameters:
    [in]   flags   Values for flags are constructed by a bitwise-inclusive OR of flags from the following list

T_ACCESS - Set the file's last access date.

T_CREATE - Set the file's creation date and time.

T_WRITE - Set the file's last write/modification date and time.

Parameters:
    [in]   year   Valid range 1980 - 2107 inclusive.
    [in]   month   Valid range 1 - 12 inclusive.
    [in]   day   Valid range 1 - 31 inclusive.
    [in]   hour   Valid range 0 - 23 inclusive.
    [in]   minute   Valid range 0 - 59 inclusive.
    [in]   second   Valid range 0 - 59 inclusive

Note:
    It is possible to set an invalid date since there is no check for the number of days in a month.
    Modify and access timestamps may be overwritten if a date time callback function has been set by dateTimeCallback().

Returns:
    The value one, true, is returned for success and the value zero, false, is returned for failure. 

Or copy timestamps from another file:
Quote
bool SdBaseFile::timestamp    (    SdBaseFile *     file   )    

Copy a file's timestamps

Parameters:
    [in]   file   File to copy timestamps from.

Note:
    Modify and access timestamps may be overwritten if a date time callback function has been set by dateTimeCallback().

Returns:
    The value one, true, is returned for success and the value zero, false, is returned for failure. 


*/