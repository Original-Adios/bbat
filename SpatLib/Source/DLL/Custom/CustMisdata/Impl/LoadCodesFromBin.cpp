#include "StdAfx.h"
#include "LoadCodesFromBin.h"
#include  <io.h>
#include "..\..\..\NonRF\Provision\DBHelper.h"

#include "ActionApp.h"
#include <fstream>


int key[]={1,2,3,4,5};
void MakeDecode(WCHAR *pstr, int *pkey);

IMPLEMENT_RUNTIME_CLASS(CLoadCodesFromBin)
//////////////////////////////////////////////////////////////////////////
CLoadCodesFromBin::CLoadCodesFromBin(void)
{
	m_bReadFile = false;
	m_strBinPath = L"";
	m_strBinData = L"";
}

CLoadCodesFromBin::~CLoadCodesFromBin(void)
{
}

BOOL CLoadCodesFromBin::LoadXMLConfig(void)
{
	std::wstring strBin = GetConfigValue(L"Option:BinFilePath", L"");	

	m_strBinPath = GetAbsoluteFilePathW(strBin.c_str());
	if (0 == m_strBinPath.length())
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid Bin File Path");
		return FALSE;
	}

	return TRUE;
}

SPRESULT CLoadCodesFromBin::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "LoadCodesFromBin";
	INPUT_CODES_T inputBarCodes[BC_MAX_CUSTMIZENUM];
	SPRESULT sRet = SP_OK;

	PC_PRODUCT_DATA ProductData;
	ZeroMemory(&ProductData, sizeof(PC_PRODUCT_DATA));
	ProductData.u32OperMask |= CBarCodeUtility::m_BarCodeInfo[BC_IMEI1].Mask;
	sRet = SP_LoadProductData(m_hDUT, &ProductData, TIMEOUT_3S);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Read IMEI1 fail");
		return sRet;
	}

	sRet = GetCodeFromBin(m_strBinPath.c_str());
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetCodeFromBin fail");
		return sRet;
	}

	sRet = LookupCode(_A2CW((char* )ProductData.szImei1), inputBarCodes);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LookupCode:%s fail", (char* )ProductData.szImei1);
		return sRet;
	}

	LogFmtStrA(SPLOGLV_INFO, "OldIMEI:%s, NewIMEI:%s", (char* )ProductData.szImei1, inputBarCodes[BC_IMEI1].szCode);
	//兼用CInputCustBarCodes输入
	// Set share memory
	CHKRESULT(SetShareMemory(ShareMemory_My_UserInputSN, (const void *)&inputBarCodes[0], sizeof(INPUT_CODES_T) * BC_MAX_NUM));
	CHKRESULT(SetShareMemory(ShareMemory_My_UserInputCustSN, (const void *)&inputBarCodes[BC_CUST_1], sizeof(INPUT_CODES_T) * (BC_MAX_CUSTMIZENUM - BC_CUST_1)));

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}

SPRESULT CLoadCodesFromBin::GetCodeFromBin(LPCWSTR pPath)
{
	if (m_bReadFile)
	{
		return SP_OK;
	}

	if (NULL == pPath)
	{
		LogFmtStrA(SPLOGLV_INFO, "%s%s",  _W2CA(pPath), "SP_E_FILE_NOT_EXIST");
		return SP_E_FILE_NOT_EXIST;
	}

	HANDLE hFile = CreateFileW(pPath, GENERIC_READ, FILE_SHARE_READ, \
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		LogFmtStrA(SPLOGLV_INFO, "%s", "CreateFileW");
		return SP_E_FILE_NOT_EXIST;
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);
	DWORD dwfileSize = GetFileSize(hFile, NULL);
	if (dwfileSize <= 4)///IMEI HEAD
	{
		CloseHandle(hFile);
		LogFmtStrA(SPLOGLV_INFO, "Read file: %s Size fail", _W2CA(pPath));
		return SP_E_PHONE_CRASHED_NVITEM_FILE;
	}

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	WCHAR *pFileData;
	pFileData = (WCHAR *)malloc(dwfileSize + sizeof(WCHAR));
	if (!pFileData)
	{
		CloseHandle(hFile);
		LogFmtStrA(SPLOGLV_INFO, "malloc:%s", "SP_E_ALLOC_MEMORY");
		return SP_E_ALLOC_MEMORY;
	}

	memset(pFileData, 0, sizeof(dwfileSize) + sizeof(WCHAR));
	DWORD dwIOSize = 0;
	if (!ReadFile(hFile, pFileData, dwfileSize, &dwIOSize, NULL) || dwfileSize != dwIOSize)
	{
		free(pFileData);
		pFileData = NULL;

		CloseHandle(hFile);
		LogFmtStrA(SPLOGLV_INFO, "%s", "ReadFile");
		return SP_E_FILE_IO;
	}

	pFileData[dwIOSize/2] = '\0';
	MakeDecode(pFileData, key);
	m_strBinData = pFileData;

	free(pFileData);
	pFileData = NULL;
	return SP_OK;
}

