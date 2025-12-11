#include "StdAfx.h"
#include "WlanAFCTx.h"
#include "WlanApiAT.h"
#include "SharedDefine.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CWlanAFCTx)

CWlanAFCTx::CWlanAFCTx(void)
{
	m_pWlanApi = NULL;

	m_nCalChan = 1;
	m_dRefLevel = 20.0;
	m_eRate = CCK_11;
	m_nStartAFCDac1 = 20;
	m_nStartAFCDac2 = 40;
	m_dFerLimit = 100.0;
	m_nEfuseLimit = 2;
	m_ePath = ANT_PRIMARY;
}


CWlanAFCTx::~CWlanAFCTx(void)
{
}

SPRESULT CWlanAFCTx::__InitAction( void )
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

SPRESULT CWlanAFCTx::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT rlt = SP_OK;
	SetRepairMode(RepairMode_Wlan);
	SetRepairBand($REPAIR_BAND_A_11B);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(true));
	EFUSE_INFO_T info;
	CHKRESULT_WITH_EXIT_EUT(m_pWlanApi->DUT_ReadEFUSEinfo(&info));
	NOTIFY("AFC efuse Info", LEVEL_ITEM, m_nEfuseLimit, info.nCdec, NOUPPLMT);
	if(!IN_RANGE(m_nEfuseLimit, info.nCdec, NOUPPLMT))
	{
		m_pWlanApi->DUT_EnterEUTMode(FALSE);
		LogFmtStrA(SPLOGLV_ERROR, "AFC Efuse write count %d < limit %d", info.nCdec, m_nEfuseLimit);
		return SP_E_WCN_EFUSE_FULL;
	}


	SetRepairItem($REPAIR_ITEM_AFC);
	CHKRESULT_WITH_EXIT_EUT(m_pRFTester->SetNetMode(NM_WIFI));
	CHKRESULT_WITH_EXIT_EUT(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL))
	//m_pWlanApi->DUT_SetBand(WLAN_BAND_24G);

	SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	CHKRESULT_WITH_EXIT_EUT(m_pWlanApi->DUT_SetRfPath(m_ePath));
	CHKRESULT_WITH_EXIT_EUT(m_pWlanApi->DUT_SetCH(m_nCalChan));
	CHKRESULT_WITH_EXIT_EUT(m_pWlanApi->DUT_SetModRate( m_eRate));
	CHKRESULT_WITH_EXIT_EUT(m_pWlanApi->DUT_TxOn(true));

	//Sleep(500);

	SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	ConfigInstrumentParam();

	int nDac = 0;
	double dFer = 0.0;
	double dSlope = 0.0;
	bool bOk = false;
	/*for(int nDac=0; nDac<64; nDac++)
	{
	CHKRESULT(GetFer(nDac, dFer));
	NOTIFY_CAL_RESULT_WITH_ARG("WIFI AFC FER", LEVEL_ITEM, NOLOWLMT, dFer, NOUPPLMT, CwcnUtility::m_WlanBandInfo[WIFI_802_11b],
	m_nCalChan, "KHZ", "DAC=%d", nDac);
	}
	return SP_OK;*/

	int nDac1 = m_nStartAFCDac1;
	double dFer1 = 0.0;
	CHKRESULT_WITH_EXIT_TXOFF_EUT(GetFer(nDac1, dFer1));

	NOTIFY_CAL_RESULT_WITH_ARG("WIFI AFC FER", LEVEL_ITEM, NOLOWLMT, dFer1, NOUPPLMT, CwcnUtility::m_WlanBandInfo[WIFI_802_11b],
		m_nCalChan, "KHZ", "DAC=%d", nDac1);

	int nDac2 = m_nStartAFCDac2;
	double dFer2 = 0.0;
	CHKRESULT_WITH_EXIT_TXOFF_EUT(GetFer(nDac2, dFer2));

	NOTIFY_CAL_RESULT_WITH_ARG("WIFI AFC FER", LEVEL_ITEM, NOLOWLMT, dFer2, NOUPPLMT, CwcnUtility::m_WlanBandInfo[WIFI_802_11b],
		m_nCalChan, "KHZ", "DAC=%d", nDac2);
	do
	{
		dSlope = (dFer1-dFer2)/double(nDac1-nDac2);
		nDac = ROUNDTOINTEGER((double)nDac1 - dFer1/dSlope);
		if(nDac < 0)
		{
			nDac = 0;
		}
		if(nDac > 63)
		{
			nDac = 63;
		}
		if(nDac == nDac1 || nDac == nDac2)
		{
			bOk = true;
			break;
		}
		CHKRESULT_WITH_EXIT_TXOFF_EUT(GetFer(nDac, dFer));
		NOTIFY_CAL_RESULT_WITH_ARG("WIFI AFC FER", LEVEL_ITEM, NOLOWLMT, dFer, NOUPPLMT, CwcnUtility::m_WlanBandInfo[WIFI_802_11b],
			m_nCalChan, "KHZ", "DAC=%d", nDac);
		double dSlop1 = (dFer - dFer1)/(nDac - nDac1);
		double dSlop2 = (dFer - dFer2)/(nDac - nDac2);
		if(dSlop1*dSlop2 < 0)
		{
			NOTIFY("WIFI AFC Linear", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "Dac1:%d, Fer1:%.2f, Dac2:%d, Fer2:%.2f; Dac3:%d, Fer3:%.2f; not linear!",
				nDac1, dFer1, nDac2, dFer2, nDac, dFer);
			rlt = SP_E_WCN_AFC_NOLINEAR;
			break;
		}
		if(fabs(dFer1) > fabs(dFer2))
		{
			nDac1 = nDac;
			dFer1 = dFer;
		}
		else
		{
			nDac2 = nDac;
			dFer2 = dFer;
		}
	} while (!bOk);

	if(bOk)
	{
		CHKRESULT_WITH_EXIT_TXOFF_EUT(m_pWlanApi->DUT_SetAFCDac(nDac));
		NOTIFY_CAL_RESULT_WITH_ARG("WIFI AFC RESULT", LEVEL_ITEM,  -m_dFerLimit, dFer, m_dFerLimit, CwcnUtility::m_WlanBandInfo[WIFI_802_11b],
			m_nCalChan, "KHZ", "DAC=%d", nDac);

		if(fabs(dFer) > fabs(m_dFerLimit))
		{
			rlt = SP_E_WCN_AFC_OVERRANGE;
		}
		else
		{
			WCN_WRITE_EFUSE_T stFlag;
			GetShareMemory(ShareMemory_My_WCNEFUSE, (void* )&stFlag, sizeof(stFlag));
			stFlag.bWriteAFC = TRUE;
			stFlag.nDac = nDac;
			CHKRESULT_WITH_EXIT_TXOFF_EUT(SetShareMemory(ShareMemory_My_WCNEFUSE, (void* )&stFlag, sizeof(stFlag)));
		}
	}

	CHKRESULT_WITH_EXIT_EUT(m_pWlanApi->DUT_TxOn(false));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(false));

	return rlt;
}

