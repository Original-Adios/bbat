#include "StdAfx.h"
#include "WcnTestSysWearATEx.h"
#include "wcnUtility.h"
// #include "ExtraLogFile.h"
// #include <process.h>
#include "SimpleAop.h"
#include "Winsock2.h"
#include "NetUtility.h"
#pragma comment(lib, "ws2_32.lib")



IMPLEMENT_RUNTIME_CLASS(CWcnTestSysWearATEx)


extern const int WLAN_TX_TIME;
extern const int WLAN_RX_TIME;
extern const int WLAN_END_TIME;
extern const int BT_TX_TIME;
extern const int BT_RX_TIME;
extern const int BT_END_TIME;
// Marlin3:
// {	0, "DSSS-1"}, /*1M_Long*/  {1, "DSSS-2"}, /*2M_Long*/ {2, "DSSS-2S"}, /*2M_Short*/ {3, "CCK-5.5"}, /*5.5M_Long*/ {4, "CCK-5.5S"}, /*5.5M_Short*/
//     {5, "CCK-11"}, /*11M_Long*/  {6, "CCK-11S"}, /*11M_Short*/  {7, "OFDM-6"}, /*6M*/ {8, "OFDM-9"}, /*9M*/ {9, "OFDM-12"}, /*12M*/
//     {10, "OFDM-18"}, /*18M*/ {11, "OFDM-24"}, /*24M*/ {12, "OFDM-36"}, /*36M*/ {13, "OFDM-48"}, /*48M*/ {14, "OFDM-54"}, /*54M*/
//     {15, "MCS-0"}, /*HT_MCS0*/ {16, "MCS-1"}, /*HT_MCS1*/ {17, "MCS-2"}, /*HT_MCS2*/ {18, "MCS-3"},  /*HT_MCS3*/ {19, "MCS-4"},  /*HT_MCS4*/
//     {20, "MCS-5"},  /*HT_MCS5*/ {21, "MCS-6"},  /*HT_MCS6*/ {22, "MCS-7"}, /*HT_MCS7*/ {23, "MCS-8"}, /*HT_MCS8*/{24, "MCS-9"}, /*HT_MCS9*/
//     {25, "MCS-10"}, /*HT_MCS10*/ {26, "MCS-11"}, /*HT_MCS11*/ {27, "MCS-12"}, /*HT_MCS12*/ {28, "MCS-13"}, /*HT_MCS13*/ {29, "MCS-14"}, /*HT_MCS14*/
//     {30, "MCS-15"}, /*HT_MCS15*/ {31, "VHT_MCS0_1SS"}, /*VHT_MCS0_1SS*/ {32, "VHT_MCS1_1SS"}, /*VHT_MCS1_1SS*/ {33, "VHT_MCS2_1SS"}, /*VHT_MCS2_1SS*/ {34, "VHT_MCS3_1SS"}, /*VHT_MCS3_1SS*/
//     {35, "VHT_MCS4_1SS"}, /*VHT_MCS4_1SS*/ {36, "VHT_MCS5_1SS"}, /*VHT_MCS5_1SS*/ {37, "VHT_MCS6_1SS"}, /*VHT_MCS6_1SS*/ {38, "VHT_MCS7_1SS"}, /*VHT_MCS7_1SS*/ {39, "VHT_MCS8_1SS"}, /*VHT_MCS8_1SS*/
//     {40, "VHT_MCS9_1SS"}, /*VHT_MCS9_1SS*/ {41, "VHT_MCS0_2SS"}, /*VHT_MCS0_2SS*/ {42, "VHT_MCS1_2SS"}, /*VHT_MCS1_2SS*/ {43, "VHT_MCS2_2SS"}, /*VHT_MCS2_2SS*/ {44, "VHT_MCS3_2SS"}, /*VHT_MCS3_2SS*/
//	   {45, "VHT_MCS4_2SS"}, /*VHT_MCS4_2SS*/ {46, "VHT_MCS5_2SS"}, /*VHT_MCS5_2SS*/ {47, "VHT_MCS6_2SS"}, /*VHT_MCS6_2SS*/ {48, "VHT_MCS7_2SS"}, /*VHT_MCS7_2SS*/ {49, "VHT_MCS8_2SS"}, /*VHT_MCS8_2SS*/
//	   {50, "VHT_MCS9_2SS"}, /*VHT_MCS9_2SS*/ {-1, "null"}
extern const int WLAN_RATE_TABLE_5G[MAX_WLAN_RATE] = {
	0, /*"DSSS-1"}*/
	1, /*"DSSS-2"*/	//2, /*DSSS-2S*/
	3, /*"CCK-5.5"*/	//4, /*CCK-5.5S*/
	5, /*"CCK-11"*/	//6, /*CCK-11S*/
	7, /*"OFDM-6"*/
	8, /*"OFDM-9"*/
	9, /*"OFDM-12"*/
	10, /*"OFDM-18"*/
	11, /*"OFDM-24"*/
	12, /*"OFDM-36"*/
	13, /*"OFDM-48"*/
	14, /*"OFDM-54"*/
	15, /*"MCS-0"*/
	16, /*"MCS-1"*/
	17, /*"MCS-2"*/
	18, /*"MCS-3"*/
	19, /*"MCS-4"*/
	20, /*"MCS-5"*/
	21, /*"MCS-6"*/
	22, /*"MCS-7"*/
	31,/* "VHT_MCS0_1SS"*/
	32, /*"VHT_MCS1_1SS"*/
	33, /*"VHT_MCS2_1SS"*/
	34, /*"VHT_MCS3_1SS"*/
	35, /*"VHT_MCS4_1SS"*/
	36, /*"VHT_MCS5_1SS"*/
	37, /*"VHT_MCS6_1SS"*/
	38, /*"VHT_MCS7_1SS"*/
	39, /*"VHT_MCS8_1SS"*/
	40, /*"VHT_MCS9_1SS"*/
	23, /*"MCS-8"*/
	24, /*"MCS-9"*/
	25, /*"MCS-10"*/
	26, /*"MCS-11"*/
	27, /*"MCS-12"*/
	28, /*"MCS-13"*/
	29, /*"MCS-14"*/
	30, /*"MCS-15"*/
	41, /*"VHT_MCS0_2SS"*/
	42, /*"VHT_MCS1_2SS"*/
	43, /*"VHT_MCS2_2SS"*/
	44, /*"VHT_MCS3_2SS"*/
	45, /*"VHT_MCS4_2SS"*/
	46, /*"VHT_MCS5_2SS"*/
	47, /*"VHT_MCS6_2SS"*/
	48, /*"VHT_MCS7_2SS"*/
	49, /*"VHT_MCS8_2SS"*/
	50 /*"VHT_MCS9_2SS"*/
};