SPRESULT CLoadCodesFromBin::LookupCode(LPCWSTR lpckey, INPUT_CODES_T * pInputCode)
{
	std::wstring::size_type start_pos = 0;
	bool bFindByCust1 = false;
		
	while (!bFindByCust1)
	{
		std::wstring::size_type _nPos = m_strBinData.find(lpckey, start_pos);
		if (std::string::npos == _nPos)
		{
			LogFmtStrA(SPLOGLV_INFO, "find by %s fail", _W2CA(lpckey));
			return SP_E_INVALID_PARAMETER;
		}

		std::wstring::size_type _nBeforeNPos = m_strBinData.rfind('\n', _nPos);
		if (std::string::npos == _nBeforeNPos)
		{
			LogFmtStrA(SPLOGLV_INFO, "_nBeforeNPos:%s", "SP_E_INVALID_PARAMETER");
			return SP_E_INVALID_PARAMETER;
		}

		std::wstring::size_type _nBackNPos = m_strBinData.find('\n', _nPos);
		if (std::string::npos == _nBackNPos)
		{
			LogFmtStrA(SPLOGLV_INFO, "_nBackNPos:%s", "SP_E_INVALID_PARAMETER");
			return SP_E_INVALID_PARAMETER;
		}

		std::wstring strData = m_strBinData.substr(_nBeforeNPos + 1, _nBackNPos - (_nBeforeNPos + 1));
		if (0 == strData.length())
		{
			LogFmtStrA(SPLOGLV_INFO, "strData:%s", "SP_E_INVALID_PARAMETER");
			return SP_E_INVALID_PARAMETER;
		}

		//值列表
		//{L"BT",L"WIFI",L"IMEI1",L"IMEI2",L"IMEI3",L"IMEI4",L"SN1",L"SN2",L"MEID1",L"MEID2",L"ENETMAC",L"Cust1",L"Cust2",L"Cust3",L"Cust4",L"Cust5",L"Cust6",L"Cust7",L"Cust8",L"Cust9",L"Cust10",L"Cust11",L"Cust12"};
		strData += L",";//方便数据分离
		int index  = 0;
		std::wstring::size_type _nValPos = 0;
		ZeroMemory(pInputCode, sizeof(INPUT_CODES_T) * BC_MAX_CUSTMIZENUM);
		while (std::string::npos != (_nValPos = strData.find(',', 0)))
		{
			std::wstring str = strData.substr(0, _nValPos);
			strData.erase(0, _nValPos + 1);
			if (0 != str.length())
			{
				switch(index)
				{
				case CODE_BT:
					if (!pInputCode[BC_BT].bEnable)
					{
						memcpy(pInputCode[BC_BT].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_BT].bEnable = true;
					}
					break;
				case CODE_WIFI:
					if (!pInputCode[BC_WIFI].bEnable)
					{
						memcpy(pInputCode[BC_WIFI].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_WIFI].bEnable = true;
					}
					break;
				case CODE_IMEI1:
					if (!pInputCode[BC_IMEI1].bEnable)
					{
						memcpy(pInputCode[BC_IMEI1].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_IMEI1].bEnable = true;
					}
					break;
				case CODE_IMEI2:
					if (!pInputCode[BC_IMEI2].bEnable)
					{
						memcpy(pInputCode[BC_IMEI2].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_IMEI2].bEnable = true;
					}
					break;
				case CODE_IMEI3:
					if (!pInputCode[BC_IMEI3].bEnable)
					{
						memcpy(pInputCode[BC_IMEI3].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_IMEI3].bEnable = true;
					}
					break;
				case CODE_IMEI4:
					if (!pInputCode[BC_IMEI4].bEnable)
					{
						memcpy(pInputCode[BC_IMEI4].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_IMEI4].bEnable = true;
					}
					break;
				case CODE_SN1:
					if (!pInputCode[BC_SN1].bEnable)
					{
						memcpy(pInputCode[BC_SN1].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_SN1].bEnable = true;
					}
					break;
				case CODE_SN2:
					if (!pInputCode[BC_SN2].bEnable)
					{
						memcpy(pInputCode[BC_SN2].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_SN2].bEnable = true;
					}
					break;
				case CODE_MEID1:
					if (!pInputCode[BC_MEID1].bEnable)
					{
						memcpy(pInputCode[BC_MEID1].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_MEID1].bEnable = true;
					}
					break;
				case CODE_MEID2:
					if (!pInputCode[BC_MEID2].bEnable)
					{
						memcpy(pInputCode[BC_MEID2].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_MEID2].bEnable = true;
					}
					break;
				case CODE_ENETMAC:
					if (!pInputCode[BC_ENETMAC].bEnable)
					{
						memcpy(pInputCode[BC_ENETMAC].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_ENETMAC].bEnable = true;
					}
					break;
				case CODE_CUST_1:
					if (!pInputCode[BC_CUST_1].bEnable)
					{
						memcpy(pInputCode[BC_CUST_1].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_1].bEnable = true;
					}
					break;
				case CODE_CUST_2:
					if (!pInputCode[BC_CUST_2].bEnable)
					{
						memcpy(pInputCode[BC_CUST_2].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_2].bEnable = true;
					}
					break;
				case CODE_CUST_3:
					if (!pInputCode[BC_CUST_3].bEnable)
					{
						memcpy(pInputCode[BC_CUST_3].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_3].bEnable = true;
					}
					break;
				case CODE_CUST_4:
					if (!pInputCode[BC_CUST_4].bEnable)
					{
						memcpy(pInputCode[BC_CUST_4].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_4].bEnable = true;
					}
					break;
				case CODE_CUST_5:
					if (!pInputCode[BC_CUST_5].bEnable)
					{
						memcpy(pInputCode[BC_CUST_5].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_5].bEnable = true;
					}
					break;
				case CODE_CUST_6:
					if (!pInputCode[BC_CUST_6].bEnable)
					{
						memcpy(pInputCode[BC_CUST_6].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_6].bEnable = true;
					}
					break;
				case CODE_CUST_7:
					if (!pInputCode[BC_CUST_7].bEnable)
					{
						memcpy(pInputCode[BC_CUST_7].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_7].bEnable = true;
					}
					break;
				case CODE_CUST_8:
					if (!pInputCode[BC_CUST_8].bEnable)
					{
						memcpy(pInputCode[BC_CUST_8].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_8].bEnable = true;
					}
					break;
				case CODE_CUST_9:
					if (!pInputCode[BC_CUST_9].bEnable)
					{
						memcpy(pInputCode[BC_CUST_9].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_9].bEnable = true;
					}
					break;
				case CODE_CUST_10:
					if (!pInputCode[BC_CUST_10].bEnable)
					{
						memcpy(pInputCode[BC_CUST_10].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_10].bEnable = true;
					}
					break;
				case CODE_CUST_11:
					if (!pInputCode[BC_CUST_11].bEnable)
					{
						memcpy(pInputCode[BC_CUST_11].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_11].bEnable = true;
					}
					break;
				case CODE_CUST_12:
					if (!pInputCode[BC_CUST_12].bEnable)
					{
						memcpy(pInputCode[BC_CUST_12].szCode, _W2CA(str.c_str()), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
						pInputCode[BC_CUST_12].bEnable = true;
					}
					break;
				default:
					break;
				}
			}

			index++;
		}

		if (CODE_MAX_CUSTMIZENUM != index)
		{
			LogFmtStrA(SPLOGLV_INFO, "%s", "data format fail");
			return SP_E_INVALID_PARAMETER;
		}

		if (pInputCode[BC_CUST_1].bEnable && 0 == strcmp(pInputCode[BC_CUST_1].szCode, _W2CA(lpckey))
			&& pInputCode[BC_IMEI1].bEnable && CheckIMEIValid(pInputCode[BC_IMEI1].szCode))
		{
			bFindByCust1 = true;
		}

		start_pos += (_nBackNPos + 1);
	}
	return SP_OK;
}

#pragma warning(disable:4244)
void MakeDecode(WCHAR *pstr, int *pkey)
{
	int len = wcslen(pstr);
	for(int i = 0; i < len; i++)
	{
		pstr[i] = pstr[i] ^ pkey[i%5];
	}
}

BOOL CLoadCodesFromBin::CheckIMEIValid(LPCSTR lpIMEI)
{	
	if (0 == strlen(lpIMEI))
	{
		return FALSE;
	}

	__int64 int64IMEI =  _atoi64(lpIMEI);
	__int64 int64CD = ComputeCD(int64IMEI / 10);
	__int64 int64IMEICD = int64IMEI % 10;
	if(int64IMEICD != int64CD)
	{
		LogFmtStrA(SPLOGLV_INFO, "imei:%s invalid", lpIMEI);
		return FALSE;
	}

	return TRUE;
}


__int64 CLoadCodesFromBin::ComputeCD(__int64 nImei)
{
	int nTime = 0;
	__int64 nCD = 0;  
	while(nImei != 0)
	{
		__int64 d = nImei % 10;
		if(nTime % 2 == 0)
		{
			d *= 2;
			if(d >= 10)
			{
				d = d % 10 + d / 10;
			}
		}   
		nCD += d;   
		nTime++;
		nImei /= 10;
	}  
	nCD %= 10;
	if(nCD != 0)
	{
		nCD = 10 - nCD;
	}  
	return nCD;
}