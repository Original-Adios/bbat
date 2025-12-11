#include "StdAfx.h"
#include <atlconv.h>
#include "GetOTP.h"
#include "../drv/HttpHelper.h"
#include <winhttp.h>
#include <algorithm>
#pragma comment(lib, "winhttp.lib")
using namespace std;

IMPLEMENT_RUNTIME_CLASS(CGetOTP)
//////////////////////////////////////////////////////////////////////////

CGetOTP::CGetOTP(void)
{
	m_strServerIP = "";
	m_bTriad = FALSE;
	ZeroMemory (m_szDid, sizeof(m_szDid));
	ZeroMemory (m_szKey, sizeof(m_szKey));
	ZeroMemory (m_Uid, sizeof(m_Uid));
}

CGetOTP::~CGetOTP(void)
{
}

BOOL CGetOTP::LoadXMLConfig(void)
{
    m_strServerIP = _W2CA(GetConfigValue(L"Option:OTP_IP", L""));
    if(0 == m_strServerIP.length())
	{
		return FALSE;
	}

	std::string strMode = _W2CA(GetConfigValue(L"Option:OtpMode", L""));
	if(0 == strMode.length())
	{
		return FALSE;
	}

	if (strMode.compare("Triad") == 0)
	{
		m_bTriad = TRUE;
	}else
	{
		m_bTriad = FALSE;
	}

    return TRUE;
}

