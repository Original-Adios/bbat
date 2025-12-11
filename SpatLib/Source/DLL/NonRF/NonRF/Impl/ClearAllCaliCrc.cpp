#include "StdAfx.h"
#include "ClearAllCaliCrc.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CClearAllCaliCrc)

///
CClearAllCaliCrc::CClearAllCaliCrc(void)
{   
}

CClearAllCaliCrc::~CClearAllCaliCrc(void)
{
}

SPRESULT CClearAllCaliCrc::__PollAction()
{
	SPRESULT sRet = SP_ClearAllCaliCrc(m_hDUT);//Bug1506419

    NOTIFY("ClearAllCaliCrc", LEVEL_ITEM, 1, (SP_OK == sRet) ? 1 : 0, 1, NULL, -1, NULL);
    return sRet;
}