#include "StdAfx.h"
#include <cassert>
#include "BTTestSysHCI.h"
#include "wcnUtility.h"
#include "SimpleAop.h"

#define CHKRESULT_WITH_NOTIFY_WCN_ITEM(statement, Itemname)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
{                                                       \
	_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}	

IMPLEMENT_RUNTIME_CLASS(CBTTestSysHCI)

CBTTestSysHCI::CBTTestSysHCI(void)
	: m_pBTApi(NULL)
{

}

CBTTestSysHCI::~CBTTestSysHCI(void)
{
}

SPRESULT CBTTestSysHCI::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());

	m_pBTApi = new CBTApiHCI(this->m_hDUT);
	if (NULL == m_pBTApi)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTApiAT fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	return SP_OK;
}

BOOL CBTTestSysHCI::LoadXMLConfig(void)
{
	BOOL bEnable = FALSE;
	int nCount = 0;
	double* parrDouble = NULL;
	int* parrInt = NULL;
	SPRESULT eBtConState = SP_OK;

	wstring strMode[MAX_BT_TYPE] = { L"BDR",L"EDR", L"BLE", L"BLE5.0" };
	wstring strAnt[ANT_BT_MAX] = { L"INVALID", L"StandAlone", L"Shared" };

	int nAvgCount = GetConfigValue(L"Param:Common:AvgCount", 3);

	m_VecBTParamBandImp.Init();
	for (int nMode = BDR; nMode < MAX_BT_TYPE; nMode++)
	{
		//HCIBTMeasParamBand stBTParamBand;
		//m_VecBTParamBandImp.eMode = (BT_TYPE)nMode;
		bEnable = (BOOL)GetConfigValue((L"Option:Mode:" + strMode[nMode]).c_str(), FALSE);

		if (!bEnable)
		{
			continue;
		}
		int nGroupCount = GetConfigValue((L"Param:" + strMode[nMode] + L":GroupCount").c_str(), FALSE);
		for (int nGroupIndex = 0; nGroupIndex < nGroupCount; nGroupIndex++)
		{
			vector<int> ChGroup;
			wchar_t    szGroup[20] = { 0 };
			swprintf_s(szGroup, L":Group%d", nGroupIndex + 1);
			wstring strGroup = L"Param:" + strMode[nMode] + szGroup;
			wstring strVal = GetConfigValue((strGroup + L":TCH").c_str(), L"1,39,78");
			parrInt = GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
				return FALSE;
			}

			for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
			{
				RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
				CHKRESULT(eBtConState);
				ChGroup.push_back(parrInt[nChTmp]);
			}
			LPCSTR lpPacket = _W2CA(GetConfigValue((strGroup + L":PacketType").c_str(), L"DH5"));
			BT_HCI_PACKET_TYPE ePacketType = BTGetHciPacket((BT_TYPE)nMode, lpPacket);
			if (HCI_INVALID_PACKET == ePacketType)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid packet type!");
				return FALSE;
			}

			if (BDR == (BT_TYPE)nMode
				&& (HCI_BDR_DH1 != ePacketType
					&& HCI_BDR_DH3 != ePacketType
					&& HCI_BDR_DH5 != ePacketType))
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT] BDR Invalid packet type!");
				return FALSE;
			}

			if (EDR == (BT_TYPE)nMode
				&& (HCI_EDR_2DH1 != ePacketType
					&& HCI_EDR_2DH3 != ePacketType
					&& HCI_EDR_2DH5 != ePacketType
					&& HCI_EDR_3DH1 != ePacketType
					&& HCI_EDR_3DH3 != ePacketType
					&& HCI_EDR_3DH5 != ePacketType))
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT] EDR Invalid packet type!");
				return FALSE;
			}

			if (BLE == (BT_TYPE)nMode
				&& (HCI_RF_PHY_1M != ePacketType
					&& HCI_RF_PHY_2M != ePacketType
					&& HCI_RF_PHY_S2 != ePacketType
					&& HCI_RF_PHY_S8 != ePacketType))
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT] BLE Invalid packet type!");
				return FALSE;
			}

			double dVsgLvl = GetConfigValue((strGroup + L":RXLVL").c_str(), -65.0);
			double dRefLvl = GetConfigValue((strGroup + L":RefLVL").c_str(), 10);
			//LPCWSTR lpPattern = GetConfigValue((strGroup + L":Pattern").c_str(), L"1");
			//int nPattern = _wtoi(lpPattern);

			int nPacketCount = GetConfigValue((strGroup + L":RxCount").c_str(), 100);
			wstring strMask = strGroup + L":TestItem:";
			DWORD dwMask = 0;
			//BDR
			bEnable = (BOOL)GetConfigValue((strMask + L"TXP").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= ((BLE == (BT_TYPE)nMode || BLE_EX == (BT_TYPE)nMode)) ? BLE_POWER : BDR_POWER;
			}
			bEnable = (BOOL)GetConfigValue((strMask + L"TSBD").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= BDR_20BW;
			}

			bEnable = (BOOL)GetConfigValue((strMask + L"TSACP").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= BDR_ACP;
			}
			bEnable = (BOOL)GetConfigValue((strMask + L"MOD").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= ((BLE == (BT_TYPE)nMode || BLE_EX == (BT_TYPE)nMode)) ? BLE_MC : BDR_MC;
			}

			bEnable = (BOOL)GetConfigValue((strMask + L"ICFT").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= BDR_ICFR;
			}

			bEnable = (BOOL)GetConfigValue((strMask + L"CFD").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= BDR_CFD;
			}
			
			if (BLE == (BT_TYPE)nMode || BLE_EX == (BT_TYPE)nMode)
			{
				bEnable = (BOOL)GetConfigValue((strMask + L"PER").c_str(), FALSE);
				if (bEnable)
				{
					dwMask |= BLE_PER;
				}
			}
			else
			{
				bEnable = (BOOL)GetConfigValue((strMask + L"BER").c_str(), FALSE);
				if (bEnable)
				{
					dwMask |= BDR_BER;
				}
			}

			//EDR
			bEnable = (BOOL)GetConfigValue((strMask + L"RTXP").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= EDR_ETP;
			}

			bEnable = (BOOL)GetConfigValue((strMask + L"CFDMODA").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= EDR_DEVM;
			}
			bEnable = (BOOL)GetConfigValue((strMask + L"IBSE").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= ((BLE == (BT_TYPE)nMode || BLE_EX == (BT_TYPE)nMode)) ? BLE_ACP : EDR_ACP;
			}

			bEnable = (BOOL)GetConfigValue((strMask + L"CFOD").c_str(), FALSE);
			if (bEnable)
			{
				dwMask |= BLE_CFD;
			}
			for (int nChTmp = 0; nChTmp < (int)ChGroup.size(); nChTmp++)
			{
				HCIBTMeasParamChan stChanParam;
				stChanParam.eMode = (BT_TYPE)nMode;
				stChanParam.dRefLvl = dRefLvl;
				stChanParam.dVsgLvl = dVsgLvl;
				stChanParam.dwMask = dwMask;
				stChanParam.ePacketType = ePacketType;
				stChanParam.ePath = BT_RFPATH_ENUM(ANT_BT_AUTO);
				stChanParam.nRfSwitchAnt1st = 0;
				stChanParam.nRfSwitchAnt2nd = 0;
				//stChanParam.ePattern = (BT_PATTERN)nPattern;
				stChanParam.nAvgCount = nAvgCount;
				stChanParam.nCh = ChGroup[nChTmp];
				stChanParam.nTotalPackets = nPacketCount;
				BT_PACKET ebtPacket = BTHciPacket2BtPacket(stChanParam.ePacketType);
				if (INVALID_PACKET == ePacketType)
				{
					LogFmtStrA(SPLOGLV_ERROR, "Invalid BT Packet type %d", ePacketType);
					return FALSE;
				}
				stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[(BT_TYPE)nMode][ebtPacket];
				m_VecBTParamBandImp.vecBTFileParamChan.push_back(stChanParam);
			}
		}


		wstring strSpec = L"Param:" + strMode[nMode] + L":Specification";
		if (BDR == (BT_TYPE)nMode)
		{
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"8,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.upp = parrDouble[1];
			//TSBD spec
			strVal = GetConfigValue((strSpec + L":TSBD").c_str(), L"-1000,1000");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid 11b 20dB bandwidth spec, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.upp = parrDouble[1];
			//TSACP spec
			strVal = GetConfigValue((strSpec + L":TSACP").c_str(), L"-40,-40,-40,-20,99,99,99,-20,-40,-40,-40");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR ACP limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			for (int nAcpIndex = 0; nAcpIndex < nCount; nAcpIndex++)
			{
				m_VecBTParamBandImp.stSpec.stBdrSpec.dTsacp[nAcpIndex].low = NOLOWLMT;
				m_VecBTParamBandImp.stSpec.stBdrSpec.dTsacp[nAcpIndex].upp = parrInt[nAcpIndex];
			}
			//mod characteristics spec
			strVal = GetConfigValue((strSpec + L":MOD:f1avg").c_str(), L"140,175");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR mod characteristics limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.upp = parrInt[1];
			//f2max
			m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.upp = 999;
			m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.low = GetConfigValue((strSpec + L":MOD:f2max").c_str(), 115);
			//mod ratio
			m_VecBTParamBandImp.stSpec.stBdrSpec.dModRatio = GetConfigValue((strSpec + L":MOD:ratio").c_str(), 1.0);
			//ICFT Initial carrier frequency tolerance
			strVal = GetConfigValue((strSpec + L":ICFT:ICFT").c_str(), L"-75,75");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR initial carrier frequency tolerance limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.upp = parrInt[1];
			//Carrier Frequency Drift
			strVal = GetConfigValue((strSpec + L":CFD").c_str(), L"-40,40");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR carrier frequency drift limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.upp = parrInt[1];
			//BER spec
			m_VecBTParamBandImp.stSpec.stBdrSpec.dBer = GetConfigValue((strSpec + L":BER").c_str(), 0.1);
		}
		else if (EDR == (BT_TYPE)nMode)
		{
			//TXP spec
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"4,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.upp = parrDouble[1];
			//RTXP spec
			strVal = GetConfigValue((strSpec + L":RTXP").c_str(), L"-4,1");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid Relative transmit power limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.upp = parrDouble[1];
			//CFMODA Carrier frequency drift and modulation acurrancy
			//CFMODA W0
			strVal = GetConfigValue((strSpec + L":CFDMODA:w0").c_str(), L"-10,10");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid carrier frequency drift and modulation accuracy w0 limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.upp = parrInt[1];
			//Wi
			strVal = GetConfigValue((strSpec + L":CFDMODA:wi").c_str(), L"-75,75");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid carrier frequency drift and modulation accuracy wi limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp = parrInt[1];
			//RMSDEVM
			m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_2DHX.low = 0;
			m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_2DHX.upp = GetConfigValue((strSpec + L":CFDMODA:RMSDEVM_2DHX").c_str(), 0.2);
			//PEAKDEVM
			m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_2DHX.low = 0;
			m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_2DHX.upp = GetConfigValue((strSpec + L":CFDMODA:PEAKDEVM_2DHX").c_str(), 0.35);

			//RMSDEVM
			m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_3DHX.low = 0;
			m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_3DHX.upp = GetConfigValue((strSpec + L":CFDMODA:RMSDEVM_3DHX").c_str(), 0.13);
			//PEAKDEVM
			m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_3DHX.low = 0;
			m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_3DHX.upp = GetConfigValue((strSpec + L":CFDMODA:PEAKDEVM_3DHX").c_str(), 0.25);

			//IBSE spec
			strVal = GetConfigValue((strSpec + L":IBSE").c_str(), L"-40,-40,-40,-20,-26,99,-26,-20,-40,-40,-40");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BDR ACP limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
			{
				m_VecBTParamBandImp.stSpec.stEdrSpec.dIbse[nIsbeIndex].low = NOLOWLMT;
				m_VecBTParamBandImp.stSpec.stEdrSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
			}
			//BER spec
			m_VecBTParamBandImp.stSpec.stEdrSpec.dBer = GetConfigValue((strSpec + L":BER").c_str(), 0.1);
		}
		else if (BLE == (BT_TYPE)nMode)
		{
			//TXP AVG spec
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"-20,10");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.upp = parrDouble[1];
			//TXP PEAK spec
			m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.low = 0;
			m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.upp = GetConfigValue((strSpec + L":TXP_PEAK_AVG").c_str(), 3);
			//IBSE spec
			strVal = GetConfigValue((strSpec + L":IBSE").c_str(), L"-30,-30,-30,-20,99,99,99,-20,-30,-30,-30");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE In-Band Spurious Emssions limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
			{
				m_VecBTParamBandImp.stSpec.stBleSpec.dIbse[nIsbeIndex].low = NOLOWLMT;
				m_VecBTParamBandImp.stSpec.stBleSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
			}
			//Modulation Characteristics
			strVal = GetConfigValue((strSpec + L":MOD:f1avg").c_str(), L"225,275");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE modulation Characteristics limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.upp = parrInt[1];
			//F2Max
			m_VecBTParamBandImp.stSpec.stBleSpec.dF2Max = GetConfigValue((strSpec + L":MOD:f2max").c_str(), 185);
			//ratio
			m_VecBTParamBandImp.stSpec.stBleSpec.dRatio = GetConfigValue((strSpec + L":MOD:ratio").c_str(), 0.8);
			//CFOD
			//LPCTSTR lpName = _A2CW(CwcnUtility::BTGetPacketString(BLE_EX, (BT_PACKET)nPacket));
			strVal = GetConfigValue((strSpec + L":CFOD" + L":Fn").c_str(), L"-150,150");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit, Need set 2 values of up and low limits.");
				return FALSE;
			}

			m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[0].upp = parrDouble[1];

			strVal = GetConfigValue((strSpec + L":CFOD" + L"F0Fn").c_str(), L"-99,50");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit");
				return FALSE;
			}

			m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax[0].upp = parrDouble[1];

			LPTSTR lpItem = L":F1F0";
			strVal = GetConfigValue((strSpec + L":CFOD" + lpItem).c_str(), L"-99,23");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE %s limit, Need set 2 values of up and low limits.", _W2CA(lpItem));
				return FALSE;
			}

			m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[0].upp = parrDouble[1];

			lpItem = L":FnFn5";
			strVal = GetConfigValue((strSpec + L":CFOD" + lpItem).c_str(), L"-99,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE %s limit, Need set 2 values of up and low limits.", _W2CA(lpItem));
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[0].upp = parrDouble[1];

			//PER spec
			m_VecBTParamBandImp.stSpec.stBleSpec.dBer = GetConfigValue((strSpec + L":PER").c_str(), 30.8);
		}

		else
		{
			//TXP AVG spec
			wstring strVal = GetConfigValue((strSpec + L":TXP").c_str(), L"-20,10");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.upp = parrDouble[1];
			//TXP PEAK spec
			m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.low = 0;
			m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.upp = GetConfigValue((strSpec + L":TXP_PEAK_AVG").c_str(), 3);
			//IBSE spec
			strVal = GetConfigValue((strSpec + L":IBSE").c_str(), L"-30,-30,-30,-20,99,99,99,-20,-30,-30,-30");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE In-Band Spurious Emssions limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			for (int nIsbeIndex = 0; nIsbeIndex < nCount; nIsbeIndex++)
			{
				m_VecBTParamBandImp.stSpec.stBleExSpec.dIbse[nIsbeIndex].low = NOLOWLMT;
				m_VecBTParamBandImp.stSpec.stBleExSpec.dIbse[nIsbeIndex].upp = parrDouble[nIsbeIndex];
			}
			//Modulation Characteristics
			strVal = GetConfigValue((strSpec + L":MOD:f1avg").c_str(), L"225,275");
			parrInt = GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE modulation Characteristics limit, Need set 2 values of up and low limits.");
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBleExSpec.dF1Avg.low = parrInt[0];
			m_VecBTParamBandImp.stSpec.stBleExSpec.dF1Avg.upp = parrInt[1];
			//F2Max
			m_VecBTParamBandImp.stSpec.stBleExSpec.dF2Max = GetConfigValue((strSpec + L":MOD:f2max").c_str(), 185);
			//ratio
			m_VecBTParamBandImp.stSpec.stBleExSpec.dRatio = GetConfigValue((strSpec + L":MOD:ratio").c_str(), 0.8);
			//CFOD
			//LPCTSTR lpName = _A2CW(CwcnUtility::BTGetPacketString(BLE_EX, (BT_PACKET)nPacket));
			strVal = GetConfigValue((strSpec + L":CFOD" + L":Fn").c_str(), L"-150,150");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit, Need set 2 values of up and low limits.");
				return FALSE;
			}

			m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[0].upp = parrDouble[1];

			strVal = GetConfigValue((strSpec + L":CFOD" + L"F0Fn").c_str(), L"-99,50");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 1)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE Fn Max limit.");
				return FALSE;
			}

			m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[0].upp = parrDouble[1];

			LPTSTR lpItem = L":F1F0";
			strVal = GetConfigValue((strSpec + L":CFOD" + lpItem).c_str(), L"-99,23");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE %s limit, Need set 2 values of up and low limits.", _W2CA(lpItem));
				return FALSE;
			}

			m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[0].upp = parrDouble[1];

			lpItem = L":FnFn5";
			strVal = GetConfigValue((strSpec + L":CFOD" + lpItem).c_str(), L"-99,20");
			parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if (nCount < 2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE %s limit, Need set 2 values of up and low limits.", _W2CA(lpItem));
				return FALSE;
			}
			m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[0].low = parrDouble[0];
			m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[0].upp = parrDouble[1];

			//PER spec
			m_VecBTParamBandImp.stSpec.stBleExSpec.dBer = GetConfigValue((strSpec + L":PER").c_str(), 30.8);
		}
	}
	//	}
	return TRUE;
}

