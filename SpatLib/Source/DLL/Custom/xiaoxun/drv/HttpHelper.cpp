#include "StdAfx.h"
#include "HttpHelper.h"

#define _UTF8

#ifdef _UNICODE
#define GetStringFromUtf8 GetStringWFromUtf8
#else
#define GetStringFromUtf8 GetStringAFromUtf8
#endif

struct callback_param_t
{
	HINTERNET hInet;
	DWORD dwErrCert;
};

static VOID CALLBACK Callback(HINTERNET, DWORD_PTR, DWORD, PVOID, DWORD);
static VOID CALLBACK Callback(HINTERNET inet,DWORD_PTR context,DWORD status,PVOID information,DWORD informationLength)
{
	if (NULL == context || NULL == information)
	{
		return ;
	}

	callback_param_t&p = *reinterpret_cast<callback_param_t*>(context);
	const DWORD flag = reinterpret_cast<DWORD>(information);
	if(/*(0 != context) && */(inet == p.hInet) && (WINHTTP_CALLBACK_STATUS_SECURE_FAILURE == status) && (sizeof(DWORD) == informationLength))
	{
		p.dwErrCert = flag;
	}
}

HINTERNET CHttpHelper::m_hSession = NULL;
HINTERNET CHttpHelper::m_hConnect = NULL;

CHttpHelper::CHttpHelper()
{
	m_hSession = NULL;
	m_hConnect = NULL;
}

CHttpHelper::~CHttpHelper(void)
{
}

BOOL CHttpHelper::HTTP_CrackUrl(LPCWSTR pwszUrl,URL_COMPONENTS&url_com)
{
#ifdef USE_WINHTTP
	return WinHttpCrackUrl(pwszUrl, 0, ICU_ESCAPE, &url_com);
#else
	return InternetCrackUrl(pwszUrl, 0, ICU_ESCAPE, &url_com);
#endif
}

