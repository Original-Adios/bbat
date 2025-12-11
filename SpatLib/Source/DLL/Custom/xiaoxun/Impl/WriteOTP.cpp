#include "StdAfx.h"
#include "WriteOTP.h"
#include "ActionApp.h"
#include <winhttp.h>
#include <algorithm>
#include "../drv/HttpHelper.h"
using namespace std;
//
IMPLEMENT_RUNTIME_CLASS(CWriteOTP)
//////////////////////////////////////////////////////////////////////////
CWriteOTP::CWriteOTP(void)
{
}

CWriteOTP::~CWriteOTP(void)
{
}

SPRESULT CWriteOTP::__PollAction(void)
{
	CONST CHAR ITEMNAME[] = "WriteCode_OTP";
	char szOTP[MISC_DATA_FOR_OTP] = {0};
	//获取三或四元组
	BOOL bTriad = FALSE;
	SPRESULT sp_result = SP_OK;

	sp_result = GetShareMemory(Xiaoxun_ShareMemory_OTP_MODE, (void* )&bTriad, sizeof(bTriad));
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(Xiaoxun_ShareMemory_OTP_MODE)");
		return sp_result;
	}

	_UNISOC_MES_CMN_CODES mes_codes;
    sp_result = GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
    if (SP_OK != sp_result)
    {
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Assigned_Codes)");
        return sp_result;
    }
	//生成三或四元组字符串
	if(0 == strlen(mes_codes.sz_cmn_wifi) 
		|| 0 == strlen(mes_codes.sz_cmn_code1) 
		|| 0 == strlen(mes_codes.sz_cmn_code2) 
		|| (TRUE == bTriad ? FALSE : (0 == strlen(mes_codes.sz_cmn_code3))))
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get OTP Failed");
        return SP_E_INVALID_PARAMETER;	
	}

	if (bTriad)
	{
		//手表软件没升级按四元组逻辑,使用11111111111111111111填充
		sprintf_s(szOTP, sizeof(szOTP), "%s_%s_%s_%s", mes_codes.sz_cmn_wifi, mes_codes.sz_cmn_code1, mes_codes.sz_cmn_code2,"11111111111111111111");
		szOTP[MAX_OTP_TETRAD_LENGTH] = '\0';
	}
	else
	{
		sprintf_s(szOTP, sizeof(szOTP), "%s_%s_%s_%s", mes_codes.sz_cmn_wifi, mes_codes.sz_cmn_code1, mes_codes.sz_cmn_code2, mes_codes.sz_cmn_code3);
		szOTP[MAX_OTP_TETRAD_LENGTH] = '\0';
	}

	sp_result = WriteCustMisdata(MISCDATA_CUSTOMER_OFFSET_XIAOXUN, szOTP, TRUE == bTriad ? MAX_OTP_TRIAD_LENGTH : MAX_OTP_TETRAD_LENGTH);
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of WriteCustMisdata");
		return sp_result;
	}

	NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SaveCustMisdata");

	//获取OTP IP
	char szOTP_IP[128] = {0};
	sp_result = GetShareMemory(Xiaoxun_ShareMemory_OTP_IP, (void* )&szOTP_IP, sizeof(szOTP_IP));
    if (SP_OK != sp_result)
    {
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(Xiaoxun_ShareMemory_OTP_IP)");
        return sp_result;
    }
	
	//OTP标记为已下载
	if (!MacPass(szOTP_IP, mes_codes.sz_cmn_wifi))
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail Of Mark OTP");
		return SP_E_INVALID_PARAMETER;
	}

	NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Mark OTP");
	
	return SP_OK;
}

bool CWriteOTP::MacPass(char *pIp, char *pWifiMac)
{
	char szUrl[MAX_PATH] = {0};
	std::string strRecv = "";
	std::string strSend = "";
	std::string strStatus ="";
	bool bPost = FALSE;

	sprintf_s(szUrl, sizeof(szUrl), "http://%s/macpass/%s", pIp, pWifiMac);
	CHttpHelper httpHelper;
	if (!httpHelper.httpQuery(szUrl, "", strSend, strRecv, strStatus, bPost))
	{
		return FALSE;
	}
	if (0 != strStatus.compare("200"))
	{
		return FALSE;
	}

	replace_all(strRecv, "\"", "");
	NOTIFY("HttpMacPass", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRecv.c_str());
	//返回"OK"判断
	if (std::string::npos == strRecv.find("OK", 0))
	{
		return FALSE;
	}

	return TRUE;
}



