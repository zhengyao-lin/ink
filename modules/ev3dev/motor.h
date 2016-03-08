#ifndef _MOD_EV3DEV_MOTOR_H_
#define _MOD_EV3DEV_MOTOR_H_

#include <string>
#include "general.h"

using namespace std;

class iev3_Motor {
public:
	iev3_MotorID id;

	iev3_Motor(iev3_MotorID id)
	: id(id)
	{ }


};

#endif
