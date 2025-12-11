#include "StdAfx.h"
#include "NTC.h"

//
IMPLEMENT_RUNTIME_CLASS(CNTC)

//////////////////////////////////////////////////////////////////////////
CNTC::CNTC(void)
{
}

CNTC::~CNTC(void)
{
}

SPRESULT CNTC::__PollAction(void)
{
	if (m_bNTC)
	{
		CHKRESULT(RunNTC());
	}
	if (m_bNTC_Sub)
	{
		CHKRESULT(RunNTC_SUB());
	}

	return SP_OK;
}
BOOL CNTC::LoadXMLConfig(void)
{
	m_bNTC= GetConfigValue(L"Option:NTC", 0);
	m_bNTC_Sub = GetConfigValue(L"Option:NTC_SUB", 0);
	m_nUpSpec = GetConfigValue(L"Option:UpSpec", 0);
	m_nDownSpec = GetConfigValue(L"Option:DownSpec", 0);
	return TRUE;
}

SPRESULT CNTC::RunNTC()
{
	double dTemp = 0.0;
	CHKRESULT(PrintErrorMsg(SP_BBAT_NTC(m_hDUT, &dTemp), "Phone: NTC", LEVEL_ITEM));

	_UiSendMsg("Phone: NTC", LEVEL_ITEM, m_nDownSpec, dTemp, m_nUpSpec, nullptr, -1, nullptr, "Temperature = %.2f", dTemp);
	if (dTemp < m_nDownSpec || dTemp > m_nUpSpec) //mV
	{
		return SP_E_BBAT_NTC_VALUE_FAIL;
	}
	return SP_OK;
}

SPRESULT CNTC::RunNTC_SUB()
{
	double dTemp = 0.0;
	CHKRESULT(PrintErrorMsg(SP_BBAT_NTC_SUB(m_hDUT, &dTemp), "Phone: NTC SUB", LEVEL_ITEM));

	_UiSendMsg("Phone: NTC SUB", LEVEL_ITEM, m_nDownSpec, dTemp, m_nUpSpec, nullptr, -1, nullptr, "Temperature = %.2f", dTemp);
	if (dTemp < m_nDownSpec || dTemp > m_nUpSpec) //mV
	{
		return SP_E_BBAT_NTC_VALUE_FAIL;
	}
	return SP_OK;
}


