#include <cstddef>
#include <cstdint>
#include "hFramework.h"
#include "MotorManager.h"
#include "soft_enc.h"
#include "ServoCtrl.h"
#include "DblMotorCtrl.h"
#include "GripperCtrl.h"
#include "Addons.h"

extern float current[9];
extern float target[9];
Coordinates offset(jointsCo, 0, 0, 0, 0, 0);
float jointTarget[6];
//Coordinates minimum(jointsCo, -180.0, -120.0, -55.0, -150.0, -180.0);
//Coordinates maximum(jointsCo, 180.0, 30.0, 170.0, 105.0, 180.0);

Coordinates minimum(jointsCo, -360.0, -360.0, -360.0, -360.0, -360.0);
Coordinates maximum(jointsCo, 360.0, 360.0, 360.0, 360.0, 360.0);

const float encoder_tics_J1 = 20;
const float encoder_tics_J2 = 18 * 4;
const float encoder_tics_J3 = 20 / 4.355;
const float encoder_tics_J5 = 4.4;
const float encoder_tics_J6 = 0.2;

extern float tempKp;
extern float tempKi;
extern float tempKd;

soft_enc enkoder2(hSens1.pin1, hSens2.pin1);

IServo& s1 = hServoModule.servo1;
IServo& s2 = hServoModule.servo2;
IServo& s3 = hServoModule.servo3;
IServo& s4 = hServoModule.servo4;
IServo& h1 = hServoModule.servo5;

void MotorManagerInitServos()
{
	hServoModule.enablePower();
}

void MotorManagerInitMotors() {}

void MotorManagerInitEncoders()
{
	//J1
	hMot1.setEncoderPolarity(Polarity::Reversed);
	//J22
	hMot2.setEncoderPolarity(Polarity::Reversed);
	//J3
//	enkoder1.init();
//	enkoder1.resetEncoderCnt();
	//J5
	enkoder2.init();
	hSens1.pin1.setIn_pu();
	hSens2.pin1.setIn_pu();
	enkoder2.resetEncoderCnt();
	//J6
	hMot4.setEncoderPolarity(Polarity::Reversed);
}

void MotorManagerInit()
{
	//UART(211100);
	MotorManagerInitEncoders();
	//UART(211200);
	MotorManagerInitServos();
	//UART(211300);
	MotorManagerInitMotors();
}

void MotorManagerUpdateTask()
{
	//UART(212100);
	ServoCtrl J1(s1, 1125, 2, 0.9, 0.4, 0.2, 0.9, 0.4, 0.2, 100, 20, 20); // up/down
	DblMotorCtrl J2(0, 600, 15, 0, 600, 15, 0, 100, 20, 20); // ???
	ServoCtrl J3(s2, 1600, 0, 3, 1.8, 1.5, 2, 1.1, 2, 100, 4, 4); // up/down
	ServoCtrl J5(s3, 1730, 0, 2.4, 0.2, 0.8, 2.8, 0.2, 0.5, 100, 20, 20); // down/up
	ServoCtrl J6(s4, 1470, 7.5, 2, 0, 0.5, 2, 0, 0.5, 100, 20, 20); // ???
	GripperCrtl H1(h1);

	hSens3.pin2.setIn(); //J1
	bool stateP32;
	bool stateP32last;
	float stopInP32;
	hSens3.pin3.setIn(); //J2
	bool stateP33;
	bool stateP33last;
	float stopInP33;
	hSens3.pin4.setIn(); //J3
	bool stateP34;
	bool stateP34last;
	float stopInP34;
	hSens4.pin2.setIn(); //J5
	bool stateP42;
	bool stateP42last;
	float stopInP42;
	hSens4.pin3.setIn(); //J6
	bool stateP43;
	bool stateP43last;
	float stopInP43;
	hSens4.pin4.setIn(); //H1
	bool stateP44;
	bool stateP44last;
	float stopInP44;

	for (;;) {
		// sensor
		current[1] = (float)hMot1.getEncoderCnt() / encoder_tics_J1 + offset.k1;
		current[2] = (float)hMot2.getEncoderCnt() / encoder_tics_J2 + offset.k2;
		current[3] = (float)hMot3.getEncoderCnt() / encoder_tics_J3 + offset.k3;
		current[5] = (float)enkoder2.getEncoderCnt() / encoder_tics_J5 + offset.k4;
		current[6] = (float)hMot4.getEncoderCnt() / encoder_tics_J6 + offset.k5;
		// motion

		stateP32 = hSens3.pin2.read(); //J1
		if (stateP32 && !stateP32last) {
			stopInP32 = current[1];
		}
		if (stateP32) {
			jointTarget[0] = stopInP32;
		}
		stateP32last = stateP32;
		stateP33 = hSens3.pin3.read(); //J2
		if (stateP33 && !stateP33last) {
			stopInP33 = current[2];
		}
		if (stateP33) {
			jointTarget[1] = stopInP33;
		}
		stateP33last = stateP33;
		stateP34 = hSens3.pin4.read(); //J3
		if (stateP34 && !stateP34last) {
			stopInP34 = current[3];
		}
		if (stateP34) {
			jointTarget[2] = stopInP34;
		}
		stateP34last = stateP34;
		stateP42 = hSens4.pin2.read(); //J5
		if (stateP42 && !stateP42last) {
			stopInP42 = current[5];
		}
		if (stateP42) {
			jointTarget[3] = stopInP42;
		}
		stateP42last = stateP42;
		stateP43 = hSens4.pin3.read(); //J6
		if (stateP43 && !stateP43last) {
			stopInP43 = current[6];
		}
		if (stateP43) {
			jointTarget[4] = stopInP43;
		}
		stateP43last = stateP43;
		stateP44 = hSens4.pin4.read(); //H1
		if (stateP44 && !stateP44last) {
			stopInP44 = 0;
		}
		if (stateP44) {
			jointTarget[5] = stopInP44;
		}
		stateP44last = stateP44;


		int t = sys.getRefTime();
		J1.update(-jointTarget[0] - current[1] , t);
		J2.update(jointTarget[1] - current[2] , t);
		J3.update(-jointTarget[2] - current[3] , t);
		J5.update(jointTarget[3] - current[5] , t);
		J6.update(jointTarget[4] - current[6] , t);
		H1.update(jointTarget[5]);

		sys.delay(50);
		LED2.toggle();

		//configuration
		//J2.set_pid_values(tempKp*10,tempKi*10,tempKd*10);
	}
}

