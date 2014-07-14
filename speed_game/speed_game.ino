/*

Speed-Game 
2014

Speed-Game is a game to be played with a single button. 
When the buzzer buzzes or when the light flashes, press
the button as quickly as you can. Such an even takes place
at random intervals, between 1 and 10 secondes.
After each event, your score is reported. After 10 events, 
the game is over and your average score is reported.
Smaller values corrrespond to better scores.


Infos:
- see project "seven_segment" for documentation and demo of the 7-segment display.

*/
  


// game parameters
int minDelayBetweenEvents = 1000; // in milliseconds
int maxDelayBetweenEvents = 10000; // in milliseconds
int maxDelayToGetSomePoints = 300; // in milliseconds
int scoreOnError = maxDelayToGetSomePoints;  // in milliseconds, score obtained if click before light/sound
int nbRoundsPerGame = 10; 
int delayForReadingScore = 300; // in milliseconds
int delayForReadingAverageScore = 2000; // in milliseconds

// pins
const int pinSwitch = 2;
const int pinBuz = 1;
const int pinLed = 3;


//--------------------------------------------------------------------------
// Module 7-segments

// Define the Arduino pins connected to the segment inputs
int arduinoPinForSegmentInput[8] = 
     { 10, 12, 13, 0, 11, 6, 7, 4 };
  // { A, B, C, D, E, F, G, DP }
  // { 2, 3, 4, 5, 6, 7, 8, 9 }

// Define digits to be displayed on the 7-segments.
// Each row gives the state of: { A, B, C, D, E, F, G }
int segmentDigits[][7] = {
  { 1,1,1,1,1,1,0 },  // 0
  { 0,1,1,0,0,0,0 },  // 1
  { 1,1,0,1,1,0,1 },  // 2
  { 1,1,1,1,0,0,1 },  // 3
  { 0,1,1,0,0,1,1 },  // 4
  { 1,0,1,1,0,1,1 },  // 5
  { 1,0,1,1,1,1,1 },  // 6
  { 1,1,1,0,0,0,0 },  // 7
  { 1,1,1,1,1,1,1 },  // 8
  { 1,1,1,0,0,1,1 },  // 9
  { 0,0,0,1,0,0,1 },  // '=' (10)
  { 1,0,0,1,0,0,0 },  // (bottom bar and top bar) (11)
  // can add other characters here
};

// Function to call in order to configure the output pins
void segmentSetup() {
  for (int i = 0; i < 8; i++) 
    pinMode(arduinoPinForSegmentInput[i], OUTPUT);
}

// Write a state to a given segment pin: state = 0 or 1
// -- do not call this function directly
void segmentWrite(int pin, boolean state) { 
  digitalWrite(pin, (state) ? HIGH : LOW);
  // for an anode instead of a cathode, replace the above with:
  // digitalWrite(pin, (state) ? LOW : HIGH);
}

// Clear all the segments, including the decimal point
void segmentClear() {
 for (int i = 0; i < 8; i++)
    segmentWrite(arduinoPinForSegmentInput[i], 0);
}

// Set the display for the decimal point: state = 0 or 1
void segmentSetDot(boolean state) { 
  segmentWrite(arduinoPinForSegmentInput[7], state);
}

// Display on the segment the character associated with the
// given code, as given by the array "segmentDigits";
// For a digit (0-9), give the value of the digit as charCode
void segmentDisplay(int charCode) {
  for (int i = 0; i < 7; i++)
    segmentWrite(arduinoPinForSegmentInput[i], segmentDigits[charCode][i]);
}


//--------------------------------------------------------------------------
// Module SWITCH

void switchSetup() {
  pinMode(pinSwitch, INPUT);
}

boolean switchPressed = false; // do not access directly

boolean switchIsPressed() {
  return switchPressed;
}
  
boolean switchWasJustPressed() {
  if (digitalRead(pinSwitch) == LOW) {
    if (!switchPressed) {
      // upon transition from HIGH to LOW
      switchPressed = true;
      return true;    
    } else {
      return false;
    }
  } else { // if (digitalRead(pinSwitch) == HIGH)
    if (switchPressed)
      switchPressed = false;
    return false;
  }
}

//--------------------------------------------------------------------------

// internal variables
long dateEvent;
boolean eventOccured;
long nbScores;
long totalScores;

void setup() {                
  Serial.begin(9600);
  pinMode(pinBuz, OUTPUT); 
  pinMode(pinLed, OUTPUT); 
  switchSetup();
  segmentSetup();
  startNewGame();
}

void startNewGame() {
  nbScores = 0;
  totalScores = 0;
  bipNbTimes(3); // triple bit to start the game
  chooseDateNextEvent();
}

void chooseDateNextEvent() {
  dateEvent = millis() + random(minDelayBetweenEvents, maxDelayBetweenEvents);
  eventOccured = false;
}

void showScoreOnSegment(int score) {
  // The goal is to convert a number of milliseconds into a single digit (0-9),
  // and an on-off decimal point which we use to mean "plus a half".
  // The "gap" is the number of milliseconds between getting (e.g.) score 5 and score 6.
  int gap = maxDelayToGetSomePoints / 10;
  // default is the worst score (9.5)
  int digit = 9;
  int dot = true;
  if (score < maxDelayToGetSomePoints) {
    digit = score / gap; // main digit, a score between 0 and 9
    dot = ((score % gap) >= gap / 2); // "plus one half"
  }
  // handle potential errors
  if (digit < 0 || digit > 9) {
    segmentDisplay(11); // display top bar and bottom bar
    return;
  }
  segmentDisplay(digit);
  segmentSetDot(dot);
}

void reportScore(int score) {
  nbScores++;
  totalScores += score;
  showScoreOnSegment(score);
  Serial.println(score); // in milliseconds
}
  
void bipNbTimes(int nb) {
  for (int i = 0; i < nb; i++) {
    digitalWrite(pinBuz, HIGH);
    delay(100);
    digitalWrite(pinBuz, LOW);  
    delay(100);
  }  
}
  
void loop() { 
  
  if (switchWasJustPressed()) {
    
    // handle clicks
    long now = millis();    
    digitalWrite(pinBuz, LOW);  
    digitalWrite(pinLed, LOW);      
      // for debug: Serial.println(String("now: ") + String(now));
      // for debug: Serial.println(String("date: ") + String(dateEvent));      
    if (now < dateEvent) {
      
      // pressed before the event: 
      bipNbTimes(2);
      reportScore(scoreOnError);
      
    } else {
      
      // pressed after the event    
      int duration = now - dateEvent;
      reportScore(duration);
      
    }
    // allow for some time to read the score being displayed
    delay(delayForReadingScore);
    segmentClear();

    if (nbScores < nbRoundsPerGame) {
      // schedule a new event
      chooseDateNextEvent();
    } else { 
      int averageScore = totalScores / nbScores;
      segmentDisplay(10); // display character '='
      delay(delayForReadingScore);
      showScoreOnSegment(averageScore);
      Serial.println(String("---average: ") + averageScore); // in milliseconds  
      delay(delayForReadingAverageScore);
      segmentClear();
      startNewGame();
    } 
    
  } else {
    
    if (!eventOccured && millis() > dateEvent) {
      // perform event if it's time to do so,
      // randomly choosing between LED or Buzzer
      eventOccured = true;
      boolean actionCode = (dateEvent % 2); 
      if (actionCode == 0) {
        digitalWrite(pinLed, HIGH);   
      } else {
        digitalWrite(pinBuz, HIGH);      
      }
    }
  }
}
