 
 /*
// TODO remove ?
extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}
*/

#include <AC_TM1637.h>

//---------------------------------------------------------------------

const byte AC_TM1637::segmentOfDigitData[] = {
  // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
};


//---------------------------------------------------------------------

AC_TM1637::AC_TM1637(byte pinCLK, byte pinDIO, byte brightness) {
  this->pinCLK = pinCLK;
  this->pinDIO = pinDIO;
  this->brightness = brightness;
}

void AC_TM1637::begin() {
	// Set the pin direction and default value.
	// Both pins are set as inputs, allowing the pull-up resistors to pull them up
  pinMode(pinCLK, INPUT);
  pinMode(pinDIO,INPUT);
	digitalWrite(pinCLK, LOW);
	digitalWrite(pinDIO, LOW);
}

void AC_TM1637::setBrightness(byte brightness) {
  this->brightness = brightness;
}

//---------------------------------------------------------------------

void AC_TM1637::setSegments(const byte segments[]) {
  return setSegments(0, 4, segments);
}

void AC_TM1637::setSegments(int pos, int length, const byte segments[]) {
  // Write COMM1
	start();
	writeByte(COMM1);
	stop();
	
	// Write COMM2 + first digit address + data bytes
	start();
	writeByte(COMM2 + (pos & 0x03));
	for (uint8_t k = 0; k < length; k++) {
	  writeByte(segments[k]);
  }
	stop();

	// Write COMM3 + brightness
	start();
	writeByte(COMM3 + (brightness & 0x0f));
	stop();
}

void AC_TM1637::setSegmentAt(int pos, byte segment) {
  byte segments[1] = { segment };
  setSegments(pos, 1, segments);
}

byte AC_TM1637::segmentOfDigit(int digit) {
	return segmentOfDigitData[digit & 0x0f];
}

//---------------------------------------------------------------------

void AC_TM1637::setDigitAt(int pos, int digit, boolean decimalPoint) {
  byte segment = segmentOfDigit(digit);
  if (decimalPoint) {
    segment |= POINT;
  }
  setSegmentAt(pos, segment);
}

void AC_TM1637::setDigits(int digit1, int digit2, int digit3, int digit4) {
  byte segs[4] = { 
    segmentOfDigit(digit1),
    segmentOfDigit(digit2),
    segmentOfDigit(digit3),
    segmentOfDigit(digit4) };
  setSegments(segs);
}

void AC_TM1637::segmentsForInvalid(byte* segments) {
  for (int pos = 0; pos < 4; pos++) {
    segments[pos] = SEG_G;  
  }
}

void AC_TM1637::segmentsForInt(int value, boolean leadingZero, byte* segments) {
  if (value < -999 || value > 9999) {
    segmentsForInvalid(segments);
    return;
  }
  boolean isNeg = false;
  if (value < 0) {
    isNeg = true;
    value = -value;
  }
  for (int pos = 3; pos >= 0; pos--) {
    byte s;
    if (value == 0) {
      if (leadingZero) {
        s = segmentOfDigit(0);
      } else {
        s = 0;
      }
    } else {
      int digit = value % 10;
      s = segmentOfDigit(digit);
      value /= 10;
    }
    segments[pos] = s;
  }
  if (isNeg) {
    segments[0] = SEG_G; // minus sign
  } 
}

void AC_TM1637::showInt(int value, boolean leadingZero) {
  byte segments[4];
  segmentsForInt(value, leadingZero, segments);
  setSegments(segments);
}

void AC_TM1637::showFloat(float value, int precision, boolean leadingZero) {
  byte segments[4];
  if (precision < 0 || precision > 3) {
    segmentsForInvalid(segments);
    return;
  }
  for (int i = 0; i < precision; i++) {
    value *= 10.;
  }
  int intValue = (int) lround(value);
  segmentsForInt(intValue, leadingZero, segments);
  segments[3-precision] |= POINT;
  setSegments(segments);
}

void AC_TM1637::showTime(int hours, int minutes) {
  byte segs[4];
  if (hours < 0 || hours >= 99 || minutes < 0 || minutes >= 99) {
    segmentsForInvalid(segs);
    return;
  }
  segs[3] = segmentOfDigit(minutes % 10);
  segs[2] = segmentOfDigit(minutes / 10);
  segs[1] = segmentOfDigit(hours % 10);
  segs[0] = 0;
  int first = hours / 10;
  if (first > 0) {
    segs[0] = segmentOfDigit(first);
  }
  segs[1] |= POINT;
  setSegments(segs);
}


//---------------------------------------------------------------------

void AC_TM1637::smallDelay()
{
	delayMicroseconds(50);
}
   
void AC_TM1637::start()
{
  pinMode(pinDIO, OUTPUT);
  smallDelay();
}
   
void AC_TM1637::stop()
{
	pinMode(pinDIO, OUTPUT);
	smallDelay();
	pinMode(pinCLK, INPUT);
	smallDelay();
	pinMode(pinDIO, INPUT);
	smallDelay();
}
  
bool AC_TM1637::writeByte(byte b)
{
  uint8_t data = b;

  // 8 Data Bits
  for(uint8_t i = 0; i < 8; i++) {
    // CLK low
    pinMode(pinCLK, OUTPUT);
    smallDelay();
    
	// Set data bit
    if (data & 0x01)
      pinMode(pinDIO, INPUT);
    else
      pinMode(pinDIO, OUTPUT);
    
    smallDelay();
	
	// CLK high
    pinMode(pinCLK, INPUT);
    smallDelay();
    data = data >> 1;
  }
  
  // Wait for acknowledge
  // CLK to zero
  pinMode(pinCLK, OUTPUT);
  pinMode(pinDIO, INPUT);
  smallDelay();
  
  // CLK to high
  pinMode(pinCLK, INPUT);
  smallDelay();
  uint8_t ack = digitalRead(pinDIO);
  if (ack == 0)
    pinMode(pinDIO, OUTPUT);
	
	
  smallDelay();
  pinMode(pinCLK, OUTPUT);
  smallDelay();
  
  return ack;
}



/*
void AC_TM1637::showNumberDecDot(int num, bool leading_zero, byte length, byte pos, int decimal_dot_place) {
  byte digits[4];
  const static int divisors[] = { 1, 10, 100, 1000 };
  bool leading = true;

  for(int8_t k = 0; k < 4; k++) {
    int divisor = divisors[4 - 1 - k];
    int d = num / divisor;

    if (d == 0) {
      if (leading_zero || !leading || (k == 3))
        {
     digits[k] = encodeDigit(d);
     if (decimal_dot_place==k)
       digits[k] += 0b10000000;
    }
      else
        digits[k] = 0;
     if (decimal_dot_place==k)
       digits[k] += 0b10000000;
    }

    else {
        digits[k] = encodeDigit(d);
        num -= d * divisor;
        leading = false;
     if (decimal_dot_place==k)
       digits[k] += 0b10000000;
    }
  }
  setSegments(pos, length, digits + (4 - length));
}
*/