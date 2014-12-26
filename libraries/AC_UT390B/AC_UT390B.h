/**
 * AC_UT390B library.
 * Code by Arthur Chargueraud
 * Adpated from http://youtu.be/qUgFji4Figw.
 *
 * This library can be used to control a UT390B telemeter.
 *
 * // TODO: add support for error codes
 */

#ifndef AC_UT390B_h
#define AC_UT390B_h

#include <Arduino.h>

class AC_UT390B
{
  public:

    /** 
      * Construct a new device given the Serial port connected to the hardware.
      */
    AC_UT390B(HardwareSerial* serialPort);

    /** 
      * Set a offset value to be added to all measures.
      * Unit is meter. Negative values are allowed. 
      * Default value is zero.
      */
    void setOffset(float offset);

    /** 
      * Set a timeout before measure is considered to be failed.
      * Value to be expressed in milliseconds.
      * Use zero for no timeout. Default value is zero.
      */
    void setTimeout(int timeout);

    /** 
      * To be called from the main setup before the device can be used.
      */
    void begin();

    /** 
      * Use "setLaserVisible(true)" to turn on the laser continuously.
      * [TODO] it is not yet known how to turn the laser off
      * once it has been turned on.
      * This function must be called after "begin()".
      */
    void setLaserVisible(boolean visible);

    /** 
      * Request a measure. The function "processMeasure" should be
      * called repeatedly until "isMeasureComplete" returns true.
      */
    void requestMeasure();

    /** 
      * Process the data returned by the telemeter. Might need to
      * be called several times before "isMeasureComplete" returns true.
      * It is safe to call this function even when there is no on-going measure.
      */
    void processMeasure();

    /** 
      * Tests whether the measure has completed.
      */
    boolean isMeasureComplete();

    /** 
      * Tests whether the measure was successful,
      * or resulted in an error.
      * When this function returns true, it is guaranteed that 
      * "isMeasureComplete" would have also returned true.
      */
    boolean isMeasureSuccessful();

    /** 
      * Read the distance as a float, representing a number of meters,
      * with precision 10^-4, that is, 0.1 millimeter.
      */
    float getMeasure();


  private:
    HardwareSerial* serialPort;
    int timeout;
    float offset;

    typedef enum { IDLE, ACQUIRE, READY, ERROR } Status;
    Status status;
    float measure;
    long dateRequest;
    static const int bufferMaxLength = 16;
    char buffer[bufferMaxLength+1];
    int bufferPos;
    boolean parsingData;

    /** 
      * Empty the Serial buffer used to communicate with the
      * telemeter is empty, so as to reduce the risk of obtaining
      * deprecated data. However, there is no guarantee that
      * there no data is going to come in.
      */
    void emptySerial();

    /* Auxiliary function used by "processMeasure".
     */
    float getMeasureFromBuffer();
};

#endif