BOOL CWlanAFCTx::LoadXMLConfig( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	LPCWSTR pPath = GetConfigValue(L"Option:Ant", L"Ant1st");
	if(0 == wcscmp(pPath, L"Ant1st"))
	{
		m_ePath = ANT_PRIMARY;
	}
	else
	{
		m_ePath = ANT_SECONDARY;
	}
	m_nCalChan	= GetConfigValue(L"Option:TCH", 1);
	m_dRefLevel	= GetConfigValue(L"Option:RefLVL", 20.0);
	LPCSTR strVal = _W2CA(GetConfigValue(L"Option:Modulation", L"CCK11"));
	m_eRate = CwcnUtility::WlanGetRate( strVal);
	if(INVALID_WLAN_RATE == m_eRate)
	{
		return FALSE;
	}
	m_nStartAFCDac1 = GetConfigValue(L"Option:CDAC1", 20);
	m_nStartAFCDac2 = GetConfigValue(L"Option:CDAC2", 20);
	m_dFerLimit = GetConfigValue(L"Option:FERLimit", 5.0);

	m_nEfuseLimit = GetConfigValue(L"Param:EFUSELimit", 2);


	return TRUE;
}

SPRESULT CWlanAFCTx::__FinalAction( void )
{
	SimpleAop(this, __FUNCTION__);
	delete m_pWlanApi;
	m_pWlanApi = NULL;
	return SP_OK;
}

void CWlanAFCTx::ConfigInstrumentParam()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_TesterParam.Init();
	m_TesterParam.nCenChan = m_nCalChan;
	m_TesterParam.nPriChan = m_nCalChan;

	//set band
	WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
	if (m_TesterParam.nCenChan <= 14)
	{
		eBand = WLAN_BAND_24G;
	}
	else
	{
		eBand = WLAN_BAND_50G;
	}
	CwcnUtility::WlanGetAntByBand(m_ePath, &m_TesterParam.ePort[0], eBand);

	m_TesterParam.nAvgCount = 10;
	m_TesterParam.eProto = WIFI_802_11b;
	m_TesterParam.nTotalPackets = 0;
	m_TesterParam.dRefLvl = m_dRefLevel;
	m_TesterParam.dVsgLvl = -70;
	m_TesterParam.nCBWType = WIFI_BW_20M;
	m_TesterParam.eRate = m_eRate;
}

SPRESULT CWlanAFCTx::GetFer(int nDac, double& dFer)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_SetAFCDac(nDac));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(WIFI_FER, (LPVOID)&m_TesterParam));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->FetchResult(WIFI_FER, (LPVOID)&m_TesterRlt));
	dFer = m_TesterRlt.fer.dAvgValue;
	return SP_OK;
}