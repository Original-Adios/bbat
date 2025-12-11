#include "StdAfx.h"
#include "UnlockSeq.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CUnlockSeq)

//////////////////////////////////////////////////////////////////////////
CUnlockSeq::CUnlockSeq(void)
{
}

CUnlockSeq::~CUnlockSeq(void)
{
}

SPRESULT CUnlockSeq::__PollAction(void)
{ 
	extern CActionApp myApp;

	::LeaveCriticalSection(&myApp.GetLock());
    return SP_OK;
}