// extern const int BT_TX_TIME;
// extern const int BT_RX_TIME;

CWcnTestSysWearATEx::CWcnTestSysWearATEx(void)
{
// 	char szIpError[128] = { 0 };
// 	CNetUtility::GetIpErrorInfor(IP_DEST_NET_UNREACHABLE, szIpError, sizeof(szIpError));
}


CWcnTestSysWearATEx::~CWcnTestSysWearATEx(void)
{
}

BOOL CWcnTestSysWearATEx::WlanLoadXML()
{
	wstring strProtocol[WIFI_MaxProtocol] = { L"Wlan11b", L"Wlan11g", L"Wlan11n", L"Wlan11ac", L"Wlan11a", L"WlanNoSupport" };
	wstring strBandWidth[WIFI_MaxProtocol] = { L"BW_20M", L"BW_40M", L"BW_80M" };
	m_vectWlanMeasParam.clear();
	m_vectWlanSpec.clear();


	for (int nProtoc = WIFI_802_11b; nProtoc < WIFI_MaxProtocol; nProtoc++)
	{
		WIFI_PROTOCOL_ENUM eProtoc = (WIFI_PROTOCOL_ENUM)nProtoc;
/*		WlanMeasBandSpec stSpec;*/

		for (int nBandWith = WIFI_BW_20M; nBandWith < WIFI_BW_MAX; nBandWith++)
		{
			wstring strBandSel = L"Option:Band:" + strProtocol[nProtoc] + L":" + strBandWidth[nBandWith];
			wstring strParamChan = L"Param:" + strProtocol[nProtoc] + L":" + strBandWidth[nBandWith];
			BOOL bEnable = GetConfigValue((strBandSel).c_str(), FALSE);
			if (!bEnable)
			{
				continue;
			}

			wstring strPath = strParamChan + L":GroupCount";
			int nGroupCount = GetConfigValue(strPath.c_str(), 0);
			for (int nGroupIndex = 0; nGroupIndex < nGroupCount; nGroupIndex++)
			{
				SPWI_WLAN_PARAM_MEAS_GROUP stMeasParam;
				stMeasParam.eMode = eProtoc;
				wstring strAntSel = L"Option:Band:ANT";
				LPCWSTR pAnt = GetConfigValue(strAntSel.c_str(), L"1 ANT_PRIMARY");
				stMeasParam.stTesterParamGroupSub.eAnt = (ANTENNA_ENUM)_wtoi(pAnt);
				stMeasParam.stTesterParamGroupSub.nAvgCount = 1;

				wchar_t    szGroup[20] = { 0 };
				swprintf_s(szGroup, L":Group%d", nGroupIndex + 1);
				strPath = strParamChan + szGroup;

				//Get modulation type
				LPCTSTR lpVal = GetConfigValue((strPath + L":Modulation").c_str(), L"MCS-7");
				E_WLAN_RATE eRate = CwcnUtility::WlanGetRate(_W2CA(lpVal));//WLAN_RATE_STRING5G
				if (INVALID_WLAN_RATE == eRate)
				{
					return SP_E_WCN_INVALID_XML_CONFIG;
				}
				stMeasParam.stTesterParamGroupSub.eRate = eRate;

				//PacketCount
				stMeasParam.stTesterParamGroupSub.nTotalPackets = GetConfigValue((strPath + L":PacketCount").c_str(), 1000);
				if (0 == stMeasParam.stTesterParamGroupSub.nTotalPackets)
				{
					stMeasParam.stTesterParamGroupSub.nTotalPackets = 200;
				}

				//Get per rx level
				stMeasParam.stTesterParamGroupSub.dVsgLvl = GetConfigValue((strPath + L":RXLVL:" + lpVal).c_str(), -65.0);
				if (!IN_RANGE(-100, stMeasParam.stTesterParamGroupSub.dVsgLvl, -25))
				{
					return SP_E_WCN_INVALID_XML_CONFIG;
				}
				//Get ref level
				stMeasParam.stTesterParamGroupSub.dRefLvl = GetConfigValue((strPath + L":RefLVL").c_str(), 10);
				//Chan band width
				stMeasParam.stTesterParamGroupSub.nCBWType = nBandWith;
				//Signal band width
				stMeasParam.stTesterParamGroupSub.nSBWType = nBandWith;
				//BLDC
				stMeasParam.stTesterParamGroupSub.nChCode = GetConfigValue((strPath + L":ChanCoding").c_str(), 0);

				int nDelta = stMeasParam.stTesterParamGroupSub.nCBWType - stMeasParam.stTesterParamGroupSub.nSBWType;
				int nFreqOff = GetConfigValue((strPath + L":PriChan").c_str(), 0);
				if (nDelta == 1 && abs(nFreqOff) > 1)
				{
					LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT] channel configuration is not correct!");
					return SP_E_WCN_INVALID_XML_CONFIG;
				}
				//TXP
				bEnable = GetConfigValue((strPath + L":TestItem:" + L"TXP").c_str(), FALSE);
				if (bEnable)
				{
					stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_TXP;
				}
				//TCFT
				bEnable = GetConfigValue((strPath + L":TestItem:" + L"TCFT").c_str(), FALSE);
				if (bEnable)
				{
					stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_FER;
				}
				//EVM
				bEnable = GetConfigValue((strPath + L":TestItem:" + L"EVM").c_str(), FALSE);
				if (bEnable)
				{
					stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_EVM;
				}
				//SPECMASK
				bEnable = GetConfigValue((strPath + L":TestItem:" + L"SPECMASK").c_str(), FALSE);
				if (bEnable)
				{
					stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_MASK;
				}
				//FLATNESS
				bEnable = GetConfigValue((strPath + L":TestItem:" + L"FLATNESS").c_str(), FALSE);
				if (bEnable)
				{
					stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_FLATNESS;
				}
				//PER
				bEnable = GetConfigValue((strPath + L":TestItem:" + L"PER").c_str(), FALSE);
				if (bEnable)
				{
					stMeasParam.stTesterParamGroupSub.dwMask |= WIFI_PER;
				}
				//m_stWlanParamBand.vecConfParamGroup.push_back(m_stGroupParam);

				wstring strVal = GetConfigValue((strPath + L":CenChan").c_str(), L"1,7,13");
				int nCount = 0;
				INT* parrInt = GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				if (nCount < 1)
				{
					LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
					return SP_E_WCN_INVALID_XML_CONFIG;
				}

				int nCenChan = 0;
				int nPrimChan = 0;
				for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
				{
					nCenChan = parrInt[nChTmp];
					stMeasParam.stChan.nCenChan = nCenChan;
					nPrimChan = nCenChan + nFreqOff * 2;
					stMeasParam.stChan.nPriChan = nPrimChan;
					m_vectWlanMeasParam.push_back(stMeasParam);
				}
			}
		}
		wstring strPath = L"Param:" + strProtocol[nProtoc] + L":Specification";
		//TXP spec
		int nCount = 0;
		wstring strVal = GetConfigValue((strPath + L":TXP").c_str(), L"8,20");
		double* parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		m_vectWlanSpec[eProtoc].dTxp.low = parrDouble[0];
		m_vectWlanSpec[eProtoc].dTxp.upp = parrDouble[1];
		m_vectWlanSpec[eProtoc].dTXPant2nd.low = parrDouble[0];
		m_vectWlanSpec[eProtoc].dTXPant2nd.upp = parrDouble[1];
		//EVM spec
		for (int nEvmIndx = 0; nEvmIndx < MAX_WLAN_RATE; nEvmIndx++)
		{
			wstring strRate = _A2CW(CwcnUtility::WlanGetRateString((E_WLAN_RATE)nEvmIndx));
			double dEvmSpec = GetConfigValue((strPath + L":EVM:" + strRate).c_str(), -999.0);
			m_vectWlanSpec[eProtoc].dEvm[nEvmIndx].low = -999.0/*NOLOWLMT*/;//Bug 1794990 不能设置“0”， 不然会使测试结果为[0.00, -34.19, -25.00]失败
			m_vectWlanSpec[eProtoc].dEvm[nEvmIndx].upp = dEvmSpec;
		}

		strVal = GetConfigValue((strPath + L":SPECMASK").c_str(), L"0,0,0,0,0,0,0,0");
		parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (WIFI_802_11b == eProtoc)
		{
			if (nCount < 4)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid SPECMASK limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			nCount = 4;
		}
		else
		{
			if (nCount < 8)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid SPECMASK limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			nCount = 8;
		}
		for (int i = 0; i < nCount; i++)
		{
			m_vectWlanSpec[eProtoc].dSpecMaskMargin[i] = parrDouble[i];
		}

		//Fer spec
		strVal = GetConfigValue((strPath + L":TCFT").c_str(), L"-15,15");
		INT* parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount, 10);
		if (nCount < 2)
		{
			LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid FER limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		m_vectWlanSpec[eProtoc].dFer.low = parrInt[0];
		m_vectWlanSpec[eProtoc].dFer.upp = parrInt[1];
		//PER spec
		m_vectWlanSpec[eProtoc].dPer = GetConfigValue((strPath + L":PER").c_str(), 10.0);
	}
	if (m_vectWlanMeasParam.size() > 0)
	{
		m_bWlan = TRUE;
	}
	else
	{
		m_bWlan = FALSE;
	}

