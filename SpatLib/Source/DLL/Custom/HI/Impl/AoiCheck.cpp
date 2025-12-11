#include "StdAfx.h"
#include "AoiCheck.h"
#include "ICallHiMesDll.h"


IMPLEMENT_RUNTIME_CLASS(CAoiCheck)
//////////////////////////////////////////////////////////////////////////
CAoiCheck::CAoiCheck(void)
{
}

CAoiCheck::~CAoiCheck(void)
{
}

SPRESULT CAoiCheck::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "AoiCheck";
	INPUT_CODES_T inputBarCodes[BC_MAX_NUM];
	CHAR szSN1[SP15_MAX_SN_LEN] = {0};
	uint32 uLength = 0;

	do
	{
		if (SP_OK == GetShareMemory(ShareMemory_My_UserInputSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
		{
			if(0 != strlen(inputBarCodes[BC_SN1].szCode))
			{
				uLength = strlen(inputBarCodes[BC_SN1].szCode);
				memcpy_s(szSN1, sizeof(szSN1), inputBarCodes[BC_SN1].szCode, uLength > CopySize(szSN1) ? CopySize(szSN1) : uLength);
				break;
			}
		}

		if (SP_OK == GetShareMemory(ShareMemory_SN1, (void* )szSN1, sizeof(szSN1)))
		{
			if(0 != strlen(szSN1))
			{
				break;
			}
		}

		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get SN1 fail");
		return SP_E_PHONE_INVALID_SN_LENGTH;
#pragma warning(disable:4127)
	}while(false);
#pragma warning(default:4127)

	HANDLE *hHiMes;
	if (FALSE == CreateMesDllObject(GetISpLogObject(), &hHiMes))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "HiMES HANDLE == NULL");
		return SP_E_FAIL;
	}

	CHKRESULT(SetShareMemory(ShareMemory_HiMesHandle, (const void*)&hHiMes, sizeof(hHiMes)));


	if (MES_SUCCESS != HiAoiCheck(hHiMes, szSN1))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", NULL == hHiMes ? "Mes Connect fail" : "");
		return SP_E_MES_ERROR;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    return SP_OK;
}
