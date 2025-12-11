#include "StdAfx.h"
#include "WcnTestSysWearCW.h"
#include "wcnUtility.h"
#include "SimpleAop.h"
#include "Winsock2.h"
#include "NetUtility.h"
#pragma comment(lib, "ws2_32.lib")

IMPLEMENT_RUNTIME_CLASS(CWcnTestSysWearCW)

CWcnTestSysWearCW::CWcnTestSysWearCW(void)
{
	ZeroMemory(&m_ca, sizeof(m_ca));
	m_bWlan = FALSE;
	m_bBt = FALSE;

	m_nWlanTx_Delay = 0;
	m_nWlanEnd_Delay = 0;

	m_nBTTx_Delay = 0;
	m_nBTEnd_Delay = 0;
	m_nTriggerTimeOut = 3000;
	m_nConnectTimeOut = 30000;
	m_dwTime = 0;
}


CWcnTestSysWearCW::~CWcnTestSysWearCW(void)
{
}


SPRESULT CWcnTestSysWearCW::__InitAction(void)
{
	return SP_OK;
}

BOOL CWcnTestSysWearCW::LoadXMLConfig(void)
{
	m_nWlanTx_Delay = GetConfigValue(L"Option:Wlan:Tx_Delay", 100);
	m_nWlanEnd_Delay = GetConfigValue(L"Option:Wlan:End_Delay", 200);

	m_nBTTx_Delay = GetConfigValue(L"Option:BT:Tx_Delay", 100);
	m_nBTEnd_Delay = GetConfigValue(L"Option:BT:End_Delay", 200);
	m_nTriggerTimeOut = GetConfigValue(L"Option:TxTriggerDelay", 3000);
	m_nConnectTimeOut = GetConfigValue(L"Option:WaitForConnect", 30000);

	if (SP_OK != WlanLoadXmlFile(m_stWlanParamImp))
	{
		return FALSE;
	}

	if (SP_OK != BTLoadXmlFile(m_stBTMeasParam.stBTParamBand))
	{
		return FALSE;
	}

	if (!m_bBt && !m_bWlan)
	{
		return FALSE;
	}

	return TRUE;
}

SPRESULT CWcnTestSysWearCW::__PollAction(void)
{
	SetRepairMode(RepairMode_Wlan);
	CInstrumentLock rfLock(m_pRFTester);
	if (m_bWlan)
	{
		CHKRESULT(ConfigWlanParam());
	}
	if (m_bBt)
	{
		CHKRESULT(ConfigBtParam());
	}
	CHKRESULT(DoList());
	if (m_bWlan)
	{
		CHKRESULT(DoWlanTest());
	}
	if (m_bBt)
	{
		CHKRESULT(DoBtTest());
	}

	CHKRESULT(WaitForReconnect(m_nConnectTimeOut));

	return SP_OK;
}


void CWcnTestSysWearCW::__LeaveAction(void)
{
}

SPRESULT CWcnTestSysWearCW::__FinalAction(void)
{
	return SP_OK;
}

