#include "StdAfx.h"
#include "OpenJig.h"

IMPLEMENT_RUNTIME_CLASS(COpenJig)
//////////////////////////////////////////////////////////////////////////
COpenJig::COpenJig(void)
{
}

COpenJig::~COpenJig(void)
{
}

BOOL COpenJig::LoadXMLConfig(void)
{

    return TRUE;
}

SPRESULT COpenJig::__PollAction(void)
{

    return SP_OK;
}
