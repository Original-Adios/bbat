#include "StdAfx.h"
#include <cassert>
#include <process.h>
#include "WcnTestSysWear.h"
#include "ExtraLogFile.h"
#include "Winsock2.h"
#include "wcnUtility.h"
#include "NetUtility.h"
#pragma comment(lib, "ws2_32.lib")

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWcnTestSysWear)

extern const int WLAN_TX_TIME = 2000;
extern const int WLAN_RX_TIME = 4000;
extern const int WLAN_END_TIME = 5000;
// Marlin2:
// {	1, "DSSS-1"},   {2, "DSSS-2"},   {5, "CCK-5.5"},  {11, "CCK-11"},
//     {6, "OFDM-6"},   {9, "OFDM-9"},   {12, "OFDM-12"}, {18, "OFDM-18"},
//     {24, "OFDM-24"}, {36, "OFDM-36"}, {48, "OFDM-48"}, {54, "OFDM-54"},
//     {7, "MCS-0"},    {13, "MCS-1"},   {19, "MCS-2"},   {26, "MCS-3"},
//     {39, "MCS-4"},   {52, "MCS-5"},   {58, "MCS-6"},   {65, "MCS-7"},
//	   {-1, "null"}
extern const int WLAN_RATE_TABLE[MAX_WLAN_RATE] = {1,2,5,11,6,9,12,18,24,36,48,54,7,13,19,26,39,52,58,65};//Marlin2
extern const int BT_TX_TIME = 2000;
extern const int BT_RX_TIME = 4000;
extern const int BT_END_TIME = 5000;

CWcnTestSysWear::CWcnTestSysWear(void)
{
	m_hThread = NULL;
	m_rltRunScrip = SP_OK;
	m_nBeginTimeOut = 10;
	m_nTestTimeOut = 3;
	m_nConnectTimeOut = 30;
	ZeroMemory(&m_ca, sizeof(m_ca));
	m_bWlan = FALSE;
	m_bBt = FALSE;
	m_nWlanEnterModeTime = 1000;
	m_nBtEnterModeTime = 4000;
	m_nTriggerTimeOut = 5000;
	m_dwTime = 0;
	m_nWlanLeaveModeTime = 1000;
}

CWcnTestSysWear::~CWcnTestSysWear(void)
{
}

SPRESULT CWcnTestSysWear::__InitAction(void)
{
	//CHKRESULT(__super::__InitAction());

	return SP_OK;
}

