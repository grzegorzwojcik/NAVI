/*
 * MADGWICK.h
 *
 *  Created on: Jul 9, 2015
 *      Author: Grzegorz WÓJCIK
 *
 *      Filter based on "An effcient orientation filter for inertial and
 *										inertial/magnetic sensor arrays"
 *		by Sebastian O.H. Madgwick.
 */

#ifndef MADGWICK_H_
#define MADGWICK_H_

// Math library required for ‘sqrt’
#include <math.h>
// System constants
#define deltat 0.001f // sampling period in seconds (shown as 1 ms)
#define gyroMeasError 3.14159265358979f * (5.0f / 180.0f) // gyroscope measurement error in rad/s (shown as 5 deg/s)
#define beta sqrt(3.0f / 4.0f) * gyroMeasError // compute beta
// Global system variables
float a_x, a_y, a_z; // accelerometer measurements
float w_x, w_y, w_z; // gyroscope measurements in rad/s
float SEq_1 = 1.0f, SEq_2 = 0.0f, SEq_3 = 0.0f, SEq_4 = 0.0f; // estimated orientation quaternion elements with initial conditions



#endif /* MADGWICK_H_ */
