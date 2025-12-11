#include "stdafx.h"
#include "L313_CheckCU.h"


#define TCL_CU_REF_LEN    (20)

typedef struct
{
	CHAR CU[TCL_CU_REF_LEN];
	CHAR reserved[4];
	INT  root_flag; // 0x52: root   0x4E: un-root
} MISCDATA_TCL;


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CL313_CheckCU)

//
CL313_CheckCU::CL313_CheckCU()
{
	m_strTargetCU = "";
	m_bCheckCU = FALSE;
	m_byGetCuFromMes = 0;
}

CL313_CheckCU::~CL313_CheckCU()
{

}

BOOL CL313_CheckCU::LoadXMLConfig(void)
{
    m_strTargetCU = _W2CA(GetConfigValue(L"Option:TargetCU", L""));
	m_bCheckCU   = (BOOL)GetConfigValue(L"Option:CheckCU", 0);
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

SPRESULT CL313_CheckCU::__PollAction(void)
{
	SPRESULT sp_result = SP_OK;
	if (m_bCheckCU && m_byGetCuFromMes > 0)
	{	
		if (MES_SUCCESS == UnisocMesActive())
		{	
			_UNISOC_MES_CMN_CODES mes_codes;
			sp_result = GetShareMemory(ShareMemory_MES_Device_Codes, (void* )&mes_codes, sizeof(mes_codes));
			if (SP_OK != sp_result)
			{
				NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Device_Codes)");
				return sp_result;
			}
			switch(m_byGetCuFromMes)
			{
			case _E_BARCODE_CODE1:m_strTargetCU = mes_codes.sz_cmn_code1;break;
			case _E_BARCODE_CODE2:m_strTargetCU = mes_codes.sz_cmn_code2;break;
			case _E_BARCODE_CODE3:m_strTargetCU = mes_codes.sz_cmn_code3;break;
			case _E_BARCODE_CODE4:m_strTargetCU = mes_codes.sz_cmn_code4;break;
			case _E_BARCODE_CODE5:m_strTargetCU = mes_codes.sz_cmn_code5;break;
			}	
			char*pszTok=NULL;
			strtok_s(mes_codes.sz_cmn_code1,"-",&pszTok);
			if (NULL != pszTok)
			{
				m_strTargetCU = pszTok;	
			}		
		}
		else
		{
			NOTIFY("L313_WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Please connect MES.");
			return SP_E_FAIL;
		}
	}

	if (m_bCheckCU && (m_strTargetCU.length()>TCL_CU_REF_LEN || m_strTargetCU.length()<1))
	{
		NOTIFY("L313_CheckCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Target CU Length:%d",m_strTargetCU.length());
		return SP_E_FAIL;
	}

	if (m_bCheckCU)
	{
		NOTIFY("L313_CheckCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strTargetCU.c_str());
	}

	MISCDATA_TCL misData;
	ZeroMemory(&misData,sizeof(misData));
	sp_result = SP_LoadMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, &misData, sizeof(misData), TIMEOUT_3S);
    if (SP_OK != sp_result)
    {
        NOTIFY("L313_CheckCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SP_LoadMiscData");
        return SP_E_FAIL;
    }

	if (m_bCheckCU)
	{
        if (NULL == strstr(misData.CU, m_strTargetCU.c_str()))
        {
            NOTIFY("L313_CheckCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Target : %s, Read : %s",m_strTargetCU.c_str(),misData.CU);
            return SP_E_FAIL;
        }
        else
        {
            NOTIFY("L313_CheckCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Target : %s, Read : %s",m_strTargetCU.c_str(),misData.CU);
        }
    }
	else
	{
		NOTIFY("ReadCUReference", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "%s",misData.CU);
	}

    return SP_OK;
}
