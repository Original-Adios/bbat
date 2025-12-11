#include "StdAfx.h"
#include "Pcie.h"
//

IMPLEMENT_RUNTIME_CLASS(CPcie)

//////////////////////////////////////////////////////////////////////////
CPcie::CPcie(void)
{
    m_nPcie2 = 0;
    m_nPcie3 = 0;
}

CPcie::~CPcie(void)
{
}

SPRESULT CPcie::__PollAction(void)
{
    SPRESULT Res = SP_OK;
    if (m_nPcie2)
    {
        CHKRESULT(PrintErrorMsg(SP_BBAT_Pcie(m_hDUT, PCIE_2), "Pcie2.0 Test", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Pcie2.0 Test", LEVEL_ITEM);
    }
    if (m_nPcie3)
    {
        CHKRESULT(PrintErrorMsg(SP_BBAT_Pcie(m_hDUT, PCIE_3), "Pcie3.0 Test", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Pcie3.0 Test", LEVEL_ITEM);
    }   
    return Res;
}

BOOL CPcie::LoadXMLConfig(void)
{
    m_nPcie2 = GetConfigValue(L"Option:Pcie2.0", 0);
    m_nPcie3 = GetConfigValue(L"Option:Pcie3.0", 0);
    return TRUE;
}


