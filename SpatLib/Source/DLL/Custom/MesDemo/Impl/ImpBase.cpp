#include "StdAfx.h"
#include "ImpBase.h"

#include <WinSock.h>
#pragma comment(lib,"ws2_32.lib")
//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{

	ZeroMemory(m_szToolName, sizeof(m_szToolName));
	ZeroMemory(m_szToolVersion, sizeof(m_szToolVersion));
	ZeroMemory(m_szMac, sizeof(m_szMac));
	ZeroMemory(m_szIp, sizeof(m_szIp));
}

CImpBase::~CImpBase(void)
{

}

SPRESULT CImpBase::GetMesDriverFromShareMemory(void)
{
	SPRESULT res = GetShareMemory(ShareMemory_MesDriver, (void* )&m_pMesDrv, sizeof(m_pMesDrv));
	if (SP_OK == res)
	{
		if (NULL == m_pMesDrv)
		{
			res = SP_E_POINTER;
		}
	}
	return res;
}

SPRESULT CImpBase::SetMesDriverIntoShareMemory(void)
{
	return SetShareMemory(ShareMemory_MesDriver, (const void* )&m_pMesDrv, sizeof(m_pMesDrv),IContainer::System);
}

SPRESULT CImpBase::SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut /* = TIMEOUT_3S */)
{
	CHAR   szRevBuf[4096] = {0};
	uint32 u32RevSize = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, lpszCmd, TRUE, szRevBuf, sizeof(szRevBuf), &u32RevSize, u32TimeOut);
	if (SP_OK != res)
	{
		return res;
	}

	if (NULL != strstr(szRevBuf, "CME ERROR"))
	{
		return SP_E_PHONE_AT_EXECUTE_FAIL;
	}

	m_strRevBuf = szRevBuf;
	replace_all(m_strRevBuf, "\r", "");
	replace_all(m_strRevBuf, "\n", "");
	replace_all(m_strRevBuf, "OK", "");

	return SP_OK;
}

SPRESULT CImpBase::MES_GetSN(char*pszSN,int iSize)
{
	SPRESULT sp_result = SP_OK;
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	char szSN[ShareMemory_SN_SIZE] = {0};
	//获取SN1（手机中读取的SN1）
	sp_result = GetShareMemory(ShareMemory_SN1, (void* )szSN, sizeof(szSN));
	if (SP_OK != sp_result || 0 == strlen(szSN))
	{
		//获取SN1（InputCodes扫描获取）
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
			return sp_result;
		}
		if (0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory(SN1 Length == 0)");
			NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
		strcpy_s(szSN,sizeof(szSN),m_InputSN[BC_SN1].szCode);
	}
	strcpy_s(pszSN,iSize,szSN);
	return SP_OK;
}

SPRESULT CImpBase::Get_Tool_Info(LPSTR lpwToolName, LPSTR lpwToolVersion)
{
	//std::wstring strVersion;
	struct st_Language 
	{
		WORD wLanguageID;
		WORD wCodePage;
	};
	char szPath[512]={0};
	GetModuleFileNameA(NULL, szPath, sizeof(szPath));  
	DWORD dwSize = GetFileVersionInfoSizeA(szPath, NULL);
	if (dwSize>0)
	{
		LPBYTE pBlock = (BYTE*)malloc(dwSize);
		if (GetFileVersionInfoA(szPath, 0, dwSize, pBlock))
		{
			char *pszValue = NULL;
			UINT uiLen1 = 0;
			VerQueryValueA(pBlock, "VarFileInfo\\Translation", (LPVOID*)&pszValue, &uiLen1);
			if (uiLen1 > 0)
			{
				st_Language stlang = *((st_Language*)pszValue);
				char szSubBlock[MAX_PATH] = {0};
				char szTranslation[128] = {0};

				UINT uiLen2 = 0;
				sprintf_s(szTranslation, "%04x%04x", stlang.wLanguageID, stlang.wCodePage);
				sprintf_s(szSubBlock, "\\StringFileInfo\\%s\\OriginalFilename", szTranslation);
				LPSTR pToolName = NULL;
				VerQueryValueA(pBlock, szSubBlock,(LPVOID*)&pToolName, &uiLen2);

				LPSTR pToolVersion = NULL;
				sprintf_s(szSubBlock, "\\StringFileInfo\\%s\\ProductVersion", szTranslation);
				VerQueryValueA(pBlock, szSubBlock,(LPVOID*)&pToolVersion, &uiLen2);


				std::string strVer = pToolVersion;
				replace_all(strVer, " ", "");
				replace_all(strVer, ",", ".");
				int iLastDot = strVer.rfind(".", std::string::npos);

				std::string strVersion = strVer.substr(0, iLastDot) + strVer.substr(iLastDot + 1);
				memcpy(lpwToolVersion, strVersion.c_str(), sizeof(char)*strVersion.length());
				memcpy(lpwToolName, pToolName, sizeof(char) * strlen(pToolName));

				free(pBlock);
				return SP_OK;
			}
		}
		free(pBlock); 
		return SP_E_FAIL;
	}
	else
	{
		return SP_E_FAIL;
	}
}

