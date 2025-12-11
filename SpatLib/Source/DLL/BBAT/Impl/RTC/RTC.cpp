#include "StdAfx.h"
#include "RTC.h"

//
IMPLEMENT_RUNTIME_CLASS(CRTC)

//////////////////////////////////////////////////////////////////////////
CRTC::CRTC(void)
{
}

CRTC::~CRTC(void)
{
}

SPRESULT CRTC::__PollAction(void)
{
	CHKRESULT(RunOpen());
	SPRESULT Res = RunRead();
	CHKRESULT(RunClose());
	return Res;
}
BOOL CRTC::LoadXMLConfig(void)
{
	return TRUE;
}

SPRESULT CRTC::RunOpen()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_RtcOpen(m_hDUT),"RTC Open",LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "RTC Open", LEVEL_ITEM);
	
	return SP_OK;
}

SPRESULT CRTC::RunRead()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_RtcRead(m_hDUT),"RTC Read",LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "RTC Read", LEVEL_ITEM);	

	return SP_OK;
}

SPRESULT CRTC::RunClose()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_RtcClose(m_hDUT),"RTC Close",LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "RTC Close", LEVEL_ITEM);

	return SP_OK;
}