BOOL CWcnTestSysWear::LoadXMLConfig(void)
{
	(__super::LoadXMLConfig());
	m_nBeginTimeOut =  GetConfigValue(L"Option:TimeOut:WaitForBegin", 30);
	m_nTestTimeOut = GetConfigValue(L"Option:TimeOut:WaitForTest", 3);
	m_nConnectTimeOut = GetConfigValue(L"Option:TimeOut:WaitForConnect", 30);
	m_strDeviceFilePath = _W2CA(GetConfigValue(L"Param:Common:DeviceFilePath", L""));
	m_nWlanEnterModeTime = GetConfigValue(L"Option:TimeOffset:EnterWlanMode", 1000);
	m_nWlanLeaveModeTime = GetConfigValue(L"Option:TimeOffset:LeaveWlanMode", 1000);
	m_nBtEnterModeTime = GetConfigValue(L"Option:TimeOffset:EnterBtMode", 2000);

	m_nTriggerTimeOut = GetConfigValue(L"Option:TimeOffset:TxTriggerDelay", 5000);

	trimA(m_strDeviceFilePath);
	if (0 == m_strDeviceFilePath.length())
	{
		return FALSE;
	}
	if(!WlanLoadXML())
	{
		return FALSE;
	}
	if(!BTLoadXML())
	{
		return FALSE;
	}
	if(!m_bBt && !m_bWlan)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CWcnTestSysWear::WlanLoadXML()
{
	//增加5G协议
	wstring strProtocol[WIFI_MaxProtocol] = {L"Wlan11b", L"Wlan11g", L"Wlan11n", L"Wlan11ac", L"Wlan11a", L"WlanNoSupport"};
	m_vectWlanMeasParam.clear();
	m_vectWlanSpec.clear();
	for( int nProtoc=WIFI_802_11b; nProtoc<WIFI_MaxProtocol; nProtoc++)
	{
		WIFI_PROTOCOL_ENUM eProtoc = (WIFI_PROTOCOL_ENUM)nProtoc;
		WlanMeasBandSpec stSpec;
		//BOOL bProtocEnable = FALSE;
		wstring strBandSel   = L"Option:Band:" + strProtocol[nProtoc];
		wstring strParamChan = L"Param:" + strProtocol[nProtoc];

		BOOL bEnable = (BOOL)GetConfigValue((strBandSel).c_str(), FALSE);
		if(!bEnable)
		{
			continue;
		}
		//bProtocEnable = TRUE;
		wstring strPath = strParamChan + L":GroupCount";
		int nGroupCount = GetConfigValue(strPath.c_str(), 0);
		for(int nGroupIndex = 0; nGroupIndex < nGroupCount; nGroupIndex++)
		{
			SPWI_WLAN_PARAM_MEAS_GROUP stMeasParam;
			stMeasParam.eMode = eProtoc;
			stMeasParam.stTesterParamGroupSub.eAnt = ANT_PRIMARY;
			stMeasParam.stTesterParamGroupSub.nAvgCount = 1;

			wchar_t    szGroup[20] = {0};
			swprintf_s(szGroup, L":Group%d", nGroupIndex+1);
			strPath = strParamChan + szGroup ;

			//Get modulation type
			LPCTSTR lpVal = GetConfigValue((strPath + L":Modulation").c_str(), L"MCS-7");
			E_WLAN_RATE eRate = CwcnUtility::WlanGetRate( _W2CA(lpVal));
			if(INVALID_WLAN_RATE == eRate)
			{
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stMeasParam.stTesterParamGroupSub.eRate = eRate;
			stMeasParam.stTesterParamGroupSub.nTotalPackets = GetConfigValue((strPath + L":PacketCount").c_str(), 1000);
			//Get per rx level

			stMeasParam.stTesterParamGroupSub.dVsgLvl = GetConfigValue((strPath + L":RXLVL:" + lpVal).c_str(), -65.0);
			if(!IN_RANGE(-100, stMeasParam.stTesterParamGroupSub.dVsgLvl, -25))
			{
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			//Get ref level
			stMeasParam.stTesterParamGroupSub.dRefLvl = GetConfigValue((strPath + L":RefLVL").c_str(), 10);
			//Chan band width
			stMeasParam.stTesterParamGroupSub.nCBWType = WIFI_BW_20M;
			//Signal band width
			stMeasParam.stTesterParamGroupSub.nSBWType = GetConfigValue((strPath + L":SBW").c_str(), 0);
			//BLDC
			stMeasParam.stTesterParamGroupSub.nChCode = GetConfigValue((strPath + L":ChanCoding").c_str(), 0);


			//TXP
			bEnable = (BOOL)GetConfigValue((strPath + L":TestItem:" + L"TXP").c_str(), FALSE);
			if (bEnable)
			{
				stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_TXP;
			}
			//PER
			bEnable = (BOOL)GetConfigValue((strPath + L":TestItem:" + L"PER").c_str(), FALSE);
			if (bEnable)
			{
				stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_PER;
			}

			wstring strVal = GetConfigValue((strPath + L":CenChan").c_str(), L"7");
			int nCount = 0;
			INT *parrInt = GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			int nFreqOff = GetConfigValue((strPath + L":PriChan").c_str(), 0);
			int nDelta = stMeasParam.stTesterParamGroupSub.nCBWType - stMeasParam.stTesterParamGroupSub.nSBWType;
			if (nDelta == 1 && abs(nFreqOff) > 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT] channel configuration is not correct!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			int nCenChan = 0;
			int nPrimChan = 0;
			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				nCenChan = parrInt[nChTmp];
				stMeasParam.stChan.nCenChan = nCenChan;
				nPrimChan = nCenChan + nFreqOff*2;
				stMeasParam.stChan.nPriChan = nPrimChan;
				m_vectWlanMeasParam.push_back(stMeasParam);
			}
		}

		//if(!bProtocEnable)
		//{
		//	continue;
		//}
		strPath = L"Param:" + strProtocol[nProtoc] + L":Specification";
		//TXP spec
		int nCount = 0;
		wstring strVal = GetConfigValue((strPath + L":TXP").c_str(), L"8,20");
		double* parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
		if(nCount < 2)
		{
			LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		m_vectWlanSpec[eProtoc].dTxp.low = parrDouble[0];
		m_vectWlanSpec[eProtoc].dTxp.upp = parrDouble[1];
		m_vectWlanSpec[eProtoc].dTXPant2nd.low = parrDouble[0];
		m_vectWlanSpec[eProtoc].dTXPant2nd.upp = parrDouble[1];
		//PER spec
		m_vectWlanSpec[eProtoc].dPer = GetConfigValue((strPath + L":PER").c_str(), 10.0);

	}
	if(m_vectWlanMeasParam.size() > 0)
	{
		m_bWlan = TRUE;
	}
	else
	{
		m_bWlan = FALSE;
	}
	return TRUE;
}



SPRESULT CWcnTestSysWear::__PollAction( void )
{
	SetRepairMode(RepairMode_Wlan);
	CInstrumentLock rfLock(m_pRFTester);
	if(m_bWlan)
	{
		CHKRESULT(ConfigWlanParam());
	}
	if(m_bBt)
	{
		CHKRESULT(ConfigBtParam());
	}
	CHKRESULT(DoList());
	if(m_bWlan)
	{
		CHKRESULT(DoWlanTest());
	}
	if(m_bBt)
	{
		CHKRESULT(DoBtTest());
	}
	CHKRESULT(WaitForReconnect(m_nConnectTimeOut));
	if(m_bWlan)
	{
		CHKRESULT(GetWlanRxResult());
	}
	if(m_bBt)
	{
		CHKRESULT(GetBtRxResult());
	}
	return SP_OK;
}


void CWcnTestSysWear::__LeaveAction( void )
{
}

SPRESULT CWcnTestSysWear::__FinalAction(void)
{
	return SP_OK;
}

SPRESULT CWcnTestSysWear::DoWlanTest()
{
	CSPTimer timer;
	SPWI_WLAN_PARAM_TESTER stTester;
	SPWI_RESULT_T stRlt;
	SPRESULT res = SP_OK;
	SPRESULT rlt = SP_OK;
	m_dwTime += m_nWlanEnterModeTime;
	for(int i=0; i< (int)m_vectWlanMeasParam.size(); i++)
	{
		SPWI_WLAN_PARAM_MEAS_GROUP *pMeasParam = &m_vectWlanMeasParam[i];
		DWORD nMask = pMeasParam->stTesterParamGroupSub.dwMask & (~WIFI_PER);
		stTester.eProto = pMeasParam->eMode;
		stTester.dRefLvl = pMeasParam->stTesterParamGroupSub.dRefLvl;
		stTester.dVsgLvl =  pMeasParam->stTesterParamGroupSub.dVsgLvl;
		//CwcnUtility::WlanGetAnt(pMeasParam->stTesterParamGroupSub.eAnt, &stTester.ePort[0]);
		stTester.eRate = pMeasParam->stTesterParamGroupSub.eRate;
		stTester.nAvgCount = pMeasParam->stTesterParamGroupSub.nAvgCount;
		stTester.nCBWType = pMeasParam->stTesterParamGroupSub.nCBWType;
		stTester.nCenChan = pMeasParam->stChan.nCenChan;
		stTester.nPriChan = pMeasParam->stChan.nPriChan;
		stTester.nSBWType = pMeasParam->stTesterParamGroupSub.nSBWType;
		stTester.nTotalPackets = pMeasParam->stTesterParamGroupSub.nTotalPackets;

		//set band
		WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
		if (stTester.nCenChan <= 14)
		{
			eBand = WLAN_BAND_24G;
		}
		else
		{
			eBand = WLAN_BAND_50G;
		}
		CwcnUtility::WlanGetAntByBand(pMeasParam->stTesterParamGroupSub.eAnt, &stTester.ePort[0], eBand);

		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_WIFI));
		BOOL bAutoLevel = FALSE;
		m_pRFTester->SetProperty(DP_WLAN_AUTO_LEVEL, 0, &bAutoLevel);
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));
		do
		{
			rlt = SP_E_SPAT_TIMEOUT;
			if(GetTickCount() > WLAN_TX_TIME/2 + m_dwTime)
			{
				res = m_pRFTester->InitTest(nMask, &stTester);
				if(SP_OK != res)
				{
					rlt = res;
					continue;
				}
				stRlt.Init();
				res = m_pRFTester->FetchResult(nMask, &stRlt);
				if(SP_OK != res)
				{
					rlt = res;
					continue;
				}
				else
				{
					rlt = SP_OK;
					break;
				}
			}
		} while (GetTickCount() < WLAN_TX_TIME + m_dwTime);
		if(SP_OK != rlt)
		{
			NOTIFY("Measure Timeout", LEVEL_ITEM|LEVEL_FT,1, 0, 1);
			return rlt;
		}
		/*int nTimeOut = m_nTestTimeOut;
		if(0 == i)
		{
		nTimeOut = m_nBeginTimeOut;
		}
		do
		{
		if(_IsUserStop())
		{
		return SP_E_USER_ABORT;
		}
		res = m_pRFTester->InitTest(nMask, &stTester);
		if(SP_OK != res)
		{
		rlt = res;
		continue;
		}
		stRlt.Init();
		res = m_pRFTester->FetchResult(nMask, &stRlt);
		if(SP_OK != m_rltRunScrip)
		{
		NOTIFY("RunScrip", LEVEL_ITEM, 1, 0, 1);
		LogFmtStrA(SPLOGLV_ERROR, "SP_apRunFile fail,err =%d", m_rltRunScrip);
		return m_rltRunScrip;
		}
		if(SP_OK != res)
		{
		rlt = res;
		continue;
		}
		else if(stRlt.txp.dAvgValue < stTester.dRefLvl-10 ||  stRlt.txp.dAvgValue > 99.0)
		{
		rlt = SP_E_RF_INVALID_RESULT;
		continue;
		}
		else
		{
		rlt = SP_OK;
		break;
		}

		} while (!timer.IsTimeOut(nTimeOut*1000));
		if(SP_OK != rlt)
		{
		NOTIFY("Measure Timeout", LEVEL_ITEM|LEVEL_FT,1, 0, 1);
		return rlt;
		}
		*/

		LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[pMeasParam->stTesterParamGroupSub.nCBWType];
		LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[pMeasParam->stTesterParamGroupSub.nSBWType];
		LPCSTR lpRate = CwcnUtility::WlanGetRateString( pMeasParam->stTesterParamGroupSub.eRate);
		if (IS_BIT_SET(nMask, WIFI_TXP))
		{
			NOTIFY("Transmit Power"
				, LEVEL_ITEM|LEVEL_FT
				, m_vectWlanSpec[pMeasParam->eMode].dTxp.low
				, stRlt.txp.dAvgValue
				, m_vectWlanSpec[pMeasParam->eMode].dTxp.upp
				, CwcnUtility::WLAN_BAND_NAME[pMeasParam->eMode]
			, pMeasParam->stChan.nCenChan
				, "dBm"
				, "%s;%s;CBW:%s;SBW:%s"
				, CwcnUtility::WLAN_ANT_NAME[pMeasParam->stTesterParamGroupSub.eAnt]
			, lpRate
				, strCBW
				, strSBW);
			if(!IN_RANGE(m_vectWlanSpec[pMeasParam->eMode].dTxp.low, stRlt.txp.dAvgValue, m_vectWlanSpec[pMeasParam->eMode].dTxp.upp))
			{
				rlt = SP_E_WCN_WLAN_TXP_FAIL;
			}
		}
		if(m_dwTime + WLAN_TX_TIME + WLAN_RX_TIME/3 > GetTickCount())
		{
			Sleep(m_dwTime + WLAN_TX_TIME + WLAN_RX_TIME/3 - GetTickCount());
		}
		if (IS_BIT_SET(pMeasParam->stTesterParamGroupSub.dwMask, WIFI_PER))
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(WIFI_PER, &stTester));
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
		}
		m_dwTime += WLAN_TX_TIME+WLAN_RX_TIME;

	}
	m_dwTime += m_nWlanLeaveModeTime;
	return rlt;
}