SPRESULT CWcnTestSysWearCW::WlanLoadXmlFile(WLAN_PARAM_CONF& stWlanParamImp)
{
	if (!stWlanParamImp.VecWlanParamBandImp.empty())
	{
		stWlanParamImp.VecWlanParamBandImp.clear();
	}
	wstring strBand[WLAN_BAND_MAX] = { L"Band_24G", L"Band_50G" };
	wstring strAnt[ANT_MAX] = { L"",L"Ant1st", L"Ant2nd", L"MIMO" };
	std::wstring strTxCommon = L"Param:Common";

	int nAvgCount = GetConfigValue((strTxCommon + L":AvgCount").c_str(), 3);

	for (int nBandIndex = WLAN_BAND_24G; nBandIndex < WLAN_BAND_MAX; nBandIndex++)
	{
		for (int nAnt = ANT_PRIMARY; nAnt < ANT_MAX; nAnt++)
		{
			wstring strChannel = L"Option:Wlan:" + strBand[nBandIndex] + L":" + strAnt[nAnt];
			BOOL bEnable = GetConfigValue(strChannel.c_str(), FALSE);
			if (!bEnable)
			{
				continue;
			}
			wstring strParamChan = L"Param:Wlan:" + strBand[nBandIndex];
			WlanMeasParamBand vecWlanParamBand;
			vecWlanParamBand.vecConfParamGroup.clear();
			vecWlanParamBand.stSpec.Int();
			vecWlanParamBand.eMode = WIFI_CW_SPECTRUM;
			vecWlanParamBand.eBand = (WLAN_BAND_ENUM)nBandIndex;

			SPWI_WLAN_PARAM_GROUP vecGroupParam;
			vecGroupParam.Init();
			vecGroupParam.stAlgoParamGroupSub.eAnt = (ANTENNA_ENUM)nAnt;


			vecGroupParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
			wstring strVal = GetConfigValue((strParamChan + L":CenChan").c_str(), L"1,7,13");
			int nCount = 0;
			INT* parrInt = GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			vecGroupParam.vectChan.resize(nCount);
			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				vecGroupParam.vectChan[nChTmp].nCenChan = parrInt[nChTmp];
				vecGroupParam.vectChan[nChTmp].nPriChan = parrInt[nChTmp];
			}
			//Get ref level
			vecGroupParam.stAlgoParamGroupSub.dRefLvl = GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
			vecGroupParam.stAlgoParamGroupSub.dwMask = WIFI_CW;
			vecWlanParamBand.vecConfParamGroup.push_back(vecGroupParam);

			//TXP spec
			strVal = GetConfigValue((strParamChan + L":Specification:TXP").c_str(), L"8,20");
			double* parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			vecWlanParamBand.stSpec.dTxp.low = parrDouble[0];
			vecWlanParamBand.stSpec.dTxp.upp = parrDouble[1];

			//TXPant2nd spec
			nCount = 0;
			strVal = GetConfigValue((strParamChan + L":Specification:TXPant2nd").c_str(), L"8,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXPant2nd limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			vecWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
			vecWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];
			stWlanParamImp.VecWlanParamBandImp.push_back(vecWlanParamBand);
		}
	}

	m_bWlan = stWlanParamImp.VecWlanParamBandImp.size() == 0 ? FALSE : TRUE;
	return SP_OK;
}


