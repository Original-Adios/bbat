#include "stdafx.h"
#include "UpdateCalFlag.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CUpdateCalFlag)

//
CUpdateCalFlag::CUpdateCalFlag()
{
}

CUpdateCalFlag::~CUpdateCalFlag()
{

}

//BOOL CUpdateCalFlag::LoadXMLConfig(void)
//{
//    return TRUE;
//}

SPRESULT CUpdateCalFlag::__PollAction(void)
{
	SPRESULT res = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&m_result, sizeof(m_result));
	if (SP_OK != res)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory fail!");
		NOTIFY("UpdateCalFlag", LEVEL_ITEM | LEVEL_CAL, 1, 0, 1, NULL, -1, NULL, "");
		return res;
	}

	BOOL bPASS = (SP_OK == m_result.errCode) ? TRUE : FALSE;
	//AT*CALVERIFY=1/0 return "SUCCESS"
	//AT*CALVERIFY? return "ERROR/1/0"  "ERROR" 表示该文件不存在
	std::string CAL_FLAG_CMD;
	
	if (bPASS)
	{
		CAL_FLAG_CMD = "AT*CALVERIFY=1";
	}
	else
	{
		CAL_FLAG_CMD = "AT*CALVERIFY=0";
	}

    std::string strRevBuf = "";
	res = SendAT(CAL_FLAG_CMD.c_str(), strRevBuf);
    if (SP_OK != res)
    {
        NOTIFY("UpdateCalFlag", LEVEL_ITEM | LEVEL_CAL, 1, 0, 1, NULL, -1, NULL, CAL_FLAG_CMD.c_str());
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
        if (NULL == strstr(strRevBuf.c_str(), "SUCCESS"))
        {
            NOTIFY("UpdateCalFlag", LEVEL_ITEM | LEVEL_CAL, 1, 0, 1, NULL, -1, NULL, strRevBuf.c_str());
            return SP_E_CHECK_TP_TYPE_ERROR;
        }
        else
        {
            NOTIFY("UpdateCalFlag", LEVEL_ITEM | LEVEL_CAL, 1, 1, 1, NULL, -1, NULL, strRevBuf.c_str());
        }
    }

    return SP_OK;
}
