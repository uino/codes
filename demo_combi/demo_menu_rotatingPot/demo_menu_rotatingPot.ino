/* 
 * Arthur Chargueraud
 * This code is GNU GPL.
 *
 * Implementation of a menu displayed on a AC_Nokia5100
 * and manipulated using a rotatingPotentiometer (a
 * normal potentiometer with the AC_RotatingPot library) 
 * and a button (with short and long clicks, using the
 * AC_Button library).
 *
 * Be careful to allocate strings in Flash and not in SRAM
 * in order to avoid running out of space.
 */

#include <AC_Nokia5100.h>
#include <AC_RotatingPot.h>
#include <AC_Button.h>
#include <avr/pgmspace.h> 
#include <AC_RAM.h>
#include "defs.h"


//*****************************************************************
/* Configuration */

// Button
// (pins: buttonPin)
AC_Button button(2);
const int buttonSensitivity = 300; // milliseconds

// Rotating potentiometer
int rotPin = A0;
const int rotSensitivity = 40; // steps out of 1024 
const boolean rotInverted = true; 
AC_RotatingPot rot(rotPin, rotSensitivity, rotInverted);

// Nokia5100 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
AC_Nokia5100 screen(3, 4, 5, 11, 13, 7);


//*****************************************************************
/* Menu definition */

const int panelItems0_size = 16;
const char panelItems0_0[] PROGMEM = "disp. submenu";
const char panelItems0_1[] PROGMEM = "disp. measures";
const char panelItems0_2[] PROGMEM = "disp. all";
const char panelItems0_3[] PROGMEM = "set a";
const char panelItems0_4[] PROGMEM = "set b";
const char panelItems0_5[] PROGMEM = "set c";
const char panelItems0_6[] PROGMEM = "set b";
const char panelItems0_7[] PROGMEM = "set d";
const char panelItems0_8[] PROGMEM = "set e";
const char panelItems0_9[] PROGMEM = "set f";
const char panelItems0_10[] PROGMEM = "set g";
const char panelItems0_11[] PROGMEM = "set h";
const char panelItems0_12[] PROGMEM = "set i";
const char panelItems0_13[] PROGMEM = "set j";
const char panelItems0_14[] PROGMEM = "set k";
const char panelItems0_15[] PROGMEM = "set l";

const int panelItems1_size = 3;
const char panelItems1_0[] PROGMEM = "set x-scale";
const char panelItems1_1[] PROGMEM = "set y-scale";
const char panelItems1_2[] PROGMEM = "set log params";

const fstring panelItems[] PROGMEM = { 
  panelItems0_0, 
  panelItems0_1, 
  panelItems0_2, 
  panelItems0_3, 
  panelItems0_4,  
  panelItems0_5, 
  panelItems0_6, 
  panelItems0_7, 
  panelItems0_8, 
  panelItems0_9, 
  panelItems0_10, 
  panelItems0_11, 
  panelItems0_12, 
  panelItems0_13, 
  panelItems0_14, 
  panelItems0_15,

  panelItems1_0, 
  panelItems1_1, 
  panelItems1_2, 
  };

// TODO: could be in flash memory as well if needed
const int nbPanels = 2;
const PanelDescr panelDescrs[nbPanels] = { 
  { panelItems0_size, panelItems+0 },
  { panelItems1_size, panelItems+panelItems0_size },
  // { panelItems2_size, panelItems+panelItems0_size+panelItems1_size },
  };



//*****************************************************************
/* Constants */

const int screenNbRows = AC_Nokia5100::LCD_ROWS;
const int screenNbCols = AC_Nokia5100::LCD_COLS; // assumed to be 14
const int bufferRowLength = 30; // = screenNbCols+1 (but using more characters for safety)

const int WHITE = AC_Nokia5100::WHITE;
const int BLACK = AC_Nokia5100::BLACK;


//*****************************************************************
/* Menu implementation */

