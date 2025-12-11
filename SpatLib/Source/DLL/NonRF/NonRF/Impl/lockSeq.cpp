#include "StdAfx.h"
#include "LockSeq.h"
#include "ActionApp.h"
//
IMPLEMENT_RUNTIME_CLASS(CLockSeq)

//////////////////////////////////////////////////////////////////////////
CLockSeq::CLockSeq(void)
{
}

CLockSeq::~CLockSeq(void)
{
}

SPRESULT CLockSeq::__PollAction(void)
{ 
	extern CActionApp myApp;

	::EnterCriticalSection(&myApp.GetLock());
    return SP_OK;
}
