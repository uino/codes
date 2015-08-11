/**
 * AC_HMC5883L library for controlling a HMC5883L 3-axis magnetometer.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Adapted from code by http://www.adafruit.com/products/1746.
 * 
 *
 * There are a couple important things to know for using a magnetometer.
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
 *
 *
 *          WARNING: experimental implementation, absolute values might
 *                   be completely wrong (however, heading should work).
 *
 *
 * Pins: (connection I2C protocol at address 0x1E)
 * - SDA : A4
 * - SCL : A5 
 * - DRDY pin (data ready interrupt) is not used. 
 */

#ifndef AC_HMC5883L_h
#define AC_HMC5883L_h

#include <Arduino.h>
#include <Wire.h>
#include <AC_Vector.h>

class AC_HMC5883L
{
  public:

  /** 
    * Construct a new device given the Serial port connected to the hardware.
    */
  AC_HMC5883L();

  /** 
    * Set the offset that applies to the measures.
    */
  void setOffset(Vector offset);
  void setOffset(float x, float y, float z);

  /** 
    * Set the scaling factor that applies to the measures.
    */
  void setScale(Vector scale);
  void setScale(float x, float y, float z);

  /** 
    * Get the offset that applies to the measures.
    */
  Vector getOffset();

  /** 
    * Get the scale that applies to the measures.
    */
  Vector getScale();

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
    GAIN_0_8 = 0x00,  // +/- 0.88
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
    * Get the value of the current gain parameter set for the device. 
    */
  GainMode getGainMode();

  /** 
    * To be called from the main setup before the device can be used.
    * Optionally set up the gain; Default value is GAIN_1_3.
    */
  void begin();
  void begin(GainMode gainMode);

  /** 
    * Measure the magnetic field in the 3 axes and store the results.
    * Use the "get" functions to obtain the results;
    */
  void update();

  /** 
    * Get the norm, i.e. the strength, of the magnetic field. Unit is Gauss.
    * Function update must be called first.
    */
  float getNorm();

  /** 
    * Get the norm of the magnetic field ignoring the Z-axis component,
    * that is, only the norm of the vector projected in the XY plane.
    * Function update must be called first.
    */
  float getNormXY();

  /** 
    * Get the heading angle, and return result in radians,
    * assuming the z-axis to be perfectly vertical.
    * Returns a value in the range [0,2*PI).
    * Note that results can be inaccurate if both the x-axis
    * the y-axis components have relatively small values.
    * Function update must be called first.
    */
  float getHeading();

  /** 
    * Get the 3 components of the measure at once.
    * Function update must be called first.
    */
  Vector getVector();

  /** 
    * Get the X-axis component of the measure. Unit is Gauss.
    * Function update must be called first.
    */
  float getX();

  /** 
    * Get the Y-axis component of the measure. Unit is Gauss.
    * Function update must be called first.
    */
  float getY();

  /** 
    * Get the Z-axis component of the measure. Unit is Gauss.
    * Function update must be called first.
    */
  float getZ();

  /** 
    * Begin a calibration phase for use as a compass; 
    * device should be horizontal. The function "stepCalibration"
    * should be called with the device in all possible angles,
    * then "endCalibration" should be called at the end.
    */
  void beginCalibration();
  
  /** 
    * End the calibration phase and set the offset and the scale
    * parameters accordingly.
    */
  void endCalibration();

  /** 
    * The function stepCalibration should be called during calibration
    * phase, in various horizontal positions of the device.
    */
  void stepCalibration();

   /** 
    * Function to perform a measure and return the raw output of the device.
    * In general, prefer using "update" and the accessor function.
    */
  Vector measureRawVector();

   /** 
    * Function to perform a measure and return the raw output of the device
    * adjusted using the calibration that was registered in the device.
    */
  Vector measureCalibratedVector();

   /** 
    * Function to perform a measure and return the value, once adjusted for
    * calibration and gain. This is the value stored in the measure field
    * when "update" is called.
    */
  Vector measureGainAdjustedVector();

   /** 
    * Return the conversion value "steps per Gauss" that applies for a given
    * gain Mode.
    */
  float gainForMode(GainMode gainMode);

//---------------------------------------------------------------------

private:
  static const byte deviceAddress = 0x1E; //0011110b, I2C 7bit address of HMC5883
  Vector offset;
  Vector scale;
  float declination;
  GainMode gainMode;
  float gain; // deduced from gainMode
  Vector measure; // last measure performed

  /** 
    * Registers
    */
  // TODO: move to CPP file.
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


public: // should only access this field for debugging
  Vector minv, maxv; // raw measure extrema, during calibration

};

#endif
