/*
 * Madgwick.h
 *
 *  Created on: Oct 14, 2026
 *      Author: Asus - TYF
 */

#ifndef MADGWICK_H_
#define MADGWICK_H_

extern float Roll, Pitch, Yaw;

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

#endif /* MADGWICK_H_ */
