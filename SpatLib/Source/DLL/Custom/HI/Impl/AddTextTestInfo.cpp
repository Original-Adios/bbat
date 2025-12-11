#include "StdAfx.h"
#include "AddTextTestInfo.h"


IMPLEMENT_RUNTIME_CLASS(CAddTextTestInfo)
//////////////////////////////////////////////////////////////////////////
CAddTextTestInfo::CAddTextTestInfo(void)
{
}

CAddTextTestInfo::~CAddTextTestInfo(void)
{
}

BOOL CAddTextTestInfo::LoadXMLConfig(void)
{
	m_strStationName = _W2CA(GetConfigValue(L"Option:StationName", L""));
	m_strFilePath = _W2CA(GetConfigValue(L"Option:FileFolder", L""));
	return TRUE;
}
SPRESULT CAddTextTestInfo::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "AddTextTestInfo";
	 
	HANDLE hHiMes  = NULL;
	SPRESULT sRet = GetShareMemory(ShareMemory_HiMesHandle, (void*)&hHiMes, sizeof(hHiMes));
 	if (SP_OK != sRet || NULL == hHiMes)
 	{
 		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", "Mes Connect fail; Pls Call AoiCheck First");
 		return SP_E_MES_ERROR;
 	}

	INPUT_CODES_T inputBarCodes[BC_MAX_NUM];
	CHAR szSN1[SP15_MAX_SN_LEN] = {0};
	CHAR szMAC[SP15_MAX_SN_LEN] = {0};
	CHAR szBT[SP15_MAX_SN_LEN] = {0};
	uint32 uLength = 0;

	do
	{
		if (SP_OK == GetShareMemory(ShareMemory_My_UserInputSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
		{
			if(0 != strlen(inputBarCodes[BC_SN1].szCode) && (0 != strlen(inputBarCodes[BC_WIFI].szCode) || 0 != strlen(inputBarCodes[BC_BT].szCode)))
			{
				uLength = strlen(inputBarCodes[BC_SN1].szCode);
				memcpy_s(szSN1, sizeof(szSN1), inputBarCodes[BC_SN1].szCode, uLength > CopySize(szSN1) ? CopySize(szSN1) : uLength);

				if(0 != strlen(inputBarCodes[BC_WIFI].szCode))
				{
					uLength = strlen(inputBarCodes[BC_WIFI].szCode);
					memcpy_s(szMAC, sizeof(szMAC), inputBarCodes[BC_WIFI].szCode, uLength > CopySize(szMAC) ? CopySize(szMAC) : uLength);
				}
				else
				{
					uLength = strlen(inputBarCodes[BC_BT].szCode);
					memcpy_s(szBT, sizeof(szBT), inputBarCodes[BC_BT].szCode, uLength > CopySize(szBT) ? CopySize(szBT) : uLength);
				}

				break;
			}
		}

		if (SP_OK == GetShareMemory(ShareMemory_SN1, (void* )szSN1, sizeof(szSN1))
			&& (SP_OK == GetShareMemory(ShareMemory_WifiMac, (void* )szMAC, sizeof(szMAC))
			|| SP_OK == GetShareMemory(ShareMemory_BtAddr, (void* )szBT, sizeof(szBT))))
		{
			if(0 != strlen(szSN1) && (0 != strlen(szMAC) ||  0 != strlen(szBT)))
			{
				break;
			}
		}

		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get SN1 or Mac fail");
		return SP_E_PHONE_INVALID_SN_LENGTH;
#pragma warning(disable:4127)
	}while(false);
#pragma warning(default:4127)

	SPTEST_RESULT_T simba_result;
	sRet  = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result,	sizeof(simba_result));
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get ItemTestResult fail");
		return sRet;
	}
	std::string strRet = (SP_OK == simba_result.errCode) ? "0" : "1";

	bool bWifi = (0 != strlen(szMAC)) ? true : false;
	std::string strMac = bWifi ? "M:" : "B:";
	strMac += bWifi ? szMAC : szBT;

	if (MES_SUCCESS != HiAddTextTestInfo(hHiMes, szSN1, m_strStationName.c_str(), strMac.c_str(), m_strFilePath.c_str(), strRet.c_str()))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
		return SP_E_MES_ERROR;
	}
	
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    return SP_OK;
}