// 	for (int i = 0; i < (int)m_vectWlanMeasParam.size(); i++)
// 	{
// 		CHKRESULT_WITH_NOTIFY_FUNNAME(WifiAddCase(m_vectWlanMeasParam[i].stChan.nCenChan, m_vectWlanMeasParam[i].stTesterParamGroupSub.eRate, \
// 			m_vectWlanMeasParam[i].stTesterParamGroupSub.eAnt, m_vectWlanMeasParam[i].stTesterParamGroupSub.nCBWType, m_vectWlanMeasParam[i].stTesterParamGroupSub.nSBWType, \
// 			m_nTriggerTimeOut, WLAN_TX_TIME, WLAN_RX_TIME, WLAN_END_TIME));
// 
// 	}

	return TRUE;
}


SPRESULT CWcnTestSysWearATEx::ConfigWlanParam()
{

	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT_WITH_NOTIFY_FUNNAME(WifiClearCase());
	if (!m_bBt)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(BtClearCase());
	}
	for (int i = 0; i < (int)m_vectWlanMeasParam.size(); i++)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(WifiAddCase(m_vectWlanMeasParam[i].stChan.nCenChan, m_vectWlanMeasParam[i].stTesterParamGroupSub.eRate, \
			m_vectWlanMeasParam[i].stTesterParamGroupSub.eAnt, m_vectWlanMeasParam[i].stTesterParamGroupSub.nCBWType, m_vectWlanMeasParam[i].stTesterParamGroupSub.nSBWType, \
			m_nTriggerTimeOut, WLAN_TX_TIME, WLAN_RX_TIME, WLAN_END_TIME));

	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearATEx::WifiAddCase(int nChanl, E_WLAN_RATE eRate, ANTENNA_ENUM chain, int cbw, int sbw, int nStartDelay, int nTxDelay, int nRxDelay, int nEndDelay)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szCmd[256];
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	int nRate = WLAN_RATE_TABLE_5G[eRate];

	sprintf_s(szCmd, "AT+ZCTSETWIFICASE5G=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", nChanl, nChanl, nRate, chain, cbw, sbw, nStartDelay, nTxDelay, nRxDelay, nEndDelay);
	//AT + ZCTSETWIFICASE5G = primary_channel, center_channel, rate, chain, cbw, sbw, start_delay, tx_delay, rx_delay, end_delay
	//chain = 1 / 2 / 3 (Primary / Secondary / MIMO)，cbw = 0 / 1 / 2 (20MHz / 40MHz / 80MHz)，sbw = 0 / 1 / 2 (20MHz / 40MHz / 80MHz)
	LogFmtStrA(SPLOGLV_INFO, "AT+ZCTSETWIFICASE5G primary_channel:%d, center_channel:%d, rate:%d, chain:%d, cbw:%d, sbw:%d, start_delay:%d, tx_delay:%d, rx_delay:%d, end_delay:%d",	\
		nChanl, nChanl, nRate, chain, cbw, sbw, nStartDelay, nTxDelay, nRxDelay, nEndDelay);
	
