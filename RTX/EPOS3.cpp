#include "EPOS3.h"

EPOS3::EPOS3()
{
	_maxVel = 100;
	_profileAcc = 100;
	_profileDec = 100;
}

int EPOS3::servoOn()
{
	ret = NEC_CoE402ServoOn(_nodeID, 1);
	return ret;
}

int EPOS3::servoOff()
{
	ret = NEC_CoE402ServoOn(_nodeID, 0);
	return ret;
}

int EPOS3::haltOff()
{
	ret = NEC_CoE402Halt(_nodeID, 0);
	return ret;
}

int EPOS3::haltOn()
{
	ret = NEC_CoE402Halt(_nodeID, 1);
	return ret;
}

int EPOS3::faultReset()
{
	ret = NEC_CoE402FaultReset(_nodeID, 2000);
	return ret;
}

int EPOS3::home()
{
	// to do
	return ret;
}

int EPOS3::readEncoder()
{
	ret = NEC_CoE402GetActualPosition(_nodeID, &_countEncoder);
	return ret;
}

int	EPOS3::readVelocity()
{
	ret = NEC_CoE402GetActualVelocity(_nodeID, &_actualVelocity);
	return ret;
}

int	EPOS3::readTorque(float& torque)
{
	ret = NEC_CoE402GetActualTorque(_nodeID, &_actualTorque);
	torque = 0.001 * _actualTorque * _ratedTorque;
	return ret;
}

int EPOS3::cmdPosition(int pos, int option)
{
	if (option)
		ret = NEC_CoE402PtpA(_nodeID, OPT_ABS | OPT_WMC, pos, _maxVel, _profileAcc, _profileDec);
	else
		ret = NEC_CoE402PtpA(_nodeID, OPT_REL | OPT_WMC, pos, _maxVel, _profileAcc, _profileDec);
	return ret;
}

int EPOS3::cmdVelocity(int vel)
{
	ret = NEC_CoE402SetTargetVelocity(_nodeID, vel);
	return ret;
}

int EPOS3::cmdTorque(float torque)
{
	if (torque >= 0)
		if (torque >= _ratedTorque)
			torque = _ratedTorque;
	else
		if (torque <= -_ratedTorque)
			torque = -_ratedTorque;

	short tarTorque = static_cast <short> (torque / _ratedTorque * 1000);
	ret = NEC_CoE402SetTargetTorque(_nodeID, tarTorque);
	
	return ret;
}

