/**
 * Navigator for SD cards connected to an arduino.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * Based on examples from http://arduino.cc/en/Reference/SD.
 *
 * SD card attached to SPI bus as follows:
 * - MOSI - pin 11
 * - MISO - pin 12
 * - CLK - pin 13
 * - CS - pin 8
 * Note that hardware CS pin (usually pin 10) must be left as output.
 * 
 * Recall to use DOS format for filnames (8 characters + 3 for extension).
 *
 */

#include <SD.h>

//*****************************************************************
/* Configuration */

// SD card pins
const int selectPin = 8;
const int hardwareCSPin = 10;

// Serial speed
const long serialBaudRate = 115200; // faster than using 9600


//*****************************************************************
/* Functions */

void readFileSerialPrint(char* filename) {
  if (SD.exists(filename)) {
    File file = SD.open(filename, FILE_READ);
    if (! file) {
      Serial.println("readFileSerialPrint: error opening file");
    } else {
      while (file.available()) {
        byte v = file.read();
        Serial.write(v);
      }
      file.close();
    }
  } else {
    Serial.println("readFileSerialPrint: file not found");
  }
}

void printDirectory(File dir, int numTabs) {
   // To limit the depth:
   // int maxDepth = 2;
   // if (numTabs > maxDepth)
   //  return;

   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print("   ");
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("      ");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void printHierarchy() {
  File root = SD.open("/");
  printDirectory(root, 0);
  root.close();
}


//*****************************************************************
/* Actions */

// write here actions to be performed
void actions() {
  if (false) {
    // print hierarchy of files
    printHierarchy();
  }

  if (true) {
    // read and print content one file
    char* filename = "logger.txt"; 
    Serial.println("=========begin contents=========");
    readFileSerialPrint(filename); 
    Serial.println("=========end contents=========");
  }
}

/* delete:

  SD.remove(filename);
  Serial.println("File deleted");
*/

/* write:

  File file = SD.open(filename, FILE_WRITE);
  if (! file) {
    Serial.println("Error opening file for write");
    return;
  }
  file.println(counter);
  file.close();
*/

// TODO: copy a local file into the SD card.


//*****************************************************************
/* Setup */

void setup()
{
  // Serial setup
  Serial.begin(serialBaudRate);

  // SD setup
  pinMode(hardwareCSPin, OUTPUT); // needed
  if (! SD.begin(selectPin)) {
    Serial.println("Card failed, or not present");
    while (1); // hang
  }

  // Actions
  actions();

  Serial.println("Finished...");
}

void loop()
{
  // do nothing
  delay(1000);
}


