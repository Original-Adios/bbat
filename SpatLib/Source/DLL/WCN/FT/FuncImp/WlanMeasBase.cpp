#include "StdAfx.h"
#include "WlanMeasBase.h"
#include "WlanApiAT.h"
#include "WlanAntSwitch.h"
#include "SimpleAop.h"

//////////////////////////////////////////////////////////////////////////
//
CWlanMeasBase::CWlanMeasBase(CImpBase *pImpBase)
: m_nTxOnSleep(300)
, m_pWlanParamBand(NULL)
{
	m_pWlanApi = new CWlanApiAT(pImpBase->m_hDUT);
	m_LossHelp.m_pSpatbase = pImpBase;
	m_pRfTesterWlan = pImpBase->m_pRFTester;
	m_rstData.per.dAvgValue = 100.0;
	m_pImpBase = pImpBase;
	bNeedRetry = FALSE;

	LPCWSTR lpProjectName = pImpBase->GetConfigValue(L"Header:ProjectInfo:ProjectName", L"");
	m_Customize.InitRfSwitch(pImpBase->GetAdjParam().nTASK_ID, SP_WIFI, (CSpatBase*)pImpBase, lpProjectName);
}

CWlanMeasBase::~CWlanMeasBase(void)
{
	if (NULL != m_pWlanApi)
	{
		delete m_pWlanApi;
		m_pWlanApi = NULL;
	}
}

SPRESULT CWlanMeasBase::WlanParamSet(WlanMeasParamBand *pWlanParamBand)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (NULL != pWlanParamBand)
	{
		m_pWlanParamBand = pWlanParamBand;
		return SP_OK;
	}
	else
	{
		return SP_E_INVALID_PARAMETER;
	}
}

SPRESULT CWlanMeasBase::WlanLossParamSet(WLAN_LOSS_PARAM *wlanLossParam)
{
	if (NULL != wlanLossParam)
	{
		//m_WlanLossParam = wlanLossParam;
		return SP_OK;
	}
	else
	{
		return SP_E_INVALID_PARAMETER;
	}
}

