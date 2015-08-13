

//----------------------------------------------------------------------------

/**
 * ADXL335 accelerometer
 *
 * Reports how much acceleration (in any direction) is measured:
 * on a linear scale, with a value between 30 and 16k (approximately)
 *
 * Wiring: 
 * - connect x-output to A0
 * - connect y-output to A1
 * - connect z-output to A2
 * - connect VCC to +3.3V or +5V 
 */
 
#include <Arduino.h>
#include <AC_Vector.h>

const int pinX = A0;
const int pinY = A1;
const int pinZ = A2;

// calibration.
const int minX = 270;
const int maxX = 408;
const int minY = 261;
const int maxY = 402;
const int minZ = 261;
const int maxZ = 397;

// float version of the Arduino "map" function
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Vector low, high;

void accel_setup() {
  low = Vector();
  high = Vector();
}

//----------------------------------------------------------------------------

/**
 * AC_TM1637 display.
 *
 * Wiring:
 * - CLK to pin 6
 * - DIO to pin 5
 * - VCC to alimentation (or to power control digital pin)
 * - GND to ground
 *
 */

#include <AC_TM1637.h>

const int pinCLK = 6;
const int pinDIO = 5;

AC_TM1637 display(pinCLK, pinDIO);

const int delayBetweenDemos = 3000;
const int periodBetweenIncrements = 5; // milliseconds

const boolean displayActive = true;

void display_setup() {
  display.begin();
}

//----------------------------------------------------------------------------

/**
 * Reading time from DS3232.
 */

#include <DS3232RTC.h> 

DS3232RTC ds3232;

//----------------------------------------------------------------------------

/**
 * SD card.
 *
 * Wiring
 * - MOSI - pin 11
 * - MISO - pin 12
 * - CLK - pin 13
 * - CS - pin 10
 * Note that hardware CS pin (usually pin 10) must be left as output.
 *   
 */

#include <SD.h>

// name of the file used by the demo (DOS format: 8 characters + 3 for extension)
char* filename = "night.txt"; 

const boolean sdActivate = true;

// pins for the SD card
const int selectPin = 10;
const int hardwareCSPin = 10;


void writeTimeToLog(File file, time_t t) {
  file.print(year(t)); 
  file.print('/');  
  file.print(month(t));
  file.print('/');
  file.print(day(t));
  file.print('\t');
  file.print(hour(t));
  file.print(':');
  file.print(minute(t));
  file.print(':');
  file.print(second(t));
}

void sd_report(time_t t, int value) {
  if (!sdActivate)
    return;
  // appends an integer at the end of the file
  File file = SD.open(filename, FILE_WRITE);
  if (! file) {
    Serial.println("Error opening file for write");
    return;
  }
  file.print(value);
  file.print("\t\t");
  writeTimeToLog(file, t);
  file.print("\n");
  file.close();
  // Serial.println("Append to file: " + String(value));
}

void sd_setup() {
  if (!sdActivate)
    return;
  pinMode(hardwareCSPin, OUTPUT); // needed
  // loop until card is detected , at most 6 seconds
  int count = 3;
  while (count > 0) {
    if (SD.begin(selectPin))
      break;
    Serial.println("Card failed, or not present");
    display.showInt(10000); // show "----"
    delay(2000);
    count--;
  }
  // SD.remove(filename);
  // Serial.println("File deleted");
}

//----------------------------------------------------------------------------
// Serial

void printTime(time_t t) {
  Serial.print(year(t)); 
  Serial.print('/');
  Serial.print(month(t));
  Serial.print('/');
  Serial.print(day(t));
  Serial.print(' ');
  Serial.print(hour(t));
  Serial.print(':');
  Serial.print(minute(t));
  Serial.print(':');
  Serial.print(second(t));
}

//----------------------------------------------------------------------------

/**
 * Serial
 */

const boolean serialActive = false;

void serial_setup() {
  Serial.begin(115200); 
}

void serial_report(time_t t, int value) {
  if (!serialActive) 
    return;
  printTime(t);
  Serial.print("\t");
  Serial.print(value);
  Serial.println("");
}

//---------------------------

const int delayBetweenReports = 250; // millis
long dateLastReport = 0;

//---------------------------

void setup() {
  serial_setup();
  display_setup();
  accel_setup();
  sd_setup();
  Serial.println("Starting up");
}

void loop(){

  int x = analogRead(pinX);
  int y = analogRead(pinY);
  int z = analogRead(pinZ);

  float xg = mapFloat(x, minX, maxX, 1., -1);
  float yg = mapFloat(y, minY, maxY, 1., -1);
  float zg = mapFloat(z, minZ, maxZ, 1., -1);

  low.x = min(low.x, xg);
  low.y = min(low.y, yg);
  low.z = min(low.z, zg);

  high.x = max(high.x, xg);
  high.y = max(high.y, yg);
  high.z = max(high.z, zg);

  long dateNow = millis();
  if (dateNow - dateLastReport > delayBetweenReports) {
    dateLastReport = dateNow;
    Vector move = high - low;  // each direction between +/- 3.2 roughly
    float strength = move.norm(); // between 0 and 16 roughtly


    time_t t = ds3232.get();
    int t_hour = hour(t);
    int t_minute = minute(t);
    int t_second = second(t);

    int value = (int) (move.norm() * 1000);

    if (value > 9999)
      value = 9999;
    
    if (displayActive)
      display.showInt(value);
    
    // display.showTime(t_minute, t_second);
    serial_report(t, value);
    sd_report(t, value);


    low = Vector(xg, yg, zg);
    high = Vector(xg, yg, zg);
  }

  delay(10);
}