void MotorManagerUpdateTargetGlobal()
{
	jointTarget[0] = target[1];
	jointTarget[1] = target[2];
	jointTarget[2] = target[3];
	jointTarget[3] = target[5];
	jointTarget[4] = target[6];
	jointTarget[5] = target[7];

	jointTarget[0] = saturateFloatUnsym(jointTarget[0], maximum.k1, minimum.k1);
	jointTarget[1] = saturateFloatUnsym(jointTarget[1], maximum.k2, minimum.k2);
	jointTarget[2] = saturateFloatUnsym(jointTarget[2], maximum.k3, minimum.k3);
	jointTarget[3] = saturateFloatUnsym(jointTarget[3], maximum.k4, minimum.k4);
	jointTarget[4] = saturateFloatUnsym(jointTarget[4], maximum.k5, minimum.k5);
}

void MotorManagerUpdateTargetGlobalTask()
{
	for (;;) {
		MotorManagerUpdateTargetGlobal();
		sys.delay(100);
	}
}

void MotorManagerUpdateTargetDef(float j1, float j2,  float j3,  float j5,  float j6)
{
	jointTarget[0] = j1;
	jointTarget[1] = j2;
	jointTarget[2] = j3;
	jointTarget[3] = j5;
	jointTarget[4] = j6;

	jointTarget[0] = saturateFloatUnsym(jointTarget[0], maximum.k1, minimum.k1);
	jointTarget[1] = saturateFloatUnsym(jointTarget[1], maximum.k2, minimum.k2);
	jointTarget[2] = saturateFloatUnsym(jointTarget[2], maximum.k3, minimum.k3);
	jointTarget[3] = saturateFloatUnsym(jointTarget[3], maximum.k4, minimum.k4);
	jointTarget[4] = saturateFloatUnsym(jointTarget[4], maximum.k5, minimum.k5);
}

void MotorManagerUpdateTargetDef(Coordinates point)
{
	if (point.type == jointsCo) {
		jointTarget[0] = point.k1;
		jointTarget[1] = point.k2;
		jointTarget[2] = point.k3;
		jointTarget[3] = point.k4;
		jointTarget[4] = point.k5;

		jointTarget[0] = saturateFloatUnsym(jointTarget[0], maximum.k1, minimum.k1);
		jointTarget[1] = saturateFloatUnsym(jointTarget[1], maximum.k2, minimum.k2);
		jointTarget[2] = saturateFloatUnsym(jointTarget[2], maximum.k3, minimum.k3);
		jointTarget[3] = saturateFloatUnsym(jointTarget[3], maximum.k4, minimum.k4);
		jointTarget[4] = saturateFloatUnsym(jointTarget[4], maximum.k5, minimum.k5);
	}
}

void MotorManagerSetOffsetDef(int t_joint, float value)
{
	switch (t_joint) {
	case 1:
		offset.k1 = value;
		break;
	case 2:
		offset.k2 = value;
		break;
	case 3:
		offset.k3 = value;
		break;
	case 5:
		offset.k4 = value;
		break;
	case 6:
		offset.k5 = value;
		break;
	}
}

void MotorManagerSetOffsetDef(Coordinates current_point)
{
	current_point.Translate(jointsCo);
	offset = current_point;
}

bool CheckIfInRange(Coordinates* point)
{
	if (point->k1 != saturateFloatUnsym(point->k1, maximum.k1, minimum.k1))return false;
	if (point->k2 != saturateFloatUnsym(point->k2, maximum.k1, minimum.k2))return false;
	if (point->k3 != saturateFloatUnsym(point->k3, maximum.k1, minimum.k3))return false;
	if (point->k4 != saturateFloatUnsym(point->k4, maximum.k1, minimum.k4))return false;
	if (point->k5 != saturateFloatUnsym(point->k5, maximum.k1, minimum.k5))return false;
	return true;
}

void setGripperValume(int volume)
{
	jointTarget[5] = volume;
}