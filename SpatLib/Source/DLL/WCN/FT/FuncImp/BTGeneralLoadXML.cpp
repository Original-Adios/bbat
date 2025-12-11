#include "StdAfx.h"
#include "BTGeneralLoadXML.h"
#include <map>

CBTGeneralLoadXML::CBTGeneralLoadXML(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
    //BDR
    m_mapPacketTypeBDR.insert(pair<string, int>("DH1", BDR_DH1));
    m_mapPacketTypeBDR.insert(pair<string, int>("DH3", BDR_DH3));
    m_mapPacketTypeBDR.insert(pair<string, int>("DH5", BDR_DH5));
	//EDR
	m_mapPacketTypeEDR.insert(pair<string, int>("2-DH1", EDR_2DH1));
	m_mapPacketTypeEDR.insert(pair<string, int>("2-DH3", EDR_2DH3));
	m_mapPacketTypeEDR.insert(pair<string, int>("2-DH5", EDR_2DH5));
	m_mapPacketTypeEDR.insert(pair<string, int>("3-DH1", EDR_3DH1));
	m_mapPacketTypeEDR.insert(pair<string, int>("3-DH3", EDR_3DH3));
	m_mapPacketTypeEDR.insert(pair<string, int>("3-DH5", EDR_3DH5));
	//BLE
	m_mapPacketTypeBLE.insert(pair<string, int>("BLE_RF_PHY_TEST_REF", BLE_RF_PHY_TEST_REF));
}

CBTGeneralLoadXML::~CBTGeneralLoadXML(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }

    m_mapPacketTypeBDR.clear();

}

