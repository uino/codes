/**
 * ACbuttonLong library.
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
 */

#ifndef ACbuttonLong_h
#define ACbuttonLong_h

#include <Arduino.h>

typedef void (*eventHandler)(void);

class ACbuttonLong
{
  public:
    const int NEVER = -1;

    /** 
      * Construct a new buttonLong device given the pin of the button
      */
    ACbuttonLong(int inputPin);

    /** 
      * To be called from the main setup before the device can be used.
      */
    void setup();

    /** 
      * Configure the threshold delay for clicks considered long,
      * to be expressed in milliseconds. Default value is 200ms.
      */
    void setLongPeriodDuration(long longPeriodInMillis);

    /** 
      * This function must be called very frequently for checking
      * activity on the button
      */
    void poll();

    /** 
      * Returns whether the button is currently down
      */
    boolean isDown();

    /** 
      * Returns the date (as returned by millis()) since which the 
      * button is down, or NEVER if it is not down.
      */
    long downSince();

    /** 
      * Returns the number of milliseconds for which the button has
      * been down, or NEVER if it is not down.
      */
    long downDuration();

    /** 
      * Register a function to be called when button goes down.
      * Provide NULL to unregister any previously-registered handler.
      */
    void onDown(eventHandler handler);

    /** 
      * Register a function to be called when button goes up,
      * except if the button has been down for longer than 
      * longPeriodDuration and a function was registered using
      * onUpAfterLong.
      * Provide NULL to unregister any previously-registered handler.
      */
     void onUp(eventHandler handler);
    
    /** 
      * Register a function to be called when button goes up
      * after being down for longer than longPeriodDuration
      * Provide NULL to unregister any previously-registered handler.
      */
     void onUpAfterLong(eventHandler handler);

  private:
    int inputPin;
    int longPeriodDuration;
    long dateLastDown;
    eventHandler downHandler;
    eventHandler upHandler;
    eventHandler upAfterLongHandler;
    void debug(String msg);
};

#endif
