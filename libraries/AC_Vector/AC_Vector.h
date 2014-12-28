/**
 * AC_Vector library for representing 3D vectors of float values.
 * Code by Arthur Chargueraud.
 * Distributed under LGPL (GNU Lesser General Public License version 3).
 *
 */

#ifndef AC_Vector_h
#define AC_Vector_h

#include <Arduino.h>

class Vector
{
public:
  float x, y, z;

  /*
   * Build a new zero vector.
   */ 
  Vector();

  /*
   * Build a new vector.
   */ 
  Vector(float x, float y, float z);

  /*
   * Return the sum of two vectors.
   */ 
  Vector operator + (Vector other);

  /*
   * Return the difference of two vectors.
   */ 
  Vector operator - (Vector other);

  /*
   * Return the multiplication of a vector by a scalar.
   */ 
  Vector operator * (float k);

  /*
   * Return the norm of the vector.
   */ 
  float norm();

  /*
   * Return the phi-coordinate of the vector 
   * (angle between the x-axis and the projection on the XY-plane).
   * Result value in the range (-PI,+PI].
   */ 
  float phi();

  /*
   * Return the theta-coordinate of the vector 
   * (angle between the vector and the XY-plane).
   * Result value in the range (-PI,+PI].
   */ 
  float theta();

  /*
   * Return the distance between the endpoint of this vector and another one.
   */ 
  float distTo(Vector other);

};

#endif
