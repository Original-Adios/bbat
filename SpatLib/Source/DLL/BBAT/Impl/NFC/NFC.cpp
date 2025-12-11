#include "StdAfx.h"
#include "NFC.h"

//
IMPLEMENT_RUNTIME_CLASS(CNfc)

//////////////////////////////////////////////////////////////////////////
CNfc::CNfc(void)
{
}

CNfc::~CNfc(void)
{
}

SPRESULT CNfc::__PollAction(void)
{
	CHKRESULT(NfcOpen());
	Sleep(500);
	SPRESULT Res = NfcRead();
	CHKRESULT(NfcClose());
	return Res;
}
BOOL CNfc::LoadXMLConfig(void)
{	
	return TRUE;
}

SPRESULT CNfc::NfcOpen()
{
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_NfcOpen(m_hDUT), 
		"NFC Open", 
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "NFC Open", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CNfc::NfcRead()
{
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_NfcRead(m_hDUT), 
		"NFC Read",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "NFC Read", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CNfc::NfcClose()
{
	CHKRESULT(PrintErrorMsg(
		SP_BBAT_NfcClose(m_hDUT), 
		"NFC Close", 
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "NFC Close", LEVEL_ITEM);

	return SP_OK;
}
