#include "StdAfx.h"
#include "Ethernet_WriteCodes.h"
#include "Utility.h"
#include <atltime.h>
#include "GenCodes.h"
#include "../DBHelper.h"
#include "UeHelper.h"
#include "MesHelper.h"

IMPLEMENT_RUNTIME_CLASS(CEthernet_WriteCodes)

//////////////////////////////////////////////////////////////////////////
CEthernet_WriteCodes::CEthernet_WriteCodes(void)
{
}

CEthernet_WriteCodes::~CEthernet_WriteCodes(void)
{
}

SPRESULT CEthernet_WriteCodes::__PollAction(void)
{
	SPRESULT sp_result = SP_OK;
	CUeHelper ue(this);
	CMesHelper mes(this);

	MES_RESULT mes_result = mes.UnisocMesActive();
	if (MES_SUCCESS == mes_result)
	{
		sp_result = mes.UnisocMesGetAssignedCodes(m_InputSN,sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			return sp_result;
		}
	}
	else
	{
		CHKRESULT_WITH_NOTIFY(GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN)), "GetShareMemory(InputCode)");
	}

	for (INT i=BC_SN1; i<=BC_SN2; i++)
	{
		if (!m_InputSN[i].bEnable)
		{
			continue;
		}
		CHKRESULT(ue.WriteSN((BC_SN1 == i) ? SN1 : SN2, m_InputSN[i].szCode));
	}

	// Write IMEI for ProductData
	PC_PRODUCT_DATA ProductData;
	ZeroMemory(&ProductData, sizeof(PC_PRODUCT_DATA));
	uint8 *pStr[BC_MAX_NUM - 2] = {0};
	pStr[BC_BT] = ProductData.szBTAddr;
	pStr[BC_WIFI] = ProductData.szWIFIAddr;
	for (int i = BC_BT; i <= BC_WIFI; i++)
	{
		if(m_InputSN[i].bEnable)
		{
			ProductData.u32OperMask |= CBarCodeUtility::m_BarCodeInfo[i].Mask;
			memcpy(pStr[i], m_InputSN[i].szCode, CBarCodeUtility::m_BarCodeInfo[i].SNlength);
		}
	}

	 if (0 != ProductData.u32OperMask)
	 {
		 CHKRESULT(ue.WriteProductData(ProductData));
	 }

	//guanglu.pan 20190906 add
	 if(m_InputSN[BC_ENETMAC].bEnable)
	 {
		 CHKRESULT(ue.WriteEthernetMAC(m_InputSN[BC_ENETMAC].szCode));
	 }
	//	
    BOOL bOK = TRUE;
    SetShareMemory(ShareMemory_My_UpdateSN, (void* )&bOK, sizeof(bOK), IContainer::System);
    if (IS_BIT_SET(ProductData.u32OperMask, FNAMASK_RW_IMEI1))
    {
        SetShareMemory(ShareMemory_IMEI1, (void* )&ProductData.szImei1, ShareMemory_IMEI_SIZE);
    }

    CHKRESULT(SaveDataIntoLocal());
    CHKRESULT(UpdateSectionAllocCodes());

    return SP_OK;
}

BOOL CEthernet_WriteCodes::LoadXMLConfig(void)
{  
    return TRUE;

}