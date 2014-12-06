/* 
 * Arthur Chargueraud
 * This code is GNU GPL.
 *
 * Implementation of a menu displayed on a ACnokia5100
 * and manipulated using a rotatingPotentiometer (a
 * normal potentiometer with the ACrotatingPot library) 
 * and a button (with short and long clicks, using the
 * ACbuttonLong library).
 *
 * Be careful to allocate strings in Flash and not in SRAM
 * in order to avoid running out of space.
 */

#include <ACnokia5100.h>
#include <ACrotatingPot.h>
#include <ACbuttonLong.h>
#include <avr/pgmspace.h> 
#include <ACmemory.h>


//*****************************************************************
/* Configuration */

// Button
// (pins: buttonPin)
ACbuttonLong button(2);
const int buttonSensitivity = 300; // milliseconds

// Rotating potentiometer
int rotPin = A0;
const int rotSensitivity = 40; // steps out of 1024 
const boolean rotInverted = true; 
ACrotatingPot rot(rotPin, rotSensitivity, rotInverted);

// Nokia5100 : for display
// (pins: scePin, rstPin, dcPin, sdinPin, sclkPin, blPin)
ACnokia5100 screen(3, 4, 5, 11, 13, 7);


//*****************************************************************
/* Menu definition */

const int nbPanels = 2;
const int nbPanelItems[nbPanels] = { 
  15, // panel 0
  3   // panel 1
  };

typedef PROGMEM char prog_char;
typedef PROGMEM const prog_char* const pstring;

PROGMEM const prog_char panelItems0_0[] = "disp. submenu";
PROGMEM const prog_char panelItems0_1[] = "disp. curve";
PROGMEM const prog_char panelItems0_2[] = "disp. all";
PROGMEM const prog_char panelItems0_3[] = "set a";
PROGMEM const prog_char panelItems0_4[] = "set b";
PROGMEM const prog_char panelItems0_5[] = "set c";
PROGMEM const prog_char panelItems0_6[] = "set b";
PROGMEM const prog_char panelItems0_7[] = "set d";
PROGMEM const prog_char panelItems0_8[] = "set e";
PROGMEM const prog_char panelItems0_9[] = "set f";
PROGMEM const prog_char panelItems0_10[] = "set g";
PROGMEM const prog_char panelItems0_11[] = "set h";
PROGMEM const prog_char panelItems0_12[] = "set i";
PROGMEM const prog_char panelItems0_13[] = "set j";
PROGMEM const prog_char panelItems0_14[] = "set k";
PROGMEM const prog_char panelItems0_15[] = "set l";

const pstring panelItems0[] = { 
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
  panelItems0_15 };

/*
cstring panelItems1[] =
  {
  "set x-scale",
  "set y-scale",
  "set log params"
  };

cstring* panelItems[nbPanels] = { 
  panelItems0, 
  panelItems1 };
*/

pstring* panelItems[nbPanels] = { 
  panelItems0 
  };



//*****************************************************************
/* Constants */

const int screenNbRows = ACnokia5100::LCD_ROWS;
const int screenNbCols = ACnokia5100::LCD_COLS; // assumed to be 14
const int bufferRowLength = 30; // = screenNbCols+1 (but using more characters for safety)

const int WHITE = ACnokia5100::WHITE;
const int BLACK = ACnokia5100::BLACK;


//*****************************************************************
/* Menu implementation */

//TODO: move to library

void displayChoices(pstring* choices, int firstChoice, int nbChoices, int selected) {
  char buffer[bufferRowLength];
  screen.clearDisplay(WHITE);
  // Serial.println("---");
  for (int i = firstChoice; i < firstChoice+nbChoices; i++) {
    boolean whiteBackground = (i != selected);
    // Later: StringCompleteTo14(choices[i]);
    pstring pline = choices[i];
    strcpy_P(buffer, pline);
    screen.setString(buffer, i, 0, whiteBackground);
    //Serial.print(i);
    //Serial.println("");
  }
  screen.updateDisplay();
}

int panel = 0;

int menuCurrent = 0; // should be between 0 and nbPanelItems[panel]
int menuPevious = -1;
/*
const char MenuItem1 [] PROGMEM = " Menu Item 1 ";
const char MenuItem2 [] PROGMEM = " Menu Item 2 ";
const char MenuItem3 [] PROGMEM = " Menu Item 3 ";
const char * const MenuItemPointers [] PROGMEM = { MenuItem1 , MenuItem2 , MenuItem3 };
void main ( void )
{
while (1) // Eternal Loop
{
char EnteredNum = USART_GetNum () ;
USART_TxString_P ( pgm_read_word (& MenuItemPointers [ EnteredNum ]) );
}
}
*/

void displayMenu() {
  if (menuCurrent == menuPevious)
    return; // if no change, nothing to do
  Serial.println(F("display menu"));
  Serial.println(menuCurrent);
  int idPage = menuCurrent / screenNbRows;
  int idFirst = idPage * screenNbRows;
  int nbShown = min(screenNbRows, nbPanelItems[panel] - idFirst);
  displayChoices(panelItems[panel], idFirst, nbShown, menuCurrent);
  menuPevious = menuCurrent;
}

void enterPanel(int idPanel) {
  Serial.println(F("enter panel"));
  panel = idPanel;
  menuPevious = -1;
  rot.setModulo(nbPanelItems[panel]);
  rot.resetValue(0);
}

void shortClick() {
  Serial.println(F("short click"));
  Serial.println(menuCurrent);
  if (panel == 0) {
    if (menuCurrent == 0) {
      enterPanel(1);
    }
  }
}

void longClick() {
  Serial.println(F("long click"));
  Serial.println(menuCurrent);
  if (panel == 1) {
    enterPanel(0);
  }
}


//*****************************************************************
/* Main */

void reportSRAM() {
  Serial.print(F("SRAM free: "));
  Serial.println(getFreeSRAM());
}

void setup()
{
  // Serial port
  Serial.begin(9600);
  reportSRAM();

  // LCD screen
  screen.setup();
  screen.setContrast(60);
  screen.clearDisplay(WHITE);
  screen.setString("Loading...", 0, 0);
  screen.updateDisplay(); 

  // Button
  button.setup();
  button.setLongPeriodDuration(buttonSensitivity);
  button.onUp(shortClick);
  button.onUpAfterLong(longClick);

  // Rotating potentiometer
  rot.setup();

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
