#include "StdAfx.h"
#include "CheckManualCode.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"


IMPLEMENT_RUNTIME_CLASS(CCheckManualCode)

//////////////////////////////////////////////////////////////////////////
CCheckManualCode::CCheckManualCode(void)
{

}

CCheckManualCode::~CCheckManualCode(void)
{
}

SPRESULT CCheckManualCode::__PollAction(void)
{ 
	CHAR* ITEM = "CheckManualCode";
	CHAR szCondition[256] = { 0 };
	SPRESULT nRet = SP_OK;

	INPUT_CODES_T InputCode[BC_MAX_NUM];
	nRet = GetShareMemory(ShareMemory_My_UserInputSN, (void*)&InputCode, sizeof(InputCode));
	if (SP_OK != nRet)
	{
		NOTIFY(ITEM, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_My_UserInputSN)");
		return nRet;
	}

		
	//从用户输入中获取ManualInputCode
	INPUT_CODES_T inputManualCode;
	nRet = GetShareMemory(ShareMemory_XunRuiInputManualCode2Compare, (void*)&inputManualCode, sizeof(INPUT_CODES_T));
	if (SP_OK != nRet)
	{
		NOTIFY(ITEM, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_XunRuiInputManualCode2Compare)");
		return nRet;
	}
	int i = 0;
	for (; i < BC_MAX_NUM; i++)
	{
		if (InputCode[i].bEnable)
		{
			int nPos = InputCode[i].nMaxLength - inputManualCode.nMaxLength;
			if (strncmp((char*)&InputCode[i].szCode[nPos], inputManualCode.szCode, inputManualCode.nMaxLength))
			{
				sprintf_s(szCondition, "The last chars Comparison : % s Scan : [% s] ManualInput : [% s] ", InputCode[i].szCodeName, InputCode[i].szCode, inputManualCode.szCode);
				NOTIFY(ITEM, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szCondition);
				return SP_E_INVALID_PARAMETER;
			}
			break;
		}
	}

	sprintf_s(szCondition, "The last chars Comparison %s Scan:[ %s ]  ManualInputCode: [ %s ]", InputCode[i].szCodeName, InputCode[i].szCode, inputManualCode.szCode);
	NOTIFY(ITEM, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, szCondition);

	return SP_OK;
}
