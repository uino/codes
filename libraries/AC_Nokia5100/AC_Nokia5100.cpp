
/**
 * AC_Nokia5100 Library -- see "AC_Nokia5100.h" for information.
 *
 */ 

#include "AC_Nokia5100.h"

#ifndef AC_Nokia5100_IN_SRAM
const byte AC_Nokia5100::ASCII[] =
#else
PROGMEM const prog_byte AC_Nokia5100::ASCII[] = 
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

AC_Nokia5100::AC_Nokia5100(int scePin, int rstPin, int dcPin,
                           int sdinPin, int sclkPin, int blPin)
{
  this->scePin = scePin;
  this->rstPin = rstPin;
  this->dcPin = dcPin;
  this->sdinPin = sdinPin;
  this->sclkPin = sclkPin;
  this->blPin = blPin;
}

void AC_Nokia5100::setPixel(int x, int y)
{
  setPixel(x, y, BLACK); 
}

void AC_Nokia5100::clearPixel(int x, int y)
{
  setPixel(x, y, WHITE);
}

void AC_Nokia5100::setPixel(int x, int y, boolean bw)
{
  // First, double check that the coordinate is in range.
  if ((x >= 0) && (x < LCD_WIDTH) && (y >= 0) && (y < LCD_HEIGHT))
  {
    byte shift = y % 8;
  
    if (bw) // If black, set the bit.
      displayMap[x + (y/8)*LCD_WIDTH] |= 1<<shift;
    else   // If white clear the bit.
      displayMap[x + (y/8)*LCD_WIDTH] &= ~(1<<shift);
  }
}

