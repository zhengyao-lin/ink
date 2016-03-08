#ifndef _MOD_EV3DEV_GENERAL_H_
#define _MOD_EV3DEV_GENERAL_H_

#include <string>
#include <inttypes.h>

#define IEV3_MOTOR_PATH "/sys/class/tacho-motor/motor"
#define IEV3_SENSOR_PATH "/sys/class/lego-sensor/sensor"

#define IEV3_MOTOR_DUTY_CYCLE_SP "duty_cycle_sp"
#define IEV3_MOTOR_POSITION_SP "position_sp"

#define IEV3_MOTOR_COMMAND_RUN_RELAT "run-to-rel-pos"

typedef uint16_t iev3_MotorID;
typedef uint16_t iev3_SensorID;

typedef int8_t iev3_Motor_Speed;
typedef int16_t iev3_Motor_Position;

#endif