SPRESULT CWcnTestSysWearCW::BTLoadXmlFile(vector<BTMeasParamBand>& VecBTParamBandImp)
{
	if (!VecBTParamBandImp.empty())
	{
		VecBTParamBandImp.clear();
	}

	BOOL bEnable = FALSE;
	int nCount = 0;
	double* parrDouble = NULL;
	int* parrInt = NULL;
	SPRESULT eBtConState = SP_OK;

	wstring strMode[MAX_BT_TYPE] = { L"BDR",L"EDR", L"BLE", L"BLE5.0" };
	wstring strAnt[ANT_BT_MAX] = { L"INVALID", L"StandAlone", L"Shared" };

	int nAvgCount = GetConfigValue(L"Param:Common:AvgCount", 3);

	for (int nAnt = ANT_SINGLE; nAnt < ANT_BT_MAX; nAnt++)
	{
		bEnable = (BOOL)GetConfigValue((L"Option:BT:Ant:" + strAnt[nAnt]).c_str(), FALSE);
		if (!bEnable)
		{
			continue;
		}

		for (int nMode = BDR; nMode < EDR; nMode++)
		{
			BTMeasParamBand stBTParamBand;
			stBTParamBand.eMode = (BT_TYPE)nMode;
			bEnable = (BOOL)GetConfigValue(L"Option:BT:Mode", FALSE);
			if (!bEnable)
			{
				continue;
			}

			vector<int> ChGroup;
			wstring strGroup = L"Param:BT:";// +strMode[nMode];
			wstring strVal = GetConfigValue((strGroup + L":TCH").c_str(), L"1,39,78");
			parrInt = GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
				CHKRESULT(eBtConState);
				ChGroup.push_back(parrInt[nChTmp]);
			}

			double dVsgLvl = GetConfigValue((strGroup + L":RXLVL").c_str(), -65.0);
			double dRefLvl = GetConfigValue((strGroup + L":RefLVL").c_str(), 15);

			for (int nChTmp = 0; nChTmp < (int)ChGroup.size(); nChTmp++)
			{
				BTMeasParamChan stChanParam;
				stChanParam.dRefLvl = dRefLvl;
				stChanParam.dVsgLvl = dVsgLvl;
				stChanParam.dwMask = BT_CW;
				stChanParam.ePacketType = BDR_DH1;
				stChanParam.ePath = BT_RFPATH_ENUM(nAnt);
				stChanParam.nAvgCount = nAvgCount;
				stChanParam.nCh = ChGroup[nChTmp];
				stChanParam.nTotalPackets = 1000;
				 stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[stBTParamBand.eMode][stChanParam.ePacketType];
				stBTParamBand.vecBTFileParamChan.push_back(stChanParam);
			}


			wstring strSpec = L"Param:BT:Specification" /*+ strMode[nMode] + L":Specification"*/;
			if (nMode == BDR)
			{
				strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"2,12");
				parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				if (nCount < 2)
				{
					LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
					return SP_E_WCN_INVALID_XML_CONFIG;
				}
				stBTParamBand.stSpec.stBdrSpec.dTxp.low = parrDouble[0];
				stBTParamBand.stSpec.stBdrSpec.dTxp.upp = parrDouble[1];

				VecBTParamBandImp.push_back(stBTParamBand);
			}
			else if (nMode == EDR)
			{
				//TXP spec
				strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"2,12");
				parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				if (nCount < 2)
				{
					LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
					return SP_E_WCN_INVALID_XML_CONFIG;
				}
				stBTParamBand.stSpec.stEdrSpec.dTxp.low = parrDouble[0];
				stBTParamBand.stSpec.stEdrSpec.dTxp.upp = parrDouble[1];

				VecBTParamBandImp.push_back(stBTParamBand);
			}
			else
			{
				//TXP AVG spec
				strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"-20,10");
				parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				if (nCount < 2)
				{
					LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit");
					return SP_E_WCN_INVALID_XML_CONFIG;
				}
				stBTParamBand.stSpec.stBleExSpec.dTxpAvg.low = parrDouble[0];
				stBTParamBand.stSpec.stBleExSpec.dTxpAvg.upp = parrDouble[1];

				VecBTParamBandImp.push_back(stBTParamBand);
			}
		}
	}

	m_bBt = VecBTParamBandImp.size() == 0 ? FALSE : TRUE;
	return SP_OK;
}


SPRESULT CWcnTestSysWearCW::ConfigWlanParam()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT_WITH_NOTIFY_FUNNAME(WifiClearCase());
	if (!m_bBt)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(BtClearCase());
	}

	CHKRESULT_WITH_NOTIFY_FUNNAME(WifiAddCase(m_stWlanParamImp, m_nWlanTx_Delay, m_nWlanEnd_Delay));

	return SP_OK;
}


