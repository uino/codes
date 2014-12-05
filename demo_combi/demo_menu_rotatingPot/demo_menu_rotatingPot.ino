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
 */

#include <ACnokia5100.h>
#include <ACrotatingPot.h>
#include <ACbuttonLong.h>


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

char* panelItems0[] =
  {
  "disp. submenu",
  "disp. curve",
  "disp. measure",
  "disp. all",
  "set a",
  "set b",
  "set c",
  "set d",
  "set e",
  "set f",
  "set x-scale",
  "set y-scale",
  "set log params",
  "sleep",
  "reset device" 
  };

char* panelItems1[] =
  {
  "set x-scale",
  "set y-scale",
  "set log params"
  };


char** panelItems[nbPanels] = { 
  panelItems0, 
  panelItems1 };


//*****************************************************************
/* Constants */

const int screenNbRows = ACnokia5100::LCD_ROWS;
const int screenNbCols = ACnokia5100::LCD_COLS; // assumed to be 14

const int WHITE = ACnokia5100::WHITE;
const int BLACK = ACnokia5100::BLACK;


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

//*****************************************************************
/* Menu implementation */

void displayChoices(char** choices, int nbChoices, int selected) {
  screen.clearDisplay(WHITE);
  for (int i = 0; i < nbChoices; i++) {
    boolean whiteBackground = (i != selected);
    // Later: StringCompleteTo14(choices[i]);
    screen.setString(choices[i], i, 0, whiteBackground);
  }
  screen.updateDisplay();
}

int panel = 0;

int menuCurrent = 0; // should be between 0 and nbPanelItems[panel]
int menuPevious = -1;

void displayMenu() {
  if (menuCurrent == menuPevious)
    return; // if no change, nothing to do
  Serial.println(menuCurrent);
  int idPage = menuCurrent / screenNbRows;
  int idFirst = idPage * screenNbRows;
  int nbShown = min(screenNbRows, nbPanelItems[panel] - idFirst);
  displayChoices(panelItems[panel]+idFirst, nbShown, menuCurrent-idFirst);
  menuPevious = menuCurrent;
}

void enterPanel(int idPanel) {
  panel = idPanel;
  menuPevious = -1;
  rot.setModulo(nbPanelItems[panel]);
  rot.resetValue(0);
}

void shortClick() {
  Serial.println("short click");
  Serial.println(menuCurrent);
  if (panel == 0) {
    if (menuCurrent == 0) {
      enterPanel(1);
    }
  }
}

void longClick() {
  Serial.println("long click");
  Serial.println(menuCurrent);
  if (panel == 1) {
    enterPanel(0);
  }
}


//*****************************************************************
/* Main */

void setup()
{
  // Serial port
  Serial.begin(9600);

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
