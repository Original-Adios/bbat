#include "stdafx.h"
#include "L313_WriteCU.h"

#define TCL_CU_REF_LEN    (20)

typedef struct
{
	CHAR CU[TCL_CU_REF_LEN];
	CHAR reserved[4];
	INT  root_flag; // 0x52: root   0x4E: un-root
} MISCDATA_TCL;
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CL313_WriteCU)

//
CL313_WriteCU::CL313_WriteCU()
{
	m_strCU="";
	m_byGetCuFromMes = 0;
}

CL313_WriteCU::~CL313_WriteCU()
{

}

BOOL CL313_WriteCU::LoadXMLConfig(void)
{
    m_strCU = _W2CA(GetConfigValue(L"Option:CU", L""));
	LPCWSTR lpszValue  = GetConfigValue(L"Option:GetCUFromMes", L"OFF");
	if (0 == _wcsicmp(lpszValue, L"CODE1"))
	{
		m_byGetCuFromMes = _E_BARCODE_CODE1;
	}
	else if (0 == _wcsicmp(lpszValue, L"CODE2"))
	{
		m_byGetCuFromMes = _E_BARCODE_CODE2;
	}
	else if (0 == _wcsicmp(lpszValue, L"CODE3"))
	{
		m_byGetCuFromMes = _E_BARCODE_CODE3;
	}
	else if (0 == _wcsicmp(lpszValue, L"CODE4"))
	{
		m_byGetCuFromMes = _E_BARCODE_CODE4;
	}
	else if (0 == _wcsicmp(lpszValue, L"CODE5"))
	{
		m_byGetCuFromMes = _E_BARCODE_CODE5;
	}
	else
	{
		m_byGetCuFromMes = 0;
	}
    return TRUE;
}

SPRESULT CL313_WriteCU::__PollAction(void)
{
	SPRESULT sp_result = SP_OK;
	char szField[8]={0};

	if (m_byGetCuFromMes>0)
	{
		if (MES_SUCCESS == UnisocMesActive())
		{	
			_UNISOC_MES_CMN_CODES mes_codes;
			sp_result = GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
			if (SP_OK != sp_result)
			{
				NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Assigned_Codes)");
				return sp_result;
			}
			switch(m_byGetCuFromMes)
			{
			case _E_BARCODE_CODE1:m_strCU = mes_codes.sz_cmn_code1;strcpy_s(szField,"Code1");break;
			case _E_BARCODE_CODE2:m_strCU = mes_codes.sz_cmn_code2;strcpy_s(szField,"Code2");break;
			case _E_BARCODE_CODE3:m_strCU = mes_codes.sz_cmn_code3;strcpy_s(szField,"Code3");break;
			case _E_BARCODE_CODE4:m_strCU = mes_codes.sz_cmn_code4;strcpy_s(szField,"Code4");break;
			case _E_BARCODE_CODE5:m_strCU = mes_codes.sz_cmn_code5;strcpy_s(szField,"Code5");break;
			}	
			char szSN[64]={0};
			sp_result = GetShareMemory(ShareMemory_MES_Check_Flow_SN, (void* )&szSN,sizeof(szSN));
			if (SP_OK != sp_result || strlen(szSN)==0)
			{
				sp_result = UnisocMesGetSn(szSN,sizeof(szSN));
				if (SP_OK != sp_result)
				{
					return sp_result;
				}
			}
			char szCU[128]={0};
			sprintf_s(szCU,"%s-%s",szSN,mes_codes.sz_cmn_code1);
			strcpy_s(mes_codes.sz_cmn_code1,szCU);
			CHKRESULT(SetShareMemory(ShareMemory_MES_Assigned_Codes, (const void*)&mes_codes, sizeof(mes_codes)));
		}
		else
		{
			NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Please connect MES.");
			return SP_E_FAIL;
		}
	}

	if (m_strCU.length()>TCL_CU_REF_LEN || m_strCU.length()<1)
	{
		if (m_byGetCuFromMes>0)
		{

			NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "CU Length:%d, Please check whether there is import CU to in AssignedCodes %s.",m_strCU.length(),szField);
		}
		else
		{
			NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "CU Length:%d",m_strCU.length());
		}	
		return SP_E_FAIL;
	}
	NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strCU.c_str());

	MISCDATA_TCL misData;
	ZeroMemory(&misData, sizeof(misData));

	sp_result = SP_LoadMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, &misData, sizeof(misData), TIMEOUT_3S);
    if (SP_OK != sp_result)
    {
        NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SP_LoadMiscData");
        return SP_E_FAIL;
    }
    else
    {
		strcpy_s(misData.CU,m_strCU.data());
        sp_result = SP_SaveMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, &misData, sizeof(misData), TIMEOUT_3S);
		if (SP_OK != sp_result)
		{
			NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SP_SaveMiscData");
			return SP_E_FAIL;
		}
		else
		{
			NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strCU.c_str());
		}
    }
    return SP_OK;
}
