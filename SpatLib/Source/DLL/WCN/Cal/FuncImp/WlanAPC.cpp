#include "StdAfx.h"
#include "WlanAPC.h"
#include "WlanApiAT.h"
#include "SharedDefine.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CWlanAPC)

CWlanAPC::CWlanAPC(void)
{
	m_pWlanApi = NULL;
	m_nTSSI = 32;
	m_nEfuseLimit = 1;
	ZeroMemory(m_dPowerLimit, sizeof(m_dPowerLimit));
	ZeroMemory(m_bEnableCal, sizeof(m_bEnableCal));
	m_bAutoChan = TRUE;
}


CWlanAPC::~CWlanAPC(void)
{
}

SPRESULT CWlanAPC::__InitAction( void )
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

SPRESULT CWlanAPC::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT rlt = SP_OK;
	SetRepairMode(RepairMode_Wlan);


	SetRepairItem($REPAIR_ITEM_APC);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_WIFI));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL))
	//m_pWlanApi->DUT_SetBand(WLAN_BAND_24G);

	SetRepairItem($REPAIR_ITEM_COMMUNICATION);

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(true));
	Sleep(1000);
	EFUSE_INFO_T info;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_ReadEFUSEinfo(&info));
	NOTIFY("APC efuse Info", LEVEL_ITEM, m_nEfuseLimit, info.nTpc, NOUPPLMT);
	if(!IN_RANGE(m_nEfuseLimit, info.nTpc, NOUPPLMT))
	{
		LogFmtStrA(SPLOGLV_ERROR, "APC Efuse write count %d < limit %d", info.nTpc, m_nEfuseLimit);
		return SP_E_WCN_EFUSE_FULL;
	}
	rlt = m_pWlanApi->DUT_EnterTxCalMode(true);
	if(SP_OK != rlt)
	{
		m_pWlanApi->DUT_EnterEUTMode(false);
		return rlt;
	}
	if(m_bAutoChan)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(ReadApcChan());
	}

	rlt = DoAPc();

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterTxCalMode(false));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_EnterEUTMode(false));
	if(SP_OK == rlt)
	{
		WCN_WRITE_EFUSE_T stFlag;
		GetShareMemory(ShareMemory_My_WCNEFUSE, (void* )&stFlag, sizeof(stFlag));
		stFlag.bWriteAPC = TRUE;
		CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_My_WCNEFUSE, (void* )&stFlag, sizeof(stFlag)));
	}
	return rlt;
}