//Code1:did,Code2:key,Code3:uid
SPRESULT CGetOTP::__PollAction(void)
{
	CONST CHAR ITEMNAME[] = "GetOTP";
	char szOTP_IP[128] = {0};

	strcpy_s(szOTP_IP, sizeof(szOTP_IP), m_strServerIP.c_str());
	CHKRESULT(SetShareMemory(Xiaoxun_ShareMemory_OTP_IP, (const void*)szOTP_IP, sizeof(szOTP_IP)));
	CHKRESULT(SetShareMemory(Xiaoxun_ShareMemory_OTP_MODE, (const void*)&m_bTriad, sizeof(m_bTriad)));

	//获取串码
	SPRESULT sp_result = SP_OK;
	_UNISOC_MES_CMN_CODES mes_codes;
    sp_result = GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
    if (SP_OK != sp_result)
    {
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Assigned_Codes)");
        return sp_result;
    }

	//判断wifi是否为空
	if (0 == strlen(mes_codes.sz_cmn_wifi))
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Assigned_Codes)[Wifi is null]");
        return SP_E_INVALID_PARAMETER;
	}

	//code1,code2,code3其中一个为空，根据wifi获取小米元组
	if (0 == strlen(mes_codes.sz_cmn_code1) 
		|| 0 == strlen(mes_codes.sz_cmn_code2)
		|| (TRUE == m_bTriad ? FALSE : (0 == strlen(mes_codes.sz_cmn_code3))))
	{
		std::string strMsg = "";
		if(!GetOTP(&mes_codes, strMsg))
		{
			NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strMsg.c_str());
			return SP_E_INVALID_PARAMETER;
		}
		CHKRESULT(SetShareMemory(ShareMemory_MES_Assigned_Codes, (const void*)&mes_codes, sizeof(mes_codes)));
	}
		
	//
	NOTIFY(ITEMNAME, LEVEL_UI,1, 1, 1, NULL, -1, NULL, "OTD DID: %s_KEY: %s_UID: %s",mes_codes.sz_cmn_code1, mes_codes.sz_cmn_code2, mes_codes.sz_cmn_code3);

    return SP_OK;
}
//OTP请求
/*****************************************************************************
1,获取三元组URL有变化
三元组为:http://server/getmac/{MAC地址}
四元组为:http://server/getmacuid/{MAC地址}
2,获取三元组返回值有变化(简单讲就是三元组比四元组少了一组UID数据),具体格式如下
三元组格式为:{MAC地址6 byte}{模组编号DID 8byte }{KEY 16 byte}
四元组格式为:{MAC地址6 byte}{模组编号DID 8byte }{KEY 16 byte}{ P2P UID 20 byte}

实际操作如下:
三元组获取:
浏览器输入:http://10.246.101.2/getmac/50EC50FFF329
web返回如下:
OK,50EC50 FFF32900 0000001181CC5C6F 7A46766D6131556C4579624573794F
					293719132        ozFvma1UiEybEsyo

四元组获取:
浏览器输入:http://10.246.101.2/getmacuid/F0B429035F33
web返回如下:
OK,F0B429035B9F 0000000000AE2220 46507669714E697A3471386851517232 4E4D375348435639543155543231575A31313141
*****************************************************************************/
bool CGetOTP::GetOTP(_pUNISOC_MES_CMN_CODES p_mes_codes, std::string &strMsg)
{
	char szUrl[MAX_PATH] = {0};
	std::string strRecv = "";
	std::string strSend = "";
	std::string strStatus ="";
	bool bPost = false;

	if (m_bTriad)
	{
		sprintf_s(szUrl, sizeof(szUrl), "http://%s/getmac/%s",m_strServerIP.c_str(), p_mes_codes->sz_cmn_wifi);
	}
	else
	{
		sprintf_s(szUrl, sizeof(szUrl), "http://%s/getmacuid/%s",m_strServerIP.c_str(), p_mes_codes->sz_cmn_wifi);
	}

	CHttpHelper httpHelper;
	if (!httpHelper.httpQuery(szUrl, "", strSend, strRecv, strStatus, bPost))
	{
		strMsg = "Fail of Query: ";
		strMsg += szUrl;
		return false;
	}
	if (0 != strStatus.compare("200"))
	{
		strMsg = "Fail of HttpQuery Status";
		return false;
	}

	replace_all(strRecv, "\"", "");
	std::string::size_type pos = strRecv.find("OK", 0);
	if (std::string::npos == pos)
	{
		strMsg = "Fail of Get OTP[OK]";
		return false;
	}

	strRecv.erase(0, pos + 5);//Delete "OKOK,"

	//返回长度判断
	if (m_bTriad)
	{
		if ((OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH + OTP_KEY_BYTE_LENGTH) != strRecv.length())
		{
			strMsg = "Fail of Length Get OTP";
			strMsg += strRecv;
			return false;
		}
	}
	else
	{
		if ((OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH + OTP_KEY_BYTE_LENGTH + OTP_UID_BYTE_LENGTH) != strRecv.length())
		{
			strMsg = "Fail of Length Get OTP";
			return false;
		}
	}

	//需要值
	//did为long类型
	std::string strTemp = strRecv.substr(OTP_WIFI_BYTE_LENGTH, OTP_DID_BYTE_LENGTH);
	int nDID = (int)strtol(strTemp.c_str(), NULL, 16);
	sprintf_s(p_mes_codes->sz_cmn_code1, sizeof(p_mes_codes->sz_cmn_code1), "%d", nDID);//did


	char szSep[OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH + OTP_KEY_BYTE_LENGTH + OTP_UID_BYTE_LENGTH] = {0};
	//key 为hex字符串
	strTemp = strRecv.substr(OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH, OTP_KEY_BYTE_LENGTH);
	int index = 0;
	for (std::string::size_type i = 0; i < strTemp.length(); i += 2)
	{
		std::string str =strTemp.substr(i, 2);
		szSep[index] = (char)strtol(str.c_str(), NULL, 16);
		index++;
	}
	szSep[strTemp.length() / 2] = '\0';
	sprintf_s(p_mes_codes->sz_cmn_code2, sizeof(p_mes_codes->sz_cmn_code2), "%s", szSep);//key

	if (m_bTriad)//三元组没有UID
	{
		memset(p_mes_codes->sz_cmn_code3, 0, sizeof(p_mes_codes->sz_cmn_code3));
	}
	else
	{
		memset(szSep, 0, sizeof(szSep));
		////uid 为hex字符串
		strTemp = strRecv.substr(OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH + OTP_KEY_BYTE_LENGTH, OTP_UID_BYTE_LENGTH);
		index = 0;
		for (std::string::size_type i = 0; i < strTemp.length(); i += 2)
		{
			std::string str =strTemp.substr(i, 2);
			szSep[index] = (char)strtol(str.c_str(), NULL, 16);
			index++;
		}
		szSep[strTemp.length() / 2] = '\0';
		sprintf_s(p_mes_codes->sz_cmn_code3, sizeof(p_mes_codes->sz_cmn_code3), "%s", szSep);//key
	}

	return true;
}