SPRESULT CBTTestSysHCI::__PollAction(void)
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairMode(RepairMode_Bluetooth);

	CHKRESULT(m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)0x5856E2));

	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->SetNetMode(NM_BT), "Instrument::SetNetMode()");
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_FT_NST, NULL), "Instrument::InitDev()");
	vector<HCIBTMeasParamChan>::iterator iterChan;
	for (iterChan = m_VecBTParamBandImp.vecBTFileParamChan.begin(); iterChan != m_VecBTParamBandImp.vecBTFileParamChan.end(); iterChan++)
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		char strInfo[64] = { 0 };
		const int MAX_MEAS_COUNT = m_u32MaxFailRetryCount;
		int nRepeatCount = 0;
		SPRESULT res = SP_OK;
		SPRESULT eRslt = SP_OK;

		//		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_Reset(), "DUT_Reset");

		HCIBTMeasParamChan chanParam = *iterChan;
		if (chanParam.dwMask == 0)
		{
			continue;
		}

		ZeroMemory(m_pBTApi->m_szCmd, 2048);

		//Show Band title
		sprintf_s(strInfo, sizeof(strInfo), "Start %s::CH-%02d:%s"
			, "HCI"
			, chanParam.nCh,
			BTGetPacketHciString(chanParam.eMode, chanParam.ePacketType)
		);

		_UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "", -1, "-");

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_Reset(), "DUT_Reset");

		//Tx performance meas
		DWORD dwMaskTmp = 0;
		switch (chanParam.eMode)
		{
		case BDR:
			dwMaskTmp = chanParam.dwMask & (BDR_POWER | BDR_20BW | BDR_ACP | BDR_MC | BDR_ICFR | BDR_CFD);
			break;
		case EDR:
			dwMaskTmp = chanParam.dwMask & (BDR_POWER | EDR_ETP | EDR_DEVM | EDR_ACP);
			break;
		case BLE_EX:
		case BLE:
			dwMaskTmp = chanParam.dwMask & (BLE_POWER | BLE_ACP | BLE_MC | BLE_CFD);
			break;
		default:
			break;
		}

		BT_PACKET ePacketType = BTHciPacket2BtPacket(chanParam.ePacketType);
		if (INVALID_PACKET == ePacketType)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Invalid BT Packet type %d", ePacketType);
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		
		SPBT_RESULT_T pTestResult;
		if (0 != dwMaskTmp)
		{
			//RF_PHY_S2 not do tx
			pTestResult.Init();
			do
			{
				if (_IsUserStop())
				{
					return SP_E_USER_ABORT;
				}
				CHKRESULT(this->MeasureUplink(chanParam.eMode, &chanParam, &pTestResult));
				//Transmit disable
				//m_pBTApi->DUT_RFOn(BT_TX, false);
				//Judge result
				res = JudgeMeasRst(chanParam.eMode, dwMaskTmp, &pTestResult, ePacketType);
				if (SP_OK == res)
				{
					break;
				}
			} while (++nRepeatCount < MAX_MEAS_COUNT);
			//Show Tx performance
			CHKRESULT(ShowMeasRst(chanParam.eMode, dwMaskTmp, &pTestResult, chanParam.nCh, chanParam.dVsgLvl, ANT_BT_AUTO, ePacketType));
			if (m_bFailStop)
			{
				CHKRESULT(res);
				eRslt = res;
			}
			else
			{
				eRslt |= res;
			}
		}

		//RX performance meas
		dwMaskTmp = 0;
		switch (chanParam.eMode)
		{
		case BDR:
		case EDR:
			dwMaskTmp = chanParam.dwMask & BDR_BER;
			break;
		case BLE_EX:
		case BLE:
			dwMaskTmp = chanParam.dwMask & BLE_PER;
			break;
		default:
			break;
		}

		if (0 != dwMaskTmp)
		{
			nRepeatCount = MAX_MEAS_COUNT;
			do
			{
				if (_IsUserStop())
				{
					return SP_E_USER_ABORT;
				}

				CHKRESULT(this->TestPER(chanParam.eMode, &chanParam, &pTestResult.BER));
				res = JudgeMeasRst(chanParam.eMode, dwMaskTmp, &pTestResult, ePacketType);
				if (SP_OK == res)
				{
					break;
				}
			} while (--nRepeatCount > 0);
			// RX OFF
			//m_pBTApi->DUT_RFOn(BT_RX, FALSE);
			//Show Result
			CHKRESULT(ShowMeasRst(chanParam.eMode, dwMaskTmp, &pTestResult, chanParam.nCh, chanParam.dVsgLvl, ANT_BT_AUTO, ePacketType));
			if (m_bFailStop)
			{
				CHKRESULT(res);
				eRslt = res;
			}
			else
			{
				eRslt |= res;
			}
		}
	}

	return SP_OK;
}