SPRESULT CWcnTestSysWear::ConfigWlanParam()
{
	char szTmp[1024] = {0};
	m_strScripBuf.clear();
	sprintf_s(szTmp, "insmod /vendor/lib/modules/sprdwl_ng.ko\n");
	m_strScripBuf += szTmp;
	sprintf_s(szTmp, "iwnpi wlan0 start\n");
	m_strScripBuf += szTmp;
	for(int i=0; i< (int)m_vectWlanMeasParam.size(); i++)
	{
		sprintf_s(szTmp, "iwnpi wlan0 set_channel %d\n", m_vectWlanMeasParam[i].stChan.nCenChan);
		m_strScripBuf += szTmp;
		sprintf_s(szTmp, "iwnpi wlan0 set_rate %d\n", WLAN_RATE_TABLE[m_vectWlanMeasParam[i].stTesterParamGroupSub.eRate]);
		m_strScripBuf += szTmp;
		sprintf_s(szTmp, "iwnpi wlan0 set_tx_count 0\n");
		m_strScripBuf += szTmp;
		sprintf_s(szTmp, "iwnpi wlan0 tx_start\n");
		m_strScripBuf += szTmp;
		sprintf_s(szTmp, "sleep %d\n", WLAN_TX_TIME/1000);
		m_strScripBuf += szTmp;
		sprintf_s(szTmp, "iwnpi wlan0 tx_stop\n");
		m_strScripBuf += szTmp;
		if (IS_BIT_SET(m_vectWlanMeasParam[i].stTesterParamGroupSub.dwMask, WIFI_PER))
		{
			sprintf_s(szTmp, "iwnpi wlan0 rx_start\n");
			m_strScripBuf += szTmp;
			sprintf_s(szTmp, "sleep %d\n", WLAN_RX_TIME/1000);
			m_strScripBuf += szTmp;
			sprintf_s(szTmp, "iwnpi wlan0 get_rx_ok > %s/wlan_test_log_ch%d.log\n", m_strDeviceFilePath.c_str(), m_vectWlanMeasParam[i].stChan.nCenChan);
			m_strScripBuf += szTmp;
			sprintf_s(szTmp, "iwnpi wlan0 rx_stop\n");
			m_strScripBuf += szTmp;
		}
		sprintf_s(szTmp, "iwnpi wlan0 stop\n");
		m_strScripBuf += szTmp;
		sprintf_s(szTmp, "rmmod sprdwl_ng\n");
		m_strScripBuf += szTmp;
	}
	if(!m_bBt)
	{
		return SendScrip();
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWear::SendScrip()
{
	return SP_OK;

// 	CHAR szApFile[MAX_PATH] = {0};
// 	sprintf_s(szApFile, "%s/wcn_test.sh", m_strDeviceFilePath.c_str());
// 	CHKRESULT_WITH_NOTIFY_FUNNAME(SP_apSaveFile(m_hDUT, szApFile, (LPVOID)m_strScripBuf.c_str(), m_strScripBuf.length()));
// 	return SP_OK;
}

SPRESULT CWcnTestSysWear::GetWlanRxResult()
{
	return SP_OK;

/*	SPRESULT rlt = SP_OK;
	CExtraLog RxRlt;
	char szTmp[1024] = {0};

	CHAR szApFile[MAX_PATH] = {0};
	for(int i=0; i< (int)m_vectWlanMeasParam.size(); i++)
	{
		if(!IS_BIT_SET(m_vectWlanMeasParam[i].stTesterParamGroupSub.dwMask, WIFI_PER))
		{
			continue;
		}
		uint32 nRadLen = 0;
		sprintf_s(szApFile, "%s/wlan_test_log_ch%d.log", m_strDeviceFilePath.c_str(), m_vectWlanMeasParam[i].stChan.nCenChan);
		int nCount = 3;
		SPRESULT res = SP_OK;
		do
		{
			res = SP_apLoadFile(m_hDUT, szApFile, szTmp, sizeof(szTmp), &nRadLen, 3000);
		} while ( nCount-- > 0 && SP_OK != res);
		if(SP_OK != res)
		{
			NOTIFY("SP_apLoadFile", LEVEL_ITEM, 1, 0, 1 );
			return res;
		}
		char * ptr = strstr(szTmp, "rx_end_count=");
		if(NULL == ptr)
		{
			LogFmtStrA(SPLOGLV_ERROR, "not found rx_end_count");
			return SP_E_INVALID_PARAMETER;
		}
		int nGood = atoi(ptr +strlen("rx_end_count="));
		double dPER = 0;
		if (nGood <  m_vectWlanMeasParam[i].stTesterParamGroupSub.nTotalPackets)
		{
			dPER = ((double)(m_vectWlanMeasParam[i].stTesterParamGroupSub.nTotalPackets - nGood))/(m_vectWlanMeasParam[i].stTesterParamGroupSub.nTotalPackets) * 100.0;
		}
		LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[m_vectWlanMeasParam[i].stTesterParamGroupSub.nCBWType];
		LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[m_vectWlanMeasParam[i].stTesterParamGroupSub.nSBWType];
		LPCSTR lpRate = CwcnUtility::WlanGetRateString( m_vectWlanMeasParam[i].stTesterParamGroupSub.eRate);
		NOTIFY("SensitivityTest"
			, LEVEL_ITEM|LEVEL_FT
			, 0.0
			, dPER
			, m_vectWlanSpec[m_vectWlanMeasParam[i].eMode].dPer
			, CwcnUtility::WLAN_BAND_NAME[m_vectWlanMeasParam[i].eMode]
			, m_vectWlanMeasParam[i].stChan.nCenChan
			, "-"
			, "%s;%s;CBW:%s;SBW:%s"
			, CwcnUtility::WLAN_ANT_NAME[m_vectWlanMeasParam[i].stTesterParamGroupSub.eAnt]
		, lpRate
			, strCBW
			, strSBW);
		if(!IN_RANGE(0.0, dPER, m_vectWlanSpec[m_vectWlanMeasParam[i].eMode].dPer))
		{
			if(rlt == SP_OK)
			{
				rlt = SP_E_WCN_WLAN_PER_FAIL;
			}
		}
	}
	return rlt;*/
}

unsigned _stdcall CWcnTestSysWear::RunListThread( LPVOID pParam )
{
	if(NULL == pParam)
	{
		return 0;
	}
	CWcnTestSysWear *pThis = (CWcnTestSysWear *)pParam;
	pThis->RunList();
	return 0;
}

SPRESULT CWcnTestSysWear::RunList()
{
	return SP_OK;

/*	CHAR szApFile[MAX_PATH] = {0};
	sprintf_s(szApFile, "%s/wcn_test.sh", m_strDeviceFilePath.c_str());
	m_rltRunScrip = SP_OK;
	SP_GetProperty(m_hDUT, SP_ATTR_CHANNEL_ATTR, 0, (LPVOID)&m_ca);
	Sleep(2000);
	m_rltRunScrip = SP_apRunFile(m_hDUT, szApFile);
	SP_EndPhoneTest(m_hDUT);
	return m_rltRunScrip;*/
}

SPRESULT CWcnTestSysWear::DoList()
{
	return SP_OK;

/*	CHAR szApFile[MAX_PATH] = {0};
	sprintf_s(szApFile, "%s/wcn_test.sh", m_strDeviceFilePath.c_str());
	m_rltRunScrip = SP_OK;
	SP_GetProperty(m_hDUT, SP_ATTR_CHANNEL_ATTR, 0, (LPVOID)&m_ca);
	m_rltRunScrip = SP_apRunFile(m_hDUT, szApFile);
	SP_EndPhoneTest(m_hDUT);
	return m_rltRunScrip;*/
	/*if(NULL != m_hThread)
	{
		CloseHandle(m_hThread);
	}
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &RunListThread, this, 0, NULL);
	if(NULL == m_hThread)
	{
		LogFmtStrA(SPLOGLV_ERROR, "_beginthreadex failed ");
		return SP_E_CREATE_OBJECT;
	}
	return SP_OK;*/
}

SPRESULT CWcnTestSysWear::WaitForReconnect( int nTimeout )
{
	CSPTimer timer;
	SPRESULT res = SP_E_SPAT_TIMEOUT;
	struct in_addr addr;
	DWORD nAddr = htonl(m_ca.Socket.dwIP);
	memcpy(&addr, &nAddr, sizeof(nAddr));
	do
	{
		if(_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		char szIpError[128] = {0};
		if(!CNetUtility::ICMPPing(inet_ntoa(addr), szIpError, sizeof(szIpError)))
		{
			LogFmtStrA(SPLOGLV_ERROR, "Ping Fail");
			continue;
		}
		res = SP_BeginPhoneTest(m_hDUT, (CHANNEL_ATTRIBUTE* )&m_ca);
		if(SP_OK == res)
		{
			CHAR strBuff[10] = {0};
			uint32 nReadLen = 0;
			res = SP_SendATCommand(m_hDUT, "AT", TRUE, strBuff, sizeof(strBuff), &nReadLen, 1000);
			if(SP_OK == res)
			{
				break;
			}
		}
		SP_EndPhoneTest(m_hDUT);
	} while (!timer.IsTimeOut(nTimeout*1000));
	NOTIFY("ReConnect", LEVEL_ITEM, 1, (SP_OK==res)?1:0, 1);
	return res;
}

BOOL CWcnTestSysWear::BTLoadXML()
{
	m_VecBTParamBandImp.clear();
	BOOL bEnable = FALSE;
	int nCount = 0;
	double* parrDouble = NULL;
	int* parrInt = NULL;

	wstring strMode[MAX_BT_TYPE] = {L"BDR",L"EDR", L"BLE", L"BLE5.0"};

	int nAvgCount = GetConfigValue(L"Param:Common:AvgCount", 1);
	for(int nMode=BDR; nMode<MAX_BT_TYPE; nMode++)
	{
		BTMeasParamBand stBTParamBand;
		stBTParamBand.eMode = (BT_TYPE)nMode;
		bEnable = (BOOL)GetConfigValue((L"Option:Band:"+strMode[nMode]).c_str(), FALSE);
		if(!bEnable)
		{
			continue;
		}
		int nGroupCount = GetConfigValue((L"Param:"+ strMode[nMode] + L":GroupCount").c_str(), FALSE);
		for(int nGroupIndex=0; nGroupIndex<nGroupCount; nGroupIndex++)
		{
			vector<int> ChGroup;
			ChGroup.clear();
			wchar_t    szGroup[20] = {0};
			swprintf_s(szGroup, L":Group%d", nGroupIndex+1);
			wstring strGroup = L"Param:"+ strMode[nMode] + szGroup;
			wstring strVal = GetConfigValue((strGroup + L":TCH").c_str(), L"39");
			parrInt = GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			for (int nChTmp=0;nChTmp<nCount;nChTmp++)
			{
				if(nChTmp <0 || nChTmp > 78)
				{
					return FALSE;
				}
				ChGroup.push_back(parrInt[nChTmp]);
			}
			LPCSTR lpPacket = _W2CA(GetConfigValue((strGroup + L":PacketType").c_str(), L"DH5"));
			BT_PACKET ePacket = CwcnUtility::BTGetPacket(stBTParamBand.eMode, lpPacket);
			if(INVALID_PACKET == ePacket)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid packet type!");
				return FALSE;
			}
			double dVsgLvl = GetConfigValue((strGroup + L":RXLVL").c_str(), -65.0);
			double dRefLvl = GetConfigValue((strGroup + L":RefLVL").c_str(), 10);
			int nPacketCount = GetConfigValue((strGroup + L":RxCount").c_str(), 10000);
			wstring strMask = strGroup + L":TestItem:";
			DWORD dwMask = 0;

			bEnable = (BOOL)GetConfigValue((strMask + L"TXP").c_str(), FALSE);
			if(bEnable)
			{
				dwMask |= (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_POWER:BDR_POWER;
			}
			bEnable = (BOOL)GetConfigValue((strMask + L"BER").c_str(), FALSE);
			if(bEnable)
			{
				dwMask |= (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_PER:BDR_BER;
			}
			for (int nChTmp=0;nChTmp<(int)ChGroup.size();nChTmp++)
			{
				BTMeasParamChan stChanParam;
				stChanParam.dRefLvl = dRefLvl;
				stChanParam.dVsgLvl = dVsgLvl;
				stChanParam.dwMask = dwMask;
				stChanParam.ePacketType = ePacket;
				stChanParam.ePath = ANT_SHARED;
				stChanParam.ePattern = BLE_TX_PBRS9;
				stChanParam.nAvgCount = nAvgCount;
				stChanParam.nCh = ChGroup[nChTmp];
				stChanParam.nTotalPackets = nPacketCount;
				stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[stBTParamBand.eMode][stChanParam.ePacketType];
				stBTParamBand.vecBTFileParamChan.push_back(stChanParam);
			}
		}
		wstring strSpec = L"Param:" + strMode[nMode] + L":Specification";
		if(nMode == BDR)
		{
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"8,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stBTParamBand.stSpec.stBdrSpec.dTxp.low = parrDouble[0];
			stBTParamBand.stSpec.stBdrSpec.dTxp.upp = parrDouble[1];

			stBTParamBand.stSpec.stBdrSpec.dTxpShared.low = parrDouble[0];
			stBTParamBand.stSpec.stBdrSpec.dTxpShared.upp = parrDouble[1];
			//BER spec
			stBTParamBand.stSpec.stBdrSpec.dBer = GetConfigValue((strSpec + L":BER").c_str(), 0.1);
			m_VecBTParamBandImp.push_back(stBTParamBand);
		}
		else if(nMode == EDR)
		{
			//TXP spec
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"4,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stBTParamBand.stSpec.stEdrSpec.dTxp.low = parrDouble[0];
			stBTParamBand.stSpec.stEdrSpec.dTxp.upp = parrDouble[1];

			stBTParamBand.stSpec.stEdrSpec.dTxpShared.low = parrDouble[0];
			stBTParamBand.stSpec.stEdrSpec.dTxpShared.upp = parrDouble[1];
			//BER spec
			stBTParamBand.stSpec.stEdrSpec.dBer = GetConfigValue((strSpec + L":BER").c_str(), 0.1);
			m_VecBTParamBandImp.push_back(stBTParamBand);
		}
		else
		{
			//TXP AVG spec
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"-20,10");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stBTParamBand.stSpec.stBleSpec.dTxpAvg.low = parrDouble[0];
			stBTParamBand.stSpec.stBleSpec.dTxpAvg.upp = parrDouble[1];

			stBTParamBand.stSpec.stBleSpec.dTxpAvgShared.low = parrDouble[0];
			stBTParamBand.stSpec.stBleSpec.dTxpAvgShared.upp = parrDouble[1];
			//PER spec
			stBTParamBand.stSpec.stBleSpec.dBer = GetConfigValue((strSpec + L":BER").c_str(), 30.8);
			m_VecBTParamBandImp.push_back(stBTParamBand);
		}
	}
	if(m_VecBTParamBandImp.size() > 0)
	{
		m_bBt = TRUE;
	}
	else
	{
		m_bBt = FALSE;
	}
	return TRUE;
}

SPRESULT CWcnTestSysWear::DoBtTest()
{
	SetRepairMode(RepairMode_Bluetooth);
	CSPTimer timer;
	SPWI_BT_PARAM_TESTER stTester;
	SPBT_RESULT_T stRlt;
	SPRESULT res = SP_OK;
	SPRESULT rlt = SP_OK;
	BOOL bFirst = TRUE;
	m_dwTime += m_nBtEnterModeTime;
	for(int i=0; i<(int)m_VecBTParamBandImp.size(); i++)
	{
		int nTimeOut = m_nTestTimeOut;
		BT_TYPE eType = m_VecBTParamBandImp[i].eMode;
		BTMeasBandSpec* pstSpec = &m_VecBTParamBandImp[i].stSpec;
		for(int j=0; j< (int)m_VecBTParamBandImp[i].vecBTFileParamChan.size(); j++)
		{
			if(bFirst)
			{
				nTimeOut = m_nBeginTimeOut;
				bFirst = FALSE;
			}
			BTMeasParamChan*pMeasParam = &m_VecBTParamBandImp[i].vecBTFileParamChan[j];
			DWORD nMask = pMeasParam->dwMask;
			stTester.eProto = eType;
			stTester.dRefLvl = pMeasParam->dRefLvl;
			stTester.dVsgLvl =  pMeasParam->dVsgLvl;
			stTester.eRfPort = CwcnUtility::BTGetAnt(pMeasParam->ePath);
			stTester.ePattern = pMeasParam->ePattern;
			stTester.nAvgCount = pMeasParam->nAvgCount;
			stTester.nCh = pMeasParam->nCh;
			stTester.ePacketType = pMeasParam->ePacketType;
			stTester.nPacketLen = pMeasParam->nPacketLen;
			stTester.nTotalPackets = pMeasParam->nTotalPackets;


			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_BT));
			BOOL bAutoLevel = FALSE;
			m_pRFTester->SetProperty(DP_BT_AUTO_LEVEL, 0, &bAutoLevel);
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));

			do
			{
				rlt = SP_E_SPAT_TIMEOUT;
				if(GetTickCount() > BT_TX_TIME/2 + m_dwTime)
				{
					res = m_pRFTester->InitTest(nMask, &stTester);
					if(SP_OK != res)
					{
						rlt = res;
						continue;
					}
					stRlt.Init();
					res = m_pRFTester->FetchResult(nMask, &stRlt);
					if(SP_OK != res)
					{
						rlt = res;
						continue;
					}
					else if(stRlt.Power.dAvgValue < stTester.dRefLvl-10 ||  stRlt.Power.dAvgValue > 99.0)
					{
						rlt = SP_E_RF_INVALID_RESULT;
						continue;
					}
					else
					{
						rlt = SP_OK;
						break;
					}
				}
			} while (GetTickCount() < BT_TX_TIME + m_dwTime);
			if(SP_OK != rlt)
			{
				NOTIFY("BT Measure Timeout", LEVEL_ITEM|LEVEL_FT,1, 0, 1);
				return rlt;
			}
			/*do
			{
			if(_IsUserStop())
			{
			return SP_E_USER_ABORT;
			}
			res = m_pRFTester->InitTest(nMask, &stTester);
			if(SP_OK != res)
			{
			rlt = res;
			continue;
			}
			stRlt.Init();
			res = m_pRFTester->FetchResult(nMask, &stRlt);
			if(SP_OK != m_rltRunScrip)
			{
			NOTIFY("RunScrip", LEVEL_ITEM, 1, 0, 1);
			LogFmtStrA(SPLOGLV_ERROR, "SP_apRunFile fail,err =%d", m_rltRunScrip);
			return m_rltRunScrip;
			}
			if(SP_OK != res)
			{
			rlt = res;
			continue;
			}
			else if(stRlt.Power.dAvgValue < stTester.dRefLvl-10 ||  stRlt.Power.dAvgValue > 99.0)
			{
			rlt = SP_E_RF_INVALID_RESULT;
			continue;
			}
			else
			{
			rlt = SP_OK;
			break;
			}
			} while (!timer.IsTimeOut(nTimeOut*1000));
			if(SP_OK != rlt)
			{
				NOTIFY("BT Measure Timeout", LEVEL_ITEM|LEVEL_FT,1, 0, 1);
				return rlt;
			}*/
			if (IS_BIT_SET(nMask, BLE_POWER) || IS_BIT_SET(nMask, BDR_POWER) )
			{
				double dLowLimit = pstSpec->stBleSpec.dTxpAvg.low;
				double dUppLimit = pstSpec->stBleSpec.dTxpAvg.upp;
				if(BDR == eType)
				{
					dLowLimit = pstSpec->stBdrSpec.dTxp.low;
					dUppLimit = pstSpec->stBdrSpec.dTxp.upp;
				}
				else if(EDR == eType)
				{
					dLowLimit = pstSpec->stEdrSpec.dTxp.low;
					dUppLimit = pstSpec->stEdrSpec.dTxp.upp;
				}
				NOTIFY("Output Power-Pavg"
					, LEVEL_ITEM|LEVEL_FT
					, dLowLimit
					, stRlt.Power.dAvgValue
					, dUppLimit
					, CwcnUtility::BT_BAND_NAME[eType]
				, pMeasParam->nCh
					, "dBm"
					, "%s"
					, CwcnUtility::BT_ANT_NAME[pMeasParam->ePath]
				);
				if(!IN_RANGE(dLowLimit, stRlt.Power.dAvgValue, dUppLimit))
				{
					rlt = SP_E_WCN_BT_TXP_FAIL;
				}
			}
			if(m_dwTime + BT_TX_TIME + BT_RX_TIME/3 > GetTickCount())
			{
				Sleep(m_dwTime + BT_TX_TIME + BT_RX_TIME/3 - GetTickCount());
				LogFmtStrA(SPLOGLV_INFO, "Before InitTest True %d", m_dwTime + BT_TX_TIME + BT_RX_TIME / 3 - GetTickCount());
			}
			if (IS_BIT_SET(pMeasParam->dwMask, BLE_PER))
			{
				CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(BLE_PER, &stTester));
				Sleep(BT_RX_TIME / 2);///???ó
				LogFmtStrA(SPLOGLV_INFO, "Before SetGen True %d", BT_RX_TIME / 2);
				CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));
				CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
			}
			else if(IS_BIT_SET(pMeasParam->dwMask, BDR_BER))
			{
				CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(BDR_BER, &stTester));
				CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));
				Sleep(BT_RX_TIME/2);
				LogFmtStrA(SPLOGLV_INFO, "BDR_BER SetGen True %d", BT_RX_TIME / 2);
				CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
			}
			m_dwTime += BT_TX_TIME+BT_RX_TIME;
		}
	}
	return rlt;
}

