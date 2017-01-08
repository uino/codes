/*

Led-tunnel
2017

Led-tunnel displays animations on a series of 15 LEDs set in a row.

*/

//--------------------------------------------------------------------------
// Configuration

// pins used for the LEDs (they should be in the right order)
const int nbLEDs = 15;
const int pinLED[nbLEDs] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 };

// To edit this table, use Notepad++ in Column edition mode (alt+shift+arrow)
// https://notepad-plus-plus.org/fr/features/column-mode-editing.html
const int nbFrames = 36; 

// Number of possible values for the intensity of a LED
const int nbIntensity = 8;

// Power that should be output on a pin for a given intensity code
const char powerForIntensity[nbIntensity] = { 75, 100, 125, 150, 175, 200, 225, 255 };

// table below should have nbFrames rows, and nbLEDs columns,
// values should be between 0 (inclusive) and nbIntensity (exclusive).
const char frameLEDIntensity[nbFrames*nbLEDs] = {
  2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
  7,4,2,0,0,0,0,0,0,0,0,0,0,0,0,
  4,7,4,2,0,0,0,0,0,0,0,0,0,0,0,
  2,4,7,4,2,0,0,0,0,0,0,0,0,0,0,
  0,2,4,7,4,2,0,0,0,0,0,0,0,0,0,
  0,0,2,4,7,4,2,0,0,0,0,0,0,0,0,
  0,0,0,2,4,7,4,2,0,0,0,0,0,0,0,
  0,0,0,0,2,4,7,4,2,0,0,0,0,0,0,
  0,0,0,0,0,2,4,7,4,2,0,0,0,0,0,
  0,0,0,0,0,0,2,4,7,4,2,0,0,0,0,
  0,0,0,0,0,0,0,2,4,7,4,2,0,0,0,
  0,0,0,0,0,0,0,0,2,4,7,4,2,0,0,
  0,0,0,0,0,0,0,0,0,2,4,7,4,2,0,
  0,0,0,0,0,0,0,0,0,0,2,4,7,4,2,
  0,0,0,0,0,0,0,0,0,0,0,2,4,7,4,
  0,0,0,0,0,0,0,0,0,0,0,0,2,4,7,
  0,0,0,0,0,0,0,0,0,0,0,0,0,2,7,
  0,0,0,0,0,0,0,0,0,0,0,0,0,2,7,
  0,0,0,0,0,0,0,0,0,0,0,0,2,4,7,
  0,0,0,0,0,0,0,0,0,0,0,2,4,7,4,
  0,0,0,0,0,0,0,0,0,0,2,4,7,4,2,
  0,0,0,0,0,0,0,0,0,2,4,7,4,2,0,
  0,0,0,0,0,0,0,0,2,4,7,4,2,0,0,
  0,0,0,0,0,0,0,2,4,7,4,2,0,0,0,
  0,0,0,0,0,0,2,4,7,4,2,0,0,0,0,
  0,0,0,0,0,2,4,7,4,2,0,0,0,0,0,
  0,0,0,0,2,4,7,4,2,0,0,0,0,0,0,
  0,0,0,2,4,7,4,2,0,0,0,0,0,0,0,
  0,0,2,4,7,4,2,0,0,0,0,0,0,0,0,
  0,2,4,7,4,2,0,0,0,0,0,0,0,0,0,
  2,4,7,4,2,0,0,0,0,0,0,0,0,0,0,
  4,7,4,2,0,0,0,0,0,0,0,0,0,0,0,
  7,4,2,0,0,0,0,0,0,0,0,0,0,0,0,
  4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
  2,0,0,0,0,0,0,0,0,0,0,0,0,0,0 
  };

// duration of a frame, in milliseconds
int frameDuration = 40; 

// Note: total animation lasts for: nbFrames * frameDuration / 1000 seconds.
// Here, 36 * 40ms = 1.44 seconds

// duration of the pause between two animations, in milliseconds
int pauseDuration = 500;


//--------------------------------------------------------------------------
// Setup

void setupPins() {
  for (int idLED = 0; idLED < nbLEDs; idLED++) {
    pinMode(pinLED[idLED], OUTPUT); 
  }
}

void setup() {                
  //Serial.begin(9600);
  setupPins();
}

//--------------------------------------------------------------------------
// Frame display

char getLEDpower(int frame, int idLed) {
  char intensity = frameLEDIntensity[frame * nbLEDs + idLed];
  return powerForIntensity[intensity];
}

void displayFrame(int frame) {
  for (int idLED = 0; idLED < nbLEDs; idLED++) {
    char power = getLEDpower(frame, idLED);
    analogWrite(pinLED[idLED], power);
  }
}

//--------------------------------------------------------------------------

// ID of current frame being displayed
int curFrame = 0;  

void loop() { 

  // process current frame
  displayFrame(curFrame);
  delay(frameDuration);

  // go to next frame, or restart if completed
  curFrame++;
  if (curFrame == nbFrames) {
    curFrame = 0;
    delay(pauseDuration);
  }
}

