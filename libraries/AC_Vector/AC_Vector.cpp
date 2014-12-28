
#include "AC_Vector.h"

Vector::Vector() {
  x = 0;
  y = 0;
  z = 0; 
}

Vector::Vector(float x, float y, float z) {
  this->x = x;
  this->y = y;
  this->z = z; 
}

Vector Vector:: operator + (Vector other) {
  return Vector(x + other.x, y + other.y, z + other.z);
}

Vector Vector:: operator - (Vector other) {
  return Vector(x - other.x, y - other.y, z - other.z);
}

Vector Vector:: operator * (float k) {
  return Vector(x * k, y * k, z * k);
}

float Vector::norm() {
  return sqrt(sq(x) + sq(y) + sq(z));
}

float Vector::phi() {
  return atan2(y, x);
}

float Vector::theta() {
  float p = sqrt(sq(x) + sq(y));
  return atan2(z, p);
}

float Vector::distTo(Vector other) {
  return (*this - other).norm();
}
