/*

Seven Segment 
2014

Seven Segment is a program to test a seven-segment digit display.
The program displays digits and special characters every second,
and display the decimal point every other character.

Segments are labelled as follows:

 --A--
 F   B
 --G--
 E   C
 --D-- (DP)       (DP=decimal point)

Modify the code so as to configure which Arduino pin is connected 
to which segment.

Note that the 7-segments usually have its pins labelled as follows:
  A:7, B:6, C:4, D:2, E:1, F:9, 10: G, 5: DP,  ground: 3,8
Don't forget to include resistors for protecting the segment LED.

This code was adapted from:
http://www.hacktronics.com/Tutorials/arduino-and-7-segment-led.html

*/

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
// Demo program

void setup() {    
  segmentSetup();
  segmentClear();
  delay(1000);
}

int steps = 0;

void loop() {
  int nbCharCodes = 12; // number of entries in array segmentDigits
  int charCode = steps % nbCharCodes;
  segmentDisplay(charCode);
  int dotState = (steps % 2 == 0) ? 1 : 0;
  segmentSetDot(dotState);
  steps++;
  delay(1000); 
}

