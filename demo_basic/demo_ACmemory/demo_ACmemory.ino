/**
 * Demo for the ACmemory library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * TODO: add comments
 */

#include <ACnokia5100.h>
#include <ACmemory.h>
#include <avr/pgmspace.h>

const boolean useScreen = true;

// uncomment below to see the difference
ACnokia5100 screen(3, 4, 5, 11, 13, 7);

// uncomment out the line below to see the difference
// #define F(str) str

// allocated in SRAM
char test1[] = "here is a first test that contains a non-small number of characters so that it will count";

// allocated in Flash (program space)
typedef PROGMEM char prog_char;
PROGMEM const prog_char test3[] = "here is a third test that contains a non-small number of characters so that it will count";

typedef const byte PROGMEM prog_byte;
const prog_byte testByte PROGMEM = 0x5f;
const prog_byte testBytes[] PROGMEM = { 0x5f, 0x6f };



const char MenuItem0 [] PROGMEM = "Menu Item 0";
const char MenuItem1 [] PROGMEM = "Menu Item 1";
const char MenuItem2 [] PROGMEM = "Menu Item 2";
typedef const char* fstring;
fstring const MenuItemPointers [] PROGMEM = { MenuItem0, MenuItem1, MenuItem2 };


void report() {
  Serial.print("SRAM free: ");
  Serial.println(getFreeSRAM());
}

void other() {
  char buffer[100];
  report();
  strcpy_P(buffer, test3);
  Serial.println( buffer );
}

void menu() {
  char buffer[100];
  report();
  fstring MenuItem = ((fstring) pgm_read_word(& MenuItemPointers[1]));
  strcpy_P(buffer, MenuItem);
  Serial.println(buffer);
}


void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");
  report();
  Serial.println(test1);
  Serial.println(F("here is a second test that contains a non-small number of characters so that it will count"));
  other();
  report();

  Serial.print("Byte: ");
  Serial.println((int) 0x5f);
  Serial.print("ReadByte: ");
  Serial.println((int) (pgm_read_byte_near(&testByte)));

  Serial.print("Byte: ");
  Serial.println((int) 0x6f);
  Serial.print("ReadByte: ");
  Serial.println( (pgm_read_byte_near(&testBytes[1])));
  
  menu();

  if (useScreen) {
    screen.setup();
    screen.setContrast(60);
    screen.clearDisplay(screen.WHITE);
    screen.setString("Loading...", 0, 0);
    screen.updateDisplay(); 
  }
}

void loop()
{
  // do nothing
  delay(1000);
  report();
}



/*
original progmem demo:



#include <avr/pgmspace.h>

typedef PROGMEM char prog_char;
typedef const prog_char* prog_charp;
const prog_char string_0[] PROGMEM = "String 0";   // "String 0" etc are strings to store - change to suit.
const prog_char string_1[] PROGMEM = "String 1";
const prog_char string_2[] PROGMEM = "String 2";
const prog_char string_3[] PROGMEM = "String 3";
const prog_char string_4[] PROGMEM = "String 4";
const prog_char string_5[] PROGMEM = "String 5";


// Then set up a table to refer to your strings.

// alternative: const prog_char* string_table[] = 	   // change "string_table" name to suit

const prog_charp string_table[] PROGMEM = 	   // change "string_table" name to suit
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5 };

char buffer[30];    // make sure this is large enough for the largest string it must hold

void setup()			  
{
  Serial.begin(9600);
}

void loop()			  
{
  for (int i = 0; i < 6; i++)
  {
    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); 
    // alternative:  strcpy_P(buffer, string_table[i]); // Necessary casts and dereferencing, just copy. 
    Serial.println( buffer );
    delay( 500 );
  }
}


-------------------------------
#define putstring(x) SerialPrint_P(PSTR(x))
void SerialPrint_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) Serial.write(c);
}
putstring("This is a nice long string that takes no memory");

-------------------------------
http://arduinoetcetera.blogspot.fr/2013/07/using-program-memory-progmem.html
http://playground.arduino.cc/Main/Printf
http://www.fourwalledcubicle.com/AVRArticles.php
http://www.github.com/abcminiuser/avr-tutorials/blob/master/Progmem/Output/Progmem.pdf?raw=true
*/