void CBTTestSysHCI::__LeaveAction(void)
{
}

SPRESULT CBTTestSysHCI::__FinalAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);

	if (NULL != m_pBTApi)
	{
		delete m_pBTApi;
		m_pBTApi = NULL;
	}

	return SP_OK;
}

SPRESULT CBTTestSysHCI::ConfigTesterParam(HCIBTMeasParamChan* pParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_stTester.dRefLvl = pParam->dRefLvl;
	m_stTester.dVsgLvl = pParam->dVsgLvl;
	m_stTester.ePacketType = BTHciPacket2BtPacket(pParam->ePacketType);
	if (INVALID_PACKET == m_stTester.ePacketType)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid BT Packet type %d", m_stTester.ePacketType);
		return SP_E_WCN_INVALID_XML_CONFIG;
	}
	m_stTester.ePattern = pParam->ePattern;
	m_stTester.eRfPort = CwcnUtility::BTGetAnt(pParam->ePath);
	m_stTester.nAvgCount = pParam->nAvgCount;
	m_stTester.nCh = pParam->nCh;
	m_stTester.eProto = pParam->eMode;
	//continues rx wave
	m_stTester.nTotalPackets = 0;
	m_stTester.nPacketLen = pParam->nPacketLen;
	return SP_OK;
}