SPRESULT CBTGeneralLoadXML::BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp)
{
	if (!VecBTParamBandImp.empty())
	{
		VecBTParamBandImp.clear();

	}

	//Get config information from *.XML
	BTMeasParamBand stBTParamBand;
	BTMeasParamChan stChanParam;
	BT_LOSS_PARAM stBtLossParam;

	//Get BT band config
	BOOL bEnable = FALSE;
	wstring strBandSel = L"Option:Band:";
	wstring strParamChan = L"Param:";
	wstring strParamXml = L"";
	int nChanCount = 0;
	int nCount = 0;
	double* parrDouble = NULL;
	int* parrInt = NULL;
	wstring szPacketType = L"";
	SPRESULT eBtConState = SP_OK;
	int nAvgCount = m_pSpatBase->GetConfigValue(L"Param:Common:AvgCount", 3);
	//BDR
	bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"BDR").c_str(), FALSE);

	stBtLossParam.m_dPreLoss = (BOOL)m_pSpatBase->GetConfigValue((strParamXml + L"Option:PreLoss").c_str(), 0);
	stBtLossParam.m_Tolernece = (BOOL)m_pSpatBase->GetConfigValue((strParamXml + L"Option:Tolernece").c_str(), 0.3);
	if (TRUE == bEnable)
	{
		stBTParamBand.eMode = BDR;
		//Get channel config
		strParamChan = L"Param:BDR:GroupCount";
		char szBuf[32] = { 0 };

		nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

		for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
		{
			strParamChan.clear();
			stChanParam.Init();
			strParamChan = L"Param:BDR:Group";
			sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex + 1);
			strParamChan += m_pSpatBase->_A2CW(szBuf);
			//Get Chan 
			vector<int> ChGroup;
			wstring strVal = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,39,78");
			parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
				CHKRESULT(eBtConState);
				ChGroup.push_back(parrInt[nChTmp]);
			}
			RSLT_RANGE_CHECK(stChanParam.nCh, 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
			stChanParam.nAvgCount = nAvgCount;
			stChanParam.nTotalPackets = m_pSpatBase->GetConfigValue((strParamChan + L":RxCount").c_str(), 816800);
			stChanParam.ePath = ANT_SHARED;
			//Get packet type
			strcpy_s(szBuf, sizeof(szBuf), m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":PacketType").c_str(), L"DH5")));
			stChanParam.ePacketType = (BT_PACKET)m_mapPacketTypeBDR.find(szBuf)->second;
			//Get per rx level
			stChanParam.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
			RSLT_RANGE_CHECK(stChanParam.dVsgLvl, -120, -25, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
			//Get ref level
			stChanParam.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
			//Get test item
			strParamChan += L":TestItem:";
			//TXP
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_POWER;
			}
			//TSBD
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TSBD").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_20BW;
			}
			//TSACP Adjancent channel power
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TSACP").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_ACP;
			}
			//MOD
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"MOD").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_MC;
			}
			//ICFT
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"ICFT").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_ICFR;
			}
			//CFD carrier frequency drift
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"CFD").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_CFD;
			}
			//BER
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"BER").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_BER;
			}

			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"RSSI").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_RSSI;
			}

			stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[stBTParamBand.eMode][stChanParam.ePacketType];

			for (int nChTmp = 0; nChTmp < (int)ChGroup.size(); nChTmp++)
			{
				stChanParam.nCh = ChGroup[nChTmp];
				stBTParamBand.vecBTFileParamChan.push_back(stChanParam);
			}
		}
		//Get specification
		strParamChan.clear();
		strParamChan = L"Param:BDR:Specification";
		//TXP spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:TXP").c_str(), L"8,20");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBdrSpec.dTxp.low = parrDouble[0];
		stBTParamBand.stSpec.stBdrSpec.dTxp.upp = parrDouble[1];
		stBTParamBand.stSpec.stBdrSpec.dTxpShared.low = parrDouble[0];
		stBTParamBand.stSpec.stBdrSpec.dTxpShared.upp = parrDouble[1];
		//TSBD spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TSBD:TSBD").c_str(), L"-1000,1000");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid 11b 20dB bandwidth spec");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBdrSpec.dTsbd.low = parrDouble[0];
		stBTParamBand.stSpec.stBdrSpec.dTsbd.upp = parrDouble[1];
		//TSACP spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TSACP:TSACP").c_str(), L"-40,-40,-40,-20,99,99,99,-20,-40,-40,-40");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR ACP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		for (int nAcpIndex = 0; nAcpIndex < nCount; nAcpIndex++)
		{
			stBTParamBand.stSpec.stBdrSpec.dTsacp[nAcpIndex].low = -99;
			stBTParamBand.stSpec.stBdrSpec.dTsacp[nAcpIndex].upp = parrInt[nAcpIndex];
		}
		//mod characteristics spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":MOD:f1avg").c_str(), L"140,175");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR mod characteristics limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBdrSpec.dModF1avg.low = parrInt[0];
		stBTParamBand.stSpec.stBdrSpec.dModF1avg.upp = parrInt[1];
		//f2max
		stBTParamBand.stSpec.stBdrSpec.dModF2max.upp = 999;
		stBTParamBand.stSpec.stBdrSpec.dModF2max.low = m_pSpatBase->GetConfigValue((strParamChan + L":MOD:f2max").c_str(), 115);
		//mod ratio
		stBTParamBand.stSpec.stBdrSpec.dModRatio = m_pSpatBase->GetConfigValue((strParamChan + L":MOD:ratio").c_str(), 1.0);
		//ICFT Initial carrier frequency tolerance
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":ICFT:ICFT").c_str(), L"-75,75");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR initial carrier frequency tolerance limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBdrSpec.dIcft.low = parrInt[0];
		stBTParamBand.stSpec.stBdrSpec.dIcft.upp = parrInt[1];
		//Carrier Frequency Drift
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFD:CFD").c_str(), L"-40,40");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR carrier frequency drift limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBdrSpec.dCfd.low = parrInt[0];
		stBTParamBand.stSpec.stBdrSpec.dCfd.upp = parrInt[1];
		//BER spec
		stBTParamBand.stSpec.stBdrSpec.dBer = m_pSpatBase->GetConfigValue((strParamChan + L":BER:BER").c_str(), 0.1);

		//RSSI
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":RSSI:RSSI").c_str(), L"-5,5");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR RSSI limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBdrSpec.dRssi.low = parrDouble[0];
		stBTParamBand.stSpec.stBdrSpec.dRssi.upp = parrDouble[1];

		VecBTParamBandImp.push_back(stBTParamBand);
	}

	//EDR
	bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"EDR").c_str(), FALSE);
	if (TRUE == bEnable)
	{
		stBTParamBand.vecBTFileParamChan.clear();
		stBTParamBand.eMode = EDR;
		//Get channel config
		strParamChan = L"Param:EDR:GroupCount";
		char szBuf[32] = { 0 };

		nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

		for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
		{
			strParamChan.clear();
			stChanParam.Init();
			strParamChan = L"Param:EDR:Group";
			sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex + 1);
			strParamChan += m_pSpatBase->_A2CW(szBuf);
			//Get Chan 
			vector<int> ChGroup;
			wstring strVal = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,39,78");
			parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
				CHKRESULT(eBtConState);
				ChGroup.push_back(parrInt[nChTmp]);
			}
			RSLT_RANGE_CHECK(stChanParam.nCh, 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
			//Get packet type
			strcpy_s(szBuf, sizeof(szBuf), m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":PacketType").c_str(), L"2-DH5")));
			stChanParam.ePacketType = (BT_PACKET)m_mapPacketTypeEDR.find(szBuf)->second;
			//Get per rx level
			stChanParam.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
			RSLT_RANGE_CHECK(stChanParam.dVsgLvl, -100, -25, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
			//Get ref level
			stChanParam.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
			stChanParam.nAvgCount = nAvgCount;
			stChanParam.nTotalPackets = m_pSpatBase->GetConfigValue((strParamChan + L":RxCount").c_str(), 816800);
			stChanParam.ePath = ANT_SHARED;
			//Get test item
			strParamChan += L":TestItem:";
			//TXP
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_POWER;
			}
			//EDR_ETP
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"RTXP").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= EDR_ETP;
			}
			//CFDMODA Carrier frequency drift and modulation accuracy
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"CFDMODA").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= EDR_DEVM;
			}
			//IBSE In-Band spurious emission
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"IBSE").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= EDR_ACP;
			}
			//BER
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"BER").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BDR_BER;
			}
			//RSSI
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"RSSI").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_RSSI;
			}

			stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[stBTParamBand.eMode][stChanParam.ePacketType];

			for (int nChTmp = 0; nChTmp < (int)ChGroup.size(); nChTmp++)
			{
				stChanParam.nCh = ChGroup[nChTmp];
				stBTParamBand.vecBTFileParamChan.push_back(stChanParam);
			}
		}
		//Get specification
		strParamChan.clear();
		strParamChan = L"Param:EDR:Specification";
		//TXP spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:TXP").c_str(), L"-6,4");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stEdrSpec.dTxp.low = parrDouble[0];
		stBTParamBand.stSpec.stEdrSpec.dTxp.upp = parrDouble[1];
		stBTParamBand.stSpec.stEdrSpec.dTxpShared.low = parrDouble[0];
		stBTParamBand.stSpec.stEdrSpec.dTxpShared.upp = parrDouble[1];
		//RTXP spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":RTXP:RTXP").c_str(), L"-4,1");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid Relative transmit power limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stEdrSpec.dRTxp.low = parrDouble[0];
		stBTParamBand.stSpec.stEdrSpec.dRTxp.upp = parrDouble[1];
		//CFMODA Carrier frequency drift and modulation acurrancy
		//CFMODA W0
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:w0").c_str(), L"-10,10");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid carrier frequency drift and modulation accuracy w0 limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stEdrSpec.dW0.low = parrInt[0];
		stBTParamBand.stSpec.stEdrSpec.dW0.upp = parrInt[1];
		//Wi
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:wi").c_str(), L"-75,75");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid carrier frequency drift and modulation accuracy wi limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stEdrSpec.dWi.low = parrInt[0];
		stBTParamBand.stSpec.stEdrSpec.dWi.upp = parrInt[1];

		//RMSDEVM
		stBTParamBand.stSpec.stEdrSpec.dRmsEvm_2DHX.low = 0;
		stBTParamBand.stSpec.stEdrSpec.dRmsEvm_2DHX.upp = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:RMSDEVM_2DHX").c_str(), 0.2);
		//PEAKDEVM
		stBTParamBand.stSpec.stEdrSpec.dPeakEvm_2DHX.low = 0;
		stBTParamBand.stSpec.stEdrSpec.dPeakEvm_2DHX.upp = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:PEAKDEVM_2DHX").c_str(), 0.35);
		//P99DEVM
		stBTParamBand.stSpec.stEdrSpec.dP99Evm_2DHX.low = 0;
		stBTParamBand.stSpec.stEdrSpec.dP99Evm_2DHX.upp = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:P99DEVM_2DHX").c_str(), 0.30);

		//RMSDEVM
		stBTParamBand.stSpec.stEdrSpec.dRmsEvm_3DHX.low = 0;
		stBTParamBand.stSpec.stEdrSpec.dRmsEvm_3DHX.upp = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:RMSDEVM_3DHX").c_str(), 0.13);
		//PEAKDEVM
		stBTParamBand.stSpec.stEdrSpec.dPeakEvm_3DHX.low = 0;
		stBTParamBand.stSpec.stEdrSpec.dPeakEvm_3DHX.upp = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:PEAKDEVM_3DHX").c_str(), 0.25);
		//P99DEVM
		stBTParamBand.stSpec.stEdrSpec.dP99Evm_3DHX.low = 0;
		stBTParamBand.stSpec.stEdrSpec.dP99Evm_3DHX.upp = m_pSpatBase->GetConfigValue((strParamChan + L":CFDMODA:P99DEVM_3DHX").c_str(), 0.25);

		//IBSE spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":IBSE:IBSE").c_str(), L"-40,-40,-40,-20,-26,99,-26,-20,-40,-40,-40");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR ACP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
		{
			stBTParamBand.stSpec.stEdrSpec.dIbse[nIsbeIndex].low = -99;
			stBTParamBand.stSpec.stEdrSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
		}
		//BER spec
		stBTParamBand.stSpec.stEdrSpec.dBer = m_pSpatBase->GetConfigValue((strParamChan + L":BER:BER").c_str(), 0.1);

		//RSSI
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":RSSI:RSSI").c_str(), L"-5,5");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid EDR RSSI limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stEdrSpec.dRssi.low = parrDouble[0];
		stBTParamBand.stSpec.stEdrSpec.dRssi.upp = parrDouble[1];

		VecBTParamBandImp.push_back(stBTParamBand);
	}
	//BLE
	bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"BLE").c_str(), FALSE);
	if (TRUE == bEnable)
	{
		stBTParamBand.vecBTFileParamChan.clear();

		stBTParamBand.eMode = BLE;
		//Get channel config
		strParamChan = L"Param:BLE:GroupCount";
		char szBuf[32] = { 0 };

		nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

		for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
		{
			strParamChan.clear();
			stChanParam.Init();
			strParamChan = L"Param:BLE:Group";
			sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex + 1);
			strParamChan += m_pSpatBase->_A2CW(szBuf);
			//Get Chan 
			vector<int> ChGroup;
			wstring strVal = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,39,78");
			parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
				CHKRESULT(eBtConState);
				ChGroup.push_back(parrInt[nChTmp]);
			}
			RSLT_RANGE_CHECK(stChanParam.nCh, 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
			//Get packet type
			strcpy_s(szBuf, sizeof(szBuf), m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":PacketType").c_str(), L"BLE_RF_PHY_TEST_REF")));
			stChanParam.ePacketType = (BT_PACKET)m_mapPacketTypeBLE.find(szBuf)->second;
			//Get per rx level
			stChanParam.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
			RSLT_RANGE_CHECK(stChanParam.dVsgLvl, -100, -25, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
			//Get ref level
			stChanParam.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
			stChanParam.nAvgCount = nAvgCount;
			stChanParam.nTotalPackets = m_pSpatBase->GetConfigValue((strParamChan + L":RxCount").c_str(), 200);
			stChanParam.ePath = ANT_SHARED;
			//Get test item
			strParamChan += L":TestItem:";
			//TXP
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_POWER;
			}
			//IBSE
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"IBSE").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_ACP;
			}
			//CFDMODA Carrier frequency drift and modulation accuracy
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"MOD").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_MC;
			}
			//IBSE In-Band spurious emission
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"CFOD").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_CFD;
			}
			//BER
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"BER").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_PER;
			}

			//RSSI
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"RSSI").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.dwMask |= BLE_RSSI;
			}

			stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[stBTParamBand.eMode][stChanParam.ePacketType];
			for (int nChTmp = 0; nChTmp < (int)ChGroup.size(); nChTmp++)
			{
				stChanParam.nCh = ChGroup[nChTmp];
				stBTParamBand.vecBTFileParamChan.push_back(stChanParam);
			}
		}
		//Get specification
		strParamChan.clear();
		strParamChan = L"Param:BLE:Specification";
		//TXP AVG spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:AVG").c_str(), L"-20,10");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBleSpec.dTxpAvg.low = parrDouble[0];
		stBTParamBand.stSpec.stBleSpec.dTxpAvg.upp = parrDouble[1];
		stBTParamBand.stSpec.stBleSpec.dTxpAvgShared.low = parrDouble[0];
		stBTParamBand.stSpec.stBleSpec.dTxpAvgShared.upp = parrDouble[1];
		//TXP PEAK spec
		stBTParamBand.stSpec.stBleSpec.dTxpPeak.low = -20;
		stBTParamBand.stSpec.stBleSpec.dTxpPeak.upp = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:PEAK").c_str(), 3);
		//IBSE spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":IBSE:IBSE").c_str(), L"-30,-30,-30,-20,99,99,99,-20,-30,-30,-30");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE In-Band Spurious Emssions limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
		{
			stBTParamBand.stSpec.stBleSpec.dIbse[nIsbeIndex].low = -99;
			stBTParamBand.stSpec.stBleSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
		}
		//Modulation Characteristics
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":MOD:f1avg").c_str(), L"225,275");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE modulation Characteristics limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBleSpec.dF1Avg.low = parrInt[0];
		stBTParamBand.stSpec.stBleSpec.dF1Avg.upp = parrInt[1];
		//F2Max
		stBTParamBand.stSpec.stBleSpec.dF2Max = m_pSpatBase->GetConfigValue((strParamChan + L":MOD:f2max").c_str(), 185);
		//ratio
		stBTParamBand.stSpec.stBleSpec.dRatio = m_pSpatBase->GetConfigValue((strParamChan + L":MOD:ratio").c_str(), 0.8);
		//CFOD
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFOD:Fn").c_str(), L"-150,150");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 1)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		if (1 == nCount)
		{
			stBTParamBand.stSpec.stBleSpec.dFnMax[RF_PHY_1M].low = -150;
			stBTParamBand.stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp = parrDouble[0];
		}
		else
		{
			stBTParamBand.stSpec.stBleSpec.dFnMax[RF_PHY_1M].low = parrDouble[0];
			stBTParamBand.stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp = parrDouble[1];
		}

		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFOD:F0Fn").c_str(), L"-99,50");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 1)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		if (1 == nCount)
		{
			stBTParamBand.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].low = -99;
			stBTParamBand.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].upp = parrDouble[0];
		}
		else
		{
			stBTParamBand.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].low = parrDouble[0];
			stBTParamBand.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].upp = parrDouble[1];
		}

		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFOD:F1F0").c_str(), L"-99,23");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 1)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE F1F0 Max limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		if (1 == nCount)
		{
			stBTParamBand.stSpec.stBleSpec.dF1F0[RF_PHY_1M].low = -99;
			stBTParamBand.stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp = parrDouble[0];
		}
		else
		{
			stBTParamBand.stSpec.stBleSpec.dF1F0[RF_PHY_1M].low = parrDouble[0];
			stBTParamBand.stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp = parrDouble[1];
		}

		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":CFOD:FnFn5").c_str(), L"-99,20");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 1)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE FnFn5 Max limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		if (1 == nCount)
		{
			stBTParamBand.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low = -99;
			stBTParamBand.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp = parrDouble[0];
		}
		else
		{
			stBTParamBand.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low = parrDouble[0];
			stBTParamBand.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp = parrDouble[1];
		}

		//PER spec
		stBTParamBand.stSpec.stBleSpec.dBer = m_pSpatBase->GetConfigValue((strParamChan + L":BER:BER").c_str(), 30.8);

		//RSSI
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":RSSI:RSSI").c_str(), L"-5,5");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if (nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE RSSI limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stBTParamBand.stSpec.stBleSpec.dRssi.low = parrDouble[0];
		stBTParamBand.stSpec.stBleSpec.dRssi.upp = parrDouble[1];

		VecBTParamBandImp.push_back(stBTParamBand);
	}
	return SP_OK;
}
