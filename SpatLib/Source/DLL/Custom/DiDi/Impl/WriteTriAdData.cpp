#include "StdAfx.h"
#include "WriteTriAdData.h"
#include "InputBarCodes.h"
#include <atltime.h>
#include "WininetHttp.h"
#include "Shellapi.h"
#include "CLocks.h"
#include "ActionApp.h"

IMPLEMENT_RUNTIME_CLASS(CWriteTriAdData)
//////////////////////////////////////////////////////////////////////////
CWriteTriAdData::CWriteTriAdData(void)
{
	m_bTestMode = FALSE;
}

CWriteTriAdData::~CWriteTriAdData(void)
{
}

BOOL CWriteTriAdData::LoadXMLConfig(void)
{
    m_strAppKey = _W2CA(GetConfigValue(L"Option:appKey", L"0589f3729aaf486e90a0fb15df353d29"));
    if (32 != m_strAppKey.length()) // Here we fix 32, it maybe changed according to DiDi 
    {
        SendCommonCallback(L"Invalid <Option::appKey> Configuration!");
        return FALSE;
    }

    m_strAppSecret = _W2CA(GetConfigValue(L"Option:appSecret", L"c41d72d8868c4599b41107a899e8c1b3"));
    if (32 != m_strAppSecret.length()) // Here we fix 32, it maybe changed according to DiDi 
    {
        SendCommonCallback(L"Invalid <Option::appSecret> Configuration!");
        return FALSE;
    }

    strncpy_s(m_triAdData.hwVer, _W2CA(GetConfigValue(L"Option:hwVer", L"")), CopySize(m_triAdData.hwVer));

	m_bTestMode = (BOOL)GetConfigValue(L"Option:TestMode", 0);
    return TRUE;
}

SPRESULT CWriteTriAdData::__PollAction(void)
{
    CHKRESULT(GetDeviceNameFromBarCode());

    // 从KOP获取信息并对蓝牙签名
	SPRESULT sp_result = SP_OK;

	//get codes from html
	if(!GetCodesFromHTML(&m_triAdData))
	{
		return SP_E_FAIL;
	}

	//write codes to DUT
	sp_result = SaveTriAdData(m_triAdData);
	if(sp_result != SP_OK)
	{
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Write codes to DUT failed!");
		return sp_result;
	}
	else
	{
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 1, 1, NULL, -1, NULL, "SaveTriAdData");
	}

	return SP_OK;
}

SPRESULT CWriteTriAdData::SaveTriAdData(CONST TRIAD_SIGN& triAd)
{
    SPRESULT res = SP_SaveMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, (LPCVOID)&triAd, sizeof(triAd), TIMEOUT_3S);
    if (SP_OK != res)
    {
        return res;
    }
    
    Sleep(200);

    TRIAD_SIGN tmp;
    res = SP_LoadMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, (LPVOID)&tmp, sizeof(tmp), TIMEOUT_3S);
    if (SP_OK != res)
    {
        return res;
    }

    if (0 == memcmp(&triAd, &tmp, sizeof(triAd)))
    {
        return SP_OK;
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: Compare the data saved and loaded are different!", __FUNCTION__);
        return SP_E_PHONE_SEND_DATA;
    }
}