BOOL CWlanAPC::LoadXMLConfig( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	wstring strBand[2] = {L"Band_24G", L"Band_50G"};
	wstring strAnt[2] = {L"Ant1st", L"Ant2nd" };
	SPWI_WLAN_PARAM_TESTER stParam;
	m_nEfuseLimit = GetConfigValue(L"Param:Common:EFUSELimit", 1);
	int nAvgCount = GetConfigValue(L"Param:Common:AvgCount", 3);
	m_nTSSI = GetConfigValue(L"Param:Common:TSSI", 32);
	m_bAutoChan = GetConfigValue(L"Param:Common:AutoChan", TRUE);
	for(int nBandIndex=0; nBandIndex<2; nBandIndex++)
	{
		for(int nAnt = 0; nAnt<2; nAnt++)
		{
			m_TesterParam[nBandIndex][nAnt].clear();
			wstring strKey = L"Option:" + strBand[nBandIndex] + L":" + strAnt[nAnt];
			m_bEnableCal[nBandIndex][nAnt] = GetConfigValue(strKey.c_str(), FALSE);
			if(!m_bEnableCal[nBandIndex][nAnt])
			{
				continue;
			}
			stParam.Init();
			if(0 == nAnt )
			{
				stParam.ePort[0] = RF_ANT_1st;
			}
			else
			{
				stParam.ePort[0] = RF_ANT_2nd;
			}
			strKey = L"Option:" + strBand[nBandIndex] + L":POWER_LIMIT";
			wstring strVal = GetConfigValue(strKey.c_str(), L"5,21");
			INT nCount = 0;
			double *pDoubleVal = CUtility::GetTokenDoubleW(strVal.c_str(), L",", nCount);
			if(nCount < 2)
			{
				return FALSE;
			}
			m_dPowerLimit[nBandIndex][nAnt][0] = pDoubleVal[0];
			m_dPowerLimit[nBandIndex][nAnt][1] = pDoubleVal[1];
			strKey = L"Option:" + strBand[nBandIndex] + L":Mode";
			wstring strMode = GetConfigValue(strKey.c_str(), L"NULL");

			stParam.eProto = CwcnUtility::WlanGetMode(_W2CA(strMode.c_str()));
			if(WIFI_INVALID_Protocol == stParam.eProto )
			{
				return FALSE;
			}
			strKey = L"Option:" + strBand[nBandIndex] + L":BW";
			wstring strBw = GetConfigValue(strKey.c_str(), L"BW_20M");
			if(L"BW_20M" == strBw)
			{
				stParam.nCBWType = WIFI_BW_20M;
			}
			else if(L"BW_40M" == strBw)
			{
				stParam.nCBWType = WIFI_BW_40M;
				if( WIFI_802_11n != stParam.eProto
					&& WIFI_802_11ac == stParam.eProto )
				{
					return FALSE;
				}
			}
			else if(L"BW_80M" == strBw)
			{
				stParam.nCBWType = WIFI_BW_80M;
				if( WIFI_802_11ac != stParam.eProto )
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
			wstring strParam = L"Param:" + strBand[nBandIndex] + L":" + strMode + L":" + strBw;
			stParam.nSBWType = GetConfigValue((strParam + L":SBW").c_str(), 0);
			LPCTSTR lpVal = GetConfigValue((strParam + L":Modulation").c_str(), L"MCS-7");
			E_WLAN_RATE eRate = CwcnUtility::WlanGetRate( _W2CA(lpVal));
			if(INVALID_WLAN_RATE == eRate)
			{
				return FALSE;
			}
			stParam.eRate = eRate;
			stParam.dRefLvl = GetConfigValue((strParam + L":RefLVL").c_str(), 20.0);

			stParam.nAvgCount = nAvgCount;
			int nFreqOff = GetConfigValue((strParam + L":PriChan").c_str(), 0);
			strVal = GetConfigValue((strParam + L":CenChan").c_str(), L"7");
			nCount = 0;
			INT *pIntVal = CUtility::GetTokenIntegerW(strVal.c_str(), L",", nCount);
			if(nCount < 1)
			{
				return FALSE;
			}
			for(int i=0; i<nCount; i++)
			{
				//if(7 != pIntVal[i] && 36 != pIntVal[i] && 149 != pIntVal[i])
				//{
				//	LogFmtStrA(SPLOGLV_ERROR, "TPC CenChan must 7 36 149");
				//	return FALSE;
				//}
				stParam.nCenChan = pIntVal[i];
				stParam.nPriChan =stParam.nCenChan + nFreqOff*2;
				m_TesterParam[nBandIndex][nAnt].push_back(stParam);
			}
		}
	}
	return TRUE;
}

SPRESULT CWlanAPC::__FinalAction( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	delete m_pWlanApi;
	m_pWlanApi = NULL;
	return SP_OK;
}

SPRESULT CWlanAPC::DoAPc()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	int nCount = 0;
	for(int nBandIndex=0; nBandIndex<2; nBandIndex++)
	{
		for(int nAnt = 0; nAnt<2; nAnt++)
		{
			if(!m_bEnableCal[nBandIndex][nAnt])
			{
				continue;
			}
			for(int nCh=0; nCh<(int)m_TesterParam[nBandIndex][nAnt].size(); nCh++)
			{
				SPWI_WLAN_PARAM_TESTER stParam = m_TesterParam[nBandIndex][nAnt][nCh];
				WIFI_PROTOCOL_ENUM eMode = stParam.eProto;
				SetRepairBand(CwcnUtility::m_WlanBandInfo[eMode]);
				ANTENNA_ENUM ePath = (ANTENNA_ENUM)nAnt/*ANT_PRIMARY*/;
// 				if(RF_ANT_2nd == stParam.ePort[0])
// 				{
// 					ePath = ANT_SECONDARY;
// 				}

				//set band
				WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
				if (stParam.nCenChan <= 14)
				{
					eBand = WLAN_BAND_24G;
				}
				else
				{
					eBand = WLAN_BAND_50G;
				}
				CwcnUtility::WlanGetAntByBand((ANTENNA_ENUM)ePath, &stParam.ePort[0], eBand);

				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetRfPath(ePath), "DUT_SetRfPath");
				//Set channel band width
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetBandWidth((WIFI_BANDWIDTH_ENUM)stParam.nCBWType), "DUT_SetBandWidth");
				//Set signal band width
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)stParam.nSBWType), "DUT_SetSigBandWidth");
				////Set Channel
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetCH(stParam.nPriChan, stParam.nCenChan), "DUT_SetCH");

				// [2]: Setup DUT data rate
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetModRate(stParam.eRate), "DUT_SetModRate");
				//Set packet length
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetPacktLen(1024), "DUT_SetPacktLen");
				//Set tx mode
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetTxMode(WIFI_TXMODE_CARRIER_SUPPRESSION), "DUT_SetTxMode");
				//Set preamble
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetPreamble(0), "DUT_SetPreamble");
				//Set payload
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetPayLoad(WIFI_PAYLOAD_1111), "DUT_SetPayLoad");
				//Set TSSI
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetTxCalTSSI(m_nTSSI), "DUT_SetTxCalTSSI");

				// [4] DUT start to transmit WIFI signal
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_TxOn(true), "DUT_TxOn(true)");

				// Wait DUT to transmit WIFI signal, default is 300ms.
				// m_nTxOnSleep could be configured by INI file.
				//    Sleep(m_nTxOnSleep);
				// [6]: Setup instrument and measure up-link result
				SetRepairItem($REPAIR_ITEM_INSTRUMENT);
				LogRawStrA(SPLOGLV_INFO, "InitTest");

				SPRESULT res = m_pRFTester->InitTest(WIFI_TXP, (LPVOID)&stParam);
				if(SP_OK != res)
				{
					NOTIFY("InitTest(TX)", LEVEL_ITEM, 1, 0, 1);
					return res;
				}
				LogRawStrA(SPLOGLV_INFO, "FetchResult");
				res = m_pRFTester->FetchResult(WIFI_TXP, &m_TesterRlt);
				if(SP_OK != res)
				{
					NOTIFY("FetchResult(TX)", LEVEL_ITEM, 1, 0, 1);
					return res;
				}
				double dPower = m_TesterRlt.txp.dAvgValue;
				NOTIFY_CAL_RESULT_WITH_ARG("WIFI TPC", LEVEL_ITEM,
					m_dPowerLimit[nBandIndex][nAnt][0], dPower, m_dPowerLimit[nBandIndex][nAnt][1],
					CwcnUtility::WLAN_BAND_NAME[eMode],
					stParam.nCenChan, "dBm",
					"%s", CwcnUtility::WLAN_ANT_NAME[ePath]);
				if(!IN_RANGE(m_dPowerLimit[nBandIndex][nAnt][0], dPower, m_dPowerLimit[nBandIndex][nAnt][1]))
				{
					LogFmtStrA(SPLOGLV_ERROR, "Wlan Tpc Over Range!");
					return SP_E_WCN_WLAN_TPC_FAIL;
				}
				m_TxCalRlt[nCount].nCh = stParam.nCenChan;
				m_TxCalRlt[nCount].ePath = ePath;
				m_TxCalRlt[nCount].nPower = int(dPower*10);

				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_SetTxCalPower(m_TxCalRlt[nCount].nPower), "DUT_SetTxCalPower");
				nCount ++;
				CHKRESULT_WITH_NOTIFY(m_pWlanApi->DUT_TxOn(false), "DUT_TxOn(false)");
			}
		}
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pWlanApi->DUT_SetTxCalPowerGroup(&m_TxCalRlt[0], nCount));
	return SP_OK;
}

