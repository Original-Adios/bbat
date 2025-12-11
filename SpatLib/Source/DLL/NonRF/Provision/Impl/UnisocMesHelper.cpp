#include "StdAfx.h"
#include "UnisocMesHelper.h"
#include <assert.h>
//////////////////////////////////////////////////////////////////////////
CUnisocMesHelper::CUnisocMesHelper(CImpBase*pImp)
: m_pImp(pImp)
{
}

CUnisocMesHelper::~CUnisocMesHelper(void)
{
}

MES_RESULT CUnisocMesHelper::UnisocMesActive()
{
	return m_pImp->UnisocMesActive();
}

SPRESULT CUnisocMesHelper::UnisocMesGetDeviceCodes(INPUT_CODES_T input_codes[])
{
	SPRESULT sp_result = SP_OK;
	_UNISOC_MES_CMN_CODES mes_codes;
	sp_result = m_pImp->GetShareMemory(ShareMemory_MES_Device_Codes, (void* )&mes_codes, sizeof(mes_codes));
	if (SP_OK != sp_result)
	{
		m_pImp->NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_MES_Device_Codes");
		return sp_result;
	}

	_ENUM_MES_V2_FLOW_CONTROL flow_control = m_pImp->UnisocMesGetFlowCtrl();
	_ENUM_UNISOC_MES_TYPE mes_type = m_pImp->UnisocMesGetMesType();

	BC_INDEX bc_index;
	if (_E_UNISOC_MES_V2 == mes_type && _E_FLOW_CONTROL_SN2 == flow_control)
	{
		bc_index = BC_SN2;
		if (((int)strlen(mes_codes.sz_cmn_sn1) > CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength))
		{
			input_codes[bc_index].bEnable = true;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_sn1);
		}
	}
	else
	{
		bc_index = BC_SN1;
		if (((int)strlen(mes_codes.sz_cmn_sn1) > CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength))
		{
			input_codes[bc_index].bEnable = true;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_sn1);
		}

		bc_index = BC_SN2;
		if (((int)strlen(mes_codes.sz_cmn_sn2) > CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength))
		{
			input_codes[bc_index].bEnable = true;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_sn2);
		}
	}

	bc_index = BC_BT;
	if ((int)strlen(mes_codes.sz_cmn_bt) == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
	{
		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_bt);
	}
	bc_index = BC_WIFI;
	if ((int)strlen(mes_codes.sz_cmn_wifi) == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
	{
		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_wifi);
	}

	bc_index = BC_IMEI1;
	if ((int)strlen(mes_codes.sz_cmn_imei1)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
	{
		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei1);
	}

	bc_index = BC_IMEI2;
	if ((int)strlen(mes_codes.sz_cmn_imei2)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
	{
		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei2);
	}

	bc_index = BC_IMEI3;
	if ((int)strlen(mes_codes.sz_cmn_imei3)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
	{
		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei3);
	}

	bc_index = BC_IMEI4;
	if ((int)strlen(mes_codes.sz_cmn_imei4)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
	{
		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei4);
	}	

	bc_index = BC_MEID1;
	if ((int)strlen(mes_codes.sz_cmn_meid1)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-2)
	{
		char cCheckCode=0;
		if (!m_pImp->GetMEIDCheckCode(mes_codes.sz_cmn_meid1,cCheckCode) || 0 == cCheckCode)
		{
			return SP_E_FAIL;
		}
		mes_codes.sz_cmn_meid1[14] = cCheckCode;
		mes_codes.sz_cmn_meid1[15] = 0;

		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_meid1);
	}	

	bc_index = BC_MEID2;
	if ((int)strlen(mes_codes.sz_cmn_meid2)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-2)
	{
		char cCheckCode=0;
		if (!m_pImp->GetMEIDCheckCode(mes_codes.sz_cmn_meid2,cCheckCode) || 0 == cCheckCode)
		{
			return SP_E_FAIL;
		}
		mes_codes.sz_cmn_meid2[14] = cCheckCode;
		mes_codes.sz_cmn_meid2[15] = 0;

		input_codes[bc_index].bEnable = true;
		strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_meid2);
	}	

	return SP_OK;
}