SPRESULT CWcnTestSysWear::ConfigBtParam()
{
	if(!m_bWlan)
	{
		m_strScripBuf.clear();
	}
	char szTmp[1024] = {0};
	int nBle = 0;
	for(int i=0; i<(int)m_VecBTParamBandImp.size(); i++)
	{
		nBle = m_VecBTParamBandImp[i].eMode == BLE ? 1:0;
		for(int j=0; j< (int)m_VecBTParamBandImp[i].vecBTFileParamChan.size(); j++)
		{
			sprintf_s(szTmp, "bdt_unisoc --case set_nonsig_tx_testmode --enable 1 --ble %d --pattern %d --channel 0x%x --pactype 0x%x --paclen 0x%x --powertype 1 --powervalue 20 --paccnt 0 --second %d\n",
				nBle,
				m_VecBTParamBandImp[i].vecBTFileParamChan[j].ePattern,
				m_VecBTParamBandImp[i].vecBTFileParamChan[j].nCh,
				GetPacketType(m_VecBTParamBandImp[i].eMode , m_VecBTParamBandImp[i].vecBTFileParamChan[j].ePacketType),
				m_VecBTParamBandImp[i].vecBTFileParamChan[j].nPacketLen,
				BT_TX_TIME/1000);
			m_strScripBuf += szTmp;
		}
	}
	return SendScrip();
}

