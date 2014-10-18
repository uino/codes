/**
 * Demo for the SD card data logging.
 * Code by Arthur Charguéraud.
 * This code is in the public domain.
 *  
 *
 * SD card attached to SPI bus as follows:
 * - MOSI - pin 11
 * - MISO - pin 12
 * - CLK - pin 13
 * - CS - pin 4 
 * Note that hardware CS pin (usually pin 10) must be left as output.
 *   
 */

#include <SD.h>

const int selectPin = 8;
const int hardwareCSPin = 10;

void setup()
{
  Serial.begin(9600);

  pinMode(hardwareCSPin, OUTPUT); // needed
  
  if (! SD.begin(selectPin)) {
    Serial.println("Card failed, or not present");
    while (1); // hang
  }

  Serial.println("Starting up");
}

int counter = 0;
char* filename = "demo_sd_logging.txt";

void loop()
{
  File dataFile = SD.open(filename, FILE_WRITE);
  if (! dataFile) {
    Serial.println("Error opening file");
    delay(1000);
    return;
  }
  dataFile.println(counter);
  dataFile.close();
  Serial.println("Wrote " + String(counter));

  delay(1000);
}