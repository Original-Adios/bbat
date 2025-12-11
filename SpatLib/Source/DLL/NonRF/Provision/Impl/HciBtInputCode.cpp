#include "StdAfx.h"
#include "HciBtInputCode.h"

IMPLEMENT_RUNTIME_CLASS(CHciBtInputCode)

CHciBtInputCode::CHciBtInputCode( void )
{

}

CHciBtInputCode::~CHciBtInputCode( void )
{

}

SPRESULT CHciBtInputCode::__PollAction( void )
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
