#include "StdAfx.h"
#include "WriteSNFromRobot.h"
#include "ShareMemoryDefine.h"
#include "CLocks.h"
#include "UeHelper.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CWriteSNFromRobot)

    /// 
    CWriteSNFromRobot::CWriteSNFromRobot(void)
    : m_bSN1(TRUE)
    , m_bSN2(FALSE)
{
    ZeroMemory((void* )m_szSN1, sizeof(m_szSN1));
    ZeroMemory((void* )m_szSN2, sizeof(m_szSN2));
}

CWriteSNFromRobot::~CWriteSNFromRobot(void)
{
}

BOOL CWriteSNFromRobot::LoadXMLConfig(void)
{
    m_bSN1 = GetConfigValue(L"Option:SN1", 1);
    m_bSN2 = GetConfigValue(L"Option:SN2", 2);

    return TRUE;
}

SPRESULT CWriteSNFromRobot::__PollAction(void)
{
    //Write SN
    CHAR szSnFromRobot[ShareMemory_SN_SIZE] = {0};
    SPRESULT res = SP_OK;
    std::wstring strSNFromRobot = L"Reserved_$0007_SNFromRobot_" + std::to_wstring(GetAdjParam().nTASK_ID);
//    res = GetShareMemory(L"Reserved_$0007_SNFromRobot", (void* )szSnFromRobot, sizeof(szSnFromRobot));
    res = GetShareMemory(strSNFromRobot.c_str(), (void* )szSnFromRobot, sizeof(szSnFromRobot));
    if (res != SP_OK)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Can not get SN info from Robot, Pls Check ");
        return SP_E_PHONE_INVALID_DATA;
    }

	CUeHelper ue(this);

	CHKRESULT(ue.WriteSN(SN1, szSnFromRobot));

	NOTIFY(DUT_INFO_SN1 , szSnFromRobot);
	CHKRESULT(SetShareMemory(ShareMemory_SN1, (const void* )szSnFromRobot, sizeof(szSnFromRobot)));

	return SP_OK;
}