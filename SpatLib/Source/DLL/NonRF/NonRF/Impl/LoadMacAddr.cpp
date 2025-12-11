#include "StdAfx.h"
#include "LoadMacAddr.h"


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CLoadMacAddr)

/// 
CLoadMacAddr::CLoadMacAddr(void)
: m_bWIFI(TRUE)
, m_bBT(TRUE)
{
}

CLoadMacAddr::~CLoadMacAddr(void)
{
}

BOOL CLoadMacAddr::LoadXMLConfig(void)
{
    m_bWIFI = GetConfigValue(L"Option:WIFI", 1);
    m_bBT   = GetConfigValue(L"Option:BT",   1);

    return TRUE;
}

SPRESULT CLoadMacAddr::__PollAction(void)
{
    if (!m_bWIFI && !m_bBT)
    {
        return SP_OK;
    }

    PC_PRODUCT_DATA data;
    ZeroMemory((void* )&data, sizeof(data));
    if (m_bWIFI)
    {
        data.u32OperMask |= FNAMASK_RW_WIFIADDR;
    }

    if (m_bBT)
    {
        data.u32OperMask |= FNAMASK_RW_BTADDR;
    }

    CHKRESULT_WITH_NOTIFY(SP_LoadProductData(m_hDUT, &data, TIMEOUT_3S), "SP_LoadProductData");
    if (m_bWIFI)
    {
        NOTIFY(DUT_INFO_WIFI, (LPCSTR)data.szWIFIAddr);
        NOTIFY("WIFI MAC.", LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, "%s", (LPCSTR)data.szWIFIAddr);
    }

    if (m_bBT)
    {
        NOTIFY(DUT_INFO_BT, (LPCSTR)data.szBTAddr);
        NOTIFY("BT MAC.",  LEVEL_ITEM|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL, "%s", (LPCSTR)data.szBTAddr);
    }

    return SP_OK;
}
