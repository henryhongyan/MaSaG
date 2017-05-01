#include "Joint.h"
#include "SystemParams.h"
#include "EPOS3.h"
#include "MAXPOS.h"
#include "mcDSA-E65.h"
#include "MCDC3006_CF.h"

Joint::Joint(void* mAxis, float gear, int encoder, int driver_type)
{
	switch (driver_type)
	{
	case TYPE_MAXPOS:
		driver = new MAXPOS;
		break;

	case TYPE_EPOS3:
		driver = new EPOS3;
		break;

	case TYPE_mcDSA_E65:
		driver = new mcDSA_E65;
		break;

	case TYPE_MCDC3006_CF:
		// to do
		break;
	}
	_gearRatio= gear;
	driver->_nodeID = mAxis;
	driver->_resEncoder = encoder;
}

void Joint::updateJoint()
{
	driver->readEncoder();
	_angle = (float)driver->_countEncoder / _gearRatio / driver->_resEncoder;
}

Joint::~Joint()
{
	delete driver;
}