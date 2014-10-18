/**
 * ACbuttonLong library.
 * Code by Arthur Charguéraud
 * Copyleft code.
 */

#ifndef ACbuttonLong_h
#define ACbuttonLong_h

#include <Arduino.h>

typedef void (*eventHandler)(void);

class ACbuttonLong
{
  public:
    const int NEVER = -1;

    ACbuttonLong(int buttonPin);

    /** 
      * This function must be called to initialize the device.
      */
    void setup();

    /** 
      * This function must be called very frequently for checking
      * activity on the button
      */
    void poll();

    /** 
      * Configure the threshold delay for clicks considered long,
      * to be expressed in milliseconds. Default value is 200ms.
      */
    void setLongPeriodDuration(long longPeriodInMillis);

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
    int buttonPin;
    int longPeriodDuration;
    long dateLastDown;
    eventHandler downHandler;
    eventHandler upHandler;
    eventHandler upAfterLongHandler;
};

#endif
