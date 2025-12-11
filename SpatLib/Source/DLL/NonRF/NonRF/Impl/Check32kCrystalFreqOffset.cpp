#include "StdAfx.h"
#include "Check32kCrystalFreqOffset.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheck32kCrystalFreqOffset)

///
CCheck32kCrystalFreqOffset::CCheck32kCrystalFreqOffset(void)
: m_uCheckSpec(0)
{

}

CCheck32kCrystalFreqOffset::~CCheck32kCrystalFreqOffset(void)
{
}

BOOL CCheck32kCrystalFreqOffset::LoadXMLConfig(void)
{
    m_uCheckSpec = GetConfigValue(L"Option:CrystalFreqOffsetSpec", 300);
    return TRUE;
}

SPRESULT CCheck32kCrystalFreqOffset::__PollAction(void)
{ 
    //LTE Active
    CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, TRUE));

    CHAR szMsg[64] = {0};
    sprintf_s(szMsg, "Send AT");
    NOTIFY(szMsg, LEVEL_UI, 1, 1, 1, NULL, -1, " ");

    //Send: AT+SP32KCRYFO
    std::string strAT = ("AT+SP32KCRYFO");
    CHAR   recvBuf[1024] = { 0 };
    uint32 recvSize = 0;
    SPRESULT res = SP_SendATCommand(m_hDUT, strAT.c_str(), TRUE, recvBuf, sizeof(recvBuf), &recvSize, TIMEOUT_3S);
    if (SP_OK != res || NULL != strstr(recvBuf, "+CME ERROR"))
    {
        NOTIFY(strAT.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", recvBuf);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }

    //Rev: +SP32KCRYFO:Cry_freq_offset
    std::string strResult = strchr(recvBuf, ':') + 1;
    int nResult = atoi(strResult.c_str());

    //Check Result
    if (abs(nResult) > m_uCheckSpec)
    {
        NOTIFY(strAT.c_str(), LEVEL_ITEM, 0, abs(nResult), m_uCheckSpec, NULL, -1, NULL, recvBuf);
        CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
        return SP_E_FAIL;
    }
    else
    {
        NOTIFY(strAT.c_str(), LEVEL_ITEM, 0, abs(nResult), m_uCheckSpec, NULL, -1, NULL, recvBuf);
        CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
        return SP_OK;
    }

    return SP_OK;
}
