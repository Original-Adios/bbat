#include "StdAfx.h"
#include "WlanLoadXMLCW.h"

CWlanLoadXMLCW::CWlanLoadXMLCW(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
}

CWlanLoadXMLCW::~CWlanLoadXMLCW(void)
{	
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }
}

SPRESULT CWlanLoadXMLCW::WlanLoadXmlFile( WLAN_PARAM_CONF &stWlanParamImp )
{
	if (!stWlanParamImp.VecWlanParamBandImp.empty())
	{
		stWlanParamImp.VecWlanParamBandImp.clear();
	}
	wstring strBand[WLAN_BAND_MAX] = {L"Band_24G", L"Band_50G"}; 
	wstring strAnt[ANT_MAX] = {L"",L"Ant1st", L"Ant2nd", L"MIMO" };
	std::wstring strTxCommon = L"Param:Common";
	m_stGroupParam.Init();
	int nAvgCount = m_pSpatBase->GetConfigValue((strTxCommon + L":AvgCount").c_str(), 3);
	BOOL bSetPwrLvl = (BOOL)m_pSpatBase->GetConfigValue(L"Option:ManualPwrLvl", FALSE);

	for(int nBandIndex=WLAN_BAND_24G; nBandIndex<WLAN_BAND_MAX; nBandIndex++)
	{
		for(int nAnt = ANT_PRIMARY; nAnt<ANT_MAX; nAnt++)
		{
			wstring strChannel = L"Option:" + strBand[nBandIndex] + L":" + strAnt[nAnt];
			BOOL bEnable = m_pSpatBase->GetConfigValue(strChannel.c_str(), FALSE);
			if(!bEnable)
			{
				continue;
			}
			m_strParamChan = L"Param:" + strBand[nBandIndex];
			m_stWlanParamBand.vecConfParamGroup.clear();
			m_stWlanParamBand.stSpec.Int();
			m_stWlanParamBand.eMode = WIFI_CW_SPECTRUM;
			m_stWlanParamBand.eBand = (WLAN_BAND_ENUM)nBandIndex;
			m_stGroupParam.Init();
			m_stGroupParam.stAlgoParamGroupSub.eAnt = (ANTENNA_ENUM)nAnt;

			if (ANT_PRIMARY == nAnt)
			{
				wstring strRfSwitchAnt1st = L"Option:" + strBand[nBandIndex] + L":" + L"RfSwitchAnt1st";
				m_stGroupParam.stAlgoParamGroupSub.nRfSwitchAnt1st = m_pSpatBase->GetConfigValue(strRfSwitchAnt1st.c_str(), 0);
			}
			if (ANT_SECONDARY == nAnt)
			{

				wstring strRfSwitchAnt2nd = L"Option:" + strBand[nBandIndex] + L":" + L"RfSwitchAnt2nd";
				m_stGroupParam.stAlgoParamGroupSub.nRfSwitchAnt2nd = m_pSpatBase->GetConfigValue(strRfSwitchAnt2nd.c_str(), 0);
			}

			m_stGroupParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
			wstring strVal = m_pSpatBase->GetConfigValue((m_strParamChan + L":CenChan").c_str(), L"1,7,13");
			int nCount = 0;
			INT *parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			m_stGroupParam.vectChan.resize(nCount);
			for (int nChTmp=0;nChTmp<nCount;nChTmp++)
			{
				m_stGroupParam.vectChan[nChTmp].nCenChan = parrInt[nChTmp];
				m_stGroupParam.vectChan[nChTmp].nPriChan = parrInt[nChTmp];
			}
			//Get ref level
			m_stGroupParam.stAlgoParamGroupSub.dRefLvl = m_pSpatBase->GetConfigValue((m_strParamChan + L":RefLVL").c_str(), 10);
			m_stGroupParam.stAlgoParamGroupSub.bSetPwrLvl = bSetPwrLvl;
			m_stGroupParam.stAlgoParamGroupSub.dwMask = WIFI_CW;
			m_stWlanParamBand.vecConfParamGroup.push_back(m_stGroupParam);
			//TXP spec
			strVal = m_pSpatBase->GetConfigValue((m_strParamChan + L":Specification:TXP").c_str(), L"8,20");
			double* parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 2)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			m_stWlanParamBand.stSpec.dTxp.low = parrDouble[0];
			m_stWlanParamBand.stSpec.dTxp.upp = parrDouble[1];
			m_stWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
			m_stWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];
			stWlanParamImp.VecWlanParamBandImp.push_back(m_stWlanParamBand);
			
		}
	}
	return SP_OK;
}