SPRESULT CWlanMeasBase::WlanMeasPerform()
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	char strInfo[256] = {0};
	const int MAX_MEAS_COUNT = m_pImpBase->m_u32MaxFailRetryCount;
	int nRepeatCount = 0;
	SPRESULT res = SP_E_FAIL;
	SPRESULT eRslt = SP_OK;
	DWORD	dwMask = 0;

	m_MeasParam.Init();
	vector<SPWI_WLAN_PARAM_GROUP>::iterator itWlanGroup;

	// [6]: Setup instrument and measure up-link result
	//CInstrumentLock rfLock(m_pRfTesterWlan);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT(m_pRfTesterWlan->SetNetMode(NM_WIFI));
	CHKRESULT(m_pRfTesterWlan->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));

	for (itWlanGroup = m_pWlanParamBand->vecConfParamGroup.begin(); itWlanGroup != m_pWlanParamBand->vecConfParamGroup.end(); itWlanGroup++)
	{
		//General config
		m_MeasParam.stTesterParamGroupSub = itWlanGroup->stAlgoParamGroupSub;
		dwMask = m_MeasParam.stTesterParamGroupSub.dwMask;
		//General config End
		if (dwMask == NULL)
		{
			continue;
		}
		m_pImpBase->SetRepairBand(CwcnUtility::m_WlanBandInfo[m_pWlanParamBand->eMode]);

		WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;
		m_MeasParam.eMode = m_pWlanParamBand->eMode;

		if (WIFI_CW_SPECTRUM != eProto)
		{
			m_MeasParam.stTesterParamGroupSub.dwMask &= ~WIFI_CW;
		}

		m_MeasParam.stTesterParamGroupSub.dPwrLvl = m_MeasParam.stTesterParamGroupSub.dRefLvl;

		//Channel cycle
		for (uint32 nCh=0;nCh<itWlanGroup->vectChan.size();nCh++)
		{
			if (m_pImpBase->_IsUserStop())
			{
				return SP_E_USER_ABORT;
			}

			nRepeatCount = 0;
			//Meas channel config
			m_MeasParam.stChan = itWlanGroup->vectChan[nCh];

			//Show
			sprintf_s(strInfo, sizeof(strInfo), "%s::PriCh-%02d::CenCh-%02d"
				, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
				, m_MeasParam.stChan.nPriChan
				, m_MeasParam.stChan.nCenChan);

			m_pImpBase->_UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "", -1, "-");

			double dFreq = CwcnUtility::WIFI_Ch2MHz(m_MeasParam.stChan.nCenChan);

			DWORD dwMaskTmpTx = dwMask & (WIFI_TXP | WIFI_FER | WIFI_IQ | WIFI_EVM | WIFI_MASK | WIFI_FLATNESS | WIFI_CW);
			//Tx performance meas
			if (0 != dwMaskTmpTx)
			{
				do
				{
					if (m_pImpBase->_IsUserStop())
					{
						return SP_E_USER_ABORT;
					}
					m_rstData.Init();
					nRepeatCount++;
					res = this->MeasureUplink(&m_MeasParam, &m_rstData);
					//FER KHZ 2 PPM
					m_rstData.fer.dAvgValue = m_rstData.fer.dAvgValue * 1000 / dFreq;

					// TX off while failure
					//m_pWlanApi->DUT_TxOn(false);
					if(SP_OK != res)
					{
						break;
					}
					//Judge result
					res = JudgeMeasRst(dwMaskTmpTx, m_MeasParam.stTesterParamGroupSub.eRate, &m_rstData, m_MeasParam.stTesterParamGroupSub.dVsgLvl);
					if (SP_OK == res)
					{
						break;
					}
				} while (++nRepeatCount < MAX_MEAS_COUNT);

				//Show Tx performance
				CHKRESULT(ShowMeasRst(dwMaskTmpTx
					, m_MeasParam.stTesterParamGroupSub.eRate
					, &m_rstData
					, m_MeasParam.stChan.nCenChan
					, m_MeasParam.stTesterParamGroupSub.dVsgLvl
					, m_MeasParam.stTesterParamGroupSub.nSBWType
					, m_MeasParam.stTesterParamGroupSub.nCBWType
					, m_MeasParam.stTesterParamGroupSub.eAnt));

				if (m_pImpBase->m_bFailStop)
				{
					CHKRESULT(res);
				}
				else
				{
					eRslt |= res;
				}
			}

			//RX performance meas
			DWORD dwMaskTmpRx = dwMask & (WIFI_PER | WIFI_RSSI);
			if (0 != dwMaskTmpRx)
			{
				nRepeatCount = 0;
				do
				{
					if (m_pImpBase->_IsUserStop())
					{
						return SP_E_USER_ABORT;
					}
					m_rstData.per.dAvgValue = 100.0;
					m_rstData.Rssi.dAvgValue = INVALID_NEGATIVE_DOUBLE_VALUE;
					res = this->TestPER(&m_MeasParam, &m_rstData);
					if(SP_OK != res)
					{
						break;
					}
					//Judge result
					res = JudgeMeasRst(dwMaskTmpRx
						, m_MeasParam.stTesterParamGroupSub.eRate
						, &m_rstData
						, m_MeasParam.stTesterParamGroupSub.dVsgLvl
						, m_MeasParam.stTesterParamGroupSub.eAnt);
					if (SP_OK == res)
					{
						break;
					}
				} while (++nRepeatCount < MAX_MEAS_COUNT);
				/*CHKRESULT*/(m_pWlanApi->DUT_RxOn(false));
				//Show Result
				res |= ShowMeasRst(dwMaskTmpRx
					, m_MeasParam.stTesterParamGroupSub.eRate
					, &m_rstData
					, m_MeasParam.stChan.nCenChan
					, m_MeasParam.stTesterParamGroupSub.dVsgLvl
					, m_MeasParam.stTesterParamGroupSub.nSBWType
					, m_MeasParam.stTesterParamGroupSub.nCBWType
					, m_MeasParam.stTesterParamGroupSub.eAnt);
				if (m_pImpBase->m_bFailStop)
				{
					CHKRESULT(res);
				}
				else
				{
					eRslt |= res;
				}

			}
		}
	}
	return eRslt;
}

