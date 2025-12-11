#include "VerifyMac.h"
#include "WlanApiAT.h"
#include "GenCodes.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CVerifyMac)
CVerifyMac::CVerifyMac(void)
{
	m_pWlanApi =NULL;
}


CVerifyMac::~CVerifyMac(void)
{
}

SPRESULT CVerifyMac::__InitAction( void )
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

SPRESULT CVerifyMac::__FinalAction( void )
{
	SimpleAop(this, __FUNCTION__);
	delete m_pWlanApi;
	m_pWlanApi = NULL;
	return SP_OK;
}

SPRESULT CVerifyMac::__PollAction( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT res = SP_OK;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(true));
	INPUT_CODES_T InputSN[BC_MAX_NUM];
	res = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&InputSN, sizeof(InputSN));
	if(SP_OK != res)
	{
		NOTIFY("Input Codes Not Selected", LEVEL_ITEM, 1, 0, 1);
		LogFmtStrA(SPLOGLV_ERROR, "Can not get Mac ShareMemory, pls select input code");
		return SP_E_WCN_CAL_NODO;
	}
	SPRESULT rlt = SP_OK;
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
	NOTIFY("Input WLAN MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", InputSN[BC_WIFI].szCode);
	NOTIFY("Input BT MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", InputSN[BC_BT].szCode);

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadMac(szWlanMac, szBtMac));
	_strupr_s(szWlanMac, sizeof(szWlanMac));
	_strupr_s(szBtMac, sizeof(szBtMac));
	NOTIFY("READ WLAN MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", szWlanMac);
	NOTIFY("READ BT MAC", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", "MAC=%s", szBtMac);
	if(0 != strcmp(szWlanMac, InputSN[BC_WIFI].szCode))
	{
		NOTIFY("WLAN MAC VERIFY", 1, 0, 1);
		rlt = SP_E_WCN_MAC_VERIFY;
	}
	else
	{
		NOTIFY("WLAN MAC VERIFY", 1, 1, 1);
	}
	if(0 != strcmp(szBtMac, InputSN[BC_BT].szCode))
	{
		NOTIFY("BT MAC VERIFY", 1, 0, 1);
		rlt = SP_E_WCN_MAC_VERIFY;
	}
	else
	{
		NOTIFY("BT MAC VERIFY", 1, 1, 1);
	}
	return rlt;
	
}