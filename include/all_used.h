#ifndef ALL_USED_H
#define ALL_USED_H

#include "main.h"

#define pi 3.14159265


#define limit_to_val(input, val) (abs(input) > (val) ? (val) * sgn(input) : (input))

#define limit_to_val_set(input, val) input = limit_to_val(input, val)

float flmod(float x, float y); // Floating point mod operation
float degToRad(float degrees); // Convert degrees to radians
float radToDeg(float radians); // Convert radians to degrees
float nearestangle(float target_angle, float reference_angle);


#endif
