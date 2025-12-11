#include "StdAfx.h"
#include "LigSensor.h"


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CLigSensor)

/// 
CLigSensor::CLigSensor(void)
{
}

CLigSensor::~CLigSensor(void)
{
}
SPRESULT CLigSensor::__PollAction(void)
{	
	CHKRESULT(RunLig());
	PrintSuccessMsg(SP_OK, "Light Sensor Test", LEVEL_ITEM);
	return SP_OK;
}
BOOL CLigSensor::LoadXMLConfig(void)
{
	m_LigValue.upp = GetConfigValue(L"Option:SpecUp", 0);
	m_LigValue.low = GetConfigValue(L"Option:SpecDown", 0);

	return TRUE;
}

SPRESULT CLigSensor::RunLig()
{
	int nLightValue = 0;

	CHKRESULT(PrintErrorMsg(
		SP_BBAT_LightTest(m_hDUT, &nLightValue),
		"Light Sensor Open",
		LEVEL_ITEM));

	m_LigValue.val = (double)nLightValue;
	_UiSendMsg("Light Sensor Check", LEVEL_ITEM, 
		m_LigValue.low, m_LigValue.val, m_LigValue.upp, 
		nullptr, -1, nullptr);
	if (!m_LigValue)
	{
		return SP_E_BBAT_LIT_SENSOR_VALUE_FAIL;
	}
	return SP_OK;
}


