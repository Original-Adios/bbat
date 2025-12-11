#include "StdAfx.h"
#include "CheckAppid.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//#include "ExtraLogFile.h"


IMPLEMENT_RUNTIME_CLASS(CCheckAppid)

//////////////////////////////////////////////////////////////////////////
CCheckAppid::CCheckAppid(void)
{
	m_u32AppidBase = 800*1024;
}

CCheckAppid::~CCheckAppid(void)
{
}

BOOL CCheckAppid::LoadXMLConfig(void)
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
    return TRUE;
}

SPRESULT CCheckAppid::__PollAction(void)
{ 
	string strAppid;
	string strDutAppid;
	INPUT_CODES_T appidCode;
	SPRESULT sp_result = SP_OK;
	bool bMesOnline = true;

	//判断是否mes在线
	MES_RESULT mesResult = UnisocMesActive();
	if(mesResult != MES_SUCCESS)
	{
		bMesOnline = false;
		//从用户输入中获取Appid
		sp_result = GetShareMemory(ShareMemory_XunRuiInputAppid, (void* )&appidCode, sizeof(INPUT_CODES_T));
		if (SP_OK != sp_result)
		{
			NOTIFY("WriteAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_XunRuiInputCodes)");
			return sp_result;
		}
		strAppid = appidCode.szCode;
	}
	else
	{
		//获取平台绑定的APPID(扩展字段code2)
		SPRESULT sp_result = SP_OK;
		_UNISOC_MES_CMN_CODES mes_codes;
		sp_result = GetShareMemory(ShareMemory_MES_Device_Codes, (void* )&mes_codes, sizeof(mes_codes));
	
		if (SP_OK != sp_result)
		{
			NOTIFY("CheckAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Device_Codes)");
			return sp_result;
		}
		else
		{
			strAppid = mes_codes.sz_cmn_code2;
		}
	}

	//读取手机端Appid
	uint8 rbuff[MISCDATA_FOR_APPID_LENGTH] = {0};
    uint32 u32Base = m_u32AppidBase;
    sp_result = SP_LoadMiscData(m_hDUT, u32Base, rbuff, MISCDATA_FOR_APPID_LENGTH, TIMEOUT_3S);
	if (SP_OK != sp_result)
	{
		NOTIFY("CheckAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LoadCustomerMiscData");
		return sp_result;
	}
	NOTIFY("CheckAppid", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "LoadCustomerMiscData");
	//对比
	strDutAppid = (char*)rbuff;
	CHAR szCondition[256] = {NULL};
	sprintf_s(szCondition, bMesOnline?"DUT:[ %s ]  MES: [ %s ]":"DUT:[ %s ]  UserInput: [ %s ]", strDutAppid.data(), strAppid.data());

	if(strDutAppid != strAppid)
	{
		NOTIFY("CheckAppid", LEVEL_ITEM, 1, 0, 1, NULL, -1, "-", szCondition);
		return SP_E_SPAT_TEST_FAIL;
	}
	else
	{
		NOTIFY("CheckAppid", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", szCondition);
		return SP_OK;
	}
}
