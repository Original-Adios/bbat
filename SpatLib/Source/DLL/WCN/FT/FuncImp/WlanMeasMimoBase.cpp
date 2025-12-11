#include "StdAfx.h"
#include "WlanMeasMimoBase.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeasBaseMimo::CWlanMeasBaseMimo(CImpBase *pImpBase) : CWlanMeasBase(pImpBase)
{
}

CWlanMeasBaseMimo::~CWlanMeasBaseMimo(void)
{

}

SPRESULT CWlanMeasBaseMimo::WlanMeasPerform()
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    char strInfo[256] = {0};
    const int MAX_MEAS_COUNT = m_pImpBase->m_u32MaxFailRetryCount;
    int nRepeatCount = 0;
    SPRESULT res = SP_E_FAIL;
	SPRESULT eRslt = SP_OK;
	DWORD	dwMask = 0;
	BOOL bShareAnt = FALSE;

	m_MeasParam.Init();
	m_MeasParam.eMode = m_pWlanParamBand->eMode;
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
        if (m_MeasParam.stTesterParamGroupSub.dwMask == NULL)
        {
			continue;
        }
        m_pImpBase->SetRepairBand(CwcnUtility::m_WlanBandInfo[m_pWlanParamBand->eMode]);

		//eband
		WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;
		m_MeasParam.eMode = m_pWlanParamBand->eMode;

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

			DWORD dwMaskTmp = dwMask & (WIFI_TXP | WIFI_FER | WIFI_EVM | WIFI_MASK | WIFI_FLATNESS | WIFI_CW);
			//Tx performance meas
			if (0 != dwMaskTmp)
			{
				do 
				{
					m_TxRstMimo[0].Init();
					m_TxRstMimo[1].Init();
					nRepeatCount++;
					res = this->MeasureUplink(&m_MeasParam, &m_TxRstMimo[0]);
					//FER KHZ 2 PPM
					m_rstData.fer.dAvgValue = m_rstData.fer.dAvgValue * 1000 / dFreq;

					// TX off while failure
					m_pWlanApi->DUT_TxOn(false);
					
					if(SP_OK != res)
					{
						continue;
					}
					//Judge result
					res = JudgeMeasRst(dwMaskTmp, m_MeasParam.stTesterParamGroupSub.eRate, &m_TxRstMimo[0], m_MeasParam.stTesterParamGroupSub.dVsgLvl);
					if (ANT_MIMO == m_MeasParam.stTesterParamGroupSub.eAnt)
					{
						res |= JudgeMeasRst(dwMaskTmp, m_MeasParam.stTesterParamGroupSub.eRate, &m_TxRstMimo[1], m_MeasParam.stTesterParamGroupSub.dVsgLvl);
					}
					if (SP_OK == res)
					{
						break;
					}  
				} while (nRepeatCount < MAX_MEAS_COUNT);
				//Show Tx performance
				CHKRESULT(ShowMeasRstMimo(dwMaskTmp
					, m_MeasParam.stTesterParamGroupSub.eRate
					, &m_TxRstMimo[0]
					, m_MeasParam.stChan.nPriChan
					, TRUE
					, m_MeasParam.stTesterParamGroupSub.nSBWType
					, m_MeasParam.stTesterParamGroupSub.nCBWType));

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
			if (IS_BIT_SET(dwMask, WIFI_PER))
			{
				nRepeatCount = 0;
				do 
				{
					if (m_pImpBase->_IsUserStop())
					{ 
						return SP_E_USER_ABORT;
					}
					res = this->TestPER(&m_MeasParam, &m_TxRstMimo[0]); 
					if(SP_OK != res)
					{
						break;
					}
					//Judge result
					res = JudgeMeasRst(WIFI_PER, m_MeasParam.stTesterParamGroupSub.eRate, &m_TxRstMimo[0], m_MeasParam.stTesterParamGroupSub.dVsgLvl);
					if (SP_OK == res)
					{
						break;
					} 
				} while (++nRepeatCount < MAX_MEAS_COUNT);
				/*CHKRESULT*/(m_pWlanApi->DUT_RxOn(false));
				//Show Result
				res |= ShowMeasRst(WIFI_PER
					, m_MeasParam.stTesterParamGroupSub.eRate
					, &m_TxRstMimo[0]
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
 
SPRESULT CWlanMeasBaseMimo::ShowMeasRstMimo(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, int nChan, BOOL bTxShow, int nSbwType, int nCbwType)
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pTestResult == NULL)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "Wlan::Invalid result!");
	}


	LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[nCbwType];
	LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[nSbwType];
	LPCSTR lpRate = CwcnUtility::WlanGetRateString( eRate);
	double dFreq = CwcnUtility::WIFI_Ch2MHz(nChan);

	for ( int nAnt = ANT_PRIMARY; nAnt < ANT_MIMO; nAnt++)
	{
		SPWI_RESULT_T *pTestResultMimo = pTestResult + nAnt - 1;
		if (bTxShow)
		{
			if (IS_BIT_SET(dwItemMask, WIFI_TXP))
			{
				m_pImpBase->_UiSendMsg("Transmit Power"
					, LEVEL_ITEM|LEVEL_FT
					, m_pWlanParamBand->stSpec.dTxp.low
					, pTestResultMimo->txp.dAvgValue
					, m_pWlanParamBand->stSpec.dTxp.upp
					, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
					, nChan
					, "dBm"
					, "Frequency:%0.2f;%s::CBW:%s::SBW:%s::%s"
					, dFreq
					, lpRate
					, strCBW
					, strSBW
					, CwcnUtility::WLAN_ANT_NAME[nAnt]);
			}

			if (IS_BIT_SET(dwItemMask, WIFI_FER))
			{
				m_pImpBase->_UiSendMsg("Freq Error"
					, LEVEL_ITEM|LEVEL_FT
					, m_pWlanParamBand->stSpec.dFer.low
					, pTestResultMimo->fer.dAvgValue
					, m_pWlanParamBand->stSpec.dFer.upp
					, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
					, nChan
					, "PPM"
					, "Frequency:%0.2f;%s::CBW:%s::SBW:%s::%s"
					, dFreq
					, lpRate
					, strCBW
					, strSBW
					, CwcnUtility::WLAN_ANT_NAME[nAnt]);
			}

			if (IS_BIT_SET(dwItemMask, WIFI_EVM))
			{
				m_pImpBase->_UiSendMsg("EVM"
					, LEVEL_ITEM|LEVEL_FT
					, m_pWlanParamBand->stSpec.dEvm[eRate].low
					, pTestResultMimo->evm.dAvgValue
					, m_pWlanParamBand->stSpec.dEvm[eRate].upp
					, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
					, nChan
					, "dB"
					, "Frequency:%0.2f;%s::CBW:%s::SBW:%s::%s"
					, dFreq
					, lpRate
					, strCBW
					, strSBW
					, CwcnUtility::WLAN_ANT_NAME[nAnt]);
			}

			if (IS_BIT_SET(dwItemMask, WIFI_MASK))
			{
				m_pImpBase->_UiSendMsg("Spectrum_Mask"
					, LEVEL_ITEM|LEVEL_FT
					, TRUE
					, pTestResultMimo->mask.bPass
					, TRUE
					, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
					, nChan
					, "-"
					, "Frequency:%0.2f;%s::CBW:%s::SBW:%s::%s"
					, dFreq
					, lpRate
					, strCBW
					, strSBW
					, CwcnUtility::WLAN_ANT_NAME[nAnt]);
			}
		}
		else
		{
			if (IS_BIT_SET(dwItemMask, WIFI_PER))
			{
				m_pImpBase->_UiSendMsg("SensitivityTest"
					, LEVEL_ITEM|LEVEL_FT
					, 0
					, pTestResultMimo->per.dAvgValue
					, m_pWlanParamBand->stSpec.dPer
					, CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
					, nChan
					, "-"
					, "Frequency:%0.2f;%s::CBW:%s::SBW:%s::%s"
					, dFreq
					, lpRate
					, strCBW
					, strSBW
					, CwcnUtility::WLAN_ANT_NAME[nAnt]);

				if (pTestResult->per.dAvgValue > m_pWlanParamBand->stSpec.dPer)
				{
					return SP_E_WCN_WLAN_PER_FAIL;
				}
			}

		}
	}
	
	return SP_OK;
}



