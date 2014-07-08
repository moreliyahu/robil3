/*
 * InverseKinematics.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: lab116-2
 */

#include "InverseKinematics.h"
#include <ros/ros.h>

#define q30 -1.55321
#define q50  -1 //-2.9078
#define phi (M_PI / 6.0)
#define alpha 0.2874
#define ksi 0.0


#define WRAP_POSNEG_PI(x) atan2(sin(x), cos(x))

#define LIMIT(value, minim, maxim) std::max<double>(std::min<double>(value, maxim), minim)

#define safe_asin(x) asin(LIMIT(x, -1, 1))
#define safe_acos(x) acos(LIMIT(x, -1, 1))
#define safe_pow(x, y) ((fabs(y - 0.5) < 0.001) ? sqrt(fabs(x)) : pow(x, y))

namespace InverseKinematics{

	double SupporterInv(double q3){
		return 2.6129*sin(
						safe_acos(
							0.64094 - 1.0372*cos(
												q3 + q30
												)
							) - 0.2874
						)
						*
						sin(
							phi + safe_asin(
									(1.7323*safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2)))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2))) + safe_asin((1.0486*sin(q3 + q30))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2))))*(((1.7787*sin(q3 + q30)*safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2)))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(3.0/2)) + (1.7967*sin(q3 + q30)*(1.0372*cos(q3 + q30) - 0.64094))/(safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2))*safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2))))/safe_pow(((3.0009*(safe_pow((1.0372*cos(q3 + q30) - 0.64094),2) - 1.0))/(2.0536*cos(q3 + q30) + 2.0584) + 1.0),(1.0/2)) + ((1.0486*cos(q3 + q30))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2)) + (1.0767*safe_pow(sin(q3 + q30),2))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(3.0/2)))/safe_pow((1.0 - (1.0996*safe_pow(sin(q3 + q30),2))/(2.0536*cos(q3 + q30) + 2.0584)),(1.0/2))) - 2.6129*cos(safe_acos(0.64094 - 1.0372*cos(q3 + q30)) - 0.2874)*cos(phi + safe_asin((1.7323*safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2)))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2))) + safe_asin((1.0486*sin(q3 + q30))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2))))*(((1.7787*sin(q3 + q30)*safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2)))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(3.0/2)) + (1.7967*sin(q3 + q30)*(1.0372*cos(q3 + q30) - 0.64094))/(safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2))*safe_pow((2.0536*cos(q3 + q30) + 2.0584),(1.0/2))))/safe_pow(((3.0009*(safe_pow((1.0372*cos(q3 + q30) - 0.64094),2) - 1.0))/(2.0536*cos(q3 + q30) + 2.0584) + 1.0),(1.0/2)) + ((1.0486*cos(q3 + q30))/sqrt((2.0536*cos(q3 + q30) + 2.0584)) + (1.0767*safe_pow(sin(q3 + q30),2))/safe_pow((2.0536*cos(q3 + q30) + 2.0584),(3.0/2)))/sqrt((1.0 - (1.0996*safe_pow(sin(q3 + q30),2))/(2.0536*cos(q3 + q30) + 2.0584)))) - (0.59273*cos(safe_acos(0.64094 - 1.0372*cos(q3 + q30)) - 0.2874)*sin(q3 + q30))/sqrt((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2))) + (2.71*cos(safe_acos(0.64094 - 1.0372*cos(q3 + q30)) - 0.2874)*cos(phi + safe_asin((1.7323*safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2)))/sqrt((2.0536*cos(q3 + q30) + 2.0584))) + safe_asin((1.0486*sin(q3 + q30))/sqrt((2.0536*cos(q3 + q30) + 2.0584))))*sin(q3 + q30))/safe_pow((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)),(1.0/2)) - (2.71*sin(safe_acos(0.64094 - 1.0372*cos(q3 + q30)) - 0.2874)*sin(phi + safe_asin((1.7323*sqrt((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2))))/sqrt((2.0536*cos(q3 + q30) + 2.0584))) + safe_asin((1.0486*sin(q3 + q30))/sqrt((2.0536*cos(q3 + q30) + 2.0584))))*sin(q3 + q30))/sqrt((1.0 - 1.0*safe_pow((1.0372*cos(q3 + q30) - 0.64094),2)));
	}

	double LoaderInv(double q3, double desired_pitch){

		double cosq3_q30 = cos(q3 + q30);
		double sinq3_q30 = sin(q3 + q30);


		ROS_INFO("1) q3 = %f",q3);
		ROS_INFO("2) desired_pitch = %f",desired_pitch);
		ROS_INFO("3) inv: cosq3_q30: %f; sinq3_q30: %f", cosq3_q30, sinq3_q30);

		double q1 = acos(0.64094 - 1.0372*cosq3_q30);
		double q2 = asin(LIMIT(
							(1.7323*sqrt(
										1.0 - pow(1.0372*cosq3_q30 - 0.64094,2)
										)
							)
							/
							sqrt(2.0536*cosq3_q30 + 2.0584)
							, -1, 1)
						)
						+
					asin(LIMIT(1.0486*sinq3_q30
						/
						sqrt(2.0536*cosq3_q30 + 2.0584)
						, -1, 1)
					);
		double q5 = desired_pitch - ( -q1 + alpha - M_PI - q2 - phi - q50);
		q5 = WRAP_POSNEG_PI(q5);//fmod(q5 + M_PI ,2*M_PI) - M_PI;
		double pitch = -(q1 + alpha) + M_PI + q2 + phi +q50 +q5;
		pitch = WRAP_POSNEG_PI(pitch);//fmod(pitch + M_PI ,2*M_PI) - M_PI;


		ROS_INFO("4) pitch = %f",pitch);
		ROS_INFO("5) q1 = %f",q1);
		ROS_INFO("6) q2 = %f",q2);
		ROS_INFO("7) q5 = %f",q5);


		return 0;
	}

}
