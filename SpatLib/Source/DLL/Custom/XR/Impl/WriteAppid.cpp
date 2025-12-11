#include "StdAfx.h"
#include "WriteAppid.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//#include "ExtraLogFile.h"


IMPLEMENT_RUNTIME_CLASS(CWriteAppid)


//////////////////////////////////////////////////////////////////////////
CWriteAppid::CWriteAppid(void)
{
	m_u32AppidBase = 800*1024;
	m_u8AppidDigts = 32;
}

CWriteAppid::~CWriteAppid(void)
{
}

BOOL CWriteAppid::LoadXMLConfig(void)
{
	std::wstring strMiscDataPosition = GetConfigValue(L"Option:APPID:MiscDataPosition", L"800K");
	if(L"768K" == strMiscDataPosition)
	{
		m_u32AppidBase = 768*1024;
	}
	else
	{
		m_u32AppidBase = 800*1024;
	}

	m_u8AppidDigts = (uint8)GetConfigValue(L"Option:APPID:Digits", 32);

    return TRUE;
}

SPRESULT CWriteAppid::__PollAction(void)
{ 
    CHAR szAppid[128] = {0};
    INPUT_CODES_T appidCode;
	_UNISOC_MES_CMN_CODES mes_codes;
	SPRESULT sp_result = SP_OK;

	//判断是否mes在线
	MES_RESULT mesResult = UnisocMesActive();
	if(mesResult != MES_SUCCESS)
	{
		//从用户输入中获取Appid
		sp_result = GetShareMemory(ShareMemory_XunRuiInputAppid, (void* )&appidCode, sizeof(INPUT_CODES_T));
		if (SP_OK != sp_result)
		{
			NOTIFY("WriteAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_XunRuiInputAppid)");
			return sp_result;
		}
		strcpy_s(szAppid,sizeof(szAppid),appidCode.szCode);
	}
	else
	{
		//从mes系统获取APPID
		sp_result = GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
		if (SP_OK != sp_result)
		{
			NOTIFY("WriteAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Assigned_Codes)");
			return sp_result;
		}
		else
		{
			strcpy_s(szAppid,sizeof(szAppid),mes_codes.sz_cmn_code2);
		}
	}
	if(strlen(szAppid) != m_u8AppidDigts)
	{
		NOTIFY("WriteAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "the length of appid is invalid.");
		return SP_E_FAIL;
	}

	//写入miscdata
	uint32 u32Base = m_u32AppidBase;  // 写入Customer区域
    uint8*  lpBuff = (uint8*)szAppid;
    CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, u32Base, lpBuff, m_u8AppidDigts + 1, TIMEOUT_3S), "SaveCustomerMiscData");

	NOTIFY("WriteAppid", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SaveCustomerMiscData");
    return SP_OK;
}
