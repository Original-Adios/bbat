#include "StdAfx.h"
#include "GetChipInfo.h"

IMPLEMENT_RUNTIME_CLASS(CGetChipInfo)
//////////////////////////////////////////////////////////////////////////
CGetChipInfo::CGetChipInfo(void)
{
}

CGetChipInfo::~CGetChipInfo(void)
{
}

SPRESULT CGetChipInfo::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "GetChipInfo";	
	HANDLE hHiMes  = NULL;
	SPRESULT sRet = GetShareMemory(ShareMemory_HiMesHandle, (void*)&hHiMes, sizeof(hHiMes));
	if (SP_OK != sRet || NULL == hHiMes)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", "Mes Connect fail; Pls Call AoiCheck First");
		return SP_E_MES_ERROR;
	}

	INPUT_CODES_T inputBarCodes[BC_MAX_NUM];
	CHAR szSN1[SP15_MAX_SN_LEN] = {0};
	do
	{
		if (SP_OK == GetShareMemory(ShareMemory_My_UserInputSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
		{
			if(0 != strlen(inputBarCodes[BC_SN1].szCode))
			{
				uint32 uLength = strlen(inputBarCodes[BC_SN1].szCode);
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

	if (MES_SUCCESS != HiGetChipInfo(hHiMes, szSN1))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
		return SP_E_MES_ERROR;
	}
	
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    return SP_OK;
}
