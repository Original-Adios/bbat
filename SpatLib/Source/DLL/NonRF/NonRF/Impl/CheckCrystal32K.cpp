#include "StdAfx.h"
#include "CheckCrystal32K.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckCrystal32K)

///
CCheckCrystal32K::CCheckCrystal32K(void)
{
	m_nLowLimit = -6;
	m_nHighLimit = 6;
}

CCheckCrystal32K::~CCheckCrystal32K(void)
{

}

BOOL CCheckCrystal32K::LoadXMLConfig(void)
{
	m_nLowLimit = GetConfigValue(L"Option:LowLimit", -6);
	m_nHighLimit = GetConfigValue(L"Option:HighLimit", 6);
	if(m_nLowLimit > m_nHighLimit)
	{
		SendCommonCallback(L"HighLimit:%d < LowLimit:%d", m_nHighLimit, m_nLowLimit);
		return FALSE;
	}
     return TRUE;
}

SPRESULT CCheckCrystal32K::__PollAction()
{
	uint32 input[6] = {0};
	uint32 ouput[6] = {0};
	input[0] = 77; //DEBUG_CMD_CALI_32K_CALIBRATION_Start

    CHKRESULT_WITH_NOTIFY_FUNNAME(SP_wcdmaActive(m_hDUT, TRUE));
	CHKRESULT_WITH_NOTIFY_FUNNAME(SP_wcdmaDebug(m_hDUT, input, ouput));
	int nFer = (int)ouput[1];
	NOTIFY("Check 32K Crystal", LEVEL_ITEM, m_nLowLimit, nFer, m_nHighLimit, NULL, -1, "Hz");
	CHKRESULT_WITH_NOTIFY_FUNNAME(SP_wcdmaActive(m_hDUT, FALSE));
	if(!IN_RANGE(m_nLowLimit, nFer, m_nHighLimit))
	{
		return SP_E_SPAT_TEST_FAIL;
	}
    return SP_OK;
}