SPRESULT CWcnTestSysWearCW::WifiClearCase()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTCLRWIFICASE", TRUE, szBuff, sizeof(szBuff), &nRet, 5000);

	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiClearCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if (NULL == strstr(szBuff, "+ZCTCLRWIFICASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiClearCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}

/*AT+WIFICWCASE= ANT, channel, tx_delay, end_delay ; ANT, channel, tx_delay, end_delay*/
SPRESULT CWcnTestSysWearCW::WifiAddCase(WLAN_PARAM_CONF& stWlanParamImp, int nTxDelay, int nEndDelay)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szCmd[256];
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	std::string strCmd = "AT+WIFICWCASE=";


	int nBandSize = stWlanParamImp.VecWlanParamBandImp.size();
	for (int nBand = 0; nBand < nBandSize; nBand++)
	{
		int nChanSize = stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].vectChan.size();
		for (int nChanIndex = 0; nChanIndex < nChanSize; nChanIndex++)
		{
			WLAN_BAND_ENUM eBand = stWlanParamImp.VecWlanParamBandImp[nBand].eBand;
			int eAnt = stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].stAlgoParamGroupSub.eAnt;
			int nChannel = stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].vectChan[nChanIndex].nCenChan;
			sprintf_s(szCmd, "%d,%d,%d,%d,%d;", eBand, eAnt, nChannel, nTxDelay, nEndDelay);
			strCmd += szCmd;
		}
	}

	strCmd.erase(strCmd.length() - 1);

	LogFmtStrA(SPLOGLV_INFO, "%s", strCmd.c_str());

	SPRESULT res = SP_SendATCommand(m_hDUT, strCmd.c_str(), TRUE, szBuff, sizeof(szBuff), &nRet, 5000);

	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "CW WifiAddCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if (NULL == strstr(szBuff, "+ZCTSETWIFICASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "CW WifiAddCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}

	return SP_OK;
}


SPRESULT CWcnTestSysWearCW::ConfigBtParam()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (!m_bWlan)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(WifiClearCase());
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME(BtClearCase());
	CHKRESULT_WITH_NOTIFY_FUNNAME(BtAddCase(m_stBTMeasParam.stBTParamBand, m_nBTTx_Delay, m_nBTEnd_Delay));

	return SP_OK;
}

SPRESULT CWcnTestSysWearCW::BtClearCase()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTCLRBTCASE", TRUE, szBuff, sizeof(szBuff), &nRet, 5000);

	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtClearCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if (NULL == strstr(szBuff, "+ZCTCLRBTCASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtClearCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}
/*AT+BTCWCASE= RFPATH, channel, tx_delay, end_delay£»RFPATH, channel, tx_delay, end_delay*/
SPRESULT CWcnTestSysWearCW::BtAddCase(vector<BTMeasParamBand>& VecBTParamBandImp, int nTxDelay, int nEndDelay)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szCmd[256];
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	std::string strCmd = "AT+BTCWCASE=";


	int nBandSize = VecBTParamBandImp.size();
	for (int nBand = 0; nBand < nBandSize; nBand++)
	{
		int nChanSize = VecBTParamBandImp[nBand].vecBTFileParamChan.size();
		for (int nChanIndex = 0; nChanIndex < nChanSize; nChanIndex++)
		{
			BT_TYPE eMode = VecBTParamBandImp[nBand].eMode;
			int ePath = VecBTParamBandImp[nBand].vecBTFileParamChan[nChanIndex].ePath;
			int nChannel = VecBTParamBandImp[nBand].vecBTFileParamChan[nChanIndex].nCh;
			sprintf_s(szCmd, "%d,%d,%d,%d,%d;", eMode, ePath, nChannel, nTxDelay, nEndDelay);
			strCmd += szCmd;
		}
	}

	strCmd.erase(strCmd.length() - 1);

	LogFmtStrA(SPLOGLV_INFO, "%s", strCmd.c_str());

	SPRESULT res = SP_SendATCommand(m_hDUT, strCmd.c_str(), TRUE, szBuff, sizeof(szBuff), &nRet, 5000);

	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "CW BT AddCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if (NULL == strstr(szBuff, "+ZCTSETBTCASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "CW BT AddCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}

	return SP_OK;
}


SPRESULT CWcnTestSysWearCW::WaitForReconnect(int nTimeout)
{
	CSPTimer timer;
	SPRESULT res = SP_E_SPAT_TIMEOUT;
	struct in_addr addr;
	DWORD nAddr = htonl(m_ca.Socket.dwIP);
	memcpy(&addr, &nAddr, sizeof(nAddr));
	int nIndex = 0;

	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}
		char szIpError[128] = { 0 };
		if (!CNetUtility::ICMPPing(inet_ntoa(addr), szIpError, sizeof(szIpError)))
		{

			LogFmtStrA(SPLOGLV_ERROR, "%d %d Ping Fail %s", ++nIndex, m_ca.Socket.dwIP, szIpError);
			continue;
		}
		else
		{
			LogFmtStrA(SPLOGLV_INFO, "%d %d Ping Success!", ++nIndex, m_ca.Socket.dwIP);
		}

		res = SP_BeginPhoneTest(m_hDUT, (CHANNEL_ATTRIBUTE*)& m_ca);
		if (SP_OK == res)
		{
			CHAR strBuff[128] = { 0 };
			uint32 nReadLen = 0;
			res = SP_SendATCommand(m_hDUT, "AT+GETTESTMODE?", TRUE, strBuff, sizeof(strBuff), &nReadLen, 1000); //at + gettestmode ?
			if (SP_OK == res && NULL != strstr(strBuff, "OK"))
			{
				break;
			}
		}
		SP_EndPhoneTest(m_hDUT);
	} while (!timer.IsTimeOut(nTimeout * 1000));
	NOTIFY("ReConnect", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1);
	return res;
}