SPRESULT CBTTestSysHCI::TxSpecialPatternMeas(BT_TYPE eMode, HCIBTMeasParamChan* pUplinkMeasParam, DWORD dwMask, SPBT_RESULT_T* pTestResult)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	ZeroMemory(m_pBTApi->m_szCmd, 2048);

	//Set pattern
	if (BDR == eMode || EDR == eMode)
	{
		//set channel
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_TX, (uint8)pUplinkMeasParam->nCh), "DUT_SetCH(BT_TX)");
		//Set packet type
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_TX, pUplinkMeasParam->ePacketType), "DUT_SetPacketType(BT_TX)");
		//Set packet length
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketLength((uint16)pUplinkMeasParam->nPacketLen), "DUT_SetPacketLength");
		//Set PowerCount
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPowerCount(0), "DUT_SetPowerCount");//连续发射
		//Set PowerLevel
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPowerLevel((uint16)pUplinkMeasParam->dRefLvl), "DUT_SetPowerLevel");
		//Set pattern
		m_pBTApi->DUT_SetTxPattern(BT_TX, pUplinkMeasParam->ePattern), "DUT_SetTxPattern";
	}
	else
	{
		//set channel
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetBleCH(BT_TX, (uint8)pUplinkMeasParam->nCh), "DUT_SetBleCH");
		//Set packet length
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetBlePatternLength((uint8)pUplinkMeasParam->nPacketLen), "DUT_SetBlePatternLength");
		//Set pattern
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetBlePattern(BT_TX, pUplinkMeasParam->ePattern), "DUT_SetBlePattern");
	}
	//Setup tester for meas
	SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	//Setup tester for meas
	SPRESULT res = m_pRFTester->InitTest(dwMask, &m_stTester);
	if (SP_OK != res)
	{
		NOTIFY("InitTest(TX)", LEVEL_ITEM, 1, 0, 1);
	}

    //Set pattern
    if (BDR == eMode || EDR == eMode)
    {
        //Transmit enable
        res = m_pBTApi->DUT_TxOn(TRUE);
    }
    else
    {
        //Transmit enable
        res = m_pBTApi->DUT_BleTxOn(TRUE);
	}

	//Fetch meas results
	if (SP_OK == res)
	{
		res = m_pRFTester->FetchResult(dwMask, pTestResult);
		if (SP_OK != res)
		{
			NOTIFY("FetchResult(TX)", LEVEL_ITEM, 1, 0, 1);
		}
	}

	//Transmit disable
	if (BDR == eMode || EDR == eMode)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_TxOn(false), "DUT_TxOn(false)");
	}
	else
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_BleTxOn(false), "DUT_BleTxOn(false)");
	}

	return res;
}

BOOL CBTTestSysHCI::MeasureUplink(BT_TYPE eMode, HCIBTMeasParamChan* pUplinkMeasParam, SPBT_RESULT_T* pTestResult)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (NULL == pUplinkMeasParam)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	switch (eMode)
	{
	case BDR:
	{
		if (!IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_POWER)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_20BW)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ACP)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ICFR)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_CFD)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_MC)
			)
		{
			return SP_OK;
		}
	}
	break;
	case EDR:
	{
		if (!IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_POWER)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, EDR_ETP)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, EDR_DEVM)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, EDR_ACP)
			)
		{
			return SP_OK;
		}
	}
	break;
	case BLE_EX:
	case BLE:
	{
		if (!IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_POWER)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_ACP)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_MC)
			&& !IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_CFD)
			)
		{
			return SP_OK;
		}
	}
	break;
	default:
		break;
	}


	SetRepairBand($REPAIR_BAND_B_EDR);
	SetRepairItem($REPAIR_ITEM_COMMUNICATION);

	CHKRESULT(ConfigTesterParam(pUplinkMeasParam));

	//BT test process
	if (BDR == eMode || EDR == eMode)
	{
		if (BDR == eMode)
		{
			if (IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_POWER)
				|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_20BW)
				|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ACP)
				|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_ICFR)
				)
			{
				DWORD dwTestMask = BDR_POWER | BDR_20BW | BDR_ACP | BDR_ICFR;
				m_stTester.ePattern = BDR_TX_PBRS9;
				pUplinkMeasParam->ePattern = m_stTester.ePattern;
				CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));
			}

			if (IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_CFD)
				|| IS_BIT_SET(pUplinkMeasParam->dwMask, BDR_MC)
				)
			{
				DWORD dwTestMask = BDR_CFD | BDR_MC;
				m_stTester.ePattern = BDR_TX_0xF0;
				pUplinkMeasParam->ePattern = m_stTester.ePattern;
				CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));
				m_stTester.ePattern = BDR_TX_0xAA;
				pUplinkMeasParam->ePattern = m_stTester.ePattern;
				CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));

				if (fabs(pTestResult->Modulation.f1avg) > 0.0001)
				{
					pTestResult->Modulation.f2f1avg_rate = pTestResult->Modulation.f2avg / pTestResult->Modulation.f1avg;
				}
			}
		}
		else
		{
			m_stTester.ePattern = BDR_TX_PBRS9;
			pUplinkMeasParam->ePattern = m_stTester.ePattern;
			CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, pUplinkMeasParam->dwMask, pTestResult));
		}

	}
	else
	{
		if (IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_POWER)
			|| IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_ACP)
			)
		{
			DWORD dwTestMask = BLE_POWER | BLE_ACP;
			if (RF_PHY_S2 == pUplinkMeasParam->ePacketType)
			{
				dwTestMask = BLE_POWER;
			}
			m_stTester.ePattern = (pUplinkMeasParam->ePacketType == RF_PHY_2M ? BLE_TX_0xAA : BLE_TX_PBRS9);
			pUplinkMeasParam->ePattern = m_stTester.ePattern;

			CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));
		}

		if (IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_MC)
			|| IS_BIT_SET(pUplinkMeasParam->dwMask, BLE_CFD)
			)
		{
			DWORD dwTestMask = BLE_MC | BLE_CFD;
			switch (pUplinkMeasParam->ePacketType)
			{
			case RF_PHY_1M:
			case RF_PHY_2M:
				m_stTester.ePattern = BLE_TX_0xF0;
				pUplinkMeasParam->ePattern = m_stTester.ePattern;

				CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));
				m_stTester.ePattern = BLE_TX_0xAA;
				pUplinkMeasParam->ePattern = m_stTester.ePattern;

				CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));
				if (fabs(pTestResult->Modulation.f1avg) > 0.0001)
				{
					pTestResult->Modulation.f2f1avg_rate = pTestResult->Modulation.f2avg / pTestResult->Modulation.f1avg;
				}
				break;
			case RF_PHY_S8:
				m_stTester.ePattern = BLE_TX_0xFF;
				pUplinkMeasParam->ePattern = m_stTester.ePattern;

				CHKRESULT(TxSpecialPatternMeas(eMode, pUplinkMeasParam, dwTestMask, pTestResult));
				break;
				//S2 not do anything
			case RF_PHY_S2:
				break;
			default:
				LogFmtStrA(SPLOGLV_ERROR, "Invalid Packet type %d", pUplinkMeasParam->ePacketType);
				return SP_E_SPAT_INVALID_PARAMETER;
			}

		}

	}

	return SP_OK;
}

