#include "StdAfx.h"
#include "BTLoadXMLUDS710.h"
#include <map>

CBTLoadXMLUDS710::CBTLoadXMLUDS710(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
}

CBTLoadXMLUDS710::~CBTLoadXMLUDS710(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }
}

SPRESULT CBTLoadXMLUDS710::BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp)
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

	wstring strMode[MAX_BT_TYPE] = {L"BDR",L"EDR", L"BLE", L"BLE5.0"};
	wstring strAnt[ANT_BT_MAX] = {L"INVALID", L"StandAlone", L"Shared"};

	int nAvgCount = m_pSpatBase->GetConfigValue(L"Param:Common:AvgCount", 3);

	for(int nAnt=ANT_SINGLE; nAnt<ANT_BT_MAX; nAnt++ )
	{
		bEnable = (BOOL)m_pSpatBase->GetConfigValue((L"Option:Ant:"+ strAnt[nAnt]).c_str(), FALSE);
		if(!bEnable)
		{
			continue;
		}
		for(int nMode=BDR; nMode<MAX_BT_TYPE; nMode++)
		{
			 BTMeasParamBand stBTParamBand;
			 stBTParamBand.eMode = (BT_TYPE)nMode;
			 bEnable = (BOOL)m_pSpatBase->GetConfigValue((L"Option:Mode:"+strMode[nMode]).c_str(), FALSE);
			 
			 if(!bEnable)
			 {
				 continue;
			 }
			 int nGroupCount = m_pSpatBase->GetConfigValue((L"Param:"+ strMode[nMode] + L":GroupCount").c_str(), FALSE);
			 for(int nGroupIndex=0; nGroupIndex<nGroupCount; nGroupIndex++)
			 {
				 vector<int> ChGroup;
				 wchar_t    szGroup[20] = {0};
				 swprintf_s(szGroup, L":Group%d", nGroupIndex+1);
				 wstring strGroup = L"Param:"+ strMode[nMode] + szGroup;
				 wstring strVal = m_pSpatBase->GetConfigValue((strGroup + L":TCH").c_str(), L"1,39,78");
				 int nTxLogicCh = m_pSpatBase->GetConfigValue((strGroup + L":TxLogicCh").c_str(), 0);
				 parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 1)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 
				 for (int nChTmp=0;nChTmp<nCount;nChTmp++)
				 {
					 RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
					 CHKRESULT(eBtConState);
					 ChGroup.push_back(parrInt[nChTmp]);
				 }
				 LPCSTR lpPacket = m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strGroup + L":PacketType").c_str(), L"DH5"));
				 BT_PACKET ePacket = CwcnUtility::BTGetPacket(stBTParamBand.eMode, lpPacket);
				 if(INVALID_PACKET == ePacket)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid packet type!");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 double dVsgLvl = m_pSpatBase->GetConfigValue((strGroup + L":RXLVL").c_str(), -65.0);
				 double dRefLvl = m_pSpatBase->GetConfigValue((strGroup + L":RefLVL").c_str(), 10);
				 int nPacketCount = m_pSpatBase->GetConfigValue((strGroup + L":RxCount").c_str(), 1000);
				 wstring strMask = strGroup + L":TestItem:";
				 DWORD dwMask = 0;
				 //BDR
				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"TXP").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_POWER:BDR_POWER;
				 }
				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"TSBD").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= BDR_20BW;
				 }

				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"TSACP").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= BDR_ACP;					
				 }
				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"MOD").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_MC:BDR_MC;
				 }

				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"ICFT").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= BDR_ICFR;
				 }

				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"CFD").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= BDR_CFD;
				 }

				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"BER").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_PER:BDR_BER;
				 }
				 //EDR
				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"RTXP").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= EDR_ETP;
				 }

				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"CFDMODA").c_str(), FALSE);
				 if (bEnable)
				 {
					 dwMask |= EDR_DEVM;
				 }
				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"IBSE").c_str(), FALSE);
				 if (bEnable)
				 {
					 dwMask |= (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_ACP:EDR_ACP;
				 }

				 bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"CFOD").c_str(), FALSE);
				 if(bEnable)
				 {
					 dwMask |= BLE_CFD;
				 }
				 for (int nChTmp=0;nChTmp<(int)ChGroup.size();nChTmp++)
				 {
					 BTMeasParamChan stChanParam;
					 stChanParam.dRefLvl = dRefLvl;
					 stChanParam.nTxLogicCh = nTxLogicCh;
					 stChanParam.dVsgLvl = dVsgLvl;
					 stChanParam.dwMask = dwMask;
					 stChanParam.ePacketType = ePacket;
					 stChanParam.ePath = BT_RFPATH_ENUM(nAnt);
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
				 wstring strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXP").c_str(), L"8,20");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBdrSpec.dTxp.low = parrDouble[0];
				 stBTParamBand.stSpec.stBdrSpec.dTxp.upp = parrDouble[1];
				 stBTParamBand.stSpec.stBdrSpec.dTxpShared.low = parrDouble[0];
				 stBTParamBand.stSpec.stBdrSpec.dTxpShared.upp = parrDouble[1];
				 //TSBD spec
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":TSBD").c_str(), L"-1000,1000");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid 11b 20dB bandwidth spec");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBdrSpec.dTsbd.low = parrDouble[0];
				 stBTParamBand.stSpec.stBdrSpec.dTsbd.upp = parrDouble[1];
				 //TSACP spec
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":TSACP").c_str(), L"-40,-40,-40,-20,99,99,99,-20,-40,-40,-40");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR ACP limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 for (int nAcpIndex = 0; nAcpIndex < nCount; nAcpIndex++)
				 {
					 stBTParamBand.stSpec.stBdrSpec.dTsacp[nAcpIndex].low = NOLOWLMT;
					 stBTParamBand.stSpec.stBdrSpec.dTsacp[nAcpIndex].upp = parrInt[nAcpIndex];
				 }
				 //mod characteristics spec
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":MOD:f1avg").c_str(), L"140,175");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR mod characteristics limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBdrSpec.dModF1avg.low = parrInt[0];
				 stBTParamBand.stSpec.stBdrSpec.dModF1avg.upp = parrInt[1];
				 //f2max
				 stBTParamBand.stSpec.stBdrSpec.dModF2max.upp = 999;
				 stBTParamBand.stSpec.stBdrSpec.dModF2max.low = m_pSpatBase->GetConfigValue((strSpec + L":MOD:f2max").c_str(), 115);
				 //mod ratio
				 stBTParamBand.stSpec.stBdrSpec.dModRatio = m_pSpatBase->GetConfigValue((strSpec + L":MOD:ratio").c_str(), 1.0);
				 //ICFT Initial carrier frequency tolerance
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":ICFT:ICFT").c_str(), L"-75,75");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR initial carrier frequency tolerance limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBdrSpec.dIcft.low = parrInt[0];
				 stBTParamBand.stSpec.stBdrSpec.dIcft.upp = parrInt[1];
				 //Carrier Frequency Drift
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFD").c_str(), L"-40,40");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR carrier frequency drift limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBdrSpec.dCfd.low = parrInt[0];
				 stBTParamBand.stSpec.stBdrSpec.dCfd.upp = parrInt[1];
				 //BER spec
				 stBTParamBand.stSpec.stBdrSpec.dBer = m_pSpatBase->GetConfigValue((strSpec + L":BER").c_str(), 0.1);
				 VecBTParamBandImp.push_back(stBTParamBand);
			 }
			 else if(nMode == EDR)
			 {
				 //TXP spec
				 wstring strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXP").c_str(), L"4,20");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stEdrSpec.dTxp.low = parrDouble[0];
				 stBTParamBand.stSpec.stEdrSpec.dTxp.upp = parrDouble[1];
				 stBTParamBand.stSpec.stEdrSpec.dTxpShared.low = parrDouble[0];
				 stBTParamBand.stSpec.stEdrSpec.dTxpShared.upp = parrDouble[1];
				 //RTXP spec
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":RTXP").c_str(), L"-4,1");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid Relative transmit power limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stEdrSpec.dRTxp.low = parrDouble[0];
				 stBTParamBand.stSpec.stEdrSpec.dRTxp.upp = parrDouble[1];
				 //CFMODA Carrier frequency drift and modulation acurrancy
				 //CFMODA W0
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFDMODA:w0").c_str(), L"-10,10");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid carrier frequency drift and modulation accuracy w0 limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stEdrSpec.dW0.low = parrInt[0];
				 stBTParamBand.stSpec.stEdrSpec.dW0.upp = parrInt[1];
				 //Wi
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFDMODA:wi").c_str(), L"-75,75");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid carrier frequency drift and modulation accuracy wi limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stEdrSpec.dWi.low = parrInt[0];
				 stBTParamBand.stSpec.stEdrSpec.dWi.upp = parrInt[1];
				 //RMSDEVM
				 stBTParamBand.stSpec.stEdrSpec.dRmsEvm_2DHX.low = 0;
				 stBTParamBand.stSpec.stEdrSpec.dRmsEvm_2DHX.upp = m_pSpatBase->GetConfigValue((strSpec + L":CFDMODA:RMSDEVM_2DHX").c_str(), 0.2);
				 //PEAKDEVM
				 stBTParamBand.stSpec.stEdrSpec.dPeakEvm_2DHX.low = 0;
				 stBTParamBand.stSpec.stEdrSpec.dPeakEvm_2DHX.upp = m_pSpatBase->GetConfigValue((strSpec + L":CFDMODA:PEAKDEVM_2DHX").c_str(), 0.35);

				 //RMSDEVM
				 stBTParamBand.stSpec.stEdrSpec.dRmsEvm_3DHX.low = 0;
				 stBTParamBand.stSpec.stEdrSpec.dRmsEvm_3DHX.upp = m_pSpatBase->GetConfigValue((strSpec + L":CFDMODA:RMSDEVM_3DHX").c_str(), 0.13);
				 //PEAKDEVM
				 stBTParamBand.stSpec.stEdrSpec.dPeakEvm_3DHX.low = 0;
				 stBTParamBand.stSpec.stEdrSpec.dPeakEvm_3DHX.upp = m_pSpatBase->GetConfigValue((strSpec + L":CFDMODA:PEAKDEVM_3DHX").c_str(), 0.25);

				 //IBSE spec
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":IBSE").c_str(), L"-40,-40,-40,-20,-26,99,-26,-20,-40,-40,-40");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR ACP limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
				 {
					 stBTParamBand.stSpec.stEdrSpec.dIbse[nIsbeIndex].low = NOLOWLMT;
					 stBTParamBand.stSpec.stEdrSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
				 }
				 //BER spec
				 stBTParamBand.stSpec.stEdrSpec.dBer = m_pSpatBase->GetConfigValue((strSpec + L":BER").c_str(), 0.1);

				 VecBTParamBandImp.push_back(stBTParamBand);
			 }
			 else
			 {
				 //TXP AVG spec
				 wstring strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXP").c_str(), L"-20,10");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBleExSpec.dTxpAvg.low = parrDouble[0];
				 stBTParamBand.stSpec.stBleExSpec.dTxpAvg.upp = parrDouble[1];
				 stBTParamBand.stSpec.stBleExSpec.dTxpAvgShared.low = parrDouble[0];
				 stBTParamBand.stSpec.stBleExSpec.dTxpAvgShared.upp = parrDouble[1];
				 //TXP PEAK spec
				 stBTParamBand.stSpec.stBleExSpec.dTxpPeak.low = 0;
				 stBTParamBand.stSpec.stBleExSpec.dTxpPeak.upp = m_pSpatBase->GetConfigValue((strSpec + L":TXP_PEAK_AVG").c_str(), 3);
				 //IBSE spec
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":IBSE").c_str(), L"-30,-30,-30,-20,99,99,99,-20,-30,-30,-30");
				 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE In-Band Spurious Emssions limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
				 {
					 stBTParamBand.stSpec.stBleExSpec.dIbse[nIsbeIndex].low = NOLOWLMT;
					 stBTParamBand.stSpec.stBleExSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
				 }
				 //Modulation Characteristics
				 strVal = m_pSpatBase->GetConfigValue((strSpec + L":MOD:f1avg").c_str(), L"225,275");
				 parrInt = m_pSpatBase->GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
				 if(nCount < 2)
				 {
					 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE modulation Characteristics limit");
					 return SP_E_WCN_INVALID_XML_CONFIG;
				 }
				 stBTParamBand.stSpec.stBleExSpec.dF1Avg.low = parrInt[0];
				 stBTParamBand.stSpec.stBleExSpec.dF1Avg.upp = parrInt[1];
				 //F2Max
				 stBTParamBand.stSpec.stBleExSpec.dF2Max = m_pSpatBase->GetConfigValue((strSpec + L":MOD:f2max").c_str(), 185);
				 //ratio
				 stBTParamBand.stSpec.stBleExSpec.dRatio = m_pSpatBase->GetConfigValue((strSpec + L":MOD:ratio").c_str(), 0.8);
				 //CFOD
				 for(int nPacket=RF_PHY_1M; nPacket<RF_PHY_MAX; nPacket++)
				 {
					 LPCTSTR lpName = m_pSpatBase->_A2CW(CwcnUtility::BTGetPacketString( BLE_EX, (BT_PACKET)nPacket ));
					 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFOD:" + lpName + L":Fn").c_str(), L"-150,150");
					 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
					 if(nCount < 2)
					 {
						 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit");
						 return SP_E_WCN_INVALID_XML_CONFIG;
					 }
					 stBTParamBand.stSpec.stBleExSpec.dFnMax[nPacket].low = parrDouble[0];
					 stBTParamBand.stSpec.stBleExSpec.dFnMax[nPacket].upp = parrDouble[1];

					 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFOD:"+ lpName + L"F0Fn").c_str(), L"-99,50");
					 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
					 if(nCount < 1)
					 {
						 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit");
						 return SP_E_WCN_INVALID_XML_CONFIG;
					 }
					 stBTParamBand.stSpec.stBleExSpec.dF0FnMax[nPacket].low = parrDouble[0];
					 stBTParamBand.stSpec.stBleExSpec.dF0FnMax[nPacket].upp = parrDouble[1];

					 LPTSTR lpItem = L":F1F0";
					 if(RF_PHY_S8 == nPacket)
					 {
						 lpItem = L":F0F3";
					 }
					 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFOD:"+ lpName + lpItem).c_str(), L"-99,23");
					 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
					 if(nCount < 2)
					 {
						 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE %s limit", m_pSpatBase->_W2CA(lpItem));
						 return SP_E_WCN_INVALID_XML_CONFIG;
					 }
					 stBTParamBand.stSpec.stBleExSpec.dF1F0[nPacket].low = parrDouble[0];
					 stBTParamBand.stSpec.stBleExSpec.dF1F0[nPacket].upp = parrDouble[1];

					 lpItem = L":FnFn5";
					 if(RF_PHY_S8 == nPacket)
					 {
						 lpItem = L":FnFn3";
					 }
					 strVal = m_pSpatBase->GetConfigValue((strSpec + L":CFOD:"+ lpName + lpItem).c_str(), L"-99,20");
					 parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
					 if(nCount < 2)
					 {
						 m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE %s limit", m_pSpatBase->_W2CA(lpItem));
						 return SP_E_WCN_INVALID_XML_CONFIG;
					 }
					 stBTParamBand.stSpec.stBleExSpec.dFnFn5[nPacket].low = parrDouble[0];
					 stBTParamBand.stSpec.stBleExSpec.dFnFn5[nPacket].upp = parrDouble[1];
				 }
				
				 //PER spec
				 stBTParamBand.stSpec.stBleExSpec.dBer = m_pSpatBase->GetConfigValue((strSpec + L":BER").c_str(), 30.8);
				 VecBTParamBandImp.push_back(stBTParamBand);
			 }
			 
		}
	}
    return SP_OK;
}
