/**
 * AC_HMC5883 library.
 * Code by Arthur Chargueraud
 * Adpated from http://www.adafruit.com/products/1746.
 * This code is GPL.
 *
 * This library can be used to control a HMC5883 3-axis magnetometer.
 *
 * There are a couple important things to know for using it.
 * 
 * First, you need the compass to be perfectly horizontal, because the
 * Z-axis component of the earth magnetic field is significant compared
 * with the bias it has towards the magnetic North.
 *
 * Second, you need to adjust for magnetic declination, due to the fact
 * that the magnetic North does not point exactly to the North pole.
 * Obtain your local declication from http://www.magnetic-declination.com/
 * and set it using the interface below before reading angles.
 *
 * Third, the magnetometer device needs to be adjusted for "gain" before
 * it can be used in a strong field, such as that created by a magnet.
 * See the interface below for selecting a gain. 
 * Recall that 1 Gauss unit equals 100 micro-Teslas.
 */

#ifndef AC_HMC5883_h
#define AC_HMC5883_h

#include <Arduino.h>
#include <Wire.h>

class AC_HMC5883
{
  public:

    /** 
      * Construct a new device given the Serial port connected to the hardware.
      */
    AC_HMC5883();

    /** 
      * Set the declination that applies to the angle measures.
      * Angle should be provided in radians, in the range (-PI,PI].
      */
    void setDeclination(float angle);

    /** 
      * Set the declination that applies to the angle measures.
      * Angle is here provided in degrees and minutes.
      */
    void setDeclinationDegrees(float angleInDegrees);
    void setDeclinationDegrees(float angleInDegrees, float minutes);

    /** 
      * Gain mode 
      */
    typedef enum {
      GAIN_1_3 = 0x20,  // +/- 1.3
      GAIN_1_9 = 0x40,  // +/- 1.9
      GAIN_2_5 = 0x60,  // +/- 2.5
      GAIN_4_0 = 0x80,  // +/- 4.0
      GAIN_4_7 = 0xA0,  // +/- 4.7
      GAIN_5_6 = 0xC0,  // +/- 5.6
      GAIN_8_1 = 0xE0   // +/- 8.1
    } GainMode;	

    /** 
      * Set the gain parameter of the device. Default is GAIN_1_3.
      */
    void setGainMode(GainMode gainMode);

    /** 
      * To be called from the main setup before the device can be used.
      * Optionally set up the gain; Default value is GAIN_1_3.
      */
    void begin();
    void begin(GainMode gainMode);

    /** 
      * Measure the magnetic field in the 3 axes and store the results.
      * Use the "get" functions to obtain the results
      */
    void makeMeasure();

    /** 
      * Get the norm, i.e. the strength, of the magnetic field. Unit is Gauss.
      */
    float getNorm();

    /** 
      * Get the heading angle, and return result in radians,
      * assuming the z-axis to be perfectly vertical.
      * Returns a value in the range [0,2*PI).
      * Note that results can be inaccurate if both the x-axis
      * the y-axis components have relatively small values.
      */
    float getHeading();

    /** 
      * Get the heading angle, and return result in degrees,
      * assuming the z-axis to be perfectly vertical.
      * Returns a value in the range [0,360), up to rounding errors.
      */
    float getHeadingDegrees();

    /** 
      * Representation of a 3D vector of Gauss values.
      */
    typedef struct { float x, y, z; } Vector;

    /** 
      * Get the 3 components of the measure at once.
      */
    Vector getMeasure();

    /** 
      * Get the X-axis component of the measure. Unit is Gauss.
      */
    float getX();

    /** 
      * Get the Y-axis component of the measure. Unit is Gauss.
      */
    float getY();

    /** 
      * Get the Z-axis component of the measure. Unit is Gauss.
      */
    float getZ();

  private:
    const byte deviceAddress = 0x1E; //0011110b, I2C 7bit address of HMC5883
    float declination;
    GainMode gainMode;
    float gainXY, gainZ; // depend on the gainMode
    Vector measure;

    /** 
      * Registers
      */
    typedef enum {
      REGISTER_CRA_REG_M  = 0x00,
      REGISTER_CRB_REG_M  = 0x01,
      REGISTER_MR_REG_M   = 0x02,
      REGISTER_OUT_X_H_M  = 0x03,
      REGISTER_OUT_X_L_M  = 0x04,
      REGISTER_OUT_Z_H_M  = 0x05,
      REGISTER_OUT_Z_L_M  = 0x06,
      REGISTER_OUT_Y_H_M  = 0x07,
      REGISTER_OUT_Y_L_M  = 0x08,
      REGISTER_SR_REG_Mg  = 0x09,
      REGISTER_IRA_REG_M  = 0x0A,
      REGISTER_IRB_REG_M  = 0x0B,
      REGISTER_IRC_REG_M  = 0x0C,
      REGISTER_TEMP_OUT_H_M = 0x31,
      REGISTER_TEMP_OUT_L_M = 0x32
    } Register;

    // auxiliary function for using Wire
    void write8(byte reg, byte value);

    // auxiliary function for setting the gain
    void setGainXYZ();

};

#endif
