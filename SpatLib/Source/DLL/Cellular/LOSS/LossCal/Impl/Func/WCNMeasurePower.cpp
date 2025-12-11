#include "StdAfx.h"
#include "WCNMeasurePower.h"
#include "ImpBase.h"
#include "WlanApiAT.h"
#include "BTApiAT.h"
#include "wcnUtility.h"

static E_WLAN_RATE g_szRate[WIFI_MaxProtocol] = { CCK_11, OFDM_54, MCS_7, MCS9_1SS, OFDM_54, HE_MCS11_1SS};

const double CWCNMeasurePower::g_GpsFreq[GPS_MAX_BAND] = {1575.42/*GPS_L1_BAND*/, 1176.45/*GPS_L5_BAND*/, 1561.098/*BD2_B1_BAND*/, 1207.14/*BD2_B2_BAND*/, 1268.52/*BD2_B3_BAND*/,1601.7188/*GLO_L1_BAND*/ };
const char CWCNMeasurePower::g_GpsBand[GPS_MAX_BAND][8] = { "GPS_L1", "GPS_L5", "BD_B1", "BD_B2", "BD_B3", "GLO_L1" };
CWCNMeasurePower::CWCNMeasurePower(CImpBase* pSpat)
	: ILossFunc(pSpat)
{
	m_pWlanApi = NULL;
	m_pBtApi = NULL;
	m_pGpsApi = NULL;
	m_pRFTester = m_pImp->m_pRFTester;
	ZeroMemory(&m_TesterParam, sizeof(m_TesterParam));
	ZeroMemory(&m_TesterRlt, sizeof(m_TesterRlt));
	ZeroMemory(&m_BtTesterParam, sizeof(m_BtTesterParam));
}


CWCNMeasurePower::~CWCNMeasurePower(void)
{
}

SPRESULT CWCNMeasurePower::WlanMeasurePower(WCN_MEASURE_T* pList, int nListCount)
{
	if (NULL == pList || 0 == nListCount)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "[%s]InValid input Param", __FUNCTION__);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	WCN_CHIPSET_TYPE_ENUM eCSType = m_pImp->m_eCSType;
	m_pImp->SetRepairMode(RepairMode_Wlan);
	m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_WIFI));
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));
	m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_EnterEUTMode(true));
	for (int i = 0; i < nListCount; i++)
	{
		for (int n = 0; n < WCN_ANT; n++)
		{
			ANTENNA_ENUM ePath = (ANTENNA_ENUM)pList[i].nPath[n];
			if (ANT_WLAN_INVALID == ePath)
			{
				continue;
			}
			ConfigWlanParam(&pList[i], n);

			WIFI_PROTOCOL_ENUM eType = (WIFI_PROTOCOL_ENUM)pList[i].nType;
			IsValidIndex(eType);
			m_pImp->SetRepairBand(CwcnUtility::m_WlanBandInfo[eType]);
			m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);

			//set band
			WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
			if ( pList[i].nChannel <= 14)
			{
				eBand = WLAN_BAND_24G;
			}
			else
			{
				eBand = WLAN_BAND_50G;
			}

			if (WCN_CS_TYPE_SC266X == eCSType)//Songshan W6
			{
				//Set Band
				CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetBand(eBand));
				if (WIFI_CW_SPECTRUM == eType)
				{
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetRfPath(ePath));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetCH(pList[i].nChannel));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_TxOn_CW(true));
					m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(WIFI_CW, (LPVOID)& m_TesterParam));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->FetchResult(WIFI_CW, (LPVOID)& m_TesterRlt));
					pList[i].dPower[n] = m_TesterRlt.CWPwr.dAvgValue;
				}
				else
				{
					//Set ANT
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetRfPath(ePath));
					//Set channel band width
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetBandWidth(WIFI_BW_20M));
					//Set signal band width
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetSigBandWidth(WIFI_BW_20M));

					if (WIFI_802_11ax == eType)
					{
						//HE_LTF_SIZE
						CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetHeLtfSize(WIFI_HE_LTF_SIZE_1));
						//GUARD INTERVAL
						CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetGuardInterval(WIFI_GUARDINTERVAL_08US));
					}
					////Set Channel
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetCH(pList[i].nChannel, pList[i].nChannel));
					// [2]: Setup DUT data rate
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetModRate(g_szRate[eType]));
					if (WIFI_802_11ax == eType)
					{
						//set channel coding
						CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetDecoMode(MODE_LDPC));
					}
					else
					{
						//set channel coding
						CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetDecoMode(MODE_BCC));
					}
					// [4] DUT start to transmit WIFI signal
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_TxOn(true));

					Sleep(100);

					m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(WIFI_TXP, (LPVOID)& m_TesterParam));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->FetchResult(WIFI_TXP, (LPVOID)& m_TesterRlt));
					pList[i].dPower[n] = m_TesterRlt.txp.dAvgValue;
				}
			}
			else//Marlin2,Marlin3
			{
				if (WIFI_CW_SPECTRUM == eType)
				{
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetRfPath(ePath));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetCH(pList[i].nChannel));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_TxOn_CW(true));
					m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(WIFI_CW, (LPVOID)& m_TesterParam));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->FetchResult(WIFI_CW, (LPVOID)& m_TesterRlt));
					pList[i].dPower[n] = m_TesterRlt.CWPwr.dAvgValue;
				}
				else
				{
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetRfPath(ePath));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetCH(pList[i].nChannel));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_SetModRate(g_szRate[eType]));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_TxOn(true));
					m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(WIFI_TXP, (LPVOID)& m_TesterParam));
					CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->FetchResult(WIFI_TXP, (LPVOID)& m_TesterRlt));
					pList[i].dPower[n] = std::round(m_TesterRlt.txp.dAvgValue * 100)/100.00;
				}
			}

			m_pImp->NOTIFY("WLAN Power", LEVEL_ITEM, -20.0, pList[i].dPower[n], 30.0, CwcnUtility::WLAN_BAND_NAME[eType],
				pList[i].nChannel, "dBm", "%s", CwcnUtility::WLAN_ANT_NAME[ePath]);
			if (!IN_RANGE(-20.0, pList[i].dPower[n], 30.0))
			{
				return SP_E_SPAT_OUT_OF_RANGE;
			}
			m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_TxOn(false));
		}
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pWlanApi->DUT_EnterEUTMode(false));
	return SP_OK;
}

