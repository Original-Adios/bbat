#include "StdAfx.h"
#include "FM.h"

//
IMPLEMENT_RUNTIME_CLASS(CFm)

//////////////////////////////////////////////////////////////////////////
CFm::CFm(void)
{
}

CFm::~CFm(void)
{
}

SPRESULT CFm::__PollAction(void)
{
	SPRESULT Res = RunOpenAction();
	if (m_nClose)
	{
		CHKRESULT(RunClose());
	}

	   
	return Res;
}
BOOL CFm::LoadXMLConfig(void)
{
	m_nClose = GetConfigValue(L"Option:Fm_Close", 0);
    m_sFreq = (uint16)(GetConfigValue(L"Option:Freq", 0.0) * 10);

	LPCWSTR lpwFmName[MAX_FM_TYPE] =
	{
		L"Lant",
		L"Sant",
		L"FP"
	};
	m_nType = GetComboxValue(lpwFmName, MAX_FM_TYPE, L"Option:Fm_Open");
	if (m_nType == MAX_FM_TYPE)
	{
		return FALSE;
	}
	//else if (m_nType == FP)
	//{
	//	m_sFreq = sTemp;
	//}
	//else
	//{
	//	//sTemp 高位字节和低位字节互换
	//	uint8 sLow = sTemp & 0xFF;
	//	uint8 sHigh = sTemp >> 8;
	//	m_sFreq = sLow << 8 | sHigh;
	//}

	m_nRssi = GetConfigValue(L"Option:Rssi", 0);
	return TRUE;
}

SPRESULT CFm::RunLantOpen()
{
	int nRssiValue = 0;
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_FmOpenLant(m_hDUT, m_sFreq, &nRssiValue), 
		"FM Long Antenna Open", 
		LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "FM Long Antenna Open", LEVEL_ITEM);

	_UiSendMsg("FM Rssi Value", LEVEL_ITEM, 
		m_nRssi, nRssiValue, MaxFMRssiNumber,
		nullptr, -1, nullptr);

	if (nRssiValue < m_nRssi)
	{
		return SP_E_BBAT_FM_VALUE_FAIL;
	}
	return SP_OK;
}

SPRESULT CFm::RunSantOpen()
{
	int nRssiValue = 0;
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_FmOpenSant(m_hDUT, m_sFreq, &nRssiValue), 
		"FM Short Antenna Open", 
		LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "FM Short Antenna Open", LEVEL_ITEM);

	_UiSendMsg("FM Rssi Value", LEVEL_ITEM, 
		m_nRssi, nRssiValue, MaxFMRssiNumber,
		nullptr, -1, nullptr);

	if (nRssiValue< m_nRssi)
	{
		return SP_E_BBAT_FM_VALUE_FAIL;
	}

	return SP_OK;
}

SPRESULT CFm::RunClose()
{
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_FmClose(m_hDUT), 
		"FM Close", LEVEL_ITEM));

	return SP_OK;
}

SPRESULT CFm::RunOpenAction()
{
	switch (m_nType)
	{
	case LANT:
		CHKRESULT(RunLantOpen());
		break;
	case SANT:
		CHKRESULT(RunSantOpen());
		break;
	case FP:
		CHKRESULT(RunFpOpen());
		break;
	default:
		return SP_E_BBAT_VALUE_FAIL;
		break;
	}

	return SP_OK;
}

SPRESULT CFm::RunFpOpen()
{
	int nRssiValue = 0;
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_FP_FmOpen(m_hDUT, m_sFreq, &nRssiValue),
		"FP FM Antenna Open",
		LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "FP FM Antenna Open", LEVEL_ITEM);

	_UiSendMsg("FP FM Rssi Value", LEVEL_ITEM,
		m_nRssi, nRssiValue, MaxFMRssiNumber,
		nullptr, -1, nullptr);

	if (nRssiValue < m_nRssi)
	{
		return SP_E_BBAT_FM_VALUE_FAIL;
	}

	return SP_OK;
}