SPRESULT CWlanMeasBase::WlanMeasLossPerform(WLAN_LOSS_PARAM wlanLossParam)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	m_LossHelp.m_NetMode = NM_WIFI;
	char strInfo[256] = {0};
	const int MAX_MEAS_COUNT = m_pImpBase->m_u32MaxFailRetryCount;
	int nRepeatCount = 0;
	SPRESULT eRslt = SP_OK;
	DWORD	dwMask = 0;

	m_MeasParam.Init();
	vector<SPWI_WLAN_PARAM_GROUP>::iterator itWlanGroup;

	// [6]: Setup instrument and measure up-link result
	//CInstrumentLock rfLock(m_pRfTesterWlan);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT(m_pRfTesterWlan->SetNetMode(NM_WIFI));
	CHKRESULT(m_pRfTesterWlan->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));

	for (itWlanGroup = m_pWlanParamBand->vecConfParamGroup.begin(); itWlanGroup != m_pWlanParamBand->vecConfParamGroup.end(); itWlanGroup++)
	{
		//General config
		m_MeasParam.stTesterParamGroupSub = itWlanGroup->stAlgoParamGroupSub;
		dwMask = m_MeasParam.stTesterParamGroupSub.dwMask;
		//General config End
		if (dwMask == NULL)
		{
			continue;
		}
		m_pImpBase->SetRepairBand(CwcnUtility::m_WlanBandInfo[m_pWlanParamBand->eMode]);

		WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;
		m_MeasParam.eMode = m_pWlanParamBand->eMode;

		if (WIFI_CW_SPECTRUM != eProto)
		{
			m_MeasParam.stTesterParamGroupSub.dwMask &= ~WIFI_CW;
		}

		//Channel cycle
		for (uint32 nCh=0;nCh<itWlanGroup->vectChan.size();nCh++)
		{
			if (m_pImpBase->_IsUserStop())
			{
				return SP_E_USER_ABORT;
			}

			nRepeatCount = 0;
			//Meas channel config
			m_MeasParam.stChan = itWlanGroup->vectChan[nCh];

			//set band
			WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
			if (m_MeasParam.stChan.nCenChan <= 14)
			{
				eBand = WLAN_BAND_24G;
			}
			else
			{
				eBand = WLAN_BAND_50G;
			}


			//Show
			sprintf_s(strInfo, sizeof(strInfo), "%s::PriCh-%02d::CenCh-%02d"
				, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, m_MeasParam.stChan.nPriChan
				, m_MeasParam.stChan.nCenChan);

			int nRealCh = CwcnUtility::WlanGetTxCh((WIFI_BANDWIDTH_ENUM)m_MeasParam.stTesterParamGroupSub.nCBWType, (WIFI_BANDWIDTH_ENUM)m_MeasParam.stTesterParamGroupSub.nSBWType, m_MeasParam.stChan.nCenChan, m_MeasParam.stChan.nPriChan);
			double dFreq = CwcnUtility::WIFI_Ch2MHz(nRealCh);

			m_pImpBase->_UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "", -1, "-");
			DWORD dwMaskTmp = WIFI_TXP ;

			m_rstData.Init();
			double dPreLoss = WLAN_BAND_24G == eBand ? wlanLossParam.m_dPreLoss : wlanLossParam.m_dPreLoss_5G;
			m_LossHelp.SetLoss(eBand, nRealCh, dFreq, RF_IO_TX, dPreLoss, (RF_ANT_E)(m_MeasParam.stTesterParamGroupSub.eAnt - 1)/*RF_ANT_1st*/);
			int nRetryCount = 0;
			do
			{
				if(nRetryCount++ > 20)
				{
					//bNeedRetry = FALSE;
					m_pImpBase->LogFmtStrA(SPLOGLV_ERROR, "Wlan Over Loss Adjust");
					return -1;
				}
				if (m_pImpBase->_IsUserStop())
				{
					return SP_E_USER_ABORT;
				}

				CHKRESULT(MeasureUplink(&m_MeasParam, &m_rstData));
				//FER KHZ 2 PPM
				m_rstData.fer.dAvgValue = m_rstData.fer.dAvgValue * 1000 / dFreq;

				double m_StandardValue = 12;

				m_StandardValue = (m_pWlanParamBand->stSpec.dTxp.low + m_pWlanParamBand->stSpec.dTxp.upp)/2;

				double dTolernece = WLAN_BAND_24G == eBand ? wlanLossParam.m_Tolernece : wlanLossParam.m_Tolernece_5G;
				CHKRESULT(m_LossHelp.AdjustLoss(m_StandardValue, m_rstData.txp.dAvgValue, RF_IO_TX, bNeedRetry, dTolernece));
			} while (bNeedRetry);
			CHKRESULT(ShowMeasRst(dwMaskTmp
					, m_MeasParam.stTesterParamGroupSub.eRate
					, &m_rstData
					, m_MeasParam.stChan.nCenChan
					, m_MeasParam.stTesterParamGroupSub.dVsgLvl
					, m_MeasParam.stTesterParamGroupSub.nSBWType
					, m_MeasParam.stTesterParamGroupSub.nCBWType
					, m_MeasParam.stTesterParamGroupSub.eAnt));
			m_pImpBase->_UiSendMsg("Wlan Loss"
				, LEVEL_ITEM|LEVEL_FT,
				wlanLossParam.m_Loss_Low,
				m_LossHelp.m_Loss,
				wlanLossParam.m_Loss_Up,
				 CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
				, m_MeasParam.stChan.nCenChan
				, "dBm"
				, "%s,Freq = %.2f"
				, CwcnUtility::WLAN_ANT_NAME[m_MeasParam.stTesterParamGroupSub.eAnt]
				,dFreq
		);
			if(!IN_RANGE(wlanLossParam.m_Loss_Low,
				m_LossHelp.m_Loss,
				wlanLossParam.m_Loss_Up))
			{
				m_pImpBase->LogFmtStrA(SPLOGLV_ERROR, "Wlan Loss Over");
				return - 1;
			}
			m_LossHelp.UpdateLoss(LOSS_MODE_WLAN);
		}
	}
	return eRslt;
}

