#pragma once
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlbase.h>
#include <atlstr.h>
#include <string>
using namespace std;

#define BUF_SIZE (1024*100)
#define USE_WINHTTP //Comment this line to user wininet.

#ifdef USE_WINHTTP
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#endif

class CHttpHelper
{
public:
	CHttpHelper();
	virtual ~CHttpHelper(void);

	BOOL HttpConnectWeb(string strUrl,string strHeaders,string strSend,string&strRecv,string&strStatus,bool bPostOrGet);
 	BOOL httpOpen(string strUrl,string&strRecv);
 	BOOL httpQuery(string strUrl,string strHeaders,string strSend,string&strRecv,string&strStatus,bool bPostOrGet);
 	BOOL httpClose();

	void	httpSetHandleLog(HANDLE);
private:
	HINTERNET	HTTP_Open(LPCWSTR pwszAgent);
	HINTERNET	HTTP_Connect(HINTERNET hSession, LPCWSTR pwszAddr, int iPortNo);
	HINTERNET	HTTP_OpenRequest(HINTERNET hConnect, LPCWSTR pwszVerb, LPCWSTR pwszObjectName, int iScheme);
	BOOL			HTTP_CrackUrl(LPCWSTR pwszUrl,URL_COMPONENTS&url_com);
	BOOL			HTTP_SetTimeouts(HINTERNET hRequest,int iResolveTimeout,int iConnectTimeout,int iSendTimeout, int iReceiveTimeout);
	BOOL			HTTP_AddRequestHeaders(HINTERNET hRequest, LPCWSTR pwszHeader);
	BOOL			HTTP_SendRequest(HINTERNET hRequest,  LPVOID lpOptional, DWORD dwOptionalLength);
	BOOL			HTTP_EndRequest(HINTERNET hRequest);
	BOOL			HTTP_QueryInfo(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
	BOOL			HTTP_ReadData(HINTERNET hRequest, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
	void			HTTP_CloseHandle(HINTERNET hHandle);

	BOOL			HTTP_SetOption(HINTERNET hRequest, DWORD dwOptionType, DWORD dwFlags);
	
	static HINTERNET m_hSession;
	static HINTERNET m_hConnect;
};

