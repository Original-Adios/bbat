#include "StdAfx.h"
#include "WriteSSN.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "timo_crypt.h"
//#include "ExtraLogFile.h"

//#ifdef _DEBUG
// #pragma comment(lib, "./Lib/cryptlib_debug.lib")
//#else
// #pragma comment(lib, "./Lib/cryptlib.lib")
//#endif 

IMPLEMENT_RUNTIME_CLASS(CWriteSSN)

#define MISCDATA_FOR_SSN_LENGTH 47
//////////////////////////////////////////////////////////////////////////
CWriteSSN::CWriteSSN(void)
{

}

CWriteSSN::~CWriteSSN(void)
{
}

BOOL CWriteSSN::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CWriteSSN::__PollAction(void)
{ 
    INPUT_CODES_T inputBarCodes[BC_MAX_NUM];
	_UNISOC_MES_CMN_CODES mes_codes;
	SPRESULT sp_result = SP_OK;


	CHAR szSN2[SP15_MAX_SN_LEN] = {0};

	//获取SN2
	do
	{
		//用户输入
		if (SP_OK == GetShareMemory(ShareMemory_My_UserInputSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
		{
			if(0!=strlen(inputBarCodes[BC_SN2].szCode))
			{
				strncpy_s(szSN2, inputBarCodes[BC_SN2].szCode, CopySize(szSN2));
				break;
			}
		}
		if (SP_OK == GetShareMemory(ShareMemory_SN2, (void* )szSN2, sizeof(szSN2)))
		{
			if(0!=strlen(szSN2))
			{
				break;
			}
		}
		//Mes系统获取
		_UNISOC_MES_CMN_CODES mes_codes;
		if (SP_OK == GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes)))
		{
			if(0 != strlen(mes_codes.sz_cmn_sn2))
			{
				strcpy_s(szSN2, mes_codes.sz_cmn_sn2);
				break;
			}
		}
		NOTIFY("WriteSSN", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get SN2 fail");
		return SP_E_FAIL;
#pragma warning(disable:4127)
	}while(false);
#pragma warning(default:4127)

	//If mesonline 获取串码

	//判断是否mes在线
	MES_RESULT mesResult = UnisocMesActive();
	if(MES_SUCCESS == mesResult)
	{
		sp_result = GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
		if (SP_OK != sp_result)
		{
			NOTIFY("WriteSSN", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Assigned_Codes)");
			return sp_result;
		}
	}

	//生成SSN
	std::string strSSN = timo_crypt::EncryptSSN(szSN2);
    char szSSN[MISCDATA_FOR_SSN_LENGTH] = {0};

	if(MISCDATA_FOR_SSN_LENGTH!=strSSN.length())
	{
		NOTIFY("WriteSSN", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get SSN fail(SSN:%s,length:%d])",strSSN.c_str(),strlen(strSSN.c_str()));
		return SP_E_FAIL;
	}
	strncpy_s(szSSN, strSSN.c_str(), MISCDATA_FOR_SSN_LENGTH - 1);
	szSSN[MISCDATA_FOR_SSN_LENGTH - 1] = '\0';//舍去最后一位
	NOTIFY("WriteSSN", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SSN:%s(length:%d)",szSSN,strlen(szSSN));

	//写入miscdata
	uint32 u32Base = MISCDATA_CUSTOMER_OFFSET;  // 写入Customer区域
    uint32 u32Size = MISCDATA_FOR_SSN_LENGTH;
    uint8*  lpBuff = (uint8*)szSSN;

    CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, u32Base, lpBuff, u32Size, TIMEOUT_3S), "SaveCustomerMiscData");

	//If mesonline 添加到串码中
	if(MES_SUCCESS == mesResult)
	{
		strcpy_s(mes_codes.sz_cmn_code1,128,szSSN);
		CHKRESULT(SetShareMemory(ShareMemory_MES_Assigned_Codes, (const void*)&mes_codes, sizeof(mes_codes)));
	}
	//
	NOTIFY("WriteSSN", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SaveCustomerMiscData");
    return SP_OK;
}
