#include "StdAfx.h"
#include "WriteGuid.h"

#define MISCDATA_FOR_GUID_LENGTH 32

IMPLEMENT_RUNTIME_CLASS(CWriteGuid)
//////////////////////////////////////////////////////////////////////////
CWriteGuid::CWriteGuid(void)
{
}

CWriteGuid::~CWriteGuid(void)
{
}

SPRESULT CWriteGuid::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "WriteGuid";
	INPUT_CODES_T inputBarCodes[BC_MAX_CUSTMIZENUM - BC_CUST_1];
	std::string strGuid = "";


	if (SP_OK != GetShareMemory(ShareMemory_My_UserInputCustSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
	{
		return SP_E_FAIL;
	}

	if (!inputBarCodes[0].bEnable || MISCDATA_FOR_GUID_LENGTH != strlen(inputBarCodes[0].szCode)) 
	{
		LogFmtStrA(SPLOGLV_DATA, "Input %s:%s Length:%d isn't set %d Fail!", inputBarCodes[0].szCodeName, inputBarCodes[0].szCode, strlen(inputBarCodes[0].szCode), MISCDATA_FOR_GUID_LENGTH);
		return SP_E_INVALID_PARAMETER;
	};
	
	strGuid = inputBarCodes[0].szCode;
	SPRESULT sRet = WriteCustMisdata(MISCDATA_BASE_ANDROID_OFFSET + 50, strGuid.c_str(), strGuid.length(), ESYS_Android);
	if (SP_OK != sRet)
	{
		return sRet;
	}
	
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "GUID:%s", strGuid.c_str());
    return SP_OK;
}