SPRESULT CWlanMeasBase::ShowMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, int nChan, double dBSLevel, int nSbwType, int nCbwType, ANTENNA_ENUM eAnt)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pTestResult == NULL)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "Wlan::Invalid result!");
	}
	BOOL bFailStop = m_pImpBase->m_bFailStop;

	double dFreq = CwcnUtility::WIFI_Ch2MHz(nChan);
	LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[nCbwType];
	LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[nSbwType];
	LPCSTR lpRate = CwcnUtility::WlanGetRateString( eRate);
	if (IS_BIT_SET(dwItemMask, WIFI_TXP))
	{
		double dTxpLow = 0.0;
		double dTxpUpp = 0.0;
		if (ANT_PRIMARY == eAnt)
		{
			dTxpLow = m_pWlanParamBand->stSpec.dTxp.low;
			dTxpUpp = m_pWlanParamBand->stSpec.dTxp.upp;
		}
		else
		{
			dTxpLow = m_pWlanParamBand->stSpec.dTXPant2nd.low;
			dTxpUpp = m_pWlanParamBand->stSpec.dTXPant2nd.upp;
		}

		m_pImpBase->_UiSendMsg("Transmit Power"
			, LEVEL_ITEM|LEVEL_FT
			, dTxpLow
			, pTestResult->txp.dAvgValue
			, dTxpUpp
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			, lpRate
			, strCBW
			, strSBW);


		if (bFailStop && (pTestResult->txp.dAvgValue < dTxpLow
			|| pTestResult->txp.dAvgValue > dTxpUpp))
		{
			return SP_E_WCN_WLAN_TXP_FAIL;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_CW))
	{
		m_pImpBase->_UiSendMsg("Transmit CW Power"
			, LEVEL_ITEM|LEVEL_FT
			, m_pWlanParamBand->stSpec.dTxp.low
			, pTestResult->CWPwr.dAvgValue
			, m_pWlanParamBand->stSpec.dTxp.upp
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			);
	}

	if (IS_BIT_SET(dwItemMask, WIFI_FER))
	{
		m_pImpBase->_UiSendMsg("Freq Error"
			, LEVEL_ITEM|LEVEL_FT
			, m_pWlanParamBand->stSpec.dFer.low
			, pTestResult->fer.dAvgValue
			, m_pWlanParamBand->stSpec.dFer.upp
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "PPM"
			, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			, lpRate
			, strCBW
			, strSBW);
	}

	if (IS_BIT_SET(dwItemMask, WIFI_IQ))
	{
		m_pImpBase->_UiSendMsg("IQ Offset Error"
			, LEVEL_ITEM | LEVEL_FT
			, m_pWlanParamBand->stSpec.dIQ.low
			, pTestResult->iqOffset.dAvgValue
			, m_pWlanParamBand->stSpec.dIQ.upp
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "dB"
			, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			, lpRate
			, strCBW
			, strSBW);
	}

	if (IS_BIT_SET(dwItemMask, WIFI_EVM))
	{
		m_pImpBase->_UiSendMsg("EVM"
			, LEVEL_ITEM|LEVEL_FT
			, m_pWlanParamBand->stSpec.dEvm[eRate].low
			, pTestResult->evm.dAvgValue
			, m_pWlanParamBand->stSpec.dEvm[eRate].upp
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "dB"
			, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			, lpRate
			, strCBW
			, strSBW);
	}

	if (IS_BIT_SET(dwItemMask, WIFI_MASK))
	{
		LPCSTR lpcMask11b[] = { "AB", "CD", "DC", "BA", "NA", "NA", "NA", "NA" };
		LPCSTR lpcMask[] = { "AB", "BC", "CD", "DE", "ED", "DC", "CB", "BA" };
		char szItemName[64] = {0};
		int nCount = 8;
		if(eRate <= CCK_11)
		{
			nCount = 4;
		}
		for(int i=0; i<nCount; i++)
		{
			if (eRate <= CCK_11)
			{
				sprintf_s(szItemName, "Spectrum_Mask(%s)_Margin", lpcMask11b[i]);
			}
			else
			{
				sprintf_s(szItemName, "Spectrum_Mask(%s)_Margin", lpcMask[i]);
			}

			m_pImpBase->_UiSendMsg(szItemName
				, LEVEL_ITEM|LEVEL_FT
				, NOLOWLMT
				, pTestResult->mask.dMagin[i]
				, m_pWlanParamBand->stSpec.dSpecMaskMargin[i]
				, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
				, dFreq
				, CwcnUtility::WLAN_ANT_NAME[eAnt]
				, lpRate
				, strCBW
				, strSBW);
		}
	}
	if(eRate > CCK_11)
	{
		if (IS_BIT_SET(dwItemMask, WIFI_FLATNESS))
		{
			m_pImpBase->_UiSendMsg("FLATNESS"
				, LEVEL_ITEM|LEVEL_FT
				, TRUE
				, pTestResult->flatness.bPass
				, TRUE
				, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
				, nChan
				, "-"
				, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s"
				, dFreq
				, CwcnUtility::WLAN_ANT_NAME[eAnt]
				, lpRate
				, strCBW
				, strSBW);
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_PER))
	{
		m_pImpBase->_UiSendMsg("SensitivityTest"
			, LEVEL_ITEM|LEVEL_FT
			, 0
			, pTestResult->per.dAvgValue
			, m_pWlanParamBand->stSpec.dPer
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s;BSLevel:%.2f"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			, lpRate
			, strCBW
			, strSBW
			, dBSLevel);

		if (bFailStop && pTestResult->per.dAvgValue > m_pWlanParamBand->stSpec.dPer)
		{
			return SP_E_WCN_WLAN_PER_FAIL;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_RSSI))
	{
		m_pImpBase->_UiSendMsg("RSSITest"
			, LEVEL_ITEM | LEVEL_FT
			, dBSLevel + m_pWlanParamBand->stSpec.dRssi.low
			, pTestResult->Rssi.dAvgValue
			, dBSLevel + m_pWlanParamBand->stSpec.dRssi.upp
			, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
			, nChan
			, "-"
			, "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s;BSLevel:%.2f"
			, dFreq
			, CwcnUtility::WLAN_ANT_NAME[eAnt]
			, lpRate
			, strCBW
			, strSBW
			, dBSLevel);

		if (bFailStop
			&& (pTestResult->Rssi.dAvgValue < dBSLevel + m_pWlanParamBand->stSpec.dRssi.low
				|| pTestResult->Rssi.dAvgValue > dBSLevel + m_pWlanParamBand->stSpec.dRssi.upp)
			)
		{
			return SP_E_WCN_WLAN_RSSI_FAIL;
		}
	}

	return SP_OK;
}

