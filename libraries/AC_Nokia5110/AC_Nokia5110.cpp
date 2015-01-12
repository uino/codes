
/**
 * AC_Nokia5110 Library -- see "AC_Nokia5110.h" for information.
 *
 */ 

#include "AC_Nokia5110.h"

AC_Nokia5110::AC_Nokia5110(byte scePin, byte rstPin, byte dcPin, byte sdinPin, byte sclkPin, byte blPin) 
  : AC_Nokia5110_common::AC_Nokia5110_common(scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
{ }

void AC_Nokia5110::setPixel(int x, int y)
{
  setPixel(x, y, BLACK); 
}

void AC_Nokia5110::clearPixel(int x, int y)
{
  setPixel(x, y, WHITE);
}

void AC_Nokia5110::setPixel(int x, int y, boolean bw)
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
void AC_Nokia5110::setLine(int x0, int y0, int x1, int y1, boolean bw)
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
void AC_Nokia5110::setRect(int x0, int y0, int x1, int y1, boolean fill, boolean bw)
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
void AC_Nokia5110::setCircle (int x0, int y0, int radius, boolean bw, int lineThickness)
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

void AC_Nokia5110::setChar(char character, int row, int col, boolean bw)
{
  setCharXY(character, col*6, row*8, bw);
}

void AC_Nokia5110::setChar(char character, int row, int col)
{
  setChar(character, row, col, BLACK);
}

void AC_Nokia5110::setString(char* dString, int row, int col, boolean bw)
{
  setStringXY(dString, col*6, row*8, bw);
}

void AC_Nokia5110::setString(char* dString, int row, int col)
{
  setString(dString, row, col, BLACK);
}

void AC_Nokia5110::setString(String str, int row, int col) {
  setString(str, row, col, BLACK);
}

void AC_Nokia5110::setString(String str, int row, int col, boolean bw) {
  setStringXY(str, col*6, row*8, bw);
}

void AC_Nokia5110::setCharXY(char character, int x, int y, boolean bw)
{
  for (int i=0; i<5; i++) // 5 columns (x) per character
  {
    byte column = getASCII(character, i);
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
void AC_Nokia5110::setStringXY(char* dString, int x, int y, boolean bw)
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

// adapated from above // TODO: factorize
void AC_Nokia5110::setStringXY(String str, int x, int y, boolean bw)
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

void AC_Nokia5110::setBitmap(char* bitArray)
{
  for (int i=0; i<(LCD_WIDTH * LCD_HEIGHT / 8); i++)
    displayMap[i] = bitArray[i];
}

void AC_Nokia5110::clearDisplay()
{
  clearDisplay(WHITE);
}

void AC_Nokia5110::clearDisplay(boolean bw)
{
  for (int i=0; i<(LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    if (bw)
      displayMap[i] = 0xFF;
    else
      displayMap[i] = 0;
  }
}

void AC_Nokia5110::updateDisplay()
{
  gotoXY(0, 0);
  for (int i=0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    writeCmd(LCD_DATA, displayMap[i]);
  }
}
