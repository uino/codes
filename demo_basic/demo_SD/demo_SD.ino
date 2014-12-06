/**
 * Demo for the SD card data logging.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *  
 *
 * SD card attached to SPI bus as follows:
 * - MOSI - pin 11
 * - MISO - pin 12
 * - CLK - pin 13
 * - CS - pin 8
 * Note that hardware CS pin (usually pin 10) must be left as output.
 *   
 * The demo program performs the following actions every two seconds:
 * - prints the content of a file on the serial port,
 * - appends to the end of the file an integer equal to the line number.
 *
 */

#include <SD.h>

// name of the file used by the demo (DOS format: 8 characters + 3 for extension)
char* filename = "demo_sd.txt"; 

// pins for the SD card
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
  SD.remove(filename);
  Serial.println("File deleted");
}

int counter = 1;

void readFileSerialPrint(char* filename) {
  if (SD.exists(filename)) {
    File file = SD.open(filename, FILE_READ);
    if (! file) {
      Serial.println("readFileSerialPrint: error reading file");
    } else {
      Serial.println("=========begin contents=========");
      while (file.available()) {
        byte v = file.read();
        Serial.write(v);
      }
      Serial.println("=========end contents=========");
      file.close();
    }
  } else {
    Serial.println("readFileSerialPrint: file not found");
  }
}

void loop()
{
  delay(3000);

  // read and print content
  readFileSerialPrint(filename);  

  // appends an integer at the end of the file
  File file = SD.open(filename, FILE_WRITE);
  if (! file) {
    Serial.println("Error opening file for write");
    return;
  }
  file.println(counter);
  file.close();
  Serial.println("Append to file: " + String(counter));

  // increment counter for next time
  counter++;
}

// Remark: seeking to the end of the file is automatically performed by "open".
// So there is no need to call: file.seek(file.size()).