SPRESULT CWcnTestSysWearCW::DoList()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SP_GetProperty(m_hDUT, SP_ATTR_CHANNEL_ATTR, 0, (LPVOID)& m_ca);
	SPRESULT sRet = WcnRunCase();
	SP_EndPhoneTest(m_hDUT);
	m_dwTime = GetTickCount();
	return sRet;
}

SPRESULT CWcnTestSysWearCW::WcnRunCase()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTTRIGER", FALSE, szBuff, sizeof(szBuff), &nRet, 5000);
	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WcnRunCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}

	Sleep(m_nTriggerTimeOut);
	LogFmtStrA(SPLOGLV_INFO, "Wait for After DUT ZCTTRIGER %d(ms)", m_nTriggerTimeOut);
	return SP_OK;
}

SPRESULT CWcnTestSysWearCW::DoWlanTest()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPWI_WLAN_PARAM_TESTER stTester;
	SPWI_RESULT_T stRlt;
	SPRESULT res = SP_OK;
	SPRESULT rlt = SP_OK;

	SetRepairMode(RepairMode_Wlan);
	SetRepairItem($REPAIR_ITEM_INSTRUMENT);


	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_WIFI));
	BOOL bAutoLevel = FALSE;
	m_pRFTester->SetProperty(DP_WLAN_AUTO_LEVEL, 0, &bAutoLevel);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));

	int nBandSize = m_stWlanParamImp.VecWlanParamBandImp.size();
	for (int nBand = 0; nBand < nBandSize; nBand++)
	{
		int nChanSize = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].vectChan.size();
		for (int nChanIndex = 0; nChanIndex < nChanSize; nChanIndex++)
		{
			if (_IsUserStop())
			{
				return SP_E_USER_ABORT;
			}

			DWORD nMask = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].stAlgoParamGroupSub.dwMask;
			nMask &= WIFI_CW;
			WIFI_PROTOCOL_ENUM eMode = m_stWlanParamImp.VecWlanParamBandImp[nBand].eMode;
			double dRefLvl = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].stAlgoParamGroupSub.dRefLvl;
			int nCenChan = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].vectChan[nChanIndex].nCenChan;
			int nPriChan = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].vectChan[nChanIndex].nPriChan;
			ANTENNA_ENUM eAnt = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].stAlgoParamGroupSub.eAnt;
			int nAvgCount = m_stWlanParamImp.VecWlanParamBandImp[nBand].vecConfParamGroup[0].stAlgoParamGroupSub.nAvgCount;

			stTester.eProto = eMode;
			stTester.dRefLvl = dRefLvl;
			stTester.dVsgLvl = -60.0;
			//CwcnUtility::WlanGetAnt(eAnt, &stTester.ePort[0]);
			//set band
			WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
			if (nCenChan <= 14)
			{
				eBand = WLAN_BAND_24G;
			}
			else
			{
				eBand = WLAN_BAND_50G;
			}
			CwcnUtility::WlanGetAntByBand(eAnt, &stTester.ePort[0], eBand);

			stTester.nAvgCount = nAvgCount;
			stTester.nCenChan = nCenChan;
			stTester.nPriChan = nPriChan;

			do
			{
				rlt = SP_E_SPAT_TIMEOUT;
				if (GetTickCount() > m_nWlanTx_Delay / 2 + m_dwTime)
				{
					res = m_pRFTester->InitTest(nMask, &stTester);
					if (SP_OK != res)
					{
						rlt = res;
						continue;
					}
					stRlt.Init();
					res = m_pRFTester->FetchResult(nMask, &stRlt);
					if (SP_OK != res)
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
			} while (GetTickCount() < m_nWlanTx_Delay + m_dwTime);

			if (SP_OK != rlt)
			{
				NOTIFY("Measure Timeout", LEVEL_ITEM | LEVEL_FT, 1, 0, 1);
				return rlt;
			}

			double low = m_stWlanParamImp.VecWlanParamBandImp[nBand].stSpec.dTxp.low;
			double upp = m_stWlanParamImp.VecWlanParamBandImp[nBand].stSpec.dTxp.upp;

			NOTIFY("Transmit Power"
				, LEVEL_ITEM | LEVEL_FT
				, low
				, stRlt.CWPwr.dAvgValue
				, upp
				, CwcnUtility::WLAN_BAND_NAME[eMode]
				, nCenChan
				, "dBm"
				, "%s"
				, CwcnUtility::WLAN_ANT_NAME[eAnt]
			);

			if (!IN_RANGE(low, stRlt.CWPwr.dAvgValue, upp))
			{
                return SP_E_WCN_WLAN_TXP_FAIL;
			}

			int nEnd = GetTickCount();
			if (m_nWlanTx_Delay > (nEnd - m_dwTime))
			{
				Sleep(m_nWlanTx_Delay - (nEnd - m_dwTime));
				LogFmtStrA(SPLOGLV_INFO, "Sleep() = %d m_dwTime = %d nEnd = %d m_nWlanTx_Delay = %d"
					, m_dwTime + m_nWlanTx_Delay - nEnd
					, m_dwTime
					, nEnd
					, m_nWlanTx_Delay);
			}
			else
			{
				LogFmtStrA(SPLOGLV_ERROR, "Pls Adj Tx_Delay %d > %d(%d - %d)"
					, m_nWlanTx_Delay
					, nEnd - m_dwTime
					, nEnd
					, m_dwTime);
			}

			Sleep(m_nWlanEnd_Delay);

			m_dwTime += (m_nWlanTx_Delay + m_nWlanEnd_Delay);
		}
	}
	return rlt;
}