BOOL CBTTestSysHCI::TestPER(BT_TYPE eMode, HCIBTMeasParamChan* pDownlinkMeasParam, SPWI_VALUES_T* pPER)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (NULL == pDownlinkMeasParam)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth HCI PER measurement start!");

	ZeroMemory(m_pBTApi->m_szCmd, 2048);
	switch (eMode)
	{
	case BDR:
	case EDR:
	{
		if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BDR_BER))
		{
			return SP_OK;
		}
	}
	break;
	case BLE_EX:
	case BLE:
	{
		if (!IS_BIT_SET(pDownlinkMeasParam->dwMask, BLE_PER))
		{
			return SP_OK;
		}
	}
	break;
	default:
		break;
	}

	SetRepairBand($REPAIR_BAND_B_EDR);

	ConfigTesterParam(pDownlinkMeasParam);

	m_stTester.nTotalPackets = pDownlinkMeasParam->nTotalPackets;
	//BT test process
	if (BDR == eMode || EDR == eMode)
	{
		//set channel
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetCH(BT_RX, (uint8)pDownlinkMeasParam->nCh), "DUT_SetCH(RX)");
		//Set packet type
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPacketType(BT_RX, pDownlinkMeasParam->ePacketType), "DUT_SetPacketType");
		//Set address
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetupAddr("EE:FF:C0:88:00:00"), "DUT_SetupAddr");//00:00:88:C0:FF:EE

		//Set packet length
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetRxPattern(BT_RX, pDownlinkMeasParam->ePattern), "DUT_SetPattern");

		//Initial equipment
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->InitTest(BDR_BER, &m_stTester), "InitTest(BER)");
		//Set Rx Gain
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetRxGain((BYTE)pDownlinkMeasParam->dVsgLvl), "DUT_SetRxGain");
		//Rx ON
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RxOn(true), "DUT_RxOn(true)");

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->SetGen(MM_MODULATION, TRUE), "SetGen(On)");

		Sleep(1000);
		int error_bits = 0;
		int total_bits = 0;
		int error_pkts = 0;
		int total_pkts = 0;
		int rssi = INVALID_NEGATIVE_INTEGER_VALUE;
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_GetRxData(error_bits, total_bits, error_pkts, total_pkts, rssi), "DUT_GetRxData");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RxOn(FALSE), "DUT_RxOn(false)");


		if (total_bits < pDownlinkMeasParam->nTotalPackets)
		{
			pPER->dAvgValue = 100.0;
		}
		else
		{
			pPER->dAvgValue = 1.0 * error_bits / total_bits * 100.0;
		}
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: HCI BER error_bits=%d total_bits=%d Ber=%.2f", error_bits, total_bits, pPER->dAvgValue);

	}
	else
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_Reset(), "DUT_Reset");
		//set channel
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_BleSetCH(BT_RX, (uint8)pDownlinkMeasParam->nCh), "DUT_SetCH(RX)");
		//Initial equipment
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->InitTest(BLE_PER, &m_stTester), "InitTest(BER)");

		int error_Packets = 0;
		int total_Packets = 0;
		uint16 nRevPacket = 0;

		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RxBleOn(TRUE, nRevPacket), "DUT_RxBleOn(On)");
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->SetGen(MM_MODULATION, TRUE), "SetGen(On)");
		Sleep(1000);
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RxBleOn(FALSE, nRevPacket), "DUT_RxBleOn(Off)");

		error_Packets = pDownlinkMeasParam->nTotalPackets - nRevPacket;
		total_Packets = pDownlinkMeasParam->nTotalPackets;
		pPER->dAvgValue = 1.0 * (error_Packets) / total_Packets * 100.0;

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: HCI PER error_Packets=%d total_Packets=%d Per=%.2f", error_Packets, total_Packets, pPER->dAvgValue);

	}
	// Turn off VSG
	CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRFTester->SetGen(MM_MODULATION, FALSE), "SetGen(OFF)");

	LogFmtStrA(SPLOGLV_VERBOSE, "Bluetooth HCI PER measurement end!");

	return SP_OK;
}

BT_HCI_PACKET_TYPE CBTTestSysHCI::BTGetHciPacket(BT_TYPE eType, LPCSTR lpPacket)
{
	for (int i = 0; i < MAX_HCI_BDR_TYPE; i++)
	{
		if (0 == strcmp(BT_HCI_PACKET_TYPE_STRING[eType].stuType[i].pszType, lpPacket))
		{
			return BT_HCI_PACKET_TYPE_STRING[eType].stuType[i].Type;
		}
	}
	return HCI_INVALID_PACKET;
}

LPCSTR CBTTestSysHCI::BTGetPacketHciString(BT_TYPE eType, BT_HCI_PACKET_TYPE ePacket)
{
	if (eType >= MAX_BT_TYPE/* || ePacket > MAX_HCI_BDR_TYPE*/)
	{
		return NULL;
	}

	for (int i = 0; i < MAX_HCI_BDR_TYPE; i++)
	{
		if (ePacket == BT_HCI_PACKET_TYPE_STRING[eType].stuType[i].Type)
		{
			return BT_HCI_PACKET_TYPE_STRING[eType].stuType[i].pszType;
		}
	}
	return NULL;
}


BT_PACKET CBTTestSysHCI::BTHciPacket2BtPacket(BT_HCI_PACKET_TYPE eType)
{
	BT_PACKET ePacket = INVALID_PACKET;
	switch (eType)
	{
	case HCI_BDR_DH1:
		ePacket = BDR_DH1;
		break;
	case HCI_BDR_DH3:
		ePacket = BDR_DH3;
		break;
	case HCI_BDR_DH5:
		ePacket = BDR_DH5;
		break;
	case HCI_EDR_2DH1:
		ePacket = EDR_2DH1;
		break;
	case HCI_EDR_2DH3:
		ePacket = EDR_2DH3;
		break;
	case HCI_EDR_2DH5:
		ePacket = EDR_2DH5;
		break;
	case HCI_EDR_3DH1:
		ePacket = EDR_3DH1;
		break;
	case HCI_EDR_3DH3:
		ePacket = EDR_3DH3;
		break;
	case HCI_EDR_3DH5:
		ePacket = EDR_3DH5;
		break;
	case HCI_RF_PHY_1M:
		ePacket = RF_PHY_1M;
		break;
	case HCI_RF_PHY_2M:
		ePacket = RF_PHY_2M;
		break;
	case HCI_RF_PHY_S2:
		ePacket = RF_PHY_S2;
		break;
	case HCI_RF_PHY_S8:
		ePacket = RF_PHY_S8;
		break;

	default:
		break;
	}

	return ePacket;
}

SPRESULT CBTTestSysHCI::JudgeMeasRst(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (pTestResult == NULL)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "BT: Invalid result!");
	}

	switch (eMode)
	{
	case BDR:
		CHKRESULT(_JudgeBdrRslt(dwItemMask, pTestResult));
		break;
	case EDR:
		CHKRESULT(_JudgeEdrRslt(dwItemMask, pTestResult, ePacketType));
		break;
	case BLE:
		CHKRESULT(_JudgeBleRslt(dwItemMask, pTestResult));
		break;
	case BLE_EX:
		CHKRESULT(_JudgeBleExRslt(dwItemMask, pTestResult, ePacketType));
		break;
	default:
		break;
	}

	return SP_OK;
}

SPRESULT CBTTestSysHCI::_JudgeBdrRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT eCmpRlst = SP_OK;
	SPRESULT eItemRlst = SP_OK;

	if (NULL == pTestResult)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	if (IS_BIT_SET(dwItemMask, BDR_POWER))
	{
		SetRepairItem($REPAIR_ITEM_TXP);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR TXP meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Power.dAvgValue,
			m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.low,
			m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.upp);

		RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.upp
			, eItemRlst, SP_E_WCN_BT_TXP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

	}

	if (IS_BIT_SET(dwItemMask, BDR_20BW))
	{
		SetRepairItem($REPAIR_ITEM_OBW);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_20BW meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->BW20dB.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.upp);

		RSLT_RANGE_CHECK(pTestResult->BW20dB.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.upp
			, eItemRlst, SP_E_WCN_BT_OBW_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BDR_ACP))
	{
		INT nTotalACPFailCount = 0;
		double dMaxAcp = -999.0;

		SetRepairItem($REPAIR_ITEM_ACLR);
		for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
		{
			LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_ACP meas = %.3f, Low = %.3f, high = %.3f",
				pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsacp[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsacp[nAcpIndx].upp);

			if (((nAcpIndx < 4) || (nAcpIndx > 6)) && (pTestResult->ACP.acp[nAcpIndx] > dMaxAcp))
			{
				dMaxAcp = pTestResult->ACP.acp[nAcpIndx];
			}

			RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsacp[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsacp[nAcpIndx].upp
				, eItemRlst, SP_E_WCN_BT_ACP_FAIL);

			if (SP_OK != eItemRlst)
			{
				nTotalACPFailCount++;
			}
		}
		if (nTotalACPFailCount <= 3 && (dMaxAcp < ACP_ABS_LIMITION))
		{
			eItemRlst = SP_OK;
			pTestResult->ACP.indicator = TRUE;
		}
		else
		{
			eItemRlst = SP_E_WCN_BT_ACP_FAIL;
			pTestResult->ACP.indicator = FALSE;
		}
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BDR_ICFR))
	{
		SetRepairItem($REPAIR_ITEM_ICFT);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_ICFT meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->InitalCarrierFreq.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.upp);

		RSLT_RANGE_CHECK(pTestResult->InitalCarrierFreq.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.upp
			, eItemRlst, SP_E_WCN_BT_ICFR_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BDR_CFD))
	{
		SetRepairItem($REPAIR_ITEM_CFD);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_CFD meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.upp);

		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.upp
			, eItemRlst, SP_E_WCN_BT_CFD_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BDR_MC))
	{
		SetRepairItem($REPAIR_ITEM_MC);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_MC:CFD meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Modulation.f1avg
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.upp);

		RSLT_RANGE_CHECK(pTestResult->Modulation.f1avg
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.upp
			, eItemRlst, SP_E_WCN_BT_MC_F1AVG_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_MC:ModFlag meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Modulation.f2max
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.upp);

		RSLT_RANGE_CHECK(pTestResult->Modulation.f2max
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.low
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.upp
			, eItemRlst, SP_E_WCN_BT_MC_F2MAX_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_MC:ModFlag:max meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Modulation.f2f1avg_rate
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModRatio
			, 999.0);

		RSLT_RANGE_CHECK(pTestResult->Modulation.f2f1avg_rate
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModRatio
			, 999.0
			, eItemRlst, SP_E_WCN_BT_MC_F2TOF1_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}
	if (IS_BIT_SET(dwItemMask, BDR_BER))
	{
		SetRepairItem($REPAIR_ITEM_BER);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BER meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->BER.dAvgValue,
			0.0,
			m_VecBTParamBandImp.stSpec.stBdrSpec.dBer);

		RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue,
			0.0,
			m_VecBTParamBandImp.stSpec.stBdrSpec.dBer
			, eItemRlst, SP_E_WCN_BT_PER_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

	}
	return eCmpRlst;
}

