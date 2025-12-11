#include "StdAfx.h"
#include "WCNInputCodes.h"

IMPLEMENT_RUNTIME_CLASS(CWCNInputCodes)

CWCNInputCodes::CWCNInputCodes( void )
{

}

CWCNInputCodes::~CWCNInputCodes( void )
{

}

SPRESULT CWCNInputCodes::__PollAction( void )
{
	CHKRESULT(__super::__PollAction());
	if(m_InputSN[BC_OTTSN].bEnable)
	{
		NOTIFY(DUT_INFO_UINAMEKEY, (const char*)m_InputSN[BC_OTTSN].szCode);
	}
	else if (m_InputSN[BC_WIFI].bEnable)
	{
		NOTIFY(DUT_INFO_UINAMEKEY, (const char*)m_InputSN[BC_WIFI].szCode);
	}
	else if (m_InputSN[BC_BT].bEnable)
	{
		NOTIFY(DUT_INFO_UINAMEKEY, (const char*)m_InputSN[BC_BT].szCode);
	}
	return SP_OK;
}
