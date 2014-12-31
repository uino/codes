/**

 * ......TODO

 * AC_ADXL335 library for reading the ADXL335 Accelerometer.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 * Adapted from code by Derek Chafin (released into the public domain).
 *
 */


#ifndef AC_ADXL335_h
#define AC_ADXL335_h

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class AC_ADXL335
{
  public:
    AC_ADXL335(int pin_x, int pin_y, int pin_z, float aref);
    void setThreshold(float deadzone);
    boolean getFreefall();
    float getX();
    float getY();
    float getZ();
    float getRho();
    float getPhi();
    float getTheta();
    void update();
  private:
    float geta2d(float gx, float gy);
    float geta3d(float gx, float gy, float gz);
    void processDeadzone(float* gv);
    float getGravity(int reading);
    float _getRho(float ax, float ay, float az);
    float _getPhi(float ax, float ay, float az);
    float _getTheta(float ax, float ay, float az);
    int _pin_x;
    int _pin_y;
    int _pin_z;
    float _bias;
    float _xg;
    float _yg;
    float _zg;
    float _aref;
    float _mvG;
    float _deadzone;
    float _rad2deg;
};
#endif