SPRESULT CBTTestSysHCI::_JudgeEdrRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (NULL == pTestResult)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	SPRESULT eCmpRlst = SP_OK;
	SPRESULT eItemRlst = SP_OK;

	if (IS_BIT_SET(dwItemMask, BDR_POWER))
	{
		SetRepairItem($REPAIR_ITEM_TXP);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR POWER meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Power.dAvgValue,
			m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.low,
			m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.upp);

		RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue, m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.low, m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.upp, eItemRlst, SP_E_WCN_BT_TXP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, EDR_ETP))
	{
		SetRepairItem($REPAIR_ITEM_RTP);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR POWER meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->RelativePower.dAvgValue
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.upp);

		RSLT_RANGE_CHECK(pTestResult->RelativePower.dAvgValue
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.upp
			, eItemRlst, SP_E_WCN_BT_RTP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, EDR_ACP))
	{
		INT nTotalACPFailCount = 0;
		double dMaxAcp = -999.0;

		SetRepairItem($REPAIR_ITEM_ACLR);
		for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
		{
			LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR ACP meas = %.3f, Low = %.3f, high = %.3f",
				pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stEdrSpec.dIbse[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stEdrSpec.dIbse[nAcpIndx].upp);

			if ((nAcpIndx != 5) && (pTestResult->ACP.acp[nAcpIndx] > dMaxAcp))
			{
				dMaxAcp = pTestResult->ACP.acp[nAcpIndx];
			}

			RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stEdrSpec.dIbse[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stEdrSpec.dIbse[nAcpIndx].upp
				, eItemRlst, SP_E_WCN_BT_ACP_FAIL);

			if (SP_OK != eItemRlst)
			{
				nTotalACPFailCount++;
			}
		}
		if (nTotalACPFailCount <= 3 && (dMaxAcp < ACP_ABS_LIMITION))
		{
			eItemRlst = SP_OK;
			pTestResult->ACP.indicator = TRUE;
		}
		else
		{
			eItemRlst = SP_E_WCN_BT_ACP_FAIL;
			pTestResult->ACP.indicator = FALSE;
		}
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, EDR_DEVM))
	{
		SetRepairItem($REPAIR_ITEM_EVM);

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:W0 meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->DEVM.w0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.upp);

		RSLT_RANGE_CHECK(pTestResult->DEVM.w0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.upp
			, eItemRlst, SP_E_WCN_BT_EVM_W0_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:Wi meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->DEVM.wi
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp
		);
		RSLT_RANGE_CHECK(pTestResult->DEVM.wi
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp
			, eItemRlst, SP_E_WCN_BT_EVM_WI_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:Wi0 meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->DEVM.wi0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp
		);
		RSLT_RANGE_CHECK(pTestResult->DEVM.wi0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp
			, eItemRlst, SP_E_WCN_BT_EVM_WI0_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		double dRmsEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_2DHX.low;
		double dRmsEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_2DHX.upp;
		double dPeakEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_2DHX.low;
		double dPeakEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_2DHX.upp;
		if (ePacketType >= EDR_3DH1)
		{
			dRmsEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_3DHX.low;
			dRmsEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_3DHX.upp;
			dPeakEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_3DHX.low;
			dPeakEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_3DHX.upp;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:avgEVM meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->DEVM.avgEVM
			, dRmsEvmLow
			, dRmsEvmUpp
		);
		RSLT_RANGE_CHECK(pTestResult->DEVM.avgEVM
			, dRmsEvmLow
			, dRmsEvmUpp
			, eItemRlst, SP_E_WCN_BT_EVM_AVG_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:maxEVM meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->DEVM.maxEVM
			, dPeakEvmLow
			, dPeakEvmUpp
		);
		RSLT_RANGE_CHECK(pTestResult->DEVM.maxEVM
			, dPeakEvmLow
			, dPeakEvmUpp
			, eItemRlst, SP_E_WCN_BT_EVM_MAX_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BDR_BER))
	{
		SetRepairItem($REPAIR_ITEM_BER);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR BER meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->BER.dAvgValue,
			0.0,
			m_VecBTParamBandImp.stSpec.stEdrSpec.dBer);

		RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue
			, 0.0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dBer
			, eItemRlst, SP_E_WCN_BT_PER_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

	}

	return eCmpRlst;
}

SPRESULT CBTTestSysHCI::_JudgeBleRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (NULL == pTestResult)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	SPRESULT eCmpRlst = SP_OK;
	SPRESULT eItemRlst = SP_OK;

	if (IS_BIT_SET(dwItemMask, BLE_POWER))
	{
		SetRepairItem($REPAIR_ITEM_TXP);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Power meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.upp
		);
		RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.upp
			, eItemRlst, SP_E_WCN_BT_TXP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Power:Relative meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.upp
		);
		RSLT_RANGE_CHECK(pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.upp
			, eItemRlst, SP_E_WCN_BT_RTP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_ACP))
	{
		SPRESULT eAcpRslt = SP_OK;
		SetRepairItem($REPAIR_ITEM_ACLR);
		for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
		{
			LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE ACP meas = %.3f, Low = %.3f, high = %.3f",
				pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stBleSpec.dIbse[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stBleSpec.dIbse[nAcpIndx].upp
			);
			RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stBleSpec.dIbse[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stBleSpec.dIbse[nAcpIndx].upp
				, eItemRlst, SP_E_WCN_BT_ACP_FAIL);

			eAcpRslt |= eItemRlst;
		}
		pTestResult->ACP.indicator = (eAcpRslt == SP_OK ? TRUE : FALSE);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eAcpRslt;
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_MC))
	{
		SetRepairItem($REPAIR_ITEM_MC);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F1 meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Modulation.f1avg
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.upp
		);
		RSLT_RANGE_CHECK(pTestResult->Modulation.f1avg
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.upp
			, eItemRlst, SP_E_WCN_BT_ACP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F1 meas = %.3f, Low = %.3f",
			pTestResult->Modulation.f2max
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF2Max
		);
		RSLT_RANGE_CHECK(pTestResult->Modulation.f2max
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF2Max
			, 999.0
			, eItemRlst, SP_E_WCN_BT_MC_F2MAX_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F2/F1 Rate meas = %.3f, low limit = %.3f",
			pTestResult->Modulation.f2f1avg_rate
			, m_VecBTParamBandImp.stSpec.stBleSpec.dRatio
		);
		RSLT_RANGE_CHECK(pTestResult->Modulation.f2f1avg_rate
			, m_VecBTParamBandImp.stSpec.stBleSpec.dRatio
			, 999.0
			, eItemRlst, SP_E_WCN_BT_MC_F2TOF1_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_CFD))
	{
		SetRepairItem($REPAIR_ITEM_CFD);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:f1avg meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.fn
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fn
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_FN_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:fn meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.f0fn
			, -99
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f0fn
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_F0FN_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:f1f0 meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.f1f0
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f1f0
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_F1F0_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:fnfn5 meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.fnfn5
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fnfn5
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_FNFN5_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_PER))
	{
		SetRepairItem($REPAIR_ITEM_BER);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->BER.dAvgValue,
			0.0,
			m_VecBTParamBandImp.stSpec.stBleSpec.dBer);

		RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue
			, 0.0
			, m_VecBTParamBandImp.stSpec.stBleSpec.dBer
			, eItemRlst, SP_E_WCN_BT_PER_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

	}

	return eCmpRlst;
}


