#include "StdAfx.h"
#include "WriteNetCodes.h"

IMPLEMENT_RUNTIME_CLASS(CWriteNetCodes)
//////////////////////////////////////////////////////////////////////////
CWriteNetCodes::CWriteNetCodes(void)
{
}

CWriteNetCodes::~CWriteNetCodes(void)
{
}



SPRESULT CWriteNetCodes::__PollAction(void)
{ 
	CONST UINT8 MAX_ITEM_NUM = 5;
	CONST UINT8 MIN_ONENET_LENGTH = 69;
	CONST CHAR ITEM_NAME[] = "WriteNetCodes";
	INPUT_CODES_T inputBarCodes[BC_MAX_CUSTMIZENUM - BC_CUST_1];

	std::string strOneNetCode = "";
	std::string strSeps = "_";
	int nLength = 0;

	if (SP_OK != GetShareMemory(ShareMemory_My_UserInputCustSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
	{
		return SP_E_FAIL;
	}
	for (UINT i = 0; i < MAX_ITEM_NUM; i++)
	{
		if (!inputBarCodes[i].bEnable/* || inputBarCodes[i].nMaxLength != strlen(inputBarCodes[i].szCode)*/) 
		{
			LogFmtStrA(SPLOGLV_DATA, "Input %s:%s Length:%d isn't set %d Fail!", inputBarCodes[i].szCodeName, inputBarCodes[i].szCode, strlen(inputBarCodes[i].szCode), inputBarCodes[i].nMaxLength);
			return SP_E_INVALID_PARAMETER;
		};
		//strOneNetCode的拼接格式，比如aaa_bbb_ccc_ddd_eee
		strOneNetCode = strOneNetCode + inputBarCodes[i].szCode + (i != MAX_ITEM_NUM-1 ? strSeps:"");
	}
	nLength = strOneNetCode.length();

	if(nLength < MIN_ONENET_LENGTH)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "[%s] Length[%d] isn't set [%d]", strOneNetCode.c_str(), nLength, MIN_ONENET_LENGTH);
		return SP_E_INVALID_PARAMETER;
	}

	UINT8 _nCount = 0;
	std::string::size_type _nPos = 0;
	while((_nPos = strOneNetCode.find(strSeps, _nPos)) != std::string::npos)
	{
		_nCount++;
		_nPos = _nPos + strSeps.length();
	}

	if(_nCount != MAX_ITEM_NUM-1)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Net Codes isn't match as aaa_bbb_ccc_ddd_eee");
		return SP_E_INVALID_PARAMETER;
	}

	SPRESULT sRet = WriteCustMisdata(MISCDATA_BASE_ANDROID_OFFSET, strOneNetCode.c_str(), strOneNetCode.length(), ESYS_Android);
	if (SP_OK != sRet)
	{
		return sRet;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "OneNetCode:%s", strOneNetCode.c_str());
    return SP_OK;
}
