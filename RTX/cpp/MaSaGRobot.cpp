#include "MaSaGRobot.h"

MaSaGRobot::MaSaGRobot()
{
	LArm = new Manipulator(&_RTX.pData->_rtxLArm);
	RArm->joint[0]->driver->_resEncoder = 4096;				// special defined since we use RE40 motors with different specifications
	RArm = new Manipulator(&_RTX.pData->_rtxRArm);
}

int MaSaGRobot::initRobot()
{
	int ret;
	ret = _RTX.Init();
	// appoint the addresses of slave devices
	for (int i = 0; i < ARM_DOF - 2; i++)
	{
		LArm->joint[i]->driver->_nodeID = _RTX.pData->_rtxLArm.mAxis[i];
		RArm->joint[i]->driver->_nodeID = _RTX.pData->_rtxRArm.mAxis[i];
	}
	return ret;
}

void MaSaGRobot::updateFeedback()
{
	LArm->encoderFB();
	RArm->encoderFB();
}

void MaSaGRobot::updateState()
{
	LArm->update();
	RArm->update();
}

void MaSaGRobot::updateSharedMemory()
{
	int i;

	// joint data 
	for (i = 0; i < ARM_DOF - 2; i++)
	{
		_RTX.pData->_rtxLArm.curPos[i] = LArm->curJoint(i);             
		_RTX.pData->_rtxRArm.curPos[i] = RArm->curJoint(i);
		_RTX.pData->_rtxLArm.tarPos[i] = LArm->tarJoint(i);
		_RTX.pData->_rtxRArm.tarPos[i] = RArm->tarJoint(i);
		_RTX.pData->_rtxLArm.errPos[i] = LArm->errJoint(i);
		_RTX.pData->_rtxRArm.errPos[i] = RArm->errJoint(i);
		_RTX.pData->_rtxLArm.plnPos[i] = LArm->plnJoint(i);
		_RTX.pData->_rtxRArm.plnPos[i] = RArm->plnJoint(i);
		_RTX.pData->_rtxLArm.curVel[i] = LArm->velJoint(i);
		_RTX.pData->_rtxRArm.curVel[i] = RArm->velJoint(i);
	}

	// tcp data
	for (i = 0; i < 6; i++)
	{
		_RTX.pData->_rtxLArm.curTCP[i] = LArm->curTCP(i);
		_RTX.pData->_rtxRArm.curTCP[i] = RArm->curTCP(i);
		_RTX.pData->_rtxLArm.tarTCP[i] = LArm->tarTCP(i);
		_RTX.pData->_rtxRArm.tarTCP[i] = RArm->tarTCP(i);
		_RTX.pData->_rtxLArm.errTCP[i] = LArm->errTCP(i);
		_RTX.pData->_rtxRArm.errTCP[i] = RArm->errTCP(i);
		_RTX.pData->_rtxLArm.plnTCP[i] = LArm->plnTCP(i);
		_RTX.pData->_rtxRArm.plnTCP[i] = RArm->plnTCP(i);
		_RTX.pData->_rtxLArm.velTCP[i] = LArm->velTCP(i);
		_RTX.pData->_rtxRArm.velTCP[i] = RArm->velTCP(i);
	}
}

void MaSaGRobot::MotionTrajGen()
{
	// Left Arm
	if (LArm->is_busy)
	{
		switch (LArm->planner_mode)
		{
		case Cubic_TCP_Timer:
			LArm->mtn->cubicTCP_timer(LArm);
			break;
		case Cubic_Joint_Timer:
			LArm->mtn->cubicJoint_timer(LArm);
			break;
		case OTG_TCP_Timer:
			LArm->mtn->otgTCP_timer(LArm);
			break;
		case OTG_Joint_Timer:
			LArm->mtn->otgJoint_timer(LArm);
			break;
		}
	}
	
	// Right Arm
	if(RArm->is_busy)
	{
		switch (RArm->planner_mode)
		{
		case Cubic_TCP_Timer:
			RArm->mtn->cubicTCP_timer(RArm);
			break;
		case Cubic_Joint_Timer:
			RArm->mtn->cubicJoint_timer(RArm);
			break;
		case OTG_TCP_Timer:
			RArm->mtn->otgTCP_timer(RArm);
			break;
		case OTG_Joint_Timer:
			RArm->mtn->otgJoint_timer(RArm);
			break;
		}
	}
}

void MaSaGRobot::ControlLaw()
{
	// Left Arm
	switch (LArm->control_mode)
	{
	case Cartesian_Mode:
		LArm->ctrl.Cartesian_Impedance(LArm);
		break;
	case Joint_Mode:
		LArm->ctrl.Joint_Impdeance(LArm);
		break;
	case Free_Mode:
		LArm->ctrl.Free_Mode(LArm);
		break;
	}

	// Right Arm
	switch (RArm->control_mode)
	{
	case Cartesian_Mode:
		RArm->ctrl.Cartesian_Impedance(RArm);
		break;
	case Joint_Mode:
		RArm->ctrl.Joint_Impdeance(RArm);
		break;
	case Free_Mode:
		RArm->ctrl.Free_Mode(RArm);
		break;
	}
}

void MaSaGRobot::commandOutput()
{
	LArm->torqueCMD();
	RArm->torqueCMD();
}

MaSaGRobot::~MaSaGRobot()
{
	delete LArm;
	delete RArm;
}

void MaSaGRobot::shoulderPOS(int arm_index, int direction_index, float angle)
{
	RTN_ERR ret;

	int targetPos;

	if (arm_index == LEFT_ARM)
	{
		LArm->joint[0]->driver->haltOff();
		Sleep(2000);

		targetPos = angle * resEncoder_ARM[0] * gearRatio_ARM[0];
		if (direction_index == INWARD)
			LArm->joint[0]->driver->cmdPosition(-targetPos, REL_MOTION);
		else if(direction_index == OUTWARD)
			LArm->joint[0]->driver->cmdPosition( targetPos, REL_MOTION);
	}
	else if (arm_index == RIGHT_ARM)
	{
		RArm->joint[0]->driver->haltOff();
		Sleep(2000);

		targetPos = angle * resEncoder_ARM[0] * gearRatio_ARM[0];
		if (direction_index == INWARD)
			RArm->joint[0]->driver->cmdPosition( targetPos, REL_MOTION);
		else if (direction_index == OUTWARD)
			RArm->joint[0]->driver->cmdPosition(-targetPos, REL_MOTION);
	}

}