SPRESULT CBTTestSysHCI::_JudgeBleExRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (NULL == pTestResult)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	SPRESULT eCmpRlst = SP_OK;
	SPRESULT eItemRlst = SP_OK;

	if (IS_BIT_SET(dwItemMask, BLE_POWER))
	{
		SetRepairItem($REPAIR_ITEM_TXP);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Avg Power meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.upp
		);
		RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.upp
			, eItemRlst, SP_E_WCN_BT_TXP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Peak Power: meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.upp
		);
		RSLT_RANGE_CHECK(pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.upp
			, eItemRlst, SP_E_WCN_BT_RTP_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_ACP) && RF_PHY_S2 != ePacketType)
	{
		SetRepairItem($REPAIR_ITEM_ACLR);
		SPRESULT eAcpRslt = SP_OK;
		for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
		{
			LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE ACP meas = %.3f, Low = %.3f, high = %.3f",
				pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dIbse[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dIbse[nAcpIndx].upp
			);
			RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dIbse[nAcpIndx].low
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dIbse[nAcpIndx].upp
				, eItemRlst, SP_E_WCN_BT_ACP_FAIL);
			eAcpRslt |= eItemRlst;
		}
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eAcpRslt;
		}
		pTestResult->ACP.indicator = (eAcpRslt == SP_OK ? TRUE : FALSE);
	}

	if (IS_BIT_SET(dwItemMask, BLE_MC) && RF_PHY_S2 != ePacketType)
	{
		SetRepairItem($REPAIR_ITEM_MC);
		double dF1AvgLowLimit = m_VecBTParamBandImp.stSpec.stBleExSpec.dF1Avg.low;
		double dF1AvgUppLimit = m_VecBTParamBandImp.stSpec.stBleExSpec.dF1Avg.upp;
		if (ePacketType == RF_PHY_2M)
		{
			dF1AvgLowLimit = dF1AvgLowLimit * 2.0;
			dF1AvgUppLimit = dF1AvgUppLimit * 2.0;
		}
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F1 avg meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->Modulation.f1avg
			, dF1AvgLowLimit
			, dF1AvgUppLimit
		);
		RSLT_RANGE_CHECK(pTestResult->Modulation.f1avg
			, dF1AvgLowLimit
			, dF1AvgUppLimit
			, eItemRlst, SP_E_WCN_BT_MC_F1AVG_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}


		if (ePacketType == RF_PHY_2M || ePacketType == RF_PHY_1M)
		{
			LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F2/F1 Rate meas = %.3f, low limit = %.3f",
				pTestResult->Modulation.f2f1avg_rate
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dRatio
			);
			RSLT_RANGE_CHECK(pTestResult->Modulation.f2f1avg_rate
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dRatio
				, 999.0
				, eItemRlst, SP_E_WCN_BT_MC_F2TOF1_FAIL);
			if (SP_OK == eCmpRlst)
			{
				eCmpRlst = eItemRlst;
			}
			double dF2MaxLowLimit = m_VecBTParamBandImp.stSpec.stBleExSpec.dF2Max;
			if (ePacketType == RF_PHY_2M)
			{
				dF2MaxLowLimit = dF2MaxLowLimit * 2.0;
			}
			LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F2 max meas = %.3f, Low = %.3f, high = %.3f",
				pTestResult->Modulation.f2max
				, dF2MaxLowLimit
				, 999.0
			);
			RSLT_RANGE_CHECK(pTestResult->Modulation.f2max
				, dF2MaxLowLimit
				, 999.0
				, eItemRlst, SP_E_WCN_BT_MC_F2MAX_FAIL);
			if (SP_OK == eCmpRlst)
			{
				eCmpRlst = eItemRlst;
			}
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_CFD) && RF_PHY_S2 != ePacketType)
	{
		SetRepairItem($REPAIR_ITEM_CFD);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:Fn Max meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.fn
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[ePacketType].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fn
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[ePacketType].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_FN_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:F0-Fn meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.f0fn
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[ePacketType].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f0fn
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[ePacketType].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_F0FN_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:f3f0 meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.f1f0
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f1f0
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_F1F0_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:|Fn-Fn-3|Max meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->CarrierFreqDrift_BLE.fnfn5
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].upp
		);
		RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fnfn5
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].low
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].upp
			, eItemRlst, SP_E_WCN_BT_CFOD_FNFN5_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_PER))
	{
		SetRepairItem($REPAIR_ITEM_BER);
		LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLEEX PER meas = %.3f, Low = %.3f, high = %.3f",
			pTestResult->BER.dAvgValue,
			0.0,
			m_VecBTParamBandImp.stSpec.stBleExSpec.dBer);

		RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue,
			0.0,
			m_VecBTParamBandImp.stSpec.stBleExSpec.dBer
			, eItemRlst, SP_E_WCN_BT_PER_FAIL);
		if (SP_OK == eCmpRlst)
		{
			eCmpRlst = eItemRlst;
		}

	}

	return eCmpRlst;
}