HINTERNET CHttpHelper::HTTP_Open(LPCWSTR pwszAgent)
{
#ifdef USE_WINHTTP
	return WinHttpOpen(pwszAgent, NULL, NULL, NULL, NULL);
#else
	return InternetOpen(pwszAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
#endif
}

HINTERNET CHttpHelper::HTTP_Connect(HINTERNET hSession, LPCWSTR pwszAddr, int iPortNo)
{
#ifdef USE_WINHTTP
	return WinHttpConnect(hSession, pwszAddr, (INTERNET_PORT) iPortNo, 0);
#else
	return InternetConnect(hSession, pwszAddr, iPortNo, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
#endif
}

HINTERNET CHttpHelper::HTTP_OpenRequest(HINTERNET hConnect, LPCWSTR pwszVerb, LPCWSTR pwszObjectName, int iScheme)
{
	DWORD dwFlags = 0;
#ifdef USE_WINHTTP
	if (iScheme == INTERNET_SCHEME_HTTPS) 
	{
		dwFlags |= WINHTTP_FLAG_SECURE;
	}
	return WinHttpOpenRequest(hConnect, pwszVerb, pwszObjectName, NULL, NULL, NULL, dwFlags);
#else
	if (iScheme == INTERNET_SCHEME_HTTPS)
	{
		dwFlags |= INTERNET_FLAG_SECURE;
	}
	return HttpOpenRequest(hConnect, pwszVerb, pwszObjectName, NULL, NULL, NULL, dwFlags, 0);
#endif
}

BOOL CHttpHelper::HTTP_SetOption(HINTERNET hRequest, DWORD dwOption, DWORD dwFlag)
{
#ifdef USE_WINHTTP
	DWORD dwFlags;
	DWORD dwBuffLen = sizeof(dwFlags); 

	BOOL bRet = WinHttpQueryOption (hRequest, dwOption,
		(LPVOID)&dwFlags, &dwBuffLen);
	if (!bRet)
	{
		return bRet;
	}

	dwFlags |= dwFlag;
	
	return WinHttpSetOption (hRequest, dwOption,
		&dwFlags, sizeof (dwFlags) );
#else
	BOOL bRet = InternetQueryOption (hRequest, dwOption,
		(LPVOID)&dwFlags, &dwBuffLen);
	if (!bRet)
	{
		return bRet;
	}

	dwFlags |= dwFlag;

	return InternetSetOption (hRequest, dwOption,
		&dwFlags, sizeof (dwFlags) );
#endif
}

BOOL CHttpHelper::HTTP_AddRequestHeaders(HINTERNET hRequest, LPCWSTR pwszHeader)
{
	SIZE_T ulSize = lstrlenW(pwszHeader);
#ifdef USE_WINHTTP
	return WinHttpAddRequestHeaders(hRequest, pwszHeader, DWORD(ulSize), WINHTTP_ADDREQ_FLAG_ADD);
#else
	return HttpAddRequestHeaders(hRequest, pwszHeader, DWORD(ulSize), HTTP_ADDREQ_FLAG_ADD);
#endif
}

BOOL CHttpHelper::HTTP_SendRequest(HINTERNET hRequest,  LPVOID lpOptional, DWORD dwOptionalLength)
{
#ifdef USE_WINHTTP
		DWORD dwErr = 0;
		BOOL bOK = TRUE;
		callback_param_t callback_param;
		const WINHTTP_STATUS_CALLBACK isCallback= WinHttpSetStatusCallback(hRequest,Callback,WINHTTP_CALLBACK_FLAG_SECURE_FAILURE,0);
		if(WINHTTP_INVALID_STATUS_CALLBACK == isCallback)
		{
			dwErr = ::GetLastError();
			bOK = FALSE;
		}
		else
		{
			bOK = WinHttpSendRequest(hRequest, 0, 0, const_cast<void*>(lpOptional), dwOptionalLength, dwOptionalLength, reinterpret_cast<DWORD_PTR>(&callback_param));
			if(!bOK)
			{
				dwErr = ::GetLastError();
			}
		}
		return bOK;
#else
	return HttpSendRequest(hRequest, 0, 0, const_cast<void*>(lpOptional), dwOptionalLength);
#endif
}

BOOL CHttpHelper::HTTP_EndRequest(HINTERNET hRequest)
{
#ifdef USE_WINHTTP
	return WinHttpReceiveResponse(hRequest, 0);
#else
	return TRUE;
#endif
}

BOOL CHttpHelper::HTTP_QueryInfo(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength)
{
#ifdef USE_WINHTTP
	return WinHttpQueryHeaders(hRequest,dwInfoLevel, 0, lpBuffer, lpdwBufferLength, 0);
#else
	return HttpQueryInfo(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, 0);
#endif
}

BOOL CHttpHelper::HTTP_ReadData(HINTERNET hRequest, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead)
{
#ifdef USE_WINHTTP
	return WinHttpReadData(hRequest, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
#else
	return InternetReadFile(hRequest, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
#endif
}

BOOL CHttpHelper::HTTP_SetTimeouts(HINTERNET hRequest,int iResolveTimeout,int iConnectTimeout,int iSendTimeout, int iReceiveTimeout)
{
// 	ResolveTimeout 是 DNS 解析的超时时间
// 	ConnectTimeout 是 连接服务器的超时时间
// 	SendTimeout 是 发送数据的超时时间
// 	ReceiveTime 是 接收数据的超时时间
#ifdef USE_WINHTTP
	return WinHttpSetTimeouts(hRequest, iResolveTimeout, iConnectTimeout, iSendTimeout, iReceiveTimeout);
#else
	return TRUE;
#endif
}

void CHttpHelper::HTTP_CloseHandle(HINTERNET hHandle)
{
	if (hHandle)
	{
#ifdef USE_WINHTTP
		WinHttpCloseHandle(hHandle);
#else
		InternetCloseHandle(hHandle);
#endif
	}
}

void CHttpHelper::httpSetHandleLog(HANDLE)
{
	//m_hHandle_Log = hHandleLog;
}

BOOL CHttpHelper::httpOpen(string strUrl, string&strRecv)
{
	URL_COMPONENTS url_com = {0};
	url_com.dwStructSize = sizeof(url_com);

	const DWORD BUF_LEN = 256;

	WCHAR wszHost[BUF_LEN]={0};
	url_com.lpszHostName = wszHost;
	url_com.dwHostNameLength = BUF_LEN;

	WCHAR wszPath[BUF_LEN]={0};
	url_com.lpszUrlPath = wszPath;
	url_com.dwUrlPathLength = BUF_LEN;

	WCHAR wszExtra[BUF_LEN]={0};
	url_com.lpszExtraInfo = wszExtra;
	url_com.dwExtraInfoLength = BUF_LEN;

	m_hSession = HTTP_Open(_T("User Agent"));
	if (NULL == m_hSession) 
	{
		return FALSE;
	}

	WCHAR wszUrl[512]={0};
	MultiByteToWideChar(CP_ACP,0,strUrl.data(),strUrl.length()+1,wszUrl,sizeof(wszUrl)); 

	if (!HTTP_CrackUrl(wszUrl,url_com))
	{
		return FALSE;
	}

	m_hConnect = HTTP_Connect(m_hSession, url_com.lpszHostName, url_com.nPort);
	if (NULL==m_hConnect) 
	{
		return FALSE;
	}

	strRecv = "OK";
	return TRUE;
}

BOOL CHttpHelper::httpQuery(string strUrl, const string strHeaders, string strSend, string &strRecv, string&strStatus, bool bPostOrGet)
{
	HINTERNET hRequest = NULL;

	URL_COMPONENTS url_com = {0};
	url_com.dwStructSize = sizeof(url_com);

	const DWORD BUF_LEN = 256;
	//Server地址
	WCHAR wszHost[BUF_LEN]={0};
	url_com.lpszHostName = wszHost;
	url_com.dwHostNameLength = BUF_LEN;

	WCHAR wszPath[BUF_LEN]={0};
	url_com.lpszUrlPath = wszPath;
	url_com.dwUrlPathLength = BUF_LEN;

	WCHAR wszExtra[BUF_LEN]={0};
	url_com.lpszExtraInfo = wszExtra;
	url_com.dwExtraInfoLength = BUF_LEN;

	if (NULL == m_hSession || NULL == m_hConnect)
	{
		if (!httpOpen(strUrl,strRecv))
		{
			return FALSE;
		}
	}

	WCHAR wszUrl[512]={0};
	MultiByteToWideChar(CP_ACP,0,strUrl.data(),strUrl.length()+1,wszUrl,sizeof(wszUrl)); 

	if (!HTTP_CrackUrl(wszUrl,url_com))
	{
		return FALSE;
	}

	WCHAR wszObjectName[512]={0};
	wsprintf(wszObjectName,_T("%s%s"),url_com.lpszUrlPath,url_com.lpszExtraInfo);

	hRequest = HTTP_OpenRequest(m_hConnect, bPostOrGet? _T("POST") : _T("GET"), wszObjectName, 0);
	if (hRequest == NULL) 
	{
		return FALSE;
	}



	char szBuff[BUF_SIZE] = {0};
	WCHAR wszBuff[BUF_SIZE]={0};
	SIZE_T ulSize = BUF_SIZE;

	if (bPostOrGet)
	{	
		CString cstrSend;
		cstrSend = strSend.c_str();
		//Unicode -> UTF-8
		int iLen = WideCharToMultiByte(CP_UTF8, 0, cstrSend, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, cstrSend, -1, szBuff, iLen, NULL, NULL);
		if (!HTTP_SendRequest(hRequest,(LPVOID)szBuff,strlen(szBuff)+1)) 
		{
			return FALSE;
		}
	}
	else
	{
		if (!HTTP_SendRequest(hRequest, NULL, NULL)) 
		{
			return FALSE;
		}
	}

	if (!HTTP_EndRequest(hRequest)) 
	{
		return FALSE;
	}

#ifdef USE_WINHTTP
	int iQueryID = WINHTTP_QUERY_STATUS_CODE;
#else
	int iQueryID = HTTP_QUERY_STATUS_CODE;
#endif
	if (HTTP_QueryInfo(hRequest, iQueryID, wszBuff, &ulSize)) 
	{
		wszBuff[ulSize] = 0;
		int iLen = WideCharToMultiByte(CP_ACP,0,wszBuff,-1,NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_ACP,0,wszBuff,-1,szBuff,iLen,NULL,NULL);
		strStatus = szBuff;
	}
	else
	{
		return FALSE;
	}
	BOOL bResult = TRUE;
	char szBuffer[BUF_SIZE]={0};
	do 
	{
		ZeroMemory(szBuffer,BUF_SIZE);
		if (!HTTP_ReadData(hRequest, szBuffer, BUF_SIZE, &ulSize)) 
		{
			bResult = FALSE;
			break;
		}
		if (ulSize == 0)
		{
			bResult = TRUE;
			break;
		}
		szBuffer[ulSize] = 0;
		//UTF-8  ->  Unicode
		int iLen = MultiByteToWideChar(CP_UTF8, 0, szBuffer, BUF_SIZE, 0, 0);
		iLen = MultiByteToWideChar(CP_UTF8, 0, szBuffer, BUF_SIZE,wszBuff, iLen);
		iLen = WideCharToMultiByte(CP_ACP,0,wszBuff,-1,NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_ACP,0,wszBuff,-1,szBuffer,iLen,NULL,NULL);
		strRecv+=szBuffer;
	} while (ulSize > 0);
	HTTP_CloseHandle(hRequest);
	return bResult;
}
BOOL CHttpHelper::httpClose()
{
	if (NULL != m_hSession)
	{
		HTTP_CloseHandle(m_hSession);
		m_hSession = NULL;
	}
	if (NULL != m_hConnect)
	{
		HTTP_CloseHandle(m_hConnect);
		m_hConnect = NULL;
	}
	return TRUE;
}
BOOL CHttpHelper::HttpConnectWeb(string strUrl,string strHeaders,string strSend,string&strRecv,string&strStatus,bool bPostOrGet) 
{  
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;

	URL_COMPONENTS url_com = {0};
	url_com.dwStructSize = sizeof(url_com);

	const DWORD BUF_LEN = 256;

	WCHAR wszHost[BUF_LEN]={0};
	url_com.lpszHostName = wszHost;
	url_com.dwHostNameLength = BUF_LEN;

	WCHAR wszPath[BUF_LEN]={0};
	url_com.lpszUrlPath = wszPath;
	url_com.dwUrlPathLength = BUF_LEN;

	WCHAR wszExtra[BUF_LEN]={0};
	url_com.lpszExtraInfo = wszExtra;
	url_com.dwExtraInfoLength = BUF_LEN;

	hSession = HTTP_Open(_T("User Agent"));
	if (NULL == hSession) 
	{
		return FALSE;
	}

	WCHAR wszUrl[512]={0};
	MultiByteToWideChar(CP_ACP,0,strUrl.data(),strUrl.length()+1,wszUrl,sizeof(wszUrl)); 

	if (!HTTP_CrackUrl(wszUrl,url_com))
	{
		return FALSE;
	}

	hConnect = HTTP_Connect(hSession, url_com.lpszHostName, url_com.nPort);
	if (NULL==hConnect) 
	{
		return FALSE;
	}

	WCHAR wszObjectName[512]={0};
	wsprintf(wszObjectName,_T("%s%s"),url_com.lpszUrlPath,url_com.lpszExtraInfo);

	hRequest = HTTP_OpenRequest(m_hConnect, bPostOrGet? _T("POST") : _T("GET"), wszObjectName, url_com.nScheme);
	if (hRequest == NULL) 
	{
		return FALSE;
	}

	if (strHeaders.length() > 0)
	{
		WCHAR wszHeaders[512]={0};
		MultiByteToWideChar(CP_ACP,0,strHeaders.data(),strHeaders.length()+1,wszHeaders,sizeof(wszHeaders)); 
		if (!HTTP_AddRequestHeaders(hRequest, wszHeaders)) 
		{
			return FALSE;
		}
	}

	if (bPostOrGet)
	{
		if (!HTTP_SendRequest(hRequest, (LPVOID)strSend.data(),strSend.length())) 
		{
			return FALSE;
		}
	}
	else
	{
		if (!HTTP_SendRequest(hRequest, NULL, NULL)) 
		{
			return FALSE;
		}
	}

	if (!HTTP_EndRequest(hRequest)) 
	{
		return FALSE;
	}

#ifdef USE_WINHTTP
	int iQueryID = WINHTTP_QUERY_STATUS_CODE;
#else
	int iQueryID = HTTP_QUERY_STATUS_CODE;
#endif
	char szBuff[BUF_SIZE] = {0},szRecv[BUF_SIZE]={0};
	WCHAR wszBuff[BUF_SIZE]={0};
	SIZE_T ulSize = BUF_SIZE;
	
	if (HTTP_QueryInfo(hRequest, iQueryID, wszBuff, &ulSize)) 
	{
		wszBuff[ulSize] = 0;
		int iLen = WideCharToMultiByte(CP_ACP,0,wszBuff,-1,NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_ACP,0,wszBuff,-1,szBuff,iLen,NULL,NULL);
		strStatus = szBuff;
	}
	else
	{
		return FALSE;
	}

	do 
	{
		if (!HTTP_ReadData(hRequest, szBuff, sizeof(szBuff), &ulSize)) 
		{
			break;
		}
		if (ulSize == 0)
		{
			break;
		}
		szBuff[ulSize] = 0;
		strcat_s(szRecv,sizeof(szRecv),szBuff);
	} while (ulSize > 0);
	strRecv = szRecv;
	HTTP_CloseHandle(hRequest);
	HTTP_CloseHandle(hConnect);
	HTTP_CloseHandle(hSession);
	return TRUE;
}