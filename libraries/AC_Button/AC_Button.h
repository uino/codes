/**
 * AC_Button library.
 * Code by Arthur Chargueraud
 * Copyleft code.
 *
 * This library can be used to control a button and make
 * the distinction between short and long click, for a 
 * specified threshold on the duration of the click.
 *
 * The library can either be queried explicitly for state,
 * or one can attach a callback function to handle events,
 * and call a polling function on a regular basis.
 *
 * // TODO: add support for double clicks
 */

#ifndef AC_Button_h
#define AC_Button_h

#include <Arduino.h>

typedef void (*eventHandler)(void);

class AC_Button
{
  public:
    const int NEVER = -1;

    /** 
      * Construct a new buttonLong device given the pin of the button
      */
    AC_Button(int inputPin);

    /** 
      * To be called from the main setup before the device can be used.
      */
    void begin();

    /** 
      * Configure the "long period duration", i.e. the threshold delay for 
      * clicks considered to be long. Value should be expressed in milliseconds. 
      * Default value is 200ms.
      */
    void setLongPeriodDuration(long longPeriodDuration);

    /** 
      * This function must be called very frequently for checking
      * activity on the button
      */
    void poll();

    /** 
      * Returns whether the button was down at the last call to "poll"
      */
    boolean isDown();

    /** 
      * Returns the date (as returned by millis()) at which the 
      * button last went from up to down.
      * Returns NEVER if the button was never down.
      */
    long downSince();

    /** 
      * Returns the number of milliseconds for which the button has
      * been down during the last phase when it was down.
      * Return NEVER if it has never been down.
      */
    long downDuration();

    /** 
      * Register a function to be called when button goes down.
      * Provide NULL to unregister any previously-registered handler.
      */
    void onDown(eventHandler handler);

    /** 
      * Register a function to be called when button has been down
      * for a period of time longer than the "long period duration".
      * Provide NULL to unregister any previously-registered handler.
      */
    void onDownLong(eventHandler handler);

    /** 
      * Register a function to be called when button goes up,
      * except if the button has been down for longer than 
      * longPeriodDuration and a function was registered using
      * onUpLong.
      * Provide NULL to unregister any previously-registered handler.
      */
     void onUp(eventHandler handler);
    
    /** 
      * Register a function to be called when button goes up
      * after being down for longer than the "long period duration".
      * Provide NULL to unregister any previously-registered handler.
      */
     void onUpLong(eventHandler handler);
   
  private:
    typedef enum { UP, DOWN, LONGDOWN } Status;

    int inputPin;
    int longPeriodDuration;
    Status status;
    long downSinceValue;
    long downDurationValue;
    eventHandler downHandler;
    eventHandler downLongHandler;
    eventHandler upHandler;
    eventHandler upLongHandler;
};

#endif
