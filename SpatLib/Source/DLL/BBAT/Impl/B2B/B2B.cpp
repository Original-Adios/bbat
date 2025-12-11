#include "StdAfx.h"
#include "B2B.h"

//
IMPLEMENT_RUNTIME_CLASS(CB2B)

//////////////////////////////////////////////////////////////////////////
CB2B::CB2B(void)
{
	m_LCD_ID = 0;
	m_TP_ID = 0;
	m_RF_ID = 0;
	m_TP_VALUE = 0;
	m_FINGER_ID = 0;
}

CB2B::~CB2B(void)
{
}

SPRESULT CB2B::__PollAction(void)
{
	if (m_LCD_ID)
	{
		CHKRESULT(Run_LCD_ID());
	}
	if (m_TP_ID)
	{
		CHKRESULT(Run_TP_ID());
	}
	if (m_RF_ID)
	{
		CHKRESULT(Run_RF_ID());
	}
	if (m_TP_VALUE)
	{
		CHKRESULT(Run_TP_VALUE());
	}
	if (m_FINGER_ID)
	{
		CHKRESULT(Run_FINGER_ID());
	}

	return SP_OK;
}
BOOL CB2B::LoadXMLConfig(void)
{
	m_LCD_ID = GetConfigValue(L"Option:LCD_ID", 0);
	m_RF_ID = GetConfigValue(L"Option:RF_ID", 0);
	m_TP_ID = GetConfigValue(L"Option:TP_ID", 0);
	m_FINGER_ID = GetConfigValue(L"Option:FINGER_ID", 0);
	m_TP_VALUE = GetConfigValue(L"Option:TP_VALUE", 0);
	return TRUE;
}

SPRESULT CB2B::Run_LCD_ID()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_LCD_ID(m_hDUT), "B2B LCD ID Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "B2B LCD ID Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CB2B::Run_TP_ID()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_TP_ID(m_hDUT), "B2B TP ID Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "B2B TP ID Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CB2B::Run_TP_VALUE()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_TP_VALUE(m_hDUT), "B2B TP Value Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "B2B TP Value Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CB2B::Run_RF_ID()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_RF_ID(m_hDUT), "B2B RF ID Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "B2B RF ID Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CB2B::Run_FINGER_ID()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_FINGER_ID(m_hDUT), "B2B Finger ID Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "B2B Finger ID Pass", LEVEL_ITEM);

	return SP_OK;
}





