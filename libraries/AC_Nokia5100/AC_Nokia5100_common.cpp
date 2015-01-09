
/**
 * AC_Nokia5100 Library -- see "AC_Nokia5100.h" for information.
 *
 */ 

#include "AC_Nokia5100_common.h"

#ifndef AC_Nokia5100_IN_SRAM
const byte AC_Nokia5100_common::ASCII[] =
#else
PROGMEM const prog_byte AC_Nokia5100_common::ASCII[] = 
#endif
  {
    // First 32 characters (0x00-0x19) are ignored. These are
    // non-displayable, control characters.
    0x00, 0x00, 0x00, 0x00, 0x00, // 0x20  
    0x00, 0x00, 0x5f, 0x00, 0x00, // 0x21 !
    0x00, 0x07, 0x00, 0x07, 0x00, // 0x22 "
    0x14, 0x7f, 0x14, 0x7f, 0x14, // 0x23 #
    0x24, 0x2a, 0x7f, 0x2a, 0x12, // 0x24 $
    0x23, 0x13, 0x08, 0x64, 0x62, // 0x25 %
    0x36, 0x49, 0x55, 0x22, 0x50, // 0x26 &
    0x00, 0x05, 0x03, 0x00, 0x00, // 0x27 '
    0x00, 0x1c, 0x22, 0x41, 0x00, // 0x28 (
    0x00, 0x41, 0x22, 0x1c, 0x00, // 0x29 )
    0x14, 0x08, 0x3e, 0x08, 0x14, // 0x2a *
    0x08, 0x08, 0x3e, 0x08, 0x08, // 0x2b +
    0x00, 0x50, 0x30, 0x00, 0x00, // 0x2c ,
    0x08, 0x08, 0x08, 0x08, 0x08, // 0x2d -
    0x00, 0x60, 0x60, 0x00, 0x00, // 0x2e .
    0x20, 0x10, 0x08, 0x04, 0x02, // 0x2f /
    0x3e, 0x51, 0x49, 0x45, 0x3e, // 0x30 0
    0x00, 0x42, 0x7f, 0x40, 0x00, // 0x31 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 0x32 2
    0x21, 0x41, 0x45, 0x4b, 0x31, // 0x33 3
    0x18, 0x14, 0x12, 0x7f, 0x10, // 0x34 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 0x35 5
    0x3c, 0x4a, 0x49, 0x49, 0x30, // 0x36 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 0x37 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 0x38 8
    0x06, 0x49, 0x49, 0x29, 0x1e, // 0x39 9
    0x00, 0x36, 0x36, 0x00, 0x00, // 0x3a :
    0x00, 0x56, 0x36, 0x00, 0x00, // 0x3b ;
    0x08, 0x14, 0x22, 0x41, 0x00, // 0x3c <
    0x14, 0x14, 0x14, 0x14, 0x14, // 0x3d =
    0x00, 0x41, 0x22, 0x14, 0x08, // 0x3e >
    0x02, 0x01, 0x51, 0x09, 0x06, // 0x3f ?
    0x32, 0x49, 0x79, 0x41, 0x3e, // 0x40 @
    0x7e, 0x11, 0x11, 0x11, 0x7e, // 0x41 A
    0x7f, 0x49, 0x49, 0x49, 0x36, // 0x42 B
    0x3e, 0x41, 0x41, 0x41, 0x22, // 0x43 C
    0x7f, 0x41, 0x41, 0x22, 0x1c, // 0x44 D
    0x7f, 0x49, 0x49, 0x49, 0x41, // 0x45 E
    0x7f, 0x09, 0x09, 0x09, 0x01, // 0x46 F
    0x3e, 0x41, 0x49, 0x49, 0x7a, // 0x47 G
    0x7f, 0x08, 0x08, 0x08, 0x7f, // 0x48 H
    0x00, 0x41, 0x7f, 0x41, 0x00, // 0x49 I
    0x20, 0x40, 0x41, 0x3f, 0x01, // 0x4a J
    0x7f, 0x08, 0x14, 0x22, 0x41, // 0x4b K
    0x7f, 0x40, 0x40, 0x40, 0x40, // 0x4c L
    0x7f, 0x02, 0x0c, 0x02, 0x7f, // 0x4d M
    0x7f, 0x04, 0x08, 0x10, 0x7f, // 0x4e N
    0x3e, 0x41, 0x41, 0x41, 0x3e, // 0x4f O
    0x7f, 0x09, 0x09, 0x09, 0x06, // 0x50 P
    0x3e, 0x41, 0x51, 0x21, 0x5e, // 0x51 Q
    0x7f, 0x09, 0x19, 0x29, 0x46, // 0x52 R
    0x46, 0x49, 0x49, 0x49, 0x31, // 0x53 S
    0x01, 0x01, 0x7f, 0x01, 0x01, // 0x54 T
    0x3f, 0x40, 0x40, 0x40, 0x3f, // 0x55 U
    0x1f, 0x20, 0x40, 0x20, 0x1f, // 0x56 V
    0x3f, 0x40, 0x38, 0x40, 0x3f, // 0x57 W
    0x63, 0x14, 0x08, 0x14, 0x63, // 0x58 X
    0x07, 0x08, 0x70, 0x08, 0x07, // 0x59 Y
    0x61, 0x51, 0x49, 0x45, 0x43, // 0x5a Z
    0x00, 0x7f, 0x41, 0x41, 0x00, // 0x5b [
    0x02, 0x04, 0x08, 0x10, 0x20, // 0x5c \
    0x00, 0x41, 0x41, 0x7f, 0x00, // 0x5d ]
    0x00, 0x41, 0x41, 0x7f, 0x00, // 0x5d ]
    0x04, 0x02, 0x01, 0x02, 0x04, // 0x5e ^
    0x40, 0x40, 0x40, 0x40, 0x40, // 0x5f _
    0x00, 0x01, 0x02, 0x04, 0x00, // 0x60 `
    0x20, 0x54, 0x54, 0x54, 0x78, // 0x61 a
    0x7f, 0x48, 0x44, 0x44, 0x38, // 0x62 b
    0x38, 0x44, 0x44, 0x44, 0x20, // 0x63 c
    0x38, 0x44, 0x44, 0x48, 0x7f, // 0x64 d
    0x38, 0x54, 0x54, 0x54, 0x18, // 0x65 e
    0x08, 0x7e, 0x09, 0x01, 0x02, // 0x66 f
    0x0c, 0x52, 0x52, 0x52, 0x3e, // 0x67 g
    0x7f, 0x08, 0x04, 0x04, 0x78, // 0x68 h
    0x00, 0x44, 0x7d, 0x40, 0x00, // 0x69 i
    0x20, 0x40, 0x44, 0x3d, 0x00, // 0x6a j 
    0x7f, 0x10, 0x28, 0x44, 0x00, // 0x6b k
    0x00, 0x41, 0x7f, 0x40, 0x00, // 0x6c l
    0x7c, 0x04, 0x18, 0x04, 0x78, // 0x6d m
    0x7c, 0x08, 0x04, 0x04, 0x78, // 0x6e n
    0x38, 0x44, 0x44, 0x44, 0x38, // 0x6f o
    0x7c, 0x14, 0x14, 0x14, 0x08, // 0x70 p
    0x08, 0x14, 0x14, 0x18, 0x7c, // 0x71 q
    0x7c, 0x08, 0x04, 0x04, 0x08, // 0x72 r
    0x48, 0x54, 0x54, 0x54, 0x20, // 0x73 s
    0x04, 0x3f, 0x44, 0x40, 0x20, // 0x74 t
    0x3c, 0x40, 0x40, 0x20, 0x7c, // 0x75 u
    0x1c, 0x20, 0x40, 0x20, 0x1c, // 0x76 v
    0x3c, 0x40, 0x30, 0x40, 0x3c, // 0x77 w
    0x44, 0x28, 0x10, 0x28, 0x44, // 0x78 x
    0x0c, 0x50, 0x50, 0x50, 0x3c, // 0x79 y
    0x44, 0x64, 0x54, 0x4c, 0x44, // 0x7a z
    0x00, 0x08, 0x36, 0x41, 0x00, // 0x7b {
    0x00, 0x00, 0x7f, 0x00, 0x00, // 0x7c |
    0x00, 0x41, 0x36, 0x08, 0x00, // 0x7d ,
    0x10, 0x08, 0x08, 0x10, 0x08, // 0x7e ~
    0x78, 0x46, 0x41, 0x46, 0x78 // 0x7f DEL
};