SPRESULT CWcnTestSysWearCW::DoBtTest()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPWI_BT_PARAM_TESTER stTester;
	SPBT_RESULT_T stRlt;
	SPRESULT res = SP_OK;
	SPRESULT rlt = SP_OK;

	SetRepairMode(RepairMode_Bluetooth);
	SetRepairItem($REPAIR_ITEM_INSTRUMENT);


	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_BT));
	BOOL bAutoLevel = FALSE;
	m_pRFTester->SetProperty(DP_WLAN_AUTO_LEVEL, 0, &bAutoLevel);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_FT_NST, NULL));

	int nBandSize = m_stBTMeasParam.stBTParamBand.size();
	for (int nBand = 0; nBand < nBandSize; nBand++)
	{
		int nChanSize = m_stBTMeasParam.stBTParamBand[nBand].vecBTFileParamChan.size();
		for (int nChanIndex = 0; nChanIndex < nChanSize; nChanIndex++)
		{
			if (_IsUserStop())
			{
				return SP_E_USER_ABORT;
			}

			BT_RFPATH_ENUM ePath = m_stBTMeasParam.stBTParamBand[nBand].vecBTFileParamChan[nChanIndex].ePath;
			DWORD nMask = m_stBTMeasParam.stBTParamBand[nBand].vecBTFileParamChan[nChanIndex].dwMask;
			nMask &= BT_CW;
			BT_TYPE eMode = m_stBTMeasParam.stBTParamBand[nBand].eMode;
			double dRefLvl = m_stBTMeasParam.stBTParamBand[nBand].vecBTFileParamChan[nChanIndex].dRefLvl;
			int nCh = m_stBTMeasParam.stBTParamBand[nBand].vecBTFileParamChan[nChanIndex].nCh;
			int nAvgCount = m_stBTMeasParam.stBTParamBand[nBand].vecBTFileParamChan[nChanIndex].nAvgCount;

			stTester.eProto = eMode;
			stTester.dRefLvl = dRefLvl;
			stTester.dVsgLvl = -60.0;

			stTester.eRfPort = CwcnUtility::BTGetAnt(ePath);

			stTester.nAvgCount = nAvgCount;
			stTester.nCh = nCh;

			do
			{
				rlt = SP_E_SPAT_TIMEOUT;
				if (GetTickCount() > m_nBTTx_Delay / 2 + m_dwTime)
				{
					res = m_pRFTester->InitTest(nMask, &stTester);
					if (SP_OK != res)
					{
						rlt = res;
						continue;
					}
					stRlt.Init();
					res = m_pRFTester->FetchResult(nMask, &stRlt);
					if (SP_OK != res)
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
			} while (GetTickCount() < m_nBTTx_Delay + m_dwTime);

			if (SP_OK != rlt)
			{
				NOTIFY("Measure Timeout", LEVEL_ITEM | LEVEL_FT, 1, 0, 1);
				return rlt;
			}

			double low = 0.0;
			double upp = 0.0;

			switch (eMode)
			{
			case BDR:
				low = m_stBTMeasParam.stBTParamBand[nBand].stSpec.stBdrSpec.dTxp.low;
				upp = m_stBTMeasParam.stBTParamBand[nBand].stSpec.stBdrSpec.dTxp.upp;
				break;
			case EDR:
				low = m_stBTMeasParam.stBTParamBand[nBand].stSpec.stEdrSpec.dTxp.low;
				upp = m_stBTMeasParam.stBTParamBand[nBand].stSpec.stEdrSpec.dTxp.upp;
				break;
			case BLE_EX:
			case BLE:
			case BLE_53:
				low = m_stBTMeasParam.stBTParamBand[nBand].stSpec.stBleExSpec.dTxpAvg.low;
				upp = m_stBTMeasParam.stBTParamBand[nBand].stSpec.stBleExSpec.dTxpAvg.upp;
				break;
			default:
				break;
			}

			NOTIFY("Transmit Power"
				, LEVEL_ITEM | LEVEL_FT
				, low
				, stRlt.Power.dAvgValue
				, upp
				, "BT CW"/*CwcnUtility::BT_BAND_NAME[eMode]*/
				, nCh
				, "dBm"
				, "%s"
				, CwcnUtility::BT_ANT_NAME[ePath]
			);


			if (!IN_RANGE(low, stRlt.Power.dAvgValue, upp))
			{
                return SP_E_WCN_BT_TXP_FAIL;
			}

			int nEnd = GetTickCount();
			if (m_nBTTx_Delay > (nEnd - m_dwTime))
			{
				Sleep(m_nBTTx_Delay - (nEnd - m_dwTime));
				LogFmtStrA(SPLOGLV_INFO, "Sleep() = %d m_dwTime = %d nEnd = %d m_nBTTx_Delay = %d"
					, m_dwTime + m_nBTTx_Delay - nEnd
					, m_dwTime
					, nEnd
					, m_nBTTx_Delay);
			}
			else
			{
				LogFmtStrA(SPLOGLV_INFO, "Pls Adj Tx_Delay %d > %d(%d - %d)"
					, m_nBTTx_Delay
					, nEnd - m_dwTime
					, nEnd
					, m_dwTime);
			}

			Sleep(m_nBTEnd_Delay);

			m_dwTime += (m_nBTTx_Delay + m_nBTEnd_Delay);
		}
	}
	return rlt;
}
