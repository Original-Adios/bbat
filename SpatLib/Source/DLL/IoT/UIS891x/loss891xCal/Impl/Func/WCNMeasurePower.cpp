#include "StdAfx.h"
#include "WCNMeasurePower.h"
#include "ImpBaseUIS8910.h"
#include "WlanApiAT.h"
#include "BTApiAT.h"
#include "wcnUtility.h"

static E_WLAN_RATE g_szRate[WIFI_MaxProtocol] = {CCK_11, OFDM_54, MCS_7, MCS8_1SS, CCK_11};

CWCNMeasurePower::CWCNMeasurePower(CImpBaseUIS8910 *pSpat)
: ILossFunc_UIS8910(pSpat)
{
	m_pWlanApi = NULL;
	m_pBtApi = NULL;
	m_pGpsApi = NULL;
	m_pRFTester = m_pImp->m_pRFTester;
	m_hDUT = m_pImp->m_hDUT;
    ZeroMemory((void *)(&m_TesterRlt),sizeof(SPWI_RESULT_T));
	ZeroMemory((void *)(&m_BtTesterParam),sizeof(SPWI_BT_PARAM_TESTER));
	ZeroMemory((void *)(&m_BtTesterRlt),sizeof(SPBT_RESULT_T));
	ZeroMemory((void *)(&m_TesterParam),sizeof(SPWI_WLAN_PARAM_TESTER));
}


CWCNMeasurePower::~CWCNMeasurePower(void)
{
}

SPRESULT CWCNMeasurePower::WlanMeasurePower(WCN_MEASURE_T *pList, int nListCount )
{
	if((NULL == pList) || (0 == nListCount))
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "[%s]InValid input Param", __FUNCTION__);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	m_pImp->SetRepairMode(RepairMode_Wlan);
	m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_WIFI));
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL))
	CHKRESULT_WITH_NOTIFY1(m_pImp, SP_WifiActive_UIS8910(m_hDUT, TRUE), "Wifi MeasurePower Active fail.");

	for(int i=0; i<nListCount; i++)
	{
		for(int n=0; n<MAX_RF_ANT; n++)
		{
			ANTENNA_ENUM ePath = (ANTENNA_ENUM)pList[i].nPath[n];
			if(ANT_WLAN_INVALID == ePath)
			{
				continue;
			}
			WIFI_PROTOCOL_ENUM eType = (WIFI_PROTOCOL_ENUM)pList[i].nType;
            IsValidIndex(eType);
			m_pImp->SetRepairBand(CwcnUtility::m_WlanBandInfo[eType]);
			m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
			ConfigWlanParam(&pList[i], n);
			m_pImp->LogFmtStrA(SPLOGLV_INFO, "InitTest CellPwr = %d", pList[i].dExpPower[n]);
			SPRESULT res = m_pRFTester->InitTest(WIFI_PER, (LPVOID)&m_TesterParam);
			if (SP_OK != res)
			{
				m_pImp->NOTIFY("InitTest(RX)", LEVEL_ITEM, 1, 0, 1);
				return res;
			}

			// DUT start to Receive WIFI signal
			PC_WIFI_RX_REQ_UIS8910_T Param;
			memset(&Param, 0, sizeof(PC_WIFI_RX_REQ_UIS8910_T));
			Param.Arfcn = pList[i].nChannel;
			CHKRESULT_WITH_NOTIFY1(m_pImp, SP_WifiRxReq_UIS8910(m_hDUT, TRUE, &Param), "Wifi Calibration RxOn fail.");

			// get Dut Rssi measure result
			m_pImp->LogRawStrA(SPLOGLV_INFO, "FetchResult");
			PC_WIFI_RSSI_REQ_UIS8910_T RSSI;
			int Count = 0;
			memset(&RSSI, 0, sizeof(PC_WIFI_RSSI_REQ_UIS8910_T));
			CHKRESULT_WITH_NOTIFY1(m_pImp, SP_WifiGetRSSI_UIS8910(m_hDUT, &RSSI), "Wifi Calibration GetRssi fail.");
			while (RSSI.Valid != 1)
			{
				Count++;
				Sleep(20);
				CHKRESULT_WITH_NOTIFY1(m_pImp, SP_WifiGetRSSI_UIS8910(m_hDUT, &RSSI), "Wifi Calibration GetRssi fail.");
				if (Count > 50)
					return SP_E_WCN_WLAN_AGC_FAIL;
				m_pImp->LogFmtStrA(SPLOGLV_VERBOSE, "Wifi Get Rssi Valid = %d; Cnt = %d", RSSI.Valid, RSSI.Cnt);
			}
			for (int Cnt = 0; Cnt < RSSI.Cnt; Cnt++)
			{
				if (!IN_RANGE(0, RSSI.AgcIdx[Cnt], 16))
				{
					m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Wifi Rx AgcIdx Over Range:AgcIdx = %d", RSSI.AgcIdx[Cnt]);
					return SP_E_WCN_WLAN_AGC_FAIL;
				}
				m_pImp->NOTIFY("Wifi Rx Rssi", LEVEL_ITEM,
					pList[i].dExpPower[n] - 20, RSSI.Rssi[Cnt] / 16.0, pList[i].dExpPower[n] + 20,
					CwcnUtility::WLAN_BAND_NAME[eType],
					pList[i].nChannel, "dB",
					"CellPwr = %0.1f; AgcIdx = %d", 
					pList[i].dExpPower[n], RSSI.AgcIdx[Cnt]);
				if (!IN_RANGE(pList[i].dExpPower[n] - 20, RSSI.Rssi[Cnt] / 16.0, pList[i].dExpPower[n] + 20))
				{
					m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Wifi Rx Rssi Over Range!");
					return SP_E_WCN_WLAN_AGC_FAIL;
				}
				pList[i].Para[n].AgcIdx[Cnt] = (int8)RSSI.AgcIdx[Cnt];
				pList[i].Para[n].Power[Cnt] = RSSI.Rssi[Cnt];
			}
			pList[i].Para[n].Cnt = (int8)RSSI.Cnt;
			CHKRESULT_WITH_NOTIFY1(m_pImp, SP_WifiRxReq_UIS8910(m_hDUT, FALSE, &Param), "Wifi Calibration RxOff fail.");
		}
	}

	CHKRESULT_WITH_NOTIFY1(m_pImp, SP_WifiActive_UIS8910(m_hDUT, FALSE), "Wifi MeasurePower DeActive fail.");

	return SP_OK;
}

