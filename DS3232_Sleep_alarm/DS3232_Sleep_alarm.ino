/**
 * Demo for the DS3232RTC library. using alarm.
 * Code by Arthur Chargueraud.
 * This code is in the public domain.
 */

#include <DS3232RTC.h> 
#include <Time.h>
#include<Wire.h>

#include <avr/sleep.h>
const byte LED = 6;
const byte VccDS = 7;

void wake ()
{
  // cancel sleep as a precaution
  sleep_disable();
  // must do this as the pin will probably stay low for a while
  detachInterrupt (0);
}  // end of wake

  

DS3232RTC ds3232;

tmElements_t initTime;

void initializeTime() {
  initTime.Second = 9;
  initTime.Minute = 9;
  initTime.Hour = 9;
  initTime.Wday = 1; // day of week, sunday is day 1
  initTime.Day = 9;
  initTime.Month = 9;
  initTime.Year = 9;
  ds3232.write(initTime);
  Serial.println("Time initialized");
}

void printTime(time_t t) {
  Serial.print(year(t)); 
  Serial.print('/');
  Serial.print(day(t));
  Serial.print('/');
  Serial.print(month(t));
  Serial.print(' ');
  Serial.print(hour(t));
  Serial.print(':');
  Serial.print(minute(t));
  Serial.print(':');
  Serial.print(second(t));
  Serial.println(); 
}

void setup()
{
  digitalWrite (2, HIGH);
  pinMode(VccDS, OUTPUT);      //led de puissance et alim horloge
    digitalWrite(VccDS, HIGH);
  
  Serial.begin(9600);   
  Serial.println("Starting up");

  // set a given time into the ds3232
  initializeTime();
  
  int example = 2;
  if (example == 0) { 
    // alarm every seconds
    ds3232.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0, 0);  
  } else if (example == 1) {
    // alarm once per minute when seconds are equal to 14
    ds3232.setAlarm(ALM1_MATCH_SECONDS, 14, 0, 0, 0);  
  } else if (example == 2) {
    // alarm once per minute when seconds are equal to 14, with interrupt
    ds3232.alarmInterrupt(ALARM_1, true); 
    ds3232.setAlarm(ALM1_MATCH_SECONDS, 14, 0, 0, 0);  
  }
  // initialize wave output
  // ds3232.squareWave(SQWAVE_1_HZ);
}



void loop(void)
{
  
  pinMode (VccDS, OUTPUT);
 digitalWrite(VccDS, HIGH);
  
   pinMode (LED, OUTPUT);
 digitalWrite(LED, HIGH);

  
  Serial.print("Date: ");
  printTime(ds3232.get());

  if (ds3232.alarm(ALARM_1)) {
    Serial.println("Alarm 1 triggered");
  }
 
  delay(300);
  Serial.print("Date: ");
  printTime(ds3232.get());

     
 delay(300);

 digitalWrite(LED, LOW);
  pinMode (LED, INPUT);

 digitalWrite(VccDS, LOW);
  pinMode (VccDS, INPUT);
  
  // disable ADC
  ADCSRA = 0;  
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();

  // Do not interrupt before we go to sleep, or the
  // ISR will detach interrupts and we won't wake.
  noInterrupts ();
  
  // will be called when pin D2 goes low  
  attachInterrupt (0, wake, LOW);
 
  // turn off brown-out enable in software
  // BODS must be set to one and BODSE must be set to zero within four clock cycles
  MCUCR = bit (BODS) | bit (BODSE);
  // The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit (BODS); 
  
  // We are guaranteed that the sleep_cpu call will be done
  // as the processor executes the next instruction after
  // interrupts are turned on.
  interrupts ();  // one cycle
  sleep_cpu ();   // one cycle



}
