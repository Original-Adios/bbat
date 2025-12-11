#include "StdAfx.h"
#include "WlanMeasBaseProbe.h"
#include "WlanApiAT.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CWlanMeasBaseProbe::CWlanMeasBaseProbe(CImpBase *pImpBase)
: CWlanMeasBase(pImpBase)
, m_hcsvHandle(NULL)
{
}

CWlanMeasBaseProbe::~CWlanMeasBaseProbe(void)
{
}

SPRESULT CWlanMeasBaseProbe::WlanParamSet(WlanMeasParamBand *pWlanParamBand)
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

SPRESULT CWlanMeasBaseProbe::WlanMeasPerform()
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	char strInfo[256] = {0};
	int nRepeatCount = 0;
	SPRESULT res = SP_OK;
	SPRESULT eRslt = SP_OK;
	double dMinRxLvl = 0.0;
	double dMaxRxLvl = 0.0;
	double dStep = 0.0;
	DWORD dwMask = 0;
	BOOL bShareAnt = FALSE;

	vector<SPWI_WLAN_PARAM_GROUP>::iterator itWlanChan;

	// [6]: Setup instrument and measure up-link result
	//CInstrumentLock rfLock(m_pRfTesterWlan);
	m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
	CHKRESULT(m_pRfTesterWlan->SetNetMode(NM_WIFI));
	CHKRESULT(m_pRfTesterWlan->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));
	for (itWlanChan = m_pWlanParamBand->vecConfParamGroup.begin(); itWlanChan != m_pWlanParamBand->vecConfParamGroup.end(); itWlanChan++)
	{
		if (m_pImpBase->_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}
		//General config
		m_MeasParam.stTesterParamGroupSub = itWlanChan->stAlgoParamGroupSub;
		dwMask = m_MeasParam.stTesterParamGroupSub.dwMask;

		if (dwMask == 0)
		{
			continue;
		}
		m_pImpBase->SetRepairBand(CwcnUtility::m_WlanBandInfo[m_pWlanParamBand->eMode]);

		//eband
		WIFI_PROTOCOL_ENUM eProto = m_pWlanParamBand->eMode;
		m_MeasParam.eMode = m_pWlanParamBand->eMode;
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
		m_MeasParam.eMode = m_pWlanParamBand->eMode;
		for (uint32 nCh=0;nCh<itWlanChan->vectChan.size();nCh++)
		{
			//Meas channel config
			nRepeatCount = 0;
			m_MeasParam.stChan = itWlanChan->vectChan[nCh];

			//Show result
			sprintf_s(strInfo, sizeof(strInfo), "%s::PriChan-%02d::CenChan-%d::DataRate:%s::SBW:%s::CBW:%s"
				, CwcnUtility::WLAN_BAND_NAME[m_pWlanParamBand->eMode]
				, m_MeasParam.stChan.nPriChan
				, m_MeasParam.stChan.nCenChan
				, CwcnUtility::WlanGetRateString(m_MeasParam.stTesterParamGroupSub.eRate)
				, CwcnUtility::WLAN_BW_NAME[m_MeasParam.stTesterParamGroupSub.nSBWType]
				, CwcnUtility::WLAN_BW_NAME[m_MeasParam.stTesterParamGroupSub.nCBWType]
				);

			m_pImpBase->_UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "BSLevel", -1, "-");

			dMaxRxLvl = m_MeasParam.stTesterParamGroupSub.dRxMaxLvl;
			dMinRxLvl = m_MeasParam.stTesterParamGroupSub.dRxMinLvl;
			dStep = m_MeasParam.stTesterParamGroupSub.dRxProbeStep;

			//RX performance meas
			double dmeasAvgValue = 0.0;
			double dmeasMinRxLvl = 0.0;
			do
			{
				if (m_pImpBase->_IsUserStop())
				{
					return SP_E_USER_ABORT;
				}
				m_MeasParam.stTesterParamGroupSub.dVsgLvl = dMaxRxLvl;
				res = this->TestPER(&m_MeasParam, &m_rstData);
				m_pWlanApi->DUT_RxOn(false);
				//Show Result
				res |= ShowMeasRst(dwMask
					, m_MeasParam.stTesterParamGroupSub.eRate
					, &m_rstData
					, FALSE
					, dMaxRxLvl);

				if (SP_OK == res)
				{
					dmeasAvgValue = m_rstData.per.dAvgValue;
					dmeasMinRxLvl = dMaxRxLvl;
				}

				dMaxRxLvl -= fabs(dStep);
			} while (dMaxRxLvl >= dMinRxLvl);


			if (m_pImpBase->m_bFailStop)
			{
				CHKRESULT(res);
				eRslt = res;
			}
			else
			{
				eRslt |= res;
			}

			//write WlanBerProbe.csv
			//CUtility uti;
			char szFileData[_MAX_PATH + 1] = { 0 };
			sprintf_s(&szFileData[0], sizeof(szFileData), "\r\n%d,%d,%s,%s,%d,%0.2f,%0.2f"
				, m_MeasParam.stTesterParamGroupSub.eAnt
				, eBand
				, CwcnUtility::WLAN_BAND_NAME[m_pWlanParamBand->eMode]
				, CwcnUtility::WlanGetRateString(m_MeasParam.stTesterParamGroupSub.eRate)
				, m_MeasParam.stChan.nCenChan
				, dmeasAvgValue
				, dmeasMinRxLvl);
			DWORD dwByteWritten = 0;
			BOOL bRet = WriteFile(m_hcsvHandle, szFileData, sizeof(szFileData), &dwByteWritten, NULL);
			if (!bRet || sizeof(szFileData) != dwByteWritten)
			{
				m_pImpBase->_UiSendMsg("Write WlanBerProbe.csv", LEVEL_ITEM, SP_OK, SP_E_FILE_IO, SP_OK, "", -1, "-");
				return SP_E_FILE_IO;
			}
			//
		}
	}
	return eRslt;
}

SPRESULT CWlanMeasBaseProbe::ShowMeasRst(DWORD  /*dwItemMask*/, E_WLAN_RATE /*eRate*/, SPWI_RESULT_T *pTestResult, BOOL /*bTxShow*/, double dBsLvl )
{
	auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
	if (pTestResult == NULL)
	{
		CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "Wlan::Invalid result!");
	}

	m_pImpBase->_UiSendMsg("SensitivityTest"
		, LEVEL_ITEM
		, 0
		, pTestResult->per.dAvgValue
		, m_pWlanParamBand->stSpec.dPer
		, ""/*m_mapBand.find(m_pWlanParamBand->eMode)->second*/
		, -1
		, "-"
		,"%.2f"
		, dBsLvl
		);

	if (pTestResult->per.dAvgValue > m_pWlanParamBand->stSpec.dPer)
	{
		return SP_E_WCN_WLAN_PER_FAIL;
	}

	return SP_OK;
}