SPRESULT CWlanMeasBase::JudgeMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, double dBSLevel, ANTENNA_ENUM eAnt/* = ANT_PRIMARY*/)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pTestResult == NULL)
	{
		return SP_E_INVALID_PARAMETER;
	}

	SPRESULT eCmpRlst = SP_OK;
	SPRESULT eItemRlst = SP_OK;

	if (IS_BIT_SET(dwItemMask, WIFI_TXP))
	{
		double dTxpLow = 0.0;
		double dTxpUpp = 0.0;
		if (ANT_PRIMARY == eAnt)
		{
			dTxpLow = m_pWlanParamBand->stSpec.dTxp.low;
			dTxpUpp = m_pWlanParamBand->stSpec.dTxp.upp;
		}
		else
		{
			dTxpLow = m_pWlanParamBand->stSpec.dTXPant2nd.low;
			dTxpUpp = m_pWlanParamBand->stSpec.dTXPant2nd.upp;
		}
		RSLT_RANGE_CHECK(pTestResult->txp.dAvgValue, dTxpLow, dTxpUpp, eItemRlst, SP_E_WCN_WLAN_TXP_FAIL);
		if(SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_CW))
	{
		RSLT_RANGE_CHECK(pTestResult->CWPwr.dAvgValue, m_pWlanParamBand->stSpec.dTxp.low, m_pWlanParamBand->stSpec.dTxp.upp, eItemRlst, SP_E_WCN_WLAN_TXP_FAIL);
		if(SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_FER))
	{
		RSLT_RANGE_CHECK(pTestResult->fer.dAvgValue, m_pWlanParamBand->stSpec.dFer.low, m_pWlanParamBand->stSpec.dFer.upp, eItemRlst, SP_E_WCN_WLAN_FER_FAIL);
		if(SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_FER);
			eCmpRlst = eItemRlst;
		}
	}

	//IQ Offset
	if (IS_BIT_SET(dwItemMask, WIFI_IQ))
	{
		RSLT_RANGE_CHECK(pTestResult->iqOffset.dAvgValue, m_pWlanParamBand->stSpec.dIQ.low, m_pWlanParamBand->stSpec.dIQ.upp, eItemRlst, SP_E_WCN_WLAN_IQ_FAIL);
		if (SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_FER);
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_EVM))
	{
		RSLT_RANGE_CHECK(pTestResult->evm.dAvgValue, m_pWlanParamBand->stSpec.dEvm[eRate].low, m_pWlanParamBand->stSpec.dEvm[eRate].upp, eItemRlst, SP_E_WCN_WLAN_EVM_FAIL);
		if(SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_EVM);
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_MASK))
	{
		int nCount = 8;
		if(eRate <= CCK_11)
		{
			nCount = 4;
		}
		for(int i=0; i<nCount; i++)
		{
			if(pTestResult->mask.dMagin[i] > m_pWlanParamBand->stSpec.dSpecMaskMargin[i])
			{
				if(SP_OK == eCmpRlst)
				{
					m_pImpBase->SetRepairItem($REPAIR_ITEM_ORFS);
					eCmpRlst = SP_E_WCN_WLAN_SEM_FAIL;
				}
				break;
			}
		}
	}
	if(eRate > CCK_11)
	{
		if (IS_BIT_SET(dwItemMask, WIFI_FLATNESS) &&
			!pTestResult->flatness.bPass)
		{
			if(SP_OK == eCmpRlst)
			{
				m_pImpBase->SetRepairItem($REPAIR_ITEM_FLATNESS);
				eCmpRlst = SP_E_WCN_WLAN_FLATNESS_FAIL;
			}
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_PER))
	{
		RSLT_RANGE_CHECK(pTestResult->per.dAvgValue, 0, m_pWlanParamBand->stSpec.dPer, eItemRlst, SP_E_WCN_WLAN_PER_FAIL);
		if(SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_BLER);
			eCmpRlst = eItemRlst;
		}
	}

	if (IS_BIT_SET(dwItemMask, WIFI_RSSI))
	{
		RSLT_RANGE_CHECK(pTestResult->Rssi.dAvgValue, dBSLevel + m_pWlanParamBand->stSpec.dRssi.low, dBSLevel + m_pWlanParamBand->stSpec.dRssi.upp, eItemRlst, SP_E_WCN_WLAN_PER_FAIL);
		if (SP_OK == eCmpRlst)
		{
			m_pImpBase->SetRepairItem($REPAIR_ITEM_BLER);
			eCmpRlst = eItemRlst;
		}
	}
	return eCmpRlst;
}

