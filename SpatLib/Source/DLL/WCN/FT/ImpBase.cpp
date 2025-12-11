#include "StdAfx.h"
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
  : m_ueChip(0)
{
}

CImpBase::~CImpBase(void)
{
}

BOOL CImpBase::LoadXMLConfig(void)
{
    m_ueChip = GetConfigValue(L"GLOBAL:CHIP", 0/*(INT)0x8910*/, TRUE);
    return TRUE;
}



