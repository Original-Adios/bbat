#include "StdAfx.h"
#include "CheckCodes.h"
#include "SharedDefine.h"
#include <algorithm>
#include "MesHelper.h"
#include "UeHelper.h"

IMPLEMENT_RUNTIME_CLASS(CCheckCodes)

CCheckCodes::CCheckCodes(void)
{
	m_bCheckCodesByInput = false;
	memset(&m_CheckCodes, 0, sizeof(m_CheckCodes));
}

CCheckCodes::~CCheckCodes(void)
{
}

BOOL CCheckCodes::LoadXMLConfig(void)
{
	m_bCheckCodesByInput = (BOOL)GetConfigValue(L"Option:CheckCodesByInput",0);
    CHKRESULT(__super::LoadXMLConfig());
    return TRUE;
}

SPRESULT CCheckCodes::__PollAction(void)
{
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
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			LogFmtStrW(SPLOGLV_ERROR, L"Get share memory < %s > failure!", ShareMemory_My_UserInputSN);
			return sp_result;
		}
	}

	for (INT i = BC_BT; i < BC_MAX_NUM; i++)
	{
		if (m_InputSN[i].bEnable)
		{
			strcpy_s(m_CheckCodes[i].szCheckCodesFormUI, m_InputSN[i].szCode);
		}
	}
  
    PC_PRODUCT_DATA ProductData;
    ZeroMemory(&ProductData, sizeof(PC_PRODUCT_DATA));
    //BT/WIFI 不能和IMEI一起读取，否则BT/WIFI 就是0
    for (int i = BC_BT; i <= BC_WIFI; i++)
    {
        if (m_InputSN[i].bEnable)
        {
            ProductData.u32OperMask |= CBarCodeUtility::m_BarCodeInfo[i].Mask;
        }
    }

    if (ProductData.u32OperMask != 0)
    {
		sp_result = ue.ReadProductData(&ProductData);
        CHKRESULT(sp_result);
        strcpy_s(m_CheckCodes[BC_BT].szCheckCodesInDUT,  (char*)ProductData.szBTAddr);
        strcpy_s(m_CheckCodes[BC_WIFI].szCheckCodesInDUT, (char*)ProductData.szWIFIAddr);
    }

    ZeroMemory(&ProductData, sizeof(PC_PRODUCT_DATA));
    //BT/WIFI 不能和IMEI一起读取，否则BT/WIFI 就是0
    for (int i = BC_IMEI1; i <= BC_IMEI4; i++)
    {
        if (m_InputSN[i].bEnable)
        {
            ProductData.u32OperMask |= CBarCodeUtility::m_BarCodeInfo[i].Mask;
        }
    }

    if (ProductData.u32OperMask != 0)
    {
		sp_result = ue.ReadProductData(&ProductData);
		CHKRESULT(sp_result);
        strcpy_s(m_CheckCodes[BC_IMEI1].szCheckCodesInDUT, (char* )ProductData.szImei1);
        strcpy_s(m_CheckCodes[BC_IMEI2].szCheckCodesInDUT, (char* )ProductData.szImei2);
        strcpy_s(m_CheckCodes[BC_IMEI3].szCheckCodesInDUT, (char*)ProductData.szImei3);
        strcpy_s(m_CheckCodes[BC_IMEI4].szCheckCodesInDUT, (char*)ProductData.szImei4);
    }

    // SN [6/20/2017 jian.zhong]
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

    BOOL bCheck = TRUE;

    for (INT i = BC_BT; i<BC_MAX_NUM; i++)
    {
        CHAR szItemName[256] = {NULL};
        CHAR szCondition[256] = {NULL};
        //Show Read IMEI
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }
        
        //Check IMEI
		std::string strDUT,strUI;
        string strCondition;
		strDUT = m_CheckCodes[i].szCheckCodesInDUT;
		strUI = m_CheckCodes[i].szCheckCodesFormUI;
		transform(strDUT.begin(), strDUT.end(), strDUT.begin(), ::toupper); 
		transform(strUI.begin(), strUI.end(), strUI.begin(), ::toupper);

        if (m_InputSN[i].bEnable)
        {
            INT nPrefixLen = strlen(m_InputSN[i].szPrefix);
            if (nPrefixLen != 0)
            {
                //CheckX 模式下 输入前缀时，检测前缀是否匹配
				sprintf_s(szItemName, "Check %s", CBarCodeUtility::m_BarCodeInfo[i].nameA);
				sprintf_s(szCondition, "DUT:[ %s ] Prefix: [ %s ])", strDUT.c_str(), m_InputSN[i].szPrefix);
				std::string strPrefix = m_InputSN[i].szPrefix;
				transform(strPrefix.begin(), strPrefix.end(), strPrefix.begin(), ::toupper);
				if ( 0 != strDUT.compare(0,nPrefixLen,strPrefix))
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
				sprintf_s(szCondition, (MES_SUCCESS == mes_result && !m_bCheckCodesByInput)?"DUT:[ %s ]  MES: [ %s ]":"DUT:[ %s ]  UI: [ %s ]", strDUT.data(), strUI.data());
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
	}
	return bCheck? SP_OK : SP_E_SPAT_TEST_FAIL;
}