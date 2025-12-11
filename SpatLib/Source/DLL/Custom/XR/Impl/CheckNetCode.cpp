#include "StdAfx.h"
#include "CheckNetCode.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//#include "ExtraLogFile.h"


IMPLEMENT_RUNTIME_CLASS(CCheckNetCode)

//////////////////////////////////////////////////////////////////////////
CCheckNetCode::CCheckNetCode(void)
{

}

CCheckNetCode::~CCheckNetCode(void)
{
}

SPRESULT CCheckNetCode::__PollAction(void)
{ 
	string strMesNetCode;
	string strInputNetCode;
	SPRESULT sp_result = SP_OK;

	//≈–∂œ «∑Òmes‘⁄œﬂ
	MES_RESULT mesResult = UnisocMesActive();
	if(mesResult != MES_SUCCESS)
	{
		NOTIFY("CheckNetCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unisoc mes is offline");
		return SP_E_FAIL;
	}
	
	INPUT_CODES_T InputCode[BC_MAX_NUM];
	sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void*)&InputCode, sizeof(InputCode));
	if (SP_OK != sp_result)
	{
		NOTIFY("CheckNetCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_My_UserInputSN)");
		return sp_result;
	}
	else
	{
		strInputNetCode = InputCode[BC_NETCODE].szCode;
	}
		
	_UNISOC_MES_CMN_CODES mes_codes;
	sp_result = GetShareMemory(ShareMemory_MES_Device_Codes, (void* )&mes_codes, sizeof(mes_codes));
	if (SP_OK != sp_result)
	{
		NOTIFY("CheckNetCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(ShareMemory_MES_Device_Codes)");
		return sp_result;
	}
	else
	{
		strMesNetCode = mes_codes.sz_cmn_net_code;
	}
	//
	bool bCompareResult = strInputNetCode == strMesNetCode;

	CHAR szCondition[256] = { NULL };
	sprintf_s(szCondition, "Scan:[ %s ]  MES: [ %s ]", strInputNetCode.c_str(), strMesNetCode.c_str());
	NOTIFY("CheckNetCode", LEVEL_ITEM, 1, bCompareResult? 1 : 0, 1, NULL, -1, NULL, "Scan:[ %s ]  MES: [ %s ]", strInputNetCode.c_str(), strMesNetCode.c_str());
	return bCompareResult? SP_OK: SP_E_FAIL;
}
