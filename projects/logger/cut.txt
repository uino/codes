
void readRecordFromFile(File file, Record& r) {
  // read and ignore the human readable date
  String date = file.readStringUntil('\t');
  String time = file.readStringUntil('\t');
  // read the date and the measures into the record
  long unixtime = file.parseInt();
  r.date = unixtime;
  for (int m = 0; m < nbMeasures; m++) {
    float value = file.parseFloat();
    r.values[m] = value;
  }
  // read what remains of the line
  file.readStringUntil('\n');
}

// returns the number effectively read
int readRecordsFromFile(File file, int maxNbRecords, Record* rs) {
  int count = 0;
  while (file.available()) {
    // ignore lines that start with a '#' symbol
    if (file.peek() == '#') {
      file.readStringUntil('\n');
      continue;
    }
    // other lines are parsed as records
    readRecordFromFile(file, rs[count]);
    // for debug: printMeasureOnSerial(r);
    count++;
    if (count >= maxNbRecords)
      return count;
  }
  return count;
}

/*
void readRecordsFromLog() {
  File file = openLog(FILE_READ);
  if (! file)
    return;
  // nbHistory = readRecordsFromFile(file, maxHistory, history);
  file.close();
}
*/





//*****************************************************************
/* Menu definition */


const int panelItems0_size = 16;
const char panelItems0_0[] PROGMEM = "disp. curve";
const char panelItems0_1[] PROGMEM = "disp. submenu";
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
/* Menu implementation */

// TODO: move to library

#ifdef USE_SCREEN
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
  screen.clearDisplay();
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
#endif

const int panelRoot = 0;
int panel = panelRoot;

int menuCurrent = 0; // should be between 0 and nbPanelItems[panel]
int menuPevious = -1;

long dateLastDisplayMeasures = 0;


void displayPanel() {
#ifdef USE_SCREEN
  // Serial.println(F("display menu"));
  // Serial.println(menuCurrent);
  if (panel == 0 || panel == 1) {
    if (menuCurrent == menuPevious)
      return; // if no change, nothing to do
    int idPage = menuCurrent / screenNbRows;
    int idFirst = idPage * screenNbRows;
    PanelDescr descr = panelDescrs[panel];
    int nbShown = min(screenNbRows, descr.nbItems - idFirst);
    displayChoices(descr.items, idFirst, nbShown, menuCurrent);
    menuPevious = menuCurrent;
  } else if (panel == 100) {
    long timeNow = millis(); 
    if (timeNow - dateLastDisplayMeasures > 1000) { // display every 1 second
      displayMeasures(currentMeasure);
    }
  }
#endif
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
      enterPanel(100);
    } else if (menuCurrent == 1) {
      enterPanel(1);
    }
  }
}

void longClick() {
  Serial.println(F("long click"));
  if (panel == 1 || panel == 100) {
    enterPanel(0);
  }
}

void rotChange() {
  menuCurrent = rot.getValue();
}



