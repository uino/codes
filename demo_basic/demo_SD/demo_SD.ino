#if 1
/*
  SD card test 
   
 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.
 	
 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module. 
 		Pin 4 used here for consistency with other Arduino examples

 
 created  28 Mar 2011
 by Limor Fried 
 modified 9 Apr 2012
 by Tom Igoe
 */
 // include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 10;    

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(10, OUTPUT);     // change this to 53 on a mega


  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   Serial.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}


void loop(void) {
  
}




#else


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
const int selectPin = 10;
const int hardwareCSPin = 10;


void setup()
{
  Serial.begin(9600);

  pinMode(hardwareCSPin, OUTPUT); // needed

  // loop until card is detected
  while (true) {
    if (SD.begin(selectPin))
      break;
    Serial.println("Card failed, or not present");
    delay(2000);
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


#endif