SPRESULT CWCNMeasurePower::Init()
{
	m_pWlanApi = new CWlanApiAT(m_pImp->m_hDUT);
	if(NULL == m_pWlanApi)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "new CWlanApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pBtApi = new CBTApiAT(m_pImp->m_hDUT, BDR);
	if(NULL == m_pBtApi)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "new CBTApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pGpsApi = new CGPSApiAT(m_pImp->m_hDUT);
	if(NULL == m_pWlanApi)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "new CGPSApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	return SP_OK;
}

SPRESULT CWCNMeasurePower::Release()
{
	delete m_pWlanApi;
	m_pWlanApi = NULL;
	delete m_pBtApi;
	m_pBtApi = NULL;
	delete m_pGpsApi;
	m_pGpsApi = NULL;
	return SP_OK;
}

void CWCNMeasurePower::ConfigWlanParam( WCN_MEASURE_T *pParam, int nAntIndex)
{
	ZeroMemory(&m_TesterParam, sizeof(m_TesterParam));
	
	m_TesterParam.nCenChan = pParam->nChannel;
	m_TesterParam.nPriChan = pParam->nChannel;

	ANTENNA_ENUM ePath = (ANTENNA_ENUM)pParam->nPath[nAntIndex];
	CwcnUtility::WlanGetAnt(ePath, &m_TesterParam.ePort[0]);
	
	m_TesterParam.nAvgCount = 1;
	m_TesterParam.eProto = (WIFI_PROTOCOL_ENUM)pParam->nType;
	m_TesterParam.nTotalPackets = 5;
	m_TesterParam.dRefLvl = 20;
	m_TesterParam.dVsgLvl = pParam->dExpPower[nAntIndex];
	m_TesterParam.nCBWType = WIFI_BW_20M;
	m_TesterParam.eRate = g_szRate[pParam->nType];
}

SPRESULT CWCNMeasurePower::BTMeasurePower(WCN_MEASURE_T *pList, int nListCount )
{
    IsValidIndex(m_BtTesterParam.ePacketType);
    if((NULL == pList) || (0 == nListCount))
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "[%s]InValid input Param", __FUNCTION__);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	int nCurMode = -1;
	BT_TYPE eType = (BT_TYPE)pList[0].nType;
    IsValidIndex(eType);
	m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->SetType(eType));
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_GetTestMode(nCurMode));
	if(nCurMode != BT_EUT)
	{
		SPRESULT res = SP_OK;
		int nRetry = 3;
		do 
		{
			res =  m_pBtApi->DUT_SetTestMode(BT_NST, 3000);
			if(SP_OK == res)
			{
				break;
			}
			Sleep(200);
		} while (--nRetry);
		CHKRESULT_WITH_NOTIFY1(m_pImp, res, "BT EnterMode"); 
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_BT));
	 
	for(int i=0; i<nListCount; i++)
	{
		for(int n=0; n<MAX_RF_ANT; n++)
		{
			BT_RFPATH_ENUM ePath = (BT_RFPATH_ENUM)pList[i].nPath[n];

			if(ANT_BT_INVALID == ePath)
			{
				continue;
			}
			ConfigBtParam( &pList[i], n);
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetRfPath(ePath));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetCH(BT_TX, m_BtTesterParam.nCh));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetPacketType(BT_TX, m_BtTesterParam.ePacketType));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetPacketLength(CwcnUtility::BT_MAX_PKTLEN[eType][m_BtTesterParam.ePacketType]));	
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetPattern(BT_TX, m_BtTesterParam.ePattern));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_RFOn(BT_TX, true));
			Sleep(500);
			m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);

			DWORD dwMask = (eType == BLE || eType == BLE_EX )? BLE_POWER:BDR_POWER;

			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(dwMask, &m_BtTesterParam));
			//Fetch meas results
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->FetchResult(dwMask, &m_BtTesterRlt));
			pList[i].dPower[n] = m_BtTesterRlt.Power.dAvgValue;
			m_pImp->NOTIFY("BT Power", LEVEL_ITEM, -20.0, pList[i].dPower[n], 30.0, CwcnUtility::BT_BAND_NAME[eType],
				pList[i].nChannel, "dBm", "%s", CwcnUtility::BT_ANT_NAME[ePath]);
			if(!IN_RANGE(-20.0, pList[i].dPower[n], 30.0))
			{
				return SP_E_SPAT_OUT_OF_RANGE;
			}
			//Transmit disable
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_RFOn(BT_TX, false));
		}
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetTestMode(BT_LEAVE, 10000));

	return SP_OK;
}

