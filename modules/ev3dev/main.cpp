#include <stdio.h>
#include "core/time.h"
#include "core/object.h"
#include "core/native/general.h"
#include "core/package/load.h"
#include "motor.h"

#define READ_BUFFER_SIZE 50

using namespace ink;

Ink_ModuleID ink_native_ev3dev_mod_id;

inline string intToString(iev3_SensorID id)
{
	char buffer[READ_BUFFER_SIZE];
	size_t i;
	
	i = sprintf(buffer, "%ld", (long)id);
	if (i >= 0) {
		buffer[i] = '\0';
	} 

	return string(buffer);
}

inline int writeToMotorFile(iev3_MotorID id, const char *file, const char *value)
{
	FILE *fp = fopen((string(IEV3_MOTOR_PATH) + intToString(id) + "/" + file).c_str(), "w");
	return fputs(value, fp);
}

inline string readFromMotorFile(iev3_MotorID id, const char *file)
{
	FILE *fp = fopen((string(IEV3_MOTOR_PATH) + intToString(id) + "/" + file).c_str(), "r");
	char buffer[READ_BUFFER_SIZE];

	fgets(buffer, READ_BUFFER_SIZE, fp);

	return string(buffer);
}

inline void wait_for_stop(iev3_MotorID id)
{
	string tmp;
	while (1) {
		tmp = readFromMotorFile(id, "state");
		printf("%s\n", tmp.c_str());
		if (tmp.find_first_of("running") == string::npos)
			break;
	}
	return;
}

inline int writeMotorCommand(iev3_MotorID id, const char *cmd)
{
	return writeToMotorFile(id, "command", cmd);
}

inline string readFromSensorFile(iev3_SensorID id, const char *file)
{
	FILE *fp = fopen((string(IEV3_SENSOR_PATH) + intToString(id) + "/" + file).c_str(), "r");
	char buffer[READ_BUFFER_SIZE];

	fgets(buffer, READ_BUFFER_SIZE, fp);

	return string(buffer);
}

Ink_Object *InkMod_ev3dev_Motor_RunRelat(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 3, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	iev3_MotorID id = as<Ink_Numeric>(argv[0])->value;
	iev3_Motor_Speed sp = as<Ink_Numeric>(argv[1])->value;
	iev3_Motor_Position pos = as<Ink_Numeric>(argv[2])->value;

	string tmp_sp = intToString(sp);
	string tmp_pos = intToString(pos);

	writeToMotorFile(id, IEV3_MOTOR_DUTY_CYCLE_SP, tmp_sp.c_str());
	writeToMotorFile(id, IEV3_MOTOR_POSITION_SP, tmp_pos.c_str());
	writeMotorCommand(id, IEV3_MOTOR_COMMAND_RUN_RELAT);

	return TRUE_OBJ;
}

void InkMod_ev3dev_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("run_relat", new Ink_FunctionObject(engine, InkMod_ev3dev_Motor_RunRelat));
}

Ink_Object *InkMod_ev3dev_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];
	Ink_Object *ev3dev_pkg = addPackage(engine, apply_to, "ev3dev", new Ink_FunctionObject(engine, InkMod_ev3dev_Loader));

	InkMod_ev3dev_bondTo(engine, ev3dev_pkg);

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		addPackage(engine, context, "ev3dev", new Ink_FunctionObject(engine, InkMod_ev3dev_Loader));

		return;
	}

	int InkMod_Init(Ink_ModuleID id)
	{
		ink_native_ev3dev_mod_id = id;
		return 0;
	}
}
