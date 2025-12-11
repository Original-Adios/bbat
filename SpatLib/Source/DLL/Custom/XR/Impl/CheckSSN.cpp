#include "StdAfx.h"
#include "CheckSSN.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "timo_crypt.h"
//#include "ExtraLogFile.h"


IMPLEMENT_RUNTIME_CLASS(CCheckSSN)

//////////////////////////////////////////////////////////////////////////
CCheckSSN::CCheckSSN(void)
{

}

CCheckSSN::~CCheckSSN(void)
{
}

BOOL CCheckSSN::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CCheckSSN::__PollAction(void)
{ 
	char szItem[32] = "CheckSSN";
	char szMesSSN[MISCDATA_FOR_SSN_LENGTH];
	//MES online才能对比SSN
	SPRESULT sp_result = SP_OK;
	MES_RESULT mesResult = UnisocMesActive();
	if(MES_SUCCESS != mesResult)
	{
		NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Can not campare SSN when MES Offline");
		return SP_E_FAIL;
	}
	//获取平台绑定的SSN(扩展字段code1)
	_UNISOC_MES_CMN_CODES mes_codes;
	sp_result = GetShareMemory(ShareMemory_MES_Device_Codes, (void* )&mes_codes, sizeof(mes_codes));
	if (SP_OK != sp_result)
	{
		NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Device_Codes)");
		return sp_result;
	}
	strcpy_s(szMesSSN,MISCDATA_FOR_SSN_LENGTH,mes_codes.sz_cmn_code1);
	//读取手机端Appid
	uint8 rbuff[MISCDATA_FOR_SSN_LENGTH] = {0};
    uint32 u32Base = MISCDATA_CUSTOMER_OFFSET;
    sp_result = SP_LoadMiscData(m_hDUT, u32Base, rbuff, MISCDATA_FOR_SSN_LENGTH, TIMEOUT_3S);
	if (SP_OK != sp_result)
	{
		NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LoadCustomerMiscData");
		return sp_result;
	}
	NOTIFY(szItem, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "LoadCustomerMiscData");
	//对比
	CHAR szCondition[256] = {NULL};
	sprintf_s(szCondition, "DUT:[ %s ]  MES: [ %s ]", (char*)rbuff, szMesSSN);
	if(0 != strcmp(szMesSSN,(char*)rbuff))
	{
		NOTIFY(szItem, LEVEL_ITEM, 1, 0, 1, NULL, -1, "-", szCondition);
		return SP_E_SPAT_TEST_FAIL;
	}
	else
	{
		NOTIFY(szItem, LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", szCondition);
		return SP_OK;
	}
}
