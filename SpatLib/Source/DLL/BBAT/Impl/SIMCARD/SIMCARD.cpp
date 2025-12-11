#include "StdAfx.h"
#include "SIMCARD.h"

//
IMPLEMENT_RUNTIME_CLASS(CSIMCARD)

//////////////////////////////////////////////////////////////////////////
CSIMCARD::CSIMCARD(void)
{
}

CSIMCARD::~CSIMCARD(void)
{
}
SPRESULT CSIMCARD::__PollAction(void)
{
	if (m_bSIMCard1)
	{
		Sleep(3000);
		CHKRESULT(RunSimCard(SimCard1));
	}
	if (m_bSIMCard2)
	{
		Sleep(3000);
		CHKRESULT(RunSimCard(SimCard2));
	}
	if (m_bSIMCard3)
	{
		Sleep(3000);
		CHKRESULT(RunSimCard(SimCard3));
	}
	if (m_bSIMCard4)
	{
		Sleep(3000);
		CHKRESULT(RunSimCard(SimCard4));
	}
	if (m_bTCard1)
	{
		CHKRESULT(RunTCard(TCard1));
	}
	if (m_bTCard2)
	{
		CHKRESULT(RunTCard(TCard2));
	}
	if (m_bFpSimCard1)
	{
		Sleep(3000);
		CHKRESULT(RunFpSimCard(SimCard1));
	}
	if (m_bFpSimCard2)
	{
		Sleep(3000);
		CHKRESULT(RunFpSimCard(SimCard2));
	}
	return SP_OK;

}
BOOL CSIMCARD::LoadXMLConfig(void)
{
	m_bSIMCard1 = GetConfigValue(L"Option:SIMCARD1:Select", 0);
	m_bSIMCard2 = GetConfigValue(L"Option:SIMCARD2:Select", 0);
	m_bSIMCard3 = GetConfigValue(L"Option:SIMCARD3:Select", 0);
	m_bSIMCard4 = GetConfigValue(L"Option:SIMCARD4:Select", 0);

	m_bTCard1 = GetConfigValue(L"Option:TCARD1:Select", 0);
	m_bTCard2 = GetConfigValue(L"Option:TCARD2:Select", 0);

	m_bFpSimCard1 = GetConfigValue(L"Option:FP_SIMCARD_1:Select", 0);
	m_bFpSimCard2 = GetConfigValue(L"Option:FP_SIMCARD_2:Select", 0);
	return TRUE;
}

SPRESULT CSIMCARD::RunSimCard(BBAT_SimCard_VALUE_BYTE SimCardId)
{
//	SP_PhoneCommand_LogFmtStrA(m_hDUT, SPLOGLV_INFO, "SP_BBAT_SimCard");
	SPRESULT res = SP_BBAT_SimCard(m_hDUT, SimCardId);
	if (res != SP_OK)
	{
		_UiSendMsg("SimCard Test", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr,
			"SimCard %d", SimCardId + 1);
		return SP_E_BBAT_CMD_FAIL;
	}
	_UiSendMsg("SimCard Test", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"SimCard %d", SimCardId + 1);

	return SP_OK;
}

SPRESULT CSIMCARD::RunTCard(BBAT_TCard_VALUE_BYTE TCardId)
{
	SPRESULT res = SP_BBAT_TCard(m_hDUT, TCardId);
	if (res != SP_OK)
	{
		_UiSendMsg("TCard Test", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr,
			"TCard %d", TCardId + 1);
		return SP_E_BBAT_CMD_FAIL;
	}
	_UiSendMsg("TCard Test", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"TCard %d", TCardId + 1);
	return SP_OK;
}

SPRESULT CSIMCARD::RunFpSimCard(BBAT_SimCard_VALUE_BYTE SimCardId)
{
	SPRESULT res = SP_BBAT_FP_SimCard(m_hDUT, SimCardId);
	if (res != SP_OK)
	{
		_UiSendMsg("FP SimCard Test", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr,
			"SimCard %d", SimCardId + 1);
		return SP_E_BBAT_CMD_FAIL;
	}
	_UiSendMsg("FP SimCard Test", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"SimCard %d", SimCardId + 1);

	return SP_OK;
}