AC_Nokia5100_common::AC_Nokia5100_common(byte scePin, byte rstPin, byte dcPin,
                           byte sdinPin, byte sclkPin, byte blPin)
{
  this->scePin = scePin;
  this->rstPin = rstPin;
  this->dcPin = dcPin;
  this->sdinPin = sdinPin;
  this->sclkPin = sclkPin;
  this->blPin = blPin;
}

void AC_Nokia5100_common::gotoXY(int x, int y)
{
  writeCmd(0, 0x80 | x);  // Column.
  writeCmd(0, 0x40 | y);  // Row.  ?
}

void AC_Nokia5100_common::setContrast(byte contrast)
{  
  writeCmd(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
  writeCmd(LCD_COMMAND, 0x80 | contrast); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  writeCmd(LCD_COMMAND, 0x20); //Set display mode
}

void AC_Nokia5100_common::invertDisplay()
{
  /* There are two ways to do this. Either through direct commands
  to the display, or by swapping each bit in the displayMap array.
  We'll leave both methods here, comment one or the other out if 
  you please. */

  /* Direct LCD Command option */
  writeCmd(LCD_COMMAND, 0x20); //Tell LCD that extended commands follow
  writeCmd(LCD_COMMAND, 0x08 | 0x05); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  writeCmd(LCD_COMMAND, 0x20); //Set display mode 
  
  /* Indirect, swap bits in displayMap option:
  for (int i=0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    displayMap[i] = ~displayMap[i] & 0xFF;
  }
  updateDisplay();
   */
}

// There are two memory banks in the LCD, data/RAM and commands.
// This function sets the DC pin high or low depending, and then 
// sends the data byte
void AC_Nokia5100_common::writeCmd(byte data_or_command, byte data) 
{
  //Tell the LCD that we are writing either to data or a command
  digitalWrite(dcPin, data_or_command); 

  //Send the data
  digitalWrite(scePin, LOW);
  SPI.transfer(data); //shiftOut(sdinPin, sclkPin, MSBFIRST, data);
  digitalWrite(scePin, HIGH);
}

//This sends the magical commands to the PCD8544
void AC_Nokia5100_common::begin() 
{
  //Configure control pins
  pinMode(scePin, OUTPUT);
  pinMode(rstPin, OUTPUT);
  pinMode(dcPin, OUTPUT);
  pinMode(sdinPin, OUTPUT);
  pinMode(sclkPin, OUTPUT);
  pinMode(blPin, OUTPUT);
  analogWrite(blPin, 255);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  
  //Reset the LCD to a known state
  digitalWrite(rstPin, LOW);
  digitalWrite(rstPin, HIGH);

  writeCmd(LCD_COMMAND, 0x21); //Tell LCD extended commands follow
  writeCmd(LCD_COMMAND, 0xB0); //Set LCD Vop (Contrast)
  writeCmd(LCD_COMMAND, 0x04); //Set Temp coefficent
  writeCmd(LCD_COMMAND, 0x14); //LCD bias mode 1:48 (try 0x13)
  //We must send 0x20 before modifying the display control mode
  writeCmd(LCD_COMMAND, 0x20); 
  writeCmd(LCD_COMMAND, 0x0C); //Set display control, normal mode.
}

byte AC_Nokia5100_common::getASCII(char c, int column) {
  int k = (c - 0x20) * 5 + column;
#ifndef AC_Nokia5100_IN_SRAM
  return ASCII[k];
#else
  return pgm_read_byte_near(& ASCII[k]);
#endif
}