SPRESULT CBTTestSysHCI::ShowMeasRst(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if (pTestResult == NULL)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "BT::Invalid result!");
	}

	switch (eMode)
	{
	case BDR:
		CHKRESULT(_ShowMeasRstBdr(eMode, dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
		break;
	case EDR:
		CHKRESULT(_ShowMeasRstEdr(eMode, dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
		break;
	case BLE_EX:
		CHKRESULT(_ShowMeasRstBleEx(eMode, dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
		break;
	case BLE:
		CHKRESULT(_ShowMeasRstBle(eMode, dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
		break;
	default:
		break;
	}

	return SP_OK;
}

SPRESULT CBTTestSysHCI::_ShowMeasRstBdr(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	//打印频率
	double dFreq = CwcnUtility::BT_Ch2MHz(BDR, nChan);
	if (IS_BIT_SET(dwItemMask, BDR_POWER))
	{
		_UiSendMsg("Output Power-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.low
			, pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTxp.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_20BW))
	{
		_UiSendMsg("20dB-Bandwidth"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.low
			, pTestResult->BW20dB.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dTsbd.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "KHz"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_ACP))
	{
		_UiSendMsg("Adjacent Channel Power"
			, LEVEL_ITEM | LEVEL_FT
			, TRUE
			, pTestResult->ACP.indicator
			, TRUE
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_ICFR))
	{
		_UiSendMsg("Initial Carrier Frequency Tolerance"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.low
			, pTestResult->InitalCarrierFreq.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dIcft.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_CFD))
	{
		_UiSendMsg("Carrier Freq Drift"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.low
			, pTestResult->CarrierFreqDrift.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dCfd.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_MC))
	{
		_UiSendMsg("f1avg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.low
			, pTestResult->Modulation.f1avg
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF1avg.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("f2max"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.low
			, pTestResult->Modulation.f2max
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModF2max.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("f2avg"
			, LEVEL_ITEM | LEVEL_FT
			, NOLOWLMT
			, pTestResult->Modulation.f2avg
			, NOUPPLMT
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("f2avg / f1avg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dModRatio
			, pTestResult->Modulation.f2f1avg_rate
			, 999.0
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_BER))
	{
		_UiSendMsg("Sensitivity"
			, LEVEL_ITEM | LEVEL_FT
			, 0.0
			, pTestResult->BER.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBdrSpec.dBer
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			, dBSLevel
		);
	}

	return SP_OK;
}

SPRESULT CBTTestSysHCI::_ShowMeasRstEdr(BT_TYPE eMode, DWORD dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	//打印频率
	double dFreq = CwcnUtility::BT_Ch2MHz(EDR, nChan);
	if (IS_BIT_SET(dwItemMask, BDR_POWER))
	{
		_UiSendMsg("Output Power-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.low
			, pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dTxp.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, EDR_ETP))
	{
		_UiSendMsg("Relative Tranmsmit Power"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.low
			, pTestResult->RelativePower.dAvgValue
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dRTxp.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dB"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, EDR_DEVM))
	{
		_UiSendMsg("wi"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low
			, pTestResult->DEVM.wi
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("w0"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.low
			, pTestResult->DEVM.w0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dW0.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("wi+w0"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.low
			, pTestResult->DEVM.wi0
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dWi.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		double dRmsEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_2DHX.low;
		double dRmsEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_2DHX.upp;
		double dPeakEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_2DHX.low;
		double dPeakEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_2DHX.upp;
		if (ePacketType >= EDR_3DH1)
		{
			dRmsEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_3DHX.low;
			dRmsEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dRmsEvm_3DHX.upp;
			dPeakEvmLow = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_3DHX.low;
			dPeakEvmUpp = m_VecBTParamBandImp.stSpec.stEdrSpec.dPeakEvm_3DHX.upp;
		}
		_UiSendMsg("RMS Evm"
			, LEVEL_ITEM | LEVEL_FT
			, dRmsEvmLow
			, pTestResult->DEVM.avgEVM
			, dRmsEvmUpp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("Peak Evm"
			, LEVEL_ITEM | LEVEL_FT
			, dPeakEvmLow
			, pTestResult->DEVM.maxEVM
			, dPeakEvmUpp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, EDR_ACP))
	{
		_UiSendMsg("In-Band Spurious Emissions"
			, LEVEL_ITEM | LEVEL_FT
			, TRUE
			, pTestResult->ACP.indicator
			, TRUE
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BDR_BER))
	{
		_UiSendMsg("Sensitivity"
			, LEVEL_ITEM | LEVEL_FT
			, 0.0
			, pTestResult->BER.dAvgValue
			, m_VecBTParamBandImp.stSpec.stEdrSpec.dBer
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			, dBSLevel
		);
	}

	return SP_OK;
}

SPRESULT CBTTestSysHCI::_ShowMeasRstBle(BT_TYPE eMode, DWORD dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	//打印频率
	double dFreq = CwcnUtility::BT_Ch2MHz(BLE, nChan);
	if (IS_BIT_SET(dwItemMask, BLE_POWER))
	{
		_UiSendMsg("Output Power-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.low
			, pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpAvg.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("Output Power-Ppk-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.low
			, pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dTxpPeak.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BLE_ACP))
	{
		_UiSendMsg("In-Band Spurious Emissons"
			, LEVEL_ITEM | LEVEL_FT
			, TRUE
			, pTestResult->ACP.indicator
			, TRUE
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BLE_MC))
	{
		_UiSendMsg("f1avg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.low
			, pTestResult->Modulation.f1avg
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1Avg.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("f2max"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF2Max
			, pTestResult->Modulation.f2max
			, 999.0
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("f2avg"
			, LEVEL_ITEM | LEVEL_FT
			, NOLOWLMT
			, pTestResult->Modulation.f2avg
			, NOUPPLMT
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("f2avg / f1avg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dRatio
			, pTestResult->Modulation.f2f1avg_rate
			, 999.0
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BLE_CFD))
	{
		_UiSendMsg("Fn"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[RF_PHY_1M].low
			, pTestResult->CarrierFreqDrift_BLE.fn
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("F0-Fn"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].low
			, pTestResult->CarrierFreqDrift_BLE.f0fn
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("F1-F0"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[RF_PHY_1M].low
			, pTestResult->CarrierFreqDrift_BLE.f1f0
			, m_VecBTParamBandImp.stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("Fn-F(n-5)"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low
			, pTestResult->CarrierFreqDrift_BLE.fnfn5
			, m_VecBTParamBandImp.stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
		);
	}

	if (IS_BIT_SET(dwItemMask, BLE_PER))
	{
		_UiSendMsg("Sensitivity - PER"
			, LEVEL_ITEM | LEVEL_FT
			, 0.0
			, pTestResult->BER.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleSpec.dBer
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;BSLevel:%.2f"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, dBSLevel
		);
	}
	return SP_OK;
}

SPRESULT CBTTestSysHCI::_ShowMeasRstBleEx(BT_TYPE eMode, DWORD dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	//打印频率
	double dFreq = CwcnUtility::BT_Ch2MHz(BLE_EX, nChan);
	if (IS_BIT_SET(dwItemMask, BLE_POWER))
	{
		_UiSendMsg("Output Power-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.low
			, pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpAvg.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("Output Power-Ppk-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.low
			, pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dTxpPeak.upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BLE_ACP) && RF_PHY_S2 != ePacketType)
	{
		_UiSendMsg("In-Band Spurious Emissons"
			, LEVEL_ITEM | LEVEL_FT
			, TRUE
			, pTestResult->ACP.indicator
			, TRUE
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "Unit"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
	}

	if (IS_BIT_SET(dwItemMask, BLE_MC))
	{
		double dF1AvgLowLimit = m_VecBTParamBandImp.stSpec.stBleExSpec.dF1Avg.low;
		double dF1AvgUppLimit = m_VecBTParamBandImp.stSpec.stBleExSpec.dF1Avg.upp;
		if (ePacketType == RF_PHY_2M)
		{
			dF1AvgLowLimit = dF1AvgLowLimit * 2.0;
			dF1AvgUppLimit = dF1AvgUppLimit * 2.0;
		}
		if (RF_PHY_1M == ePacketType || RF_PHY_S8 == ePacketType || RF_PHY_2M == ePacketType)
		{
			_UiSendMsg("f1avg"
				, LEVEL_ITEM | LEVEL_FT
				, dF1AvgLowLimit
				, pTestResult->Modulation.f1avg
				, dF1AvgUppLimit
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);
		}

		if (RF_PHY_1M == ePacketType || RF_PHY_2M == ePacketType)
		{
			_UiSendMsg("f2avg"
				, LEVEL_ITEM | LEVEL_FT
				, NOLOWLMT
				, pTestResult->Modulation.f2avg
				, NOUPPLMT
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);

			_UiSendMsg("f2avg / f1avg"
				, LEVEL_ITEM | LEVEL_FT
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dRatio
				, pTestResult->Modulation.f2f1avg_rate
				, 999.0
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);
			double dF2MaxLowLimit = m_VecBTParamBandImp.stSpec.stBleExSpec.dF2Max;
			if (ePacketType == RF_PHY_2M)
			{
				dF2MaxLowLimit = dF2MaxLowLimit * 2.0;
			}

			_UiSendMsg("f2max"
				, LEVEL_ITEM | LEVEL_FT
				, dF2MaxLowLimit
				, pTestResult->Modulation.f2max
				, 999.0
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
			);

		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_CFD) && RF_PHY_S2 != ePacketType)
	{
		_UiSendMsg("Fn"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[ePacketType].low
			, pTestResult->CarrierFreqDrift_BLE.fn
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnMax[ePacketType].upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);

		_UiSendMsg("F0-Fn"
			, LEVEL_ITEM | LEVEL_FT
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[ePacketType].low
			, pTestResult->CarrierFreqDrift_BLE.f0fn
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dF0FnMax[ePacketType].upp
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
		);
		if (ePacketType == RF_PHY_S8)
		{
			_UiSendMsg("|F0-F3|max"
				, LEVEL_ITEM | LEVEL_FT
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].low
				, pTestResult->CarrierFreqDrift_BLE.f1f0
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].upp
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);

			_UiSendMsg("|Fn-F(n-3)|max"
				, LEVEL_ITEM | LEVEL_FT
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].low
				, pTestResult->CarrierFreqDrift_BLE.fnfn5
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].upp
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);
		}
		else
		{
			_UiSendMsg("F1-F0"
				, LEVEL_ITEM | LEVEL_FT
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].low
				, pTestResult->CarrierFreqDrift_BLE.f1f0
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dF1F0[ePacketType].upp
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);

			_UiSendMsg("Fn-F(n-5)"
				, LEVEL_ITEM | LEVEL_FT
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].low
				, pTestResult->CarrierFreqDrift_BLE.fnfn5
				, m_VecBTParamBandImp.stSpec.stBleExSpec.dFnFn5[ePacketType].upp
				, CwcnUtility::BT_BAND_NAME[eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s"
				, dFreq
				, CwcnUtility::BT_ANT_NAME[ePath]
				, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			);
		}
	}

	if (IS_BIT_SET(dwItemMask, BLE_PER))
	{
		_UiSendMsg("Sensitivity - PER"
			, LEVEL_ITEM | LEVEL_FT
			, 0.0
			, pTestResult->BER.dAvgValue
			, m_VecBTParamBandImp.stSpec.stBleExSpec.dBer
			, CwcnUtility::BT_BAND_NAME[eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
			, CwcnUtility::BTGetPacketString(eMode, ePacketType)
			, dBSLevel
		);
	}

	return SP_OK;
}