/*	LogFmtStrA(SPLOGLV_INFO, "AT+ZCTSETWIFICASE5G primary_channel:%d, center_channel:%d, rate:%d, start_delay:%d, tx_delay:%d, rx_delay:%d, end_delay:%d",
		nChanl, nChanl, nRate, nStartDelay, nTxDelay, nRxDelay, nEndDelay);

	sprintf_s(szCmd, "AT+ZCTSETWIFICASE5G=%d,%d,%d,%d,%d,%d,%d", nChanl, nChanl, nRate, nStartDelay, nTxDelay, nRxDelay, nEndDelay);
*/	
	SPRESULT res = SP_SendATCommand(m_hDUT, szCmd, TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiAddCase5G:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if (NULL == strstr(szBuff, "+ZCTSETWIFICASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiAddCase5G:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}

	return SP_OK;
}


SPRESULT CWcnTestSysWearATEx::ConfigBtParam()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_nBtChanCnt = 0;
	if (!m_bWlan)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(WifiClearCase());
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME(BtClearCase());
	for (int i = 0; i < (int)m_VecBTParamBandImp.size(); i++)
	{
		BT_PATTERN ePattern = BDR_TX_PBRS9;
		if (m_VecBTParamBandImp[i].eMode == BLE || m_VecBTParamBandImp[i].eMode == BLE_EX)
		{
			ePattern = BLE_TX_PBRS9;
		}
		for (int j = 0; j < (int)m_VecBTParamBandImp[i].vecBTFileParamChan.size(); j++)
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(BtAddCase(m_VecBTParamBandImp[i].eMode, m_VecBTParamBandImp[i].vecBTFileParamChan[j].nCh, \
				m_VecBTParamBandImp[i].vecBTFileParamChan[j].ePacketType, ePattern, m_nTriggerTimeOut, BT_TX_TIME, BT_RX_TIME, BT_END_TIME));
			m_nBtChanCnt++;
		}
	}
	return SP_OK;
}


SPRESULT CWcnTestSysWearATEx::BtAddCase(BT_TYPE eModeType, int nChan, BT_PACKET ePacketType, BT_PATTERN ePattern, int nStartDelay, int nTxDelay, int nRxDelay, int nEndDelay)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szCmd[256];
	char szBuff[1024] = { 0 };
	uint32 nRet = 0;
	int nPacket = GetPacketType(eModeType, ePacketType);
	int nMode = 2;
	if (eModeType == BDR || eModeType == EDR)
	{
		nMode = 1;
	}
	sprintf_s(szCmd, " AT+ZCTSETBTCASE5G=%d,%d,%d,%d,%d,%d,%d,%d,%d", nMode, ePattern, nChan, nPacket, 20, nStartDelay, nTxDelay, nRxDelay, nEndDelay);
	SPRESULT res = SP_SendATCommand(m_hDUT, szCmd, TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if (SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtAddCase5G:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if (NULL == strstr(szBuff, "+ZCTSETBTCASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtAddCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}


SPRESULT CWcnTestSysWearATEx::WaitForReconnect(int nTimeout)
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

		res = SP_BeginPhoneTest(m_hDUT, (CHANNEL_ATTRIBUTE*)&m_ca);
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