// This function was grabbed from the SparkFun ColorLCDShield 
// library.
void AC_Nokia5100::setLine(int x0, int y0, int x1, int y1, boolean bw)
{
  int dy = y1 - y0; // Difference between y0 and y1
  int dx = x1 - x0; // Difference between x0 and x1
  int stepx, stepy;

  if (dy < 0)
  {
    dy = -dy;
    stepy = -1;
  }
  else
    stepy = 1;

  if (dx < 0)
  {
    dx = -dx;
    stepx = -1;
  }
  else
    stepx = 1;

  dy <<= 1; // dy is now 2*dy
  dx <<= 1; // dx is now 2*dx
  setPixel(x0, y0, bw); // Draw the first pixel.

  if (dx > dy) 
  {
    int fraction = dy - (dx >> 1);
    while (x0 != x1)
    {
      if (fraction >= 0)
      {
        y0 += stepy;
        fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      setPixel(x0, y0, bw);
    }
  }
  else
  {
    int fraction = dx - (dy >> 1);
    while (y0 != y1)
    {
      if (fraction >= 0)
      {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      setPixel(x0, y0, bw);
    }
  }
}

// This function was grabbed from the SparkFun ColorLCDShield
// library.
void AC_Nokia5100::setRect(int x0, int y0, int x1, int y1, boolean fill, boolean bw)
{
  // check if the rectangle is to be filled
  if (fill == 1)
  {
    int xDiff;

    if(x0 > x1)
      xDiff = x0 - x1; //Find the difference between the x vars
    else
      xDiff = x1 - x0;

    while(xDiff > 0)
    {
      setLine(x0, y0, x0, y1, bw);

      if(x0 > x1)
        x0--;
      else
        x0++;

      xDiff--;
    }
  }
  else 
  {
    // best way to draw an unfilled rectangle is to draw four lines
    setLine(x0, y0, x1, y0, bw);
    setLine(x0, y1, x1, y1, bw);
    setLine(x0, y0, x0, y1, bw);
    setLine(x1, y0, x1, y1, bw);
  }
}

// This function was grabbed from the SparkFun ColorLCDShield 
// library.
void AC_Nokia5100::setCircle (int x0, int y0, int radius, boolean bw, int lineThickness)
{
  for(int r = 0; r < lineThickness; r++)
  {
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    setPixel(x0, y0 + radius, bw);
    setPixel(x0, y0 - radius, bw);
    setPixel(x0 + radius, y0, bw);
    setPixel(x0 - radius, y0, bw);

    while(x < y)
    {
      if(f >= 0)
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;

      setPixel(x0 + x, y0 + y, bw);
      setPixel(x0 - x, y0 + y, bw);
      setPixel(x0 + x, y0 - y, bw);
      setPixel(x0 - x, y0 - y, bw);
      setPixel(x0 + y, y0 + x, bw);
      setPixel(x0 - y, y0 + x, bw);
      setPixel(x0 + y, y0 - x, bw);
      setPixel(x0 - y, y0 - x, bw);
    }
    radius--;
  }
}

void AC_Nokia5100::setChar(char character, int row, int col, boolean bw)
{
  setCharXY(character, col*6, row*8, bw);
}

void AC_Nokia5100::setChar(char character, int row, int col)
{
  setChar(character, row, col, BLACK);
}

void AC_Nokia5100::setString(char* dString, int row, int col, boolean bw)
{
  setStringXY(dString, col*6, row*8, bw);
}

void AC_Nokia5100::setString(char* dString, int row, int col)
{
  setString(dString, row, col, BLACK);
}

void AC_Nokia5100::setString(String str, int row, int col) {
  setString(str, row, col, BLACK);
}

void AC_Nokia5100::setString(String str, int row, int col, boolean bw) {
  setStringXY(str, col*6, row*8, bw);
}

void AC_Nokia5100::setCharXY(char character, int x, int y, boolean bw)
{
  byte column; // temp byte to store character's column bitmap
  for (int i=0; i<5; i++) // 5 columns (x) per character
  {
    int k = (character - 0x20) * 5 + i;
#ifndef AC_Nokia5100_IN_SRAM
    column = ASCII[k];
#else
    column = pgm_read_byte_near(& ASCII[k]);
#endif
    for (int j=0; j<8; j++) // 8 rows (y) per character
    {
      if (column & (0x01 << j)) // test bits to set pixels
        setPixel(x+i, y+j, bw);
      else
        setPixel(x+i, y+j, !bw);
    }
  }
}

// This function was grabbed from the SparkFun ColorLCDShield
// library.
void AC_Nokia5100::setStringXY(char* dString, int x, int y, boolean bw)
{
  while (*dString != 0x00) // loop until null terminator
  {
    setCharXY(*dString++, x, y, bw);
    x+=5;
    for (int i=y; i<y+8; i++)
    {
      setPixel(x, i, !bw);
    }
    x++;
    if (x > (LCD_WIDTH - 5)) // Enables wrap around
    {
      x = 0;
      y += 8;
    }
  }
}

// adapated from above
void AC_Nokia5100::setStringXY(String str, int x, int y, boolean bw)
{
  int nb = str.length();
  for (int k = 0; k < nb; k++) 
  {
    char c = str[k];
    setCharXY(c, x, y, bw);
    x+=5;
    for (int i=y; i<y+8; i++)
    {
      setPixel(x, i, !bw);
    }
    x++;
    if (x > (LCD_WIDTH - 5)) // Enables wrap around
    {
      x = 0;
      y += 8;
    }
  }
}

void AC_Nokia5100::setBitmap(char* bitArray)
{
  for (int i=0; i<(LCD_WIDTH * LCD_HEIGHT / 8); i++)
    displayMap[i] = bitArray[i];
}

void AC_Nokia5100::clearDisplay()
{
  clearDisplay(WHITE);
}

void AC_Nokia5100::clearDisplay(boolean bw)
{
  for (int i=0; i<(LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    if (bw)
      displayMap[i] = 0xFF;
    else
      displayMap[i] = 0;
  }
}

void AC_Nokia5100::gotoXY(int x, int y)
{
  writeCmd(0, 0x80 | x);  // Column.
  writeCmd(0, 0x40 | y);  // Row.  ?
}

void AC_Nokia5100::updateDisplay()
{
  gotoXY(0, 0);
  for (int i=0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    writeCmd(LCD_DATA, displayMap[i]);
  }
}


void AC_Nokia5100::setContrast(byte contrast)
{  
  writeCmd(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
  writeCmd(LCD_COMMAND, 0x80 | contrast); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  writeCmd(LCD_COMMAND, 0x20); //Set display mode
}

void AC_Nokia5100::invertDisplay()
{
  /* There are two ways to do this. Either through direct commands
  to the display, or by swapping each bit in the displayMap array.
  We'll leave both methods here, comment one or the other out if 
  you please. */

  /* Direct LCD Command option
  writeCmd(LCD_COMMAND, 0x20); //Tell LCD that extended commands follow
  writeCmd(LCD_COMMAND, 0x08 | 0x05); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  writeCmd(LCD_COMMAND, 0x20); //Set display mode  */
  
  /* Indirect, swap bits in displayMap option: */
  for (int i=0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    displayMap[i] = ~displayMap[i] & 0xFF;
  }
  updateDisplay();
}

// There are two memory banks in the LCD, data/RAM and commands.
// This function sets the DC pin high or low depending, and then 
// sends the data byte
void AC_Nokia5100::writeCmd(byte data_or_command, byte data) 
{
  //Tell the LCD that we are writing either to data or a command
  digitalWrite(dcPin, data_or_command); 

  //Send the data
  digitalWrite(scePin, LOW);
  SPI.transfer(data); //shiftOut(sdinPin, sclkPin, MSBFIRST, data);
  digitalWrite(scePin, HIGH);
}

//This sends the magical commands to the PCD8544
void AC_Nokia5100::begin() 
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