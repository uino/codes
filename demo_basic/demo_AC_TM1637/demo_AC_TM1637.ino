/**
 * Demo for the AC_TM1637 library.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 *
 * The demo displays various values,
 * then display the current counter which increments
 * at some frequency.
 *
 * Wiring:
 * - CLK to pin 11
 * - DIO to pin 12
 * - VCC to alimentation (or to power control digital pin)
 * - GND to ground
 *
 */

#include <AC_TM1637.h>

// comment out line below if no pin is used to control the power
#define POWER_CONTROL 13

const int pinCLK = 11;
const int pinDIO = 12;

AC_TM1637 display(pinCLK, pinDIO);

const int delayBetweenDemos = 3000;
const int periodBetweenIncrements = 5; // milliseconds

void setup()
{
  Serial.begin(9600);   
  Serial.println("Starting up");

#ifdef POWER_CONTROL
  pinMode(POWER_CONTROL, OUTPUT);
  digitalWrite(POWER_CONTROL, HIGH);
#endif 

  display.begin();

  // showFloat only works with devices where every digit has a decimal point
  display.showFloat(19.934, 2); // display 19.93
  delay(delayBetweenDemos);
  display.showFloat(19.934, 1, true); // leading zero; display 019.9
  delay(delayBetweenDemos);
  display.showFloat(-9.92, 2); // display -9.92
  delay(delayBetweenDemos);
  display.showFloat(-99.2, 2); // overflow
  delay(delayBetweenDemos);

  display.showTime(14, 32);
  delay(delayBetweenDemos);
  display.showTime(7, 8);
  delay(delayBetweenDemos);

  display.setDigits(5,6,7,8);
  delay(delayBetweenDemos);
  display.setDigits(0,9,0,0);
  delay(delayBetweenDemos);
  display.setDigitAt(1, 9); // display becomes 800
  delay(delayBetweenDemos);

  display.showInt(1234);
  delay(delayBetweenDemos);
  display.showInt(-268);
  delay(delayBetweenDemos);
  display.showInt(67, true); // leadingZero
  delay(delayBetweenDemos);
  display.showInt(-68, true); // leadingZero
  delay(delayBetweenDemos);
  display.showInt(-1000); // overflow
  delay(delayBetweenDemos);
  display.showInt(-999); 
  delay(delayBetweenDemos);
  display.showInt(12345); // overflow
  delay(delayBetweenDemos);
}

int counter = 0;

void loop()
{
  counter = (counter+1);
  display.showInt(counter);
  delay(periodBetweenIncrements);
}



/*

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   1000

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

TM1637Display display(CLK, DIO);

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up");
  int pinAlim = 13;
  pinMode(pinAlim, OUTPUT); // TODO: for pinAlim

  digitalWrite(pinAlim, HIGH);
}

void loop()
{
  int k = 8;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
 // display.setBrightness(0x0f);
  display.setBrightness(8);


  // All segments on
  Serial.println("all on");
  display.setSegments(data);
  delay(TEST_DELAY);

  // Selectively set different digits
  Serial.println("digits");
  data[0] = 0b01001001;
  data[1] = display.encodeDigit(1);
  data[2] = display.encodeDigit(2);
  data[3] = display.encodeDigit(3);
  
  for(k = 3; k >= 0; k--) {
	display.setSegments(data, 1, k);
	delay(TEST_DELAY);
	}
	
   Serial.println("other");
  display.setSegments(data+2, 2, 2);
  delay(TEST_DELAY);
  
  display.setSegments(data+2, 2, 1);
  delay(TEST_DELAY);
  
  display.setSegments(data+1, 3, 1);
  delay(TEST_DELAY);
 
  
  // Show decimal numbers with/without leading zeros
  bool lz = false;
  for (uint8_t z = 0; z < 2; z++) {
	for(k = 0; k < 10000; k += k*4 + 7) {
		display.showNumberDec(k, lz);
		delay(TEST_DELAY);
	}
	lz = true;
  }
  
  // Show decimal number whose length is smaller than 4
  for(k = 0; k < 4; k++)
	data[k] = 0;
  display.setSegments(data);
  
  display.showNumberDec(153, false, 3, 1);
  delay(TEST_DELAY);
  display.showNumberDec(22, false, 2, 2);
  delay(TEST_DELAY);
  display.showNumberDec(0, true, 1, 3);
  delay(TEST_DELAY);
  display.showNumberDec(0, true, 1, 2);
  delay(TEST_DELAY);
  display.showNumberDec(0, true, 1, 1);
  delay(TEST_DELAY);
  display.showNumberDec(0, true, 1, 0);
  delay(TEST_DELAY);

  // Brightness Test
  for(k = 0; k < 4; k++)
	data[k] = 0xff;
  for(k = 0; k < 16; k++) {
    display.setBrightness(k);
    display.setSegments(data);
    delay(TEST_DELAY);
  }
    
  // Done!
  display.setSegments(SEG_DONE);
  
  while(1);
}


*/