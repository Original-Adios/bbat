#include "StdAfx.h"
#include "CheckOTP.h"
#include "ActionApp.h"

//
IMPLEMENT_RUNTIME_CLASS(CCheckOTP)
//////////////////////////////////////////////////////////////////////////
CCheckOTP::CCheckOTP(void)
{
	m_bTriad = TRUE;
}

CCheckOTP::~CCheckOTP(void)
{
}

BOOL CCheckOTP::LoadXMLConfig(void)
{
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

SPRESULT CCheckOTP::__PollAction(void)
{
    extern CActionApp  myApp;
	CONST CHAR ITEMNAME[] = "CheckOTP";
	char szOTP[MISC_DATA_FOR_OTP] = {0};
	
	//获取平台OTP
	SPRESULT sp_result = SP_OK;
	_UNISOC_MES_CMN_CODES mes_codes;

	sp_result = GetShareMemory(ShareMemory_MES_Device_Codes, (void* )&mes_codes, sizeof(mes_codes));
    if (SP_OK != sp_result)
    {
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Device_Codes)");
        return sp_result;
    }
	
	//读取手机端OTP
	uint8 rbuff[MISC_DATA_FOR_OTP] = {0};
	uint32 u32Base =MISCDATA_CUSTOMER_OFFSET_XIAOXUN;
    uint8* lpBuff  = &rbuff[0];   

	sp_result = SP_LoadMiscData(m_hDUT, u32Base, lpBuff, MISC_DATA_FOR_OTP, TIMEOUT_3S);
	if (SP_OK != sp_result)
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LoadCustomerMiscData");
		return sp_result;
	}
	NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "LoadCustomerMiscData");
	
	//对比
	if (m_bTriad)
	{
		//手表软件没升级按四元组逻辑,使用11111111111111111111填充
		sprintf_s(szOTP, sizeof(szOTP), "%s_%s_%s_11111111111111111111", mes_codes.sz_cmn_wifi, mes_codes.sz_cmn_code1, mes_codes.sz_cmn_code2);
		szOTP[MAX_OTP_TRIAD_LENGTH] = '\0';
	}
	else
	{
		sprintf_s(szOTP, sizeof(szOTP), "%s_%s_%s_%s", mes_codes.sz_cmn_wifi, mes_codes.sz_cmn_code1, mes_codes.sz_cmn_code2, mes_codes.sz_cmn_code3);
		szOTP[MAX_OTP_TETRAD_LENGTH] = '\0';
	}

	if(0 != strncmp(szOTP, (char*)rbuff, TRUE == m_bTriad ? MAX_OTP_TRIAD_LENGTH : MAX_OTP_TETRAD_LENGTH))
	{
		NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Compare Failed");
		return SP_E_FAIL;
	}
	
	NOTIFY(ITEMNAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}




