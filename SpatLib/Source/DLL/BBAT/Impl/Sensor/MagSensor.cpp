#include "StdAfx.h"
#include "MagSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(CMagSensor)

//////////////////////////////////////////////////////////////////////////
CMagSensor::CMagSensor(void)
{

}

CMagSensor::~CMagSensor(void)
{
}
SPRESULT CMagSensor::__PollAction(void)
{
	CHKRESULT(RunMagTest());

	return SP_OK;
}
BOOL CMagSensor::LoadXMLConfig(void)
{
	m_MagValueX.upp = GetConfigValue(L"Option:X_SpecUp", 0.0);
	m_MagValueX.low = GetConfigValue(L"Option:X_SpecDown", 0.0);
	m_MagValueY.upp = GetConfigValue(L"Option:Y_SpecUp", 0.0);
	m_MagValueY.low = GetConfigValue(L"Option:Y_SpecDown", 0.0);
	m_MagValueZ.upp = GetConfigValue(L"Option:Z_SpecUp", 0.0);
	m_MagValueZ.low = GetConfigValue(L"Option:Z_SpecDown", 0.0);
	return TRUE;
}

SPRESULT CMagSensor::RunMagTest()
{
	MagReturn DataRecv;
	memset(&DataRecv, 0, sizeof(DataRecv));

	CHKRESULT(PrintErrorMsg(SP_BBAT_MagneticTest(m_hDUT, &DataRecv), "Magnetic Test Cmd Fail", LEVEL_ITEM));

	//Need Check
	CHKRESULT(PrintErrorMsg(CheckMag(DataRecv), "Axis Fail", LEVEL_ITEM));
    _UiSendMsg("Magnetic Test", LEVEL_ITEM,
        1, 1, 1,
        nullptr, -1, nullptr,
        "Pass");

	return SP_OK;
}

SPRESULT CMagSensor::CheckMag(MagReturn DataRecv)
{
	m_MagValueX.val = DataRecv.X_Axis;
	m_MagValueY.val = DataRecv.Y_Axis;
	m_MagValueZ.val = DataRecv.Z_Axis;
	SPRESULT sRes = SP_OK;

	_UiSendMsg("X Axis", LEVEL_ITEM, 
		m_MagValueX.low, m_MagValueX.val, m_MagValueX.upp, 
		nullptr, -1, nullptr);
	if (!m_MagValueX)
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}

	_UiSendMsg("Y Axis", LEVEL_ITEM, 
		m_MagValueY.low, m_MagValueY.val, m_MagValueY.upp, 
		nullptr, -1, nullptr);
	if (!m_MagValueY)
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}

	_UiSendMsg("Z Axis", LEVEL_ITEM, 
		m_MagValueZ.low, m_MagValueZ.val, m_MagValueZ.upp, 
		nullptr, -1, nullptr);
	if (!m_MagValueZ)
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}

	if ((m_MagValueX.val == 0) && (m_MagValueY.val == 0) && (m_MagValueZ.val == 0))
	{
		sRes = SP_E_BBAT_MAG_SENSOR_VALUE_FAIL;
	}

	return sRes;
}

