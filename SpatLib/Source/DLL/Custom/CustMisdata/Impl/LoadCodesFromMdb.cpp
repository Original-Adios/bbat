#include "StdAfx.h"
#include "LoadCodesFromMdb.h"
#include  <io.h>
#include "..\..\..\NonRF\Provision\DBHelper.h"

#include "ActionApp.h"


IMPLEMENT_RUNTIME_CLASS(CLoadCodesFromMdb)
//////////////////////////////////////////////////////////////////////////
CLoadCodesFromMdb::CLoadCodesFromMdb(void)
{
	m_strBatchName = L"";
	m_strMdbPath = L"";
}

CLoadCodesFromMdb::~CLoadCodesFromMdb(void)
{
}

BOOL CLoadCodesFromMdb::LoadXMLConfig(void)
{
	m_strBatchName = GetConfigValue(L"Option:BatchName", L"");
	if (0 == m_strBatchName.length())
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid BatchName");
		return FALSE;
	}
	std::wstring strMDB = GetConfigValue(L"Option:MdbFilePatch", L"");	

	std::wstring strPath = GetAbsoluteFilePathW(strMDB.c_str());
	if (0 == strPath.length())
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid Access Mdb File");
		return FALSE;
	}
	else
	{
		m_strMdbPath = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + strPath;
	}

	return TRUE;
}

SPRESULT CLoadCodesFromMdb::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "LoadCodesFromMdb";
	INPUT_CODES_T inputBarCodes[BC_MAX_CUSTMIZENUM];

	CHKRESULT_WITH_NOTIFY(GetShareMemory(ShareMemory_My_UserInputSN, (void* )&inputBarCodes, sizeof(inputBarCodes)), "GetShareMemory(InputCode)");

	if (!(inputBarCodes[BC_SN1].bEnable || inputBarCodes[BC_IMEI1].bEnable)) 
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Barcode Input SN1 or IMEI1");
		return SP_E_INVALID_PARAMETER;
	}

	CDBHelper db(m_strMdbPath, GetISpLogObject());

	char szSqlCmd[1024] = {0};
	sprintf_s(szSqlCmd, sizeof(szSqlCmd), "SELECT * FROM [IMEI] WHERE (SN1 = '%s' OR '%s' = '') AND (IMEI1 = '%s' OR '%s' = '')", 
		inputBarCodes[BC_SN1].szCode, inputBarCodes[BC_SN1].szCode,
		inputBarCodes[BC_IMEI1].szCode, inputBarCodes[BC_IMEI1].szCode);
	std::string strResult = "";
	if (!db.QueryItem(szSqlCmd, strResult))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "db.QueryItem");
		return SP_E_INVALID_PARAMETER;
	}
	
	//分离custIMEI.mdb值到inputBarCodes
	//ID BatchName IMEI1   IMEI2   IMEI3   IMEI4   BT  WIFI    SN1 SN2 TIME    MEID1   MEID2   ENETMAC Cust1   Cust2   Cust3   Cust4   Cust5   Cust6   Cust7   Cust8   Cust9   Cust10  Cust11  Cust12
	int k = 0;
	std::string::size_type pos = 0;
	while ((pos = strResult.find(",", 0)) != std::string::npos)
	{  
		std::string strTemp = strResult.substr(0, pos);
		strResult.erase(0, pos + 1);
		if (0 == strTemp.length() || 0 == k || 10 == k)
		{
			k++;
			continue;
		}
		else if (1 == k)
		{
			if (m_strBatchName.compare(_A2CW(strTemp.c_str())))
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "BatchName Isn't Equal [%s-%s]", _W2CA(m_strBatchName.c_str()), strTemp.c_str());
				return SP_E_INVALID_PARAMETER;
			}
		}
		else if (2 == k)
		{
			inputBarCodes[BC_IMEI1].bEnable = 1;
			memcpy(inputBarCodes[BC_IMEI1].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (3 == k)
		{
			inputBarCodes[BC_IMEI2].bEnable = 1;
			memcpy(inputBarCodes[BC_IMEI2].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (4 == k)
		{
			inputBarCodes[BC_IMEI3].bEnable = 1;
			memcpy(inputBarCodes[BC_IMEI3].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (5 == k)
		{
			inputBarCodes[BC_IMEI4].bEnable = 1;
			memcpy(inputBarCodes[BC_IMEI4].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (6 == k)
		{
			inputBarCodes[BC_BT].bEnable = 1;
			replace_all(strTemp, ":", "");
			memcpy(inputBarCodes[BC_BT].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (7 == k)
		{
			inputBarCodes[BC_WIFI].bEnable = 1;
		    replace_all(strTemp, ":", "");
			memcpy(inputBarCodes[BC_WIFI].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (8 == k)
		{
			inputBarCodes[BC_SN1].bEnable = 1;
			memcpy(inputBarCodes[BC_SN1].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (9 == k)
		{
			inputBarCodes[BC_SN2].bEnable = 1;
			memcpy(inputBarCodes[BC_SN2].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (11 == k)
		{
			inputBarCodes[BC_MEID1].bEnable = 1;
			memcpy(inputBarCodes[BC_MEID1].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (12 == k)
		{
			inputBarCodes[BC_MEID2].bEnable = 1;
			memcpy(inputBarCodes[BC_MEID2].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (13 == k)
		{
			inputBarCodes[BC_ENETMAC].bEnable = 1;
			memcpy(inputBarCodes[BC_ENETMAC].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (14 == k)
		{
			inputBarCodes[BC_CUST_1].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_1].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (15 == k)
		{
			inputBarCodes[BC_CUST_2].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_2].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (16 == k)
		{
			inputBarCodes[BC_CUST_3].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_3].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (17 == k)
		{
			inputBarCodes[BC_CUST_4].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_4].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (18 == k)
		{
			inputBarCodes[BC_CUST_5].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_5].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (19 == k)
		{
			inputBarCodes[BC_CUST_6].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_6].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (120 == k)
		{
			inputBarCodes[BC_CUST_7].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_7].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (21 == k)
		{
			inputBarCodes[BC_CUST_8].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_8].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (22 == k)
		{
			inputBarCodes[BC_CUST_9].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_9].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (23 == k)
		{
			inputBarCodes[BC_CUST_10].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_10].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (24 == k)
		{
			inputBarCodes[BC_CUST_11].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_11].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}
		else if (25 == k)
		{
			inputBarCodes[BC_CUST_12].bEnable = 1;
			memcpy(inputBarCodes[BC_CUST_12].szCode, strTemp.c_str(), sizeof(CHAR)*(BARCODEMAXLENGTH + 1));
		}

		k++;
	}
	//兼用CInputCustBarCodes输入
	// Set share memory
	CHKRESULT(SetShareMemory(ShareMemory_My_UserInputSN, (const void *)&inputBarCodes[0], sizeof(INPUT_CODES_T) * BC_MAX_NUM));
	CHKRESULT(SetShareMemory(ShareMemory_My_UserInputCustSN, (const void *)&inputBarCodes[BC_CUST_1], sizeof(INPUT_CODES_T) * (BC_MAX_CUSTMIZENUM - BC_CUST_1)));

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}

