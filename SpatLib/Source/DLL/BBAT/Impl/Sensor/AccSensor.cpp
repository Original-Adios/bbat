#include "StdAfx.h"
#include "AccSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(CAccSensor)

//////////////////////////////////////////////////////////////////////////
CAccSensor::CAccSensor(void)
{
}

CAccSensor::~CAccSensor(void)
{
}
SPRESULT CAccSensor::__PollAction(void)
{
	CHKRESULT(RunAccTest());
	return SP_OK;
}
BOOL CAccSensor::LoadXMLConfig(void)
{
	std::string sAccuracy = _W2CA(GetConfigValue(L"Option:Accuracy", L""));
	m_AccValueX.upp = GetConfigValue(L"Option:X_SpecUp", 0.0);
	m_AccValueX.low = GetConfigValue(L"Option:X_SpecDown", 0.0);
	m_AccValueY.upp = GetConfigValue(L"Option:Y_SpecUp", 0.0);
	m_AccValueY.low = GetConfigValue(L"Option:Y_SpecDown", 0.0);
	m_AccValueZ.upp = GetConfigValue(L"Option:Z_SpecUp", 0.0);
	m_AccValueZ.low = GetConfigValue(L"Option:Z_SpecDown", 0.0);

	if (sAccuracy == "1k")
	{
		m_dAccuracy = 1e3;
	}
	else if (sAccuracy == "1m")
	{
		m_dAccuracy = 1e6;
	}
	return TRUE;
}

SPRESULT CAccSensor::RunAccTest()
{
	AccReturn DataRecv;
	memset(&DataRecv, 0, sizeof(DataRecv));

	CHKRESULT(PrintErrorMsg(
		SP_BBAT_AccelerometerTest(m_hDUT, &DataRecv),
		"Acc Sensor Cmd Fail", LEVEL_ITEM));

	CHKRESULT(PrintErrorMsg(CheckAcc(DataRecv), "Axis Fail", LEVEL_ITEM));

    _UiSendMsg("Acc Sensor Test", LEVEL_ITEM,
        1, 1, 1,
        nullptr, -1, nullptr,
        "Pass");

	return SP_OK;
}

SPRESULT CAccSensor::CheckAcc(AccReturn DataRecv)
{
	SPRESULT sRes = SP_OK;

	m_AccValueX.val = double(DataRecv.X_Axis) / m_dAccuracy;
	m_AccValueY.val = double(DataRecv.Y_Axis) / m_dAccuracy;
	m_AccValueZ.val = double(DataRecv.Z_Axis) / m_dAccuracy;	

	_UiSendMsg("X Axis", LEVEL_ITEM, 
		m_AccValueX.low, m_AccValueX.val, m_AccValueX.upp,
		nullptr, -1, nullptr);
	if (!m_AccValueX)
	{
		sRes = SP_E_BBAT_SENSOR_VALUE_FAIL;
	}

	_UiSendMsg("Y Axis", LEVEL_ITEM, 
		m_AccValueY.low, m_AccValueY.val, m_AccValueY.upp, 
		nullptr, -1, nullptr);
	if (!m_AccValueY)
	{
		sRes = SP_E_BBAT_SENSOR_VALUE_FAIL;
	}

	_UiSendMsg("Z Axis", LEVEL_ITEM, 
		m_AccValueZ.low, m_AccValueZ.val, m_AccValueZ.upp, 
		nullptr, -1, nullptr);
	if (!m_AccValueZ)
	{
		sRes = SP_E_BBAT_SENSOR_VALUE_FAIL;
	}
	return sRes;

}
