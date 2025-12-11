#include "StdAfx.h"
#include "ProSensor.h"

//
IMPLEMENT_RUNTIME_CLASS(CProSensor)

//////////////////////////////////////////////////////////////////////////
CProSensor::CProSensor(void)
{

}

CProSensor::~CProSensor(void)
{
}
SPRESULT CProSensor::__PollAction(void)
{
	CHKRESULT(RunProTest());
	return SP_OK;
}
BOOL CProSensor::LoadXMLConfig(void)
{
	m_ProValue.upp = GetConfigValue(L"Option:SpecUp", 0);
	m_ProValue.low = GetConfigValue(L"Option:SpecDown", 0);
	return TRUE;
}

SPRESULT CProSensor::RunProTest()
{
	int nProLevel = 0;

	CHKRESULT(PrintErrorMsg(
		SP_BBAT_ProximityTest(m_hDUT, &nProLevel),
		"Proximity Sensor Open Fail",
		LEVEL_ITEM));

	//Need Check
	m_ProValue.val = (double)nProLevel;
	_UiSendMsg("Proximity Sensor Check", LEVEL_ITEM, 
		m_ProValue.low, m_ProValue.val, m_ProValue.upp, 
		nullptr, -1, nullptr);

	if (!m_ProValue)
	{
		return SP_E_BBAT_PRO_SENSOR_VALUE_FAIL;
	}
	return SP_OK;

}