SPRESULT CWCNMeasurePower::Init()
{
	m_pWlanApi = new CWlanApiAT(m_pImp->m_hDUT);
	if (NULL == m_pWlanApi)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "new CWlanApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pBtApi = new CBTApiAT(m_pImp->m_hDUT, BDR);
	if (NULL == m_pBtApi)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "new CBTApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pGpsApi = new CGPSApiAT(m_pImp->m_hDUT);
	if (NULL == m_pWlanApi)
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

void CWCNMeasurePower::ConfigWlanParam(WCN_MEASURE_T* pParam, int nAntIndex)
{
	ZeroMemory(&m_TesterParam, sizeof(m_TesterParam));

	m_TesterParam.nCenChan = pParam->nChannel;
	m_TesterParam.nPriChan = pParam->nChannel;

	ANTENNA_ENUM ePath = (ANTENNA_ENUM)pParam->nPath[nAntIndex];
	//CwcnUtility::WlanGetAnt(ePath, &m_TesterParam.ePort[0]);

	//set band
	WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
	if (pParam->nChannel <= 14)
	{
		eBand = WLAN_BAND_24G;
	}
	else
	{
		eBand = WLAN_BAND_50G;
	}
	CwcnUtility::WlanGetAntByBand(ePath, &m_TesterParam.ePort[0], eBand);


	m_TesterParam.nAvgCount = 3;
	m_TesterParam.eProto = (WIFI_PROTOCOL_ENUM)pParam->nType;
	m_TesterParam.nTotalPackets = 0;
	m_TesterParam.dRefLvl = pParam->dExpPower[nAntIndex];
	m_TesterParam.dVsgLvl = -70;
	m_TesterParam.nCBWType = WIFI_BW_20M;
	m_TesterParam.eRate = g_szRate[pParam->nType];
}

SPRESULT CWCNMeasurePower::BTMeasurePower(WCN_MEASURE_T* pList, int nListCount)
{
	if (NULL == pList || 0 == nListCount)
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
	if (nCurMode != BT_EUT)
	{
		SPRESULT res = SP_OK;
		int nRetry = 3;
		do
		{
			res = m_pBtApi->DUT_SetTestMode(BT_NST, 3000);
			if (SP_OK == res)
			{
				break;
			}
			Sleep(200);
		} while (--nRetry);
		CHKRESULT_WITH_NOTIFY1(m_pImp, res, "BT EnterMode");
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_BT));

	for (int i = 0; i < nListCount; i++)
	{
		for (int n = 0; n < WCN_ANT; n++)
		{
			BT_RFPATH_ENUM ePath = (BT_RFPATH_ENUM)pList[i].nPath[n];
			if (ANT_BT_INVALID == ePath)
			{
				continue;
			}
			ConfigBtParam(&pList[i], n);
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetRfPath(ePath));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetCH(BT_TX, m_BtTesterParam.nCh));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetPacketType(BT_TX, m_BtTesterParam.ePacketType));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetPacketLength(CwcnUtility::BT_MAX_PKTLEN[eType][m_BtTesterParam.ePacketType]));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_SetPattern(BT_TX, m_BtTesterParam.ePattern));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pBtApi->DUT_RFOn(BT_TX, true));
			Sleep(500);
			m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);

			DWORD dwMask = (eType == BLE || eType == BLE_EX) ? BLE_POWER : BDR_POWER;

			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(dwMask, &m_BtTesterParam));
			//Fetch meas results
			SPBT_RESULT_T tBtTesterRlt;
			ZeroMemory(&tBtTesterRlt, sizeof(tBtTesterRlt));
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->FetchResult(dwMask, &tBtTesterRlt));
			pList[i].dPower[n] = std::round(tBtTesterRlt.Power.dAvgValue * 100)/100.00;
			m_pImp->NOTIFY("BT Power", LEVEL_ITEM, -20.0, pList[i].dPower[n], 30.0, CwcnUtility::BT_BAND_NAME[eType],
				pList[i].nChannel, "dBm", "%s", CwcnUtility::BT_ANT_NAME[ePath]);
			if (!IN_RANGE(-20.0, pList[i].dPower[n], 30.0))
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

