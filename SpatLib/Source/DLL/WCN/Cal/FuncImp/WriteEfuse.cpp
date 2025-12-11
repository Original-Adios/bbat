#include "WriteEfuse.h"
#include "WlanApiAT.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "BTApiAT.h"
#include "GenCodes.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CWriteEfuse)

CWriteEfuse::CWriteEfuse(void)
{
	m_pWlanApi = NULL;
	m_nEfuseLimit = 2;

	m_bWriteMac = TRUE;
	m_bWriteAfc = TRUE;
	m_bWriteApc = TRUE;
	m_bWriteOTTSN = FALSE;
}


CWriteEfuse::~CWriteEfuse(void)
{
}


SPRESULT CWriteEfuse::__InitAction( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());
	m_pWlanApi = new CWlanApiAT(m_hDUT);
	if(NULL == m_pWlanApi)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	return SP_OK;
}

SPRESULT CWriteEfuse::__FinalAction( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	delete m_pWlanApi;
	m_pWlanApi = NULL;
	return SP_OK;
}

SPRESULT CWriteEfuse::__PollAction( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT res = SP_OK;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(true));
	INPUT_CODES_T InputSN[BC_MAX_NUM];
	if(m_bWriteOTTSN || m_bWriteMac)
	{
		res = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&InputSN, sizeof(InputSN));
		if(SP_OK != res)
		{
			NOTIFY("Input Codes Not Selected", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "Can not get Mac ShareMemory, pls select input code");
			return SP_E_WCN_CAL_NODO;
		}
	}

	if(m_bWriteOTTSN)
	{
		char szOTTSN[13] = {0};
		_strupr_s(InputSN[BC_OTTSN].szCode, sizeof(InputSN[BC_OTTSN].szCode));
		if(strlen(InputSN[BC_OTTSN].szCode) != 12)
		{
			LogFmtStrA(SPLOGLV_ERROR, "OTTSN %s lengh != 12!");
			return SP_E_INVALID_PARAMETER;
		}
	
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadOTTSN(szOTTSN, 13));
		_strupr_s(szOTTSN, sizeof(szOTTSN));

		if(0 == strcmp(szOTTSN, InputSN[BC_OTTSN].szCode))
		{
			NOTIFY("OTTSN EXISTED", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "OTTSN=%s", InputSN[BC_OTTSN].szCode);
		}
		else
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_WriteOTTSN(InputSN[BC_OTTSN].szCode));
			NOTIFY("WRITE OTTSN", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "OTTSN=%s", InputSN[BC_OTTSN].szCode);

			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadOTTSN(szOTTSN ,13));
			_strupr_s(szOTTSN, sizeof(szOTTSN));

			NOTIFY("READ OTTSN", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "OTTSN=%s", szOTTSN);
			if(0 != strcmp(szOTTSN, InputSN[BC_OTTSN].szCode))
			{
				NOTIFY("OTTSN VERIFY", 1, 0, 1);
				return SP_E_WCN_OTTSN_VERIFY;
			}
		}

		if(E_GENCODE_SECTION == InputSN[BC_OTTSN].eGenCodeType )
		{
			CGenCodes GenCode;
			GenCode.Init(CBarCodeUtility::m_BarCodeInfo[BC_OTTSN].nameW, this);
			CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&InputSN[BC_OTTSN]), "CompleteCodes");
		}
	}

	if(m_bWriteMac)
	{
		EFUSE_INFO_T info;
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadEFUSEinfo(&info));
		NOTIFY("Mac efuse Info", LEVEL_ITEM, m_nEfuseLimit, info.nMac, NOUPPLMT);
		if(!IN_RANGE(m_nEfuseLimit, info.nMac, NOUPPLMT))
		{
			LogFmtStrA(SPLOGLV_ERROR, "Mac Efuse write count %d < limit %d", info.nCdec, m_nEfuseLimit);
			return SP_E_WCN_EFUSE_FULL;
		}

		char szWlanMac[13] = {0};
		char szBtMac[13] = {0};
		_strupr_s(InputSN[BC_WIFI].szCode, sizeof(InputSN[BC_WIFI].szCode));
		_strupr_s(InputSN[BC_BT].szCode, sizeof(InputSN[BC_BT].szCode));
		if(strlen(InputSN[BC_WIFI].szCode) != 12)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Wlan Mac %s lengh != 12!");
			return SP_E_INVALID_PARAMETER;
		}
		if(strlen(InputSN[BC_BT].szCode) != 12)
		{
			LogFmtStrA(SPLOGLV_ERROR, "BT Mac %s lengh != 12!");
			return SP_E_INVALID_PARAMETER;
		}

		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadMac(szWlanMac, szBtMac));
		_strupr_s(szWlanMac, sizeof(szWlanMac));
		_strupr_s(szBtMac, sizeof(szBtMac));
		if(0 == strcmp(szWlanMac, InputSN[BC_WIFI].szCode) && 0 == strcmp(szBtMac, InputSN[BC_BT].szCode))
		{
			NOTIFY("WLAN MAC EXISTED", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", InputSN[BC_WIFI].szCode);
			NOTIFY("BT MAC EXISTED", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", InputSN[BC_BT].szCode);
		}
		else
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_WriteMac(InputSN[BC_WIFI].szCode, InputSN[BC_BT].szCode));
			NOTIFY("WRITE WLAN MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", InputSN[BC_WIFI].szCode);
			NOTIFY("WRITE BT MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", InputSN[BC_BT].szCode);

			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadMac(szWlanMac, szBtMac));
			_strupr_s(szWlanMac, sizeof(szWlanMac));
			_strupr_s(szBtMac, sizeof(szBtMac));
			NOTIFY("READ WLAN MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", szWlanMac);
			NOTIFY("READ BT MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", szBtMac);
			if(0 != strcmp(szWlanMac, InputSN[BC_WIFI].szCode))
			{
				NOTIFY("WLAN MAC VERIFY", 1, 0, 1);
				return SP_E_WCN_MAC_VERIFY;
			}
			if(0 != strcmp(szBtMac, InputSN[BC_BT].szCode))
			{
				NOTIFY("BT MAC VERIFY", 1, 0, 1);
				return SP_E_WCN_MAC_VERIFY;
			}
		}

		if(E_GENCODE_SECTION == InputSN[BC_WIFI].eGenCodeType )
		{
			CGenCodes GenCode;
			GenCode.Init(CBarCodeUtility::m_BarCodeInfo[BC_WIFI].nameW, this);
			CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&InputSN[BC_WIFI]), "CompleteCodes");
		}

		if(E_GENCODE_SECTION == InputSN[BC_BT].eGenCodeType )
		{
			CGenCodes GenCode;
			GenCode.Init(CBarCodeUtility::m_BarCodeInfo[BC_BT].nameW, this);
			CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&InputSN[BC_BT]), "CompleteCodes");
		}
	}
	
	if(m_bWriteAfc)
	{
		WCN_WRITE_EFUSE_T stFlag;
		res = GetShareMemory(ShareMemory_My_WCNEFUSE, (void* )&stFlag, sizeof(stFlag));
		if(SP_OK != res)
		{
			NOTIFY("WlanAFC Not Selected", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "Can not get wcn cal ShareMemory, pls select wcn cal action");
			return SP_E_WCN_CAL_NODO;
		}
		if(!stFlag.bWriteAFC)
		{
			NOTIFY("WlanAFC Not Selected", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "Wlan AFC not do, pls select Wlan AFC action");
			return SP_E_WCN_CAL_NODO;
		}
		int nDac = stFlag.nDac;
		int nDacReaded = 0;

		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadAFCDac(nDacReaded));
		if(nDacReaded == nDac)
		{
			NOTIFY("DAC EXISTED", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "DAC=%d", nDac);
		}
		else
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_SaveAFCDac(nDac));
			NOTIFY("Save AFC EFUSE", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "DAC=%d", nDac);

			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadAFCDac(nDacReaded));
			NOTIFY("Read AFC EFUSE", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "DAC=%d", nDacReaded);
			if(nDac != nDacReaded)
			{
				NOTIFY("Verify AFC EFUSE", LEVEL_ITEM, 1, 0, 1);
				return SP_E_WCN_DAC_VERIFY;
			}
		}
	}

	if(m_bWriteApc)
	{
		WCN_WRITE_EFUSE_T stFlag;
		res = GetShareMemory(ShareMemory_My_WCNEFUSE, (void* )&stFlag, sizeof(stFlag));
		if(SP_OK != res)
		{
			NOTIFY("WlanAPC Not Selected", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "Can not get wcn cal ShareMemory, pls select wcn cal action");
			return SP_E_WCN_CAL_NODO;
		}
		if(!stFlag.bWriteAPC)
		{
			NOTIFY("WlanAPC Not Selected", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "Wlan APC not do, pls select Wlan APC action");
			return SP_E_WCN_CAL_NODO;
		}
		SPRESULT res = m_pWlanApi->DUT_WriteTxCalEFUSE();
		NOTIFY("Save APC EFUSE", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, "-");
		if(SP_OK != res)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Save APC EFUSE fail!");
			return res;
		}
	}
	
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(false));
	return SP_OK;
}

BOOL CWriteEfuse::LoadXMLConfig( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_nEfuseLimit = GetConfigValue(L"Param:EFUSELimit", 2);
	m_bWriteMac =  GetConfigValue(L"Option:WriteMAC", TRUE);
	m_bWriteAfc =  GetConfigValue(L"Option:WriteAFC", TRUE);
	m_bWriteApc =  GetConfigValue(L"Option:WriteAPC", TRUE);
	m_bWriteOTTSN = GetConfigValue(L"Option:WriteOTTSN", FALSE);
	return TRUE;
}
