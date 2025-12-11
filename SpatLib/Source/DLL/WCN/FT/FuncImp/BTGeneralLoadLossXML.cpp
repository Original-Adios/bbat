#include "StdAfx.h"
#include "BTGeneralLoadLossXML.h"
#include <map>

CBTGeneralLoadLossXML::CBTGeneralLoadLossXML(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
    //BDR
    m_mapPacketTypeBDR.insert(pair<string, int>("DH1", BDR_DH1));
    m_mapPacketTypeBDR.insert(pair<string, int>("DH3", BDR_DH3));
    m_mapPacketTypeBDR.insert(pair<string, int>("DH5", BDR_DH5));
}

CBTGeneralLoadLossXML::~CBTGeneralLoadLossXML(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }

    m_mapPacketTypeBDR.clear();
    m_mapPacketTypeEDR.clear();
    m_mapPacketTypeBLE.clear();

}

SPRESULT CBTGeneralLoadLossXML::BTLoadLossXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp, BT_LOSS_PARAM& BtLossParm)
{
	if (!VecBTParamBandImp.empty())
	{
		VecBTParamBandImp.clear();
	}

	//Get config information from *.XML
	BTMeasParamBand stBTParamBand;
	BTMeasParamChan stChanParam;

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
	double m_dPreLoss = m_pSpatBase->GetConfigValue(L"Option:PreLoss", 0.5);
	double m_Tolernece = m_pSpatBase->GetConfigValue(L"Option:Tolernece", 0.3);
	BtLossParm.m_dPreLoss = m_dPreLoss;
	BtLossParm.m_Tolernece = m_Tolernece;
	int nAvgCount = m_pSpatBase->GetConfigValue(L"Param:Common:AvgCount", 3);
	//BDR
	bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"BDR").c_str(), FALSE);
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

		VecBTParamBandImp.push_back(stBTParamBand);
	}

	return SP_OK;
}
