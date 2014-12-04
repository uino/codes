/**
 * ACrotatingPot library.
 * Code by Arthur Chargueraud
 * Copyleft code.
 *
 * The purpose of this library is to report rotations of
 * a potentiometer, with the specificity that when the
 * potentiometer reaches its minimal or its maximal value,
 * then rotation in the opposite direction is temporarily
 * ignored. This basically allows for using the potentiometer
 * as an infinitely rotating device.
 *
 * The library can either be queried explicitly for state,
 * or one can attach a callback function to handle events,
 * and call a polling function on a regular basis.
 */

#ifndef ACrotatingPot_h
#define ACrotatingPot_h

#include <Arduino.h>

typedef void (*eventHandler)(void);

class ACrotatingPot
{
  public:

    /** 
      * Construct a new rotating potentiometer device given the pin of the potentiometer
      */
    ACrotatingPot(int inputPin);

    /** 
      * Equivalent to ACrotatingPot(inputPin) followed with
      * setSensitivity(sensitivity) and rotationInverted(inverted).
      */
    ACrotatingPot(int inputPin, int sensitivity, boolean inverted);

    /** 
      * To be called from the main setup before the device can be used.
      */
    void setup();

    /** 
      * Call rotationInverted(true) to invert the direction of rotation
      * of the potentiometer
      */
    void rotationInverted(boolean inverted);

    /** 
      * Configure the threshold amount of rotation for stepping to
      * the next value. Must be in interval 1 to 100 (about 1024/10). 
      * Default value is 20.
      */
    void setSensitivity(int sensitivity);

    /** 
      * This function must be called very frequently for checking
      * activity on the button
      */
    void poll();

    /** 
      * Assigns the current value of the rotating potentiometer.
      * New values will be computed relative to this one, based
      * on the rotation performed.
      */
    void resetValue(int newValue);

    /** 
      * Same as resetValue(0).
      */
    void resetValue();

    /** 
      * Returns the current value associated to the device.
      */
    int getValue();

    /** 
      * Register a function to be called when a change in the value
      * is detected. The function can use the function currentValue 
      * to get the new value.
      * Provide NULL to unregister any previously-registered handler.
      */
    void onChange(eventHandler handler);

  private:
    int inputPin;
    int sensitivity;
    boolean inverted;
    eventHandler changeHandler;
   
    int value; // current value of the device
    int valueOffset; // how much was accumulated before moving from lastReference
    int reference; // reference position from which to compute rotations
    int ignoring; // 0 for no direction ignored, +1 or -1 for a particular direction

    int readSensor();
    void debug();
};

#endif
