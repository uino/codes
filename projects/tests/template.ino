

//----------------------------------------------------------------------------

/**
 * Serial
 */

void serial_setup() {
  Serial.begin(115200); 
}

//----------------------------------------------------------------------------

void setup() {
  serial_setup();

  Serial.println("Starting up");
}

//----------------------------------------------------------------------------