void CWCNMeasurePower::ConfigBtParam(WCN_MEASURE_T* pParam, int nAntIndex)
{
	BT_RFPATH_ENUM ePath = (BT_RFPATH_ENUM)pParam->nPath[nAntIndex];
	m_BtTesterParam.eRfPort = CwcnUtility::BTGetAnt(ePath);

	m_BtTesterParam.nAvgCount = 10;
	if (pParam->nType == BLE)
	{
		m_BtTesterParam.eProto = BLE;
	}
	else if (pParam->nType == BLE_EX)
	{
		m_BtTesterParam.eProto = BLE_EX;
	}
	else
	{
		m_BtTesterParam.eProto = BDR;
	}
	m_BtTesterParam.nTotalPackets = 0;


	m_BtTesterParam.dRefLvl = pParam->dExpPower[nAntIndex];
	m_BtTesterParam.dVsgLvl = -70;
	if (pParam->nType == BLE || pParam->nType == BLE_EX)
	{
		m_BtTesterParam.ePattern = BLE_TX_PBRS9;
		if (pParam->nType == BLE_EX && m_BtTesterParam.ePacketType == RF_PHY_2M)
		{
			m_BtTesterParam.ePattern = BLE_TX_0xAA;
		}
	}
	else
	{
		m_BtTesterParam.ePattern = BDR_TX_PBRS9;
	}

	if (pParam->nType == BDR)
	{
		m_BtTesterParam.ePacketType = BDR_DH5;
	}
	else if (pParam->nType == EDR)
	{
		m_BtTesterParam.ePacketType = EDR_2DH1;
	}
	else if (pParam->nType == BLE)
	{
		m_BtTesterParam.ePacketType = BLE_RF_PHY_TEST_REF;
	}
	else
	{
		m_BtTesterParam.ePacketType = RF_PHY_1M;
	}
	m_BtTesterParam.nCh = pParam->nChannel;

	m_BtTesterParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[m_BtTesterParam.eProto][m_BtTesterParam.ePacketType];
}

SPRESULT CWCNMeasurePower::GetGPSCNR(GPS_BAND eBand, RF_ANT_E eAnt, double& dPower, double dCellPower)
{
	m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetNetMode(NM_GPS));
	m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);

	GPSParam stParam;
	stParam.dVsgLvl = dCellPower;

	int nCmdSleep = 3000;

	stParam.eRfPort = eAnt;

	stParam.dDlFreqMHz = g_GpsFreq[eBand];

	m_pImp->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
    WCN_CHIPSET_TYPE_ENUM eCSType = m_pImp->m_eCSType;
	double dTargetCnr = 50.0;
	double dCNR = 0;
	int nTotal = 10;
	do
	{
		int nTestCnt = 5;
		int nAverage = 0;
		int m_cnr = -999999;
		int nPreSNr = -999999;
		int nRetry = 15;
		dCNR = 0;
		m_pImp->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
		CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->InitTest(TRUE, &stParam));
		CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetGen(MM_CW, TRUE));

		//CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetTestMode(GPS_LEAVE, TIMEOUT_3S));
        if (WCN_CS_TYPE_SC266X == eCSType)//Songshan W6
        {
            CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetSystemType(CPMODE_GPS_MASK, TIMEOUT_3S));
            CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetBand(eBand));
            CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetCwFreqID(eBand));
        }
