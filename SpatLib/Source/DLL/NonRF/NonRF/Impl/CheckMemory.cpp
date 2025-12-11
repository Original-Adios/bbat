#include "StdAfx.h"
#include "CheckMemory.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckMemory)

///
CCheckMemory::CCheckMemory(void)
{
   
}

CCheckMemory::~CCheckMemory(void)
{

}

BOOL CCheckMemory::LoadXMLConfig(void)
{
     m_strMemory = _W2CA(GetConfigValue(L"Option:EMMC_DDR", L""));
     trimA(m_strMemory);
     return TRUE;
}

SPRESULT CCheckMemory::__PollAction(void)
{
    LPCSTR AT = "AT+EMMCDDRSIZE";
    std::string strRsp = "";
    CHKRESULT_WITH_NOTIFY(SendAT(AT, strRsp), AT);
    if (m_strMemory.length() > 0)
    {
        if (m_strMemory == strRsp)
        {
            NOTIFY("CheckMemory", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
            return SP_OK;
        }
        else
        {
            NOTIFY("CheckMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
            return SP_E_SPAT_CHECK_MEMORY;
        }
    }
    else
    {
        NOTIFY("LoadMemory", LEVEL_ITEM|LEVEL_INFO, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_E_SPAT_CHECK_MEMORY;
    }
}

