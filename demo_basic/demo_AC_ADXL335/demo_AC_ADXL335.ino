/*

// ......... TODO ..........


AC_ADXL335 Accelerometer Demo
Derek Chafin
September 22, 2011
Public Domain

Demonstrates how to use the AC_ADXL335 library

Pin Connections:

analog 0: connected to x-axis
analog 1: connected to y-axis
analog 2: connected to z-axis
aref: connected to +3.3V or 5V 
*/

#include <AC_ADXL335.h>

const int pin_x = A0;
const int pin_y = A1;
const int pin_z = A2;
const float aref = 5; // 3.3;
AC_ADXL335 accel(pin_x, pin_y, pin_z, aref);

void setup()
{
  Serial.begin(9600);
  
  Serial.println("X,\tY,\tZ,\tRho,\tPhi,\tTheta");
}


//this function was taken from my format float library
String formatFloat(double value, int places, int* string_width)
{
  //if value is positive infinity
  if (isinf(value) > 0)
  {
    return "+Inf";
  }
    
  //Arduino does not seem to have negative infinity
  //keeping this code block for reference
  //if value is negative infinity
  if(isinf(value) < 0)
  {
    return "-Inf";
  }
  
  //if value is not a number
  if(isnan(value) > 0)
  {
    return "NaN";
  }
  
  //always include a space for the dot
  int num_width = 1;

  //if the number of decimal places is less than 1
  if (places < 1)
  {
    //set places to 1
    places = 1;
    
    //and truncate the value
    value = (float)((int)value);
  }
  
  //add the places to the right of the decimal
  num_width += places;
  
  //if the value does not contain an integral part  
  if (value < 1.0 && value > -1.0)
  {
    //add one for the integral zero
    num_width++;
  }
  else
  {

    //get the integral part and
    //get the number of places to the left of decimal
    num_width += ((int)log10(abs(value))) + 1;
  }
  //if the value in less than 0
  if (value < 0.0)
  {
    //add a space for the minus sign
    num_width++;
  }
  
  //make a string the size of the number
  //plus 1 for string terminator
  char s[num_width + 1]; 
  
  //put the string terminator at the end
  s[num_width] = '\0';
  
  
  //initalize the array to all zeros
  for (int i = 0; i < num_width; i++)
  {
    s[i] = '0';
  }
  
  //characters that are not changed by 
  //the function below will be zeros
  
  //set the out variable string width
  //lets the caller know what we came up with
  *string_width = num_width;
  
  //use the avr-libc function dtosrtf to format the value
  return String(dtostrf(value,num_width,places,s));  
}

void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}

void loop()
{
  // Display raw
  int x = analogRead(pin_x);
  int y = analogRead(pin_y);
  int z = analogRead(pin_z);
  displayFloat(x, 6, 1);
  Serial.print(", ");
  displayFloat(y, 6, 1);
  Serial.print(", ");
  displayFloat(z, 6, 1);
  Serial.println("");


/*
  //this is required to update the values
  accel.update();
  
  //this tells us how long the string is
  int string_width;

  float x;
  float y;
  float z;
  
  //for these variables see wikipedia's
  //definition of spherical coordinates
  float rho;
  float phi;
  float theta;  
  
  x = accel.getX();
  y = accel.getY();
  //if the project is laying flat and top up the z axis reads ~1G
  z = accel.getZ();
  rho = accel.getRho();
  phi = accel.getPhi();
  theta = accel.getTheta();
  
  Serial.print(formatFloat(x, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(y, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(z, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(rho, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(phi, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(theta, 3, &string_width));
  Serial.println("");
  */
  delay(200);
}







/*
  //convert read values to degrees -90 to 90 - Needed for atan2
  int xAng = map(xRead, minVal, maxVal, -90, 90);
  int yAng = map(yRead, minVal, maxVal, -90, 90);
  int zAng = map(zRead, minVal, maxVal, -90, 90);

  //Caculate 360deg values like so: atan2(-yAng, -zAng)
  //atan2 outputs the value of -π to π (radians)
  //We are then converting the radians to degrees
  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);


  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" | y: ");
  Serial.print(y);
  Serial.print(" | z: ");
  Serial.println(z);

  float norm = x*x + y*y + z*z;
  Serial.println(norm);

*/


/*
AC_ADXL335 Accelerometer Demo

Demonstrates how to use the AC_ADXL335 library

Pin Connections:

analog 0: connected to x-axis
analog 1: connected to y-axis
analog 2: connected to z-axis
aref: connected to +3.3V or 5V 
*/

/*
#include <AC_ADXL335.h>

const int pin_x = A0;
const int pin_y = A1;
const int pin_z = A2;
const float aref = 5; // 3.3;
AC_ADXL335 accel(pin_x, pin_y, pin_z, aref);

void setup()
{
  Serial.begin(9600);
  
  Serial.println("X,\tY,\tZ,\tRho,\tPhi,\tTheta");
}
*/

/*
void displayFloat(float value, int nbChars, int precision) {
  // using: void dtostrf(float value, int nbChars, int precision, char* target) 
  const int nbDigits = 10; // >= nbChars
  char buffer[nbDigits];
  dtostrf(value, nbChars, precision, buffer);
  Serial.print(buffer);
}

void loop()
{
  // Display raw
  int x = analogRead(pin_x);
  int y = analogRead(pin_y);
  int z = analogRead(pin_z);
  displayFloat(x, 6, 1);
  Serial.print(", ");
  displayFloat(y, 6, 1);
  Serial.print(", ");
  displayFloat(z, 6, 1);
  Serial.println("");
*/

/*
  //this is required to update the values
  accel.update();
  
  //this tells us how long the string is
  int string_width;

  float x;
  float y;
  float z;
  
  //for these variables see wikipedia's
  //definition of spherical coordinates
  float rho;
  float phi;
  float theta;  
  
  x = accel.getX();
  y = accel.getY();
  //if the project is laying flat and top up the z axis reads ~1G
  z = accel.getZ();
  rho = accel.getRho();
  phi = accel.getPhi();
  theta = accel.getTheta();
  
  Serial.print(formatFloat(x, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(y, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(z, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(rho, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(phi, 3, &string_width));
  Serial.print(",\t");
  Serial.print(formatFloat(theta, 3, &string_width));
  Serial.println("");
  */