void CWlanMeasBase::ConfigTesterParam( SPWI_WLAN_PARAM_MEAS_GROUP *pMeasParam )
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	m_stTester.nCenChan = pMeasParam->stChan.nCenChan;
	m_stTester.nPriChan = pMeasParam->stChan.nPriChan;
	//set band
	WLAN_BAND_ENUM eBand = WLAN_BAND_24G;
	if (m_MeasParam.stChan.nCenChan <= 14)
	{
		eBand = WLAN_BAND_24G;
	}
	else
	{
		eBand = WLAN_BAND_50G;
	}
	CwcnUtility::WlanGetAntByBand(pMeasParam->stTesterParamGroupSub.eAnt, &m_stTester.ePort[0], eBand);

	m_stTester.eProto = m_pWlanParamBand->eMode;
	m_stTester.nAvgCount = pMeasParam->stTesterParamGroupSub.nAvgCount;
	m_stTester.nTotalPackets = pMeasParam->stTesterParamGroupSub.nTotalPackets;
	m_stTester.nCBWType = pMeasParam->stTesterParamGroupSub.nCBWType;
	m_stTester.nSBWType = pMeasParam->stTesterParamGroupSub.nSBWType;
	m_stTester.eRate = pMeasParam->stTesterParamGroupSub.eRate;
	m_stTester.dRefLvl = pMeasParam->stTesterParamGroupSub.dRefLvl;
	m_stTester.dVsgLvl = pMeasParam->stTesterParamGroupSub.dVsgLvl;
}


