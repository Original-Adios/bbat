#include "StdAfx.h"
#include "CheckCodesEx.h"
#include "DBHelper.h"
#include "MesHelper.h"
#include "UeHelper.h"

IMPLEMENT_RUNTIME_CLASS(CCheckCodesEx)

//////////////////////////////////////////////////////////////////////////
CCheckCodesEx::CCheckCodesEx(void)
{
	m_bCheckCodesByInput = false;
}

CCheckCodesEx::~CCheckCodesEx(void)
{
}

BOOL CCheckCodesEx::LoadXMLConfig(void)
{
	if (!__super::LoadXMLConfig())
	{
		return FALSE;
	}

	return TRUE;
}

SPRESULT CCheckCodesEx::__PollAction(void)
{
	// Get Input codes from share memory
	SPRESULT sp_result = SP_OK;
	CMesHelper mes(this);
	CUeHelper ue(this);

	MES_RESULT mes_result = mes.UnisocMesActive();
	if (MES_SUCCESS == mes_result && !m_bCheckCodesByInput)
	{
		sp_result = mes.UnisocMesGetDeviceCodes(m_InputSN);
		if (SP_OK != sp_result)
		{
			return sp_result;
		}
	}
	else
	{
		CHKRESULT_WITH_NOTIFY(GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN)), "GetShareMemory(InputCode)");
	}

	for (INT i = BC_BT; i < BC_MAX_NUM; i++)
	{
		if (m_InputSN[i].bEnable)
		{
			strcpy_s(m_CheckCodes[i].szCheckCodesFormUI, m_InputSN[i].szCode);
		}
	}

	// Check SN
	for (INT i=BC_SN1; i<=BC_SN2; i++)
	{
		if (m_InputSN[i].bEnable)
		{
			CHAR  szValue[SP15_MAX_SN_LEN + 1] = {0};
			sp_result = ue.ReadSN((BC_SN1 == i) ? SN1 : SN2, szValue);
			CHKRESULT(sp_result);
			strcpy_s(m_CheckCodes[i].szCheckCodesInDUT, szValue);
		}
	}

	// Check GEID (IMEI & MEID) for UMS312
	INT mask = 0;
	CONST INT MAX_GEID_COUNT = 6;
	struct 
	{
		BC_INDEX bcIndex;
		INT Mask;
	} arrayGEID[MAX_GEID_COUNT] = {
		{BC_IMEI1, GEIDMASK_RW_IMEI1},
		{BC_IMEI2, GEIDMASK_RW_IMEI2},
		{BC_IMEI3, GEIDMASK_RW_IMEI3},
		{BC_IMEI4, GEIDMASK_RW_IMEI4},
		{BC_MEID1, GEIDMASK_RW_MEID1},
		{BC_MEID2, GEIDMASK_RW_MEID2}
	};
	for (INT i=BC_START; i<BC_MAX_NUM; i++)
	{
		if (!m_InputSN[i].bEnable)
		{
			continue;
		}

		for (INT j=0; j<MAX_GEID_COUNT; j++)
		{
			if (i == arrayGEID[j].bcIndex)
			{
				mask |= arrayGEID[j].Mask;
			}
		}
	}

	if (0 != mask)
	{
		PC_GEID_T GEID;
		GEID.u32Mask = mask;
		CHKRESULT(ue.ReadGEID(&GEID));
		strcpy_s(m_CheckCodes[BC_IMEI1].szCheckCodesInDUT, GEID.IMEI1);
		strcpy_s(m_CheckCodes[BC_IMEI2].szCheckCodesInDUT, GEID.IMEI2);
		strcpy_s(m_CheckCodes[BC_IMEI3].szCheckCodesInDUT, GEID.IMEI3);
		strcpy_s(m_CheckCodes[BC_IMEI4].szCheckCodesInDUT, GEID.IMEI4);
		strcpy_s(m_CheckCodes[BC_MEID1].szCheckCodesInDUT, GEID.MEID1);
		strcpy_s(m_CheckCodes[BC_MEID2].szCheckCodesInDUT, GEID.MEID2);
	}

	// Load BT & WIFI MAC address
	if(m_InputSN[BC_BT].bEnable)
	{
		CHKRESULT(ue.ReadBTAddrByAT(m_CheckCodes[BC_BT].szCheckCodesInDUT));
	}

	if(m_InputSN[BC_WIFI].bEnable)
	{
		CHKRESULT(ue.ReadWIFIAddrByAT(m_CheckCodes[BC_WIFI].szCheckCodesInDUT));
	}

	CHKRESULT(CheckAll((MES_SUCCESS == mes_result && !m_bCheckCodesByInput)));

	BOOL bOK = TRUE;
	CHKRESULT(SetShareMemory(ShareMemory_My_UpdateSN, (void* )&bOK, sizeof(bOK), IContainer::System));

	return SP_OK;
}

SPRESULT CCheckCodesEx::CheckAll(BOOL bMes)
{
	BOOL bCheck = TRUE;

	for (INT i = BC_BT; i<BC_MAX_NUM; i++)
	{
		CHAR szItemName[256] = {NULL};
		CHAR szCondition[256] = {NULL};
	
		if (!m_InputSN[i].bEnable)
		{
			continue;
		}
		
		std::string strDUT,strUI;
		strDUT = m_CheckCodes[i].szCheckCodesInDUT;
		strUI = m_CheckCodes[i].szCheckCodesFormUI;
		transform(strDUT.begin(), strDUT.end(), strDUT.begin(), ::toupper); 
		transform(strUI.begin(), strUI.end(), strUI.begin(), ::toupper);

		INT nPrefixLen = strlen(m_InputSN[i].szPrefix);
		if (nPrefixLen != 0)
		{
			//CheckX 模式下 输入前缀时，检测前缀是否匹配
			sprintf_s(szItemName, "Check %s", CBarCodeUtility::m_BarCodeInfo[i].nameA);
			sprintf_s(szCondition, "DUT:[ %s ] Prefix: [ %s ])", strDUT.c_str(), m_InputSN[i].szPrefix);
			if ( 0 != strDUT.compare(0,nPrefixLen,m_InputSN[i].szPrefix))
			{
				LogFmtStrA(SPLOGLV_ERROR, "CheckCodes: %s(%s) check prefix(%s) fail!", CBarCodeUtility::m_BarCodeInfo[i].nameA, m_CheckCodes[i].szCheckCodesInDUT, m_InputSN[i].szPrefix);
				NOTIFY(szItemName, LEVEL_ITEM, 1, 0, 1, NULL, -1, "-", szCondition);
				bCheck = FALSE;
			}
			else
			{
				NOTIFY(szItemName, LEVEL_ITEM | LEVEL_INFO, 1, 1, 1, NULL, -1, "-", szCondition);
			}
		}
		else
		{
			sprintf_s(szItemName, "Check %s ", CBarCodeUtility::m_BarCodeInfo[i].nameA);
			sprintf_s(szCondition, bMes?"DUT:[ %s ]  MES: [ %s ]":"DUT:[ %s ]  UI: [ %s ]", strDUT.data(), strUI.data());
			if (strDUT != strUI)
			{
				LogFmtStrA(SPLOGLV_ERROR, szCondition);
				NOTIFY(szItemName, LEVEL_ITEM, 1, 0, 1, NULL, -1, "-", szCondition);
				bCheck = FALSE; 
			}
			else
			{
				NOTIFY(szItemName, LEVEL_ITEM | LEVEL_INFO, 1, 1, 1, NULL, -1, "-", szCondition);
			}
		}
	}
	return bCheck? SP_OK : SP_E_SPAT_TEST_FAIL;
}