///regex来提取MAC
BOOL CImpBase::ParseMac(LPCSTR lpcFile, LPCSTR lpcIP, LPSTR lpMacOut)
{
	std::string	strMsg;
	ifstream fsdata;

	size_t iImeiSize  = 0;
	LPSTR lpImeiData = NULL;

	fsdata.open(lpcFile, ios::in);
	if (!fsdata)
	{
		return false;
	}
	fsdata.seekg(0, ios::end);
	iImeiSize = (size_t)fsdata.tellg();
	fsdata.seekg(0, ios::beg);
	lpImeiData = (char *)malloc(iImeiSize);
	if (!lpImeiData)
	{
		fsdata.close();
		return false;
	}
	memset(lpImeiData, 0, iImeiSize);
	fsdata.read(lpImeiData, iImeiSize);
	fsdata.close();


	LPSTR lpStr1 = lpImeiData;
	LPSTR lpStr2 = NULL;
	lpStr2 = strstr(lpStr1, lpcIP);
	if (NULL == lpStr2)
	{
		return false; 
	}

	int nPos = lpStr2 - lpStr1;
	lpStr1[nPos] = '\0';

	const static regex expression("([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})",regex::icase);
	cmatch what;
	BOOL bRet = FALSE;
	while (NULL != lpStr2)
	{
		if(std::regex_search(lpStr1, what, expression))
		{
			std::string str;
			str += what[1];
			str += "-";
			str += what[2];
			str += "-";
			str += what[3];
			str += "-";
			str += what[4];
			str += "-";
			str += what[5];
			str += "-";
			str += what[6];

			memcpy(lpMacOut, str.data(), str.length()*sizeof(char));
			lpStr2 = strstr(lpStr1, lpMacOut);
			lpStr1 = lpStr2 + strlen(lpMacOut);
			bRet = TRUE;
		}
		else
		{
			break;
		}
	}

	free(lpImeiData);
	lpImeiData = NULL;

	return bRet;
}


BOOL CImpBase::GetHostMAC(LPSTR lpMac, LPCSTR lpcIP)
{
	ShellExecuteA (NULL, "open", "cmd.exe", "/C ipconfig -all >c://NetConfig.txt", NULL, SW_HIDE);

	Sleep(1000);
	if (!ParseMac("c://NetConfig.txt", lpcIP, lpMac))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CImpBase::GetHostIP(LPSTR lpIP)
{
	//1.初始化wsa
	WSADATA wsaData;  
	int ret = WSAStartup(MAKEWORD(2,2), &wsaData);  
	if (0 != ret)  
	{  
		return FALSE;  
	}  
	//2.获取主机名  
	char hostname[256];  
	ret = gethostname(hostname, sizeof(hostname));  
	if (SOCKET_ERROR == ret)  
	{  
		return FALSE;  
	}  
	//3.获取主机ip  
	HOSTENT * host = gethostbyname(hostname);  
	if (NULL == host)  
	{  
		return FALSE;  
	}  
	//4.转化为char*并拷贝返回  
	std::string strIP = inet_ntoa(*(in_addr*)*host->h_addr_list);
	memcpy(lpIP, strIP.c_str(), sizeof(char)* strIP.length());
	lpIP[strIP.length()] = '\0';
	return TRUE;  
}