SPRESULT CUnisocMesHelper::UnisocMesGetAssignedCodes(INPUT_CODES_T input_codes[],UINT32 uiSize)
{
	SPRESULT sp_result = SP_OK;
	_ENUM_UNISOC_MES_TYPE mes_type = m_pImp->UnisocMesGetMesType();
	_ENUM_MES_V2_FLOW_CONTROL flow_control = m_pImp->UnisocMesGetFlowCtrl();

	sp_result = m_pImp->GetShareMemory(ShareMemory_My_UserInputSN, (void* )input_codes, uiSize);
	if (SP_OK != sp_result)
	{
		m_pImp->LogFmtStrW(SPLOGLV_ERROR, L"Get share memory < %s > <%s> failure!", ShareMemory_SN2,ShareMemory_My_UserInputSN);
		m_pImp->NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN2");
		return sp_result;
	}

	if (_E_UNISOC_MES_V2 == mes_type && _E_FLOW_CONTROL_SN2 == flow_control)
	{
		if (input_codes[BC_SN2].bEnable && 0 == strlen(input_codes[BC_SN2].szCode))
		{
			m_pImp->LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory(SN1 Length == 0)");
			m_pImp->NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
	}
	else
	{		
		if (input_codes[BC_SN1].bEnable && 0 == strlen(input_codes[BC_SN1].szCode))
		{
			m_pImp->LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory(SN1 Length == 0)");
			m_pImp->NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
	}

	_UNISOC_MES_CMN_CODES mes_codes;
	sp_result = m_pImp->GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
	if (SP_OK != sp_result && 
		((input_codes[BC_SN2].bEnable 
		&& (_E_UNISOC_MES_V2 != mes_type || _E_FLOW_CONTROL_SN2 != flow_control) //当SN2走流程的时候，不需要判断SN2
		&& (strlen(input_codes[BC_SN1].szCode) > 0 && 0 != strcmp(input_codes[BC_SN1].szCode,input_codes[BC_SN2].szCode))) //当扫描SN1和SN2写入相同的值时，不需要判断SN2
		|| input_codes[BC_WIFI].bEnable 
		|| input_codes[BC_BT].bEnable 
		|| input_codes[BC_IMEI1].bEnable 
		|| input_codes[BC_IMEI2].bEnable 
		|| input_codes[BC_IMEI3].bEnable
		|| input_codes[BC_IMEI4].bEnable
		|| input_codes[BC_MEID1].bEnable
		|| input_codes[BC_MEID2].bEnable))
	{
		m_pImp->NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_MES_Assigned_Codes");
		return sp_result;
	}

	if (SP_OK == sp_result)
	{
		BC_INDEX bc_index;
		if (_E_UNISOC_MES_V2 != mes_type || _E_FLOW_CONTROL_SN2 != flow_control)
		{
			//以SN1走流程时，AssignedCodes表中SN2有数据
			bc_index = BC_SN2;
			if (((int)strlen(mes_codes.sz_cmn_sn2) > CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength))
			{
				input_codes[bc_index].bEnable = true;
				input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
				strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_sn2);
			}
		}

		bc_index = BC_BT;
		if ((int)strlen(mes_codes.sz_cmn_bt) == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
		{
			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_bt);
		}
		bc_index = BC_WIFI;
		if ((int)strlen(mes_codes.sz_cmn_wifi) == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
		{
			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_wifi);
		}

		bc_index = BC_IMEI1;
		if ((int)strlen(mes_codes.sz_cmn_imei1)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
		{
			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei1);
		}

		bc_index = BC_IMEI2;
		if ((int)strlen(mes_codes.sz_cmn_imei2)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
		{
			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei2);
		}

		bc_index = BC_IMEI3;
		if ((int)strlen(mes_codes.sz_cmn_imei3)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
		{
			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei3);
		}

		bc_index = BC_IMEI4;
		if ((int)strlen(mes_codes.sz_cmn_imei4)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-1)
		{
			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_imei4);
		}

		bc_index = BC_MEID1;
		if ((int)strlen(mes_codes.sz_cmn_meid1)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-2)
		{	
			char cCheckCode=0;
			if (!m_pImp->GetMEIDCheckCode(mes_codes.sz_cmn_meid1,cCheckCode) || 0 == cCheckCode)
			{
				return SP_E_FAIL;
			}
			mes_codes.sz_cmn_meid1[14] = cCheckCode;
			mes_codes.sz_cmn_meid1[15] = 0;

			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_meid1);
		}

		bc_index = BC_MEID2;
		if ((int)strlen(mes_codes.sz_cmn_meid2)== CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength-2)
		{
			char cCheckCode=0;
			if (!m_pImp->GetMEIDCheckCode(mes_codes.sz_cmn_meid2,cCheckCode) || 0 == cCheckCode)
			{
				return SP_E_FAIL;
			}
			mes_codes.sz_cmn_meid2[14] = cCheckCode;
			mes_codes.sz_cmn_meid2[15] = 0;

			input_codes[bc_index].bEnable = true;
			input_codes[bc_index].eGenCodeType = E_GENCODE_MAN;
			strcpy_s(input_codes[bc_index].szCode,sizeof(input_codes[bc_index].szCode),mes_codes.sz_cmn_meid2);
		}
	}

	return SP_OK;
}