SPRESULT CWlanAPC::ReadApcChan()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	std::vector<int> vectChan;
	SPRESULT res = m_pWlanApi->DUT_ReadAPCChannel(vectChan);
	if(SP_OK != res)
	{
		return SP_OK;
	}
	if(vectChan.size() == 0)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Read APC channel error!");
		return SP_E_SPAT_NOT_SUPPORT;
	}
	SPWI_WLAN_PARAM_TESTER stParam;
	for(int nBandIndex=0; nBandIndex<2; nBandIndex++)
	{
		for(int nAnt = 0; nAnt<2; nAnt++)
		{
			if(!m_bEnableCal[nBandIndex][nAnt] || 0 == m_TesterParam[nBandIndex][nAnt].size())
			{
				continue;
			}
			stParam = m_TesterParam[nBandIndex][nAnt][0];
			m_TesterParam[nBandIndex][nAnt].clear();
			for(int nCh=0; nCh<(int)vectChan.size(); nCh++)
			{
				if((0 == nBandIndex && vectChan[nCh] <= 14)
					|| (1 == nBandIndex && vectChan[nCh] >= 36))
				{
					stParam.nCenChan = vectChan[nCh];
					stParam.nPriChan = stParam.nCenChan;
					stParam.nCBWType = WIFI_BW_20M;
					stParam.nSBWType = WIFI_BW_20M;
					m_TesterParam[nBandIndex][nAnt].push_back(stParam);
				}
			}
		}
	}
	return SP_OK;
}