SPRESULT CWcnTestSysWear::GetBtRxResult()
{
	return SP_OK;
}

int CWcnTestSysWear::GetPacketType(BT_TYPE eMode, BT_PACKET ePacket)
{
	int nPacket = -1;
	if (BDR == eMode)
	{
		switch(ePacket)
		{
		case BDR_DH1:
			nPacket = 4;
			break;
		case BDR_DH3:
			nPacket = 11;
			break;
		case BDR_DH5:
			nPacket = 15;
			break;
		default:
			nPacket = 4;
			break;
		}
	}
	else if (EDR == eMode)
	{
		switch(ePacket)
		{
		case EDR_2DH1:
			nPacket = 20;
			break;
		case EDR_2DH3:
			nPacket = 26;
			break;
		case EDR_2DH5:
			nPacket = 30;
			break;
		case EDR_3DH1:
			nPacket = 24;
			break;
		case EDR_3DH3:
			nPacket = 27;
			break;
		case EDR_3DH5:
			nPacket = 31;
			break;
		default:
			nPacket = 20;
			break;
		}
	}
	else if (BLE_EX == eMode)
	{
		switch(ePacket)
		{
		case RF_PHY_1M:
			nPacket = 1;
			break;
		case RF_PHY_2M:
			nPacket = 2;
			break;
		case RF_PHY_S2:
			nPacket = 4;
			break;
		case RF_PHY_S8:
			nPacket = 3;
			break;
		default:
			nPacket = 1;
			break;
		}
	}
	else
	{
		nPacket = 1;
	}
	return nPacket;
}




