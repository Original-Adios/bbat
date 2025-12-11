#include "StdAfx.h"
#include "WriteKudaCodes.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//#include "ExtraLogFile.h"


IMPLEMENT_RUNTIME_CLASS(CWriteKudaCodes)


//////////////////////////////////////////////////////////////////////////
CWriteKudaCodes::CWriteKudaCodes(void)
{

}

CWriteKudaCodes::~CWriteKudaCodes(void)
{
}

BOOL CWriteKudaCodes::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CWriteKudaCodes::__PollAction(void)
{ 
	INPUT_CODES_T InputAppid;
	_UNISOC_MES_CMN_CODES mes_codes;
	char KudaCode[MISCDATA_FOR_KUDA_LENGTH];
	//获取酷达Codes
	if (SP_OK != GetShareMemory(ShareMemory_XunRuiInputKuda, (void* )KudaCode, sizeof(KudaCode)))
	{
		NOTIFY("WriteKudaCodes", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get Kuda Codes fail");
		return SP_E_FAIL;
	}

	//检查酷达Codes
	if(strlen(KudaCode) < 69)
	{
		NOTIFY("WriteKudaCodes", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Length of Kuda Codes[%d] is wrong",strlen(KudaCode));
		return SP_E_FAIL;
	}
	*(KudaCode + strlen(KudaCode)) = '\0';

	//写入miscdata
	uint32 u32Base = MISCDATA_KUDA_OFFSET;  // 写入Customer区域
    uint8*  lpBuff = (uint8*)KudaCode;
    CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, u32Base, lpBuff, MISCDATA_FOR_KUDA_LENGTH, TIMEOUT_3S), "SaveCustomerMiscData");
	NOTIFY("WriteKudaCodes", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SaveCustomerMiscData");


	// 读取出来和写入的数据进行比较
	uint8 rbuff[MISCDATA_FOR_KUDA_LENGTH] = {0};
	u32Base = MISCDATA_KUDA_OFFSET; // 读取Customer区域
	lpBuff = &rbuff[0];

	CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, u32Base, lpBuff, MISCDATA_FOR_KUDA_LENGTH, TIMEOUT_3S), "LoadCustomerMiscData");
	if (0 != memcmp((uint8*)KudaCode, rbuff, strlen(KudaCode)))
	{
		NOTIFY("VerifyMiscData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Verify after write fails");
		return SP_E_FAIL;
	}
	else
	{
		NOTIFY("VerifyMiscData", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "\'%s\'",KudaCode);
	}


    return SP_OK;
}