//         if (WCN_CS_TYPE_SC265X == eCSType)//Marlin3
//         {
//             CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetBand(eBand));
//         }
		CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, GPS_ModeEnable(GPS_CW));

		Sleep(nCmdSleep);
		m_pGpsApi->DUT_GetCNR(m_cnr);
		Sleep(nCmdSleep);
        do
        {
            if (SP_OK != m_pGpsApi->DUT_GetCNR(m_cnr))
            {
                m_pImp->NOTIFY("GPS GetCNR", 1, 0, 1);
                CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, GPS_ModeEnable(GPS_LEAVE));
                return SP_E_SPAT_TIMEOUT;
            }

			if (abs(nPreSNr - m_cnr) <= 2)
			{
				dCNR += m_cnr;
				nAverage++;
				if (nAverage == nTestCnt)
				{
					break;
				}
			}
			nPreSNr = m_cnr;
			Sleep(nCmdSleep);
		} while (--nRetry);

		if (nAverage != nTestCnt)
		{
			m_pImp->NOTIFY("GPS Measure", 1, 0, 1);
			CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, GPS_ModeEnable(GPS_LEAVE));
			return SP_E_SPAT_TIMEOUT;
		}
		dCNR = dCNR / nTestCnt;

		m_pImp->NOTIFY("GPS CNR", LEVEL_ITEM, NOLOWLMT, dCNR, NOUPPLMT, g_GpsBand[eBand],
			-1, "dBm", "Freq = %0.2f CellPower = %.2f", stParam.dDlFreqMHz, stParam.dVsgLvl);
		stParam.dVsgLvl += dTargetCnr - dCNR;
        CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, GPS_ModeEnable(GPS_LEAVE));
	} while (!IN_RANGE(dTargetCnr - 1, dCNR, dTargetCnr + 1) && nTotal--
		&& IN_RANGE(-130.0, stParam.dVsgLvl, -60.0));

	if (!IN_RANGE(dTargetCnr - 1, dCNR, dTargetCnr + 1))
	{
		//CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetTestMode(GPS_LEAVE, TIMEOUT_3S));
		m_pImp->NOTIFY("GPS CNR", LEVEL_ITEM, dTargetCnr - 1, dCNR, dTargetCnr + 1, "GPS",
			-1, "dBm");
		return SP_E_SPAT_OUT_OF_RANGE;
	}
	string str = g_GpsBand[eBand];
	str += " Level";
	m_pImp->NOTIFY("GPS Power", LEVEL_ITEM, NOLOWLMT, stParam.dVsgLvl, NOUPPLMT, str.c_str(),
		-1, "dBm");
	dPower = std::round(stParam.dVsgLvl * 100)/100.00;
	//CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_SetTestMode(GPS_LEAVE, TIMEOUT_3S));
	CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pRFTester->SetGen(MM_CW, FALSE));

	m_pImp->SetRepairItem($REPAIR_ITEM_SNR);
	return SP_OK;
}


SPRESULT CWCNMeasurePower::GPS_ModeEnable(GPS_MODE eMode)
{
    SPRESULT result = SP_E_WCN_BASE_ERROR;
    int nCurMode = -1;

    CHKRESULT_WITH_NOTIFY_FUNNAME1(m_pImp, m_pGpsApi->DUT_GetTestMode(nCurMode));
    if (nCurMode == eMode)
    {
        result = SP_OK;
    }
    else
    {
        unsigned long      ulTimeOut = 10000;//(GPS_LEAVE != m_eMode) ? 10000 : 3000;
        unsigned int          nCount = 0;
        const unsigned int MAX_RETRY = 1;

        do
        {
            if (SP_OK == m_pGpsApi->DUT_SetTestMode(eMode, ulTimeOut))
            {
                Sleep(TIMEOUT_1S);
                result = SP_OK;
                break;
            }
            else
            {
                Sleep(200);
            }

        } while (++nCount < MAX_RETRY);
    }

    return result;
}