void CWCNMeasurePower::ConfigBtParam(WCN_MEASURE_T *pParam, int nAntIndex)
{
	BT_RFPATH_ENUM ePath = (BT_RFPATH_ENUM)pParam->nPath[nAntIndex];
	m_BtTesterParam.eRfPort = CwcnUtility::BTGetAnt(ePath);
	
	m_BtTesterParam.nAvgCount = 10;
	if(pParam->nType ==  BLE )	
	{
		m_BtTesterParam.eProto = BLE;
	}
	else if(pParam->nType == BLE_EX)
	{
		m_BtTesterParam.eProto = BLE_EX;
	}
	else
	{
		m_BtTesterParam.eProto = BDR;
	}
	m_BtTesterParam.nTotalPackets = 0;

	
	m_BtTesterParam.dRefLvl =  pParam->dExpPower[nAntIndex];
	m_BtTesterParam.dVsgLvl = -70;
	if(pParam->nType ==  BLE || pParam->nType ==  BLE_EX)
	{
		m_BtTesterParam.ePattern = BLE_TX_PBRS9;
	}
	else
	{
		m_BtTesterParam.ePattern = BDR_TX_PBRS9;
	}

	if(pParam->nType ==  BDR)
	{
		m_BtTesterParam.ePacketType = BDR_DH5;
	}
	else if(pParam->nType ==  EDR)
	{
		m_BtTesterParam.ePacketType = EDR_2DH1;
	}
	else if(pParam->nType ==  BLE)
	{
		m_BtTesterParam.ePacketType = BLE_RF_PHY_TEST_REF;
	}
	else
	{
		m_BtTesterParam.ePacketType = RF_PHY_1M;
	}
	m_BtTesterParam.nCh = pParam->nChannel;
}

SPRESULT CWCNMeasurePower::GetGPSSNR( double& dSNR )
{
	int nCurMode = -1;
	m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_GPS));
	m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_GPS));
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_GetTestMode(nCurMode));
	if(nCurMode != GPS_EUT)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetTestMode(GPS_EUT, 3000));
	}
	
	m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	GPSParam stParam;
	stParam.dVsgLvl = -130;
	
	
	int nCmdSleep = 3000;
	int nRetryCount = 5;
	stParam.eRfPort =  RF_ANT_1st;

	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(TRUE, &stParam));

	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetGen(MM_MODULATION, TRUE));
	m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	double m_snr = -999999.0;
	double nPreSNr = -999999.0;
	int sv_id  = 0;
	int sv_num = 0;
	int nCount = 15;
	int nAverage = 0;
	Sleep(nCmdSleep);
//	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_GetSNR(m_snr, sv_id, sv_num));
	m_pGpsApi->DUT_GetSNR(m_snr, sv_id, sv_num);
	do 
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_GetSNR(m_snr, sv_id, sv_num));
		if(sv_num > 0 )
		{
			if(abs(nPreSNr-m_snr) <= 2)
			{
				dSNR += m_snr;
				nAverage++;
				if(nAverage == nRetryCount)
				{
					break;
				}
			}
			nPreSNr = m_snr;
		}
		Sleep(nCmdSleep);
	} while (--nCount);
	if(nAverage != nRetryCount)
	{
		m_pImp->NOTIFY("GPS Measure", 1, 0, 1);
		return SP_E_SPAT_TIMEOUT;
	}
	
	dSNR = dSNR/nRetryCount;

	m_pImp->NOTIFY("GPS SNR", LEVEL_ITEM, NOLOWLMT, dSNR, NOUPPLMT, "GPS",
		-1, "dB");

	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetGen(MM_MODULATION, false));
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetTestMode(GPS_LEAVE, 10000));
	m_pImp->SetRepairItem($REPAIR_ITEM_SNR);
	return SP_OK;
}