//TODO: move to library

// assumes buffer length >= bufferRowLength, and storing null-terminated string.
void completeLineWithSpaces(char* buffer) {
  int len = strlen(buffer);
  for (int k = len; k < screenNbCols; k++) {
    buffer[k] = ' ';
  }
  buffer[screenNbCols] = '\0';
}

void displayChoices(const fstring* choices, int firstChoice, int nbChoices, int selected) {
  char buffer[bufferRowLength];
  screen.clearDisplay(WHITE);
  for (int line = 0; line < nbChoices; line++) {
    int item = firstChoice + line;
    boolean whiteBackground = (item != selected);
    fstring fline = ((fstring) pgm_read_word(& choices[item]));
    strcpy_P(buffer, fline);
    completeLineWithSpaces(buffer);
    screen.setString(buffer, line, 0, whiteBackground);
  }
  screen.updateDisplay();
}

int panel = 0;

int menuCurrent = 0; // should be between 0 and nbPanelItems[panel]
int menuPevious = -1;

void displayMenu() {
  if (menuCurrent == menuPevious)
    return; // if no change, nothing to do
  // Serial.println(F("display menu"));
  // Serial.println(menuCurrent);
  int idPage = menuCurrent / screenNbRows;
  int idFirst = idPage * screenNbRows;
  PanelDescr descr = panelDescrs[panel];
  int nbShown = min(screenNbRows, descr.nbItems - idFirst);
  displayChoices(descr.items, idFirst, nbShown, menuCurrent);
  menuPevious = menuCurrent;
}

void enterPanel(int idPanel) {
  Serial.println(F("enter panel"));
  panel = idPanel;
  menuPevious = -1;
  rot.setModulo(panelDescrs[panel].nbItems);
  rot.resetValue(0);
}

void shortClick() {
  Serial.println(F("short click"));
  if (panel == 0) {
    if (menuCurrent == 0) {
      enterPanel(1);
    }
  }
}

void longClick() {
  Serial.println(F("long click"));
  if (panel == 1) {
    enterPanel(0);
  }
}


//*****************************************************************
/* Main */

void reportSRAM() {
  Serial.print(F("SRAM free: "));
  Serial.println(AC_RAM::getFree());
}

void setup()
{
  // Serial port
  Serial.begin(9600);
  reportSRAM();

  // LCD screen
  screen.begin();
  screen.setContrast(60);
  screen.clearDisplay(WHITE);
  screen.setString("Loading...", 0, 0);
  screen.updateDisplay(); 

  // Button
  button.begin();
  button.setLongPeriodDuration(buttonSensitivity);
  button.onUp(shortClick);
  button.onUpLong(longClick);

  // Rotating potentiometer
  rot.begin();

  // Setup for the menu demo 
  enterPanel(0);
}

void loop()
{
  button.poll();
  rot.poll();
  menuCurrent = rot.getValue();
  displayMenu();
  delay(50);
}


//*****************************************************************
/* Debug */


  /* 
  Serial.println("---------");
  for (int i = 0; i < nbShown; i++) {
    Serial.println(panelItems[panel][idFirst+i]);  
  } */
  /*
  Serial.print("panel : ");
  Serial.println(panel);
  Serial.print("menuCurrent : ");
  Serial.println(menuCurrent);
  Serial.print("idFirst : ");
  Serial.println(idFirst);
  Serial.print("shown : ");
  Serial.println(nbShown);
  */



//*****************************************************************
/* String manipulation constants */
/* LATER
String spaces[8] = {
  "",
  " ",
  "  ",
  "   ",
  "    ",
  "     ",
  "      ",
  "       ",
};

String StringCompleteTo14(String base) {
  int nb = 14;
  int len = base.length();
  if (len >= nb)
    return base;
  else if (len >= 7)
    return base + spaces[nb-len];
  else
    return base + spaces[7] + spaces[nb-len-7];
}
*/