SPRESULT CWriteTriAdData::GetDeviceNameFromBarCode(void)
{
    BarCode_T barCodes[MAX_BARCODE_NUM];
    SPRESULT res = GetShareMemory(ShareMemory_DiDiInputCodes, (void* )&barCodes[0], sizeof(BarCode_T)*MAX_BARCODE_NUM);
    if (SP_OK != res)
    {
        NOTIFY("No BarCode", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
        return res;
    }

    for (INT i=0; i<MAX_BARCODE_NUM; i++)
    {
        if (0 == _stricmp(barCodes[i].szLabel, "DeviceName") && barCodes[i].bActived)
        {
            strncpy_s(m_triAdData.devName, barCodes[i].szCode, CopySize(m_triAdData.devName));
            return SP_OK;
        }
    }
   
    NOTIFY("DeviceName NotFound", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    return SP_E_INVALID_PARAMETER;
}

BOOL CWriteTriAdData::GetSign()
{
	extern CActionApp myApp;
	CLocks Lock(myApp.GetLock());

	m_strSign = "";
    CHAR szPathName[MAX_PATH] = {0};
    ::GetModuleFileNameA(NULL, szPathName, MAX_PATH);
    std::string strPath = szPathName;
	int nIndex = strPath.find_last_of("\\");
	if(nIndex != -1)
	{
		strPath = strPath.substr(0, nIndex + 1);
	}

	time_t timestamp = time(NULL);
	char szTimeInterval[32] = {0};
	sprintf_s(szTimeInterval, sizeof(szTimeInterval), "%ld000", timestamp);

	std::string strcmd = strPath + "Customized\\DiDi\\script.exe ";
	strcmd += "{\\\"mode\\\":\\\"get_triad_sign\\\",\\\"appKey\\\":\\\"";
	strcmd += m_strAppKey;
	strcmd += "\\\",\\\"appSecret\\\":\\\"";
	strcmd += m_strAppSecret;
	strcmd += "\\\",\\\"timestamp\\\":\\\"";
	strcmd += szTimeInterval;
	strcmd += "\\\",\\\"data\\\":{\\\"deviceName\\\":\\\"";
	strcmd += m_triAdData.devName;
	strcmd += "\\\"}}";

	STARTUPINFOA si = { sizeof(si) }; 
	PROCESS_INFORMATION pi; 

	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = FALSE; 
	BOOL bRet = ::CreateProcessA( 
		NULL,
		(LPSTR)strcmd.c_str(), 
		NULL, 
		NULL, 
		FALSE, 
		0, 
		NULL, 
		NULL, 
		&si, 
		&pi); 

	if(!bRet) 
	{ 
		int nErr = GetLastError();
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Execute script.exe failed(ErrorCode: %d)!", nErr);
		return FALSE;
	} 

	if(WAIT_OBJECT_0 != WaitForSingleObject(pi.hProcess, 1000))
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Execute script.exe timeout!");
		return FALSE;
	}

	//read sign from result.txt
	strPath += "result.txt";
	FILE *fp = NULL;
	fopen_s(&fp, strPath.c_str(), "r");
	char szLine[4096] = {0};
	if(fp != NULL)
	{
		fgets(szLine, 4096, fp);
		std::string strLine = szLine;

		m_strTimestamp = GetItemString(strLine, "timestamp");
		if(m_strTimestamp != szTimeInterval)
		{
			NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Timestamp is not matched!");
			return FALSE;
		}

		m_strSign = GetItemString(strLine, "sign");
		if(m_strSign.empty())
		{
			NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Get sign from result.txt failed!");
			return FALSE;
		}
	}
	else
	{
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Open result.txt failed!");
		return FALSE;
	}

	if(fp != NULL)
	{
		fclose(fp);
	}

	return TRUE;
}

BOOL CWriteTriAdData::GetBlueTooth(TRIAD_SIGN *pTriadSign)
{
	extern CActionApp myApp;
	CLocks Lock(myApp.GetLock());

	CHAR szPathName[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL, szPathName, MAX_PATH);
	std::string strPath = szPathName;
	int nIndex = strPath.find_last_of("\\");
	if(nIndex != -1)
	{
		strPath = strPath.substr(0, nIndex + 1);
	}

	time_t timestamp = time(NULL);
	char szTimeInterval[32] = {0};
	sprintf_s(szTimeInterval, sizeof(szTimeInterval), "%ld000", timestamp);

	std::string strcmd = strPath + "Customized\\DiDi\\script.exe ";
	strcmd += "{\\\"mode\\\":\\\"get_bleTid_and_bleKey\\\",\\\"appKey\\\":\\\"";
	strcmd += m_strAppKey;
	strcmd += "\\\",\\\"appSecret\\\":\\\"";
	strcmd += m_strAppSecret;
	strcmd += "\\\",\\\"timestamp\\\":\\\"";
	strcmd += szTimeInterval;
	strcmd += "\\\",\\\"data\\\":{\\\"deviceName\\\":\\\"";
	strcmd += m_triAdData.devName;
	strcmd += "\\\",\\\"deviceSecret\\\":\\\"";
	strcmd += pTriadSign->deviceSecret;
	strcmd += "\\\",\\\"productKey\\\":\\\"";
	strcmd += pTriadSign->productKey;
	strcmd += "\\\"}}";

	STARTUPINFOA si = { sizeof(si) }; 
	PROCESS_INFORMATION pi; 

	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = FALSE; 
	BOOL bRet = ::CreateProcessA( 
		NULL,
		(LPSTR)strcmd.c_str(), 
		NULL, 
		NULL, 
		FALSE, 
		0, 
		NULL, 
		NULL, 
		&si, 
		&pi); 

	if(!bRet) 
	{ 
		int nErr = GetLastError();
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Execute script.exe failed(ErrorCode: %d)!", nErr);
		return FALSE;
	} 

	if(WAIT_OBJECT_0 != WaitForSingleObject(pi.hProcess, 1000))
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Execute script.exe timeout!");
		return FALSE;
	}

	//read sign from result.txt
	strPath += "result.txt";
	FILE *fp = NULL;
	fopen_s(&fp, strPath.c_str(), "r");
	char szLine[4096] = {0};
	if(fp != NULL)
	{
		fgets(szLine, 4096, fp);
		fclose(fp);
		fp = NULL;
		std::string strLine = szLine;

		std::string strTime = GetItemString(strLine, "timestamp");
		if(strTime != szTimeInterval)
		{
			NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Timestamp is not matched!");
			return FALSE;
		}

		std::string strblueTooth = GetItemString(strLine, "bleTid");
		if(strblueTooth.empty())
		{
			NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Get bleTid from result.txt failed!");
			return FALSE;
		}
		strcpy_s(pTriadSign->bleTid, strblueTooth.c_str());

		std::string strblueToothKey = GetItemString(strLine, "bleKey");
		if(strblueToothKey.empty())
		{
			NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Get bleKey from result.txt failed!");
			return FALSE;
		}
		strcpy_s(pTriadSign->bleKey, strblueToothKey.c_str());
	}
	else
	{
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Open result.txt failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL CWriteTriAdData::GetCodesFromHTML(TRIAD_SIGN *pTriadSign)
{
	//get triad
	if(!GetSign())
	{
		return FALSE;
	}

	CWininetHttp http;
	std::string strHeader = "Accept: */*"
							"Content-Type:application/json\r\n";
	std::string strURL;
	if(m_bTestMode)
	{
		//test url
		strURL = "http://pinzhi.didichuxing.com/hm_kop_stable/gateway?";
		strURL += "&api=bh.b.o.ecu.query&apiVersion=1.0.0&appKey=e6d0a5c0e90746ab943f1b1eea647de2&appVersion=1.0.0&hwId=myhw&mobileType=iphone&osType=1&osVersion=7.1&timestamp=";
	}
	else
	{
		strURL = "http://bgkop.qingqikeji.com/gateway?";
		strURL += "&api=bh.b.o.ecu.query&apiVersion=1.0.0&appKey=0589f3729aaf486e90a0fb15df353d29&appVersion=1.0.0&hwId=myhw&mobileType=iphone&osType=1&osVersion=7.1&timestamp=";
	}

	strURL += m_strTimestamp;
	strURL += "&ttid=120&sign=";
	strURL += m_strSign;

	std::string strContent = "{\"deviceName\":\"";
	strContent += m_triAdData.devName;
	strContent += "\"}";
	std::string strRet = http.RequestJsonInfo(strURL, Hr_Post, strHeader, strContent);

	if(strRet.find("\"code\":200") == -1)
	{
		NOTIFY("SaveTriAdData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Get codes from html failed!");
		return FALSE;
	}

	//parse codes
	if(!ParseCodes(strRet, pTriadSign))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWriteTriAdData::ParseCodes(std::string strContent, TRIAD_SIGN *pTriadSign)
{
	/*
	{\"code\":200,\"data\":{\"deviceName\":\"ECD1019010100001\",\"productKey\":\"AyhLF06Knmn\",\"deviceSecret\":\"784D45656A62494D7252625757724D30\"},\"msg\":\"success\"}{\"code\":200,\"msg\":\"success\"}
	*/
	std::string strProductKey = GetItemString(strContent, "productKey");
	if(strProductKey.empty())
	{
		return FALSE;
	}
	strcpy_s(pTriadSign->productKey, strProductKey.c_str());

	std::string strDeviceSecret = GetItemString(strContent, "deviceSecret");
	if(strDeviceSecret.empty())
	{
		return FALSE;
	}
	strcpy_s(pTriadSign->deviceSecret, strDeviceSecret.c_str());

	//get blue & blue key
	if(!GetBlueTooth(pTriadSign))
	{
		return FALSE;
	}

	return TRUE;
}

std::string CWriteTriAdData::GetItemString(CONST std::string strContent, CONST std::string strItem)
{
	std::string strFind = "\"";
	strFind += strItem;
	strFind += "\":\"";
	int n = strItem.length() + 4;
	int nIndex = strContent.find(strFind); 
	if(nIndex != -1)
	{
		int nLen = strContent.length();
		std::string strTmp = strContent.substr(nIndex + n, nLen);
		nIndex = strTmp.find("\"");
		if(nIndex != -1)
		{
			return strTmp.substr(0, nIndex);
		}
        else
        {
            return "";
        }
	}
	else
	{
		return "";
	}
}