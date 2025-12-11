#include "StdAfx.h"
#include "WlanProbeLoadXML.h"

CWlanProbeLoadXML::CWlanProbeLoadXML(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
	m_vectProtco.resize(WIFI_MaxProtocol);
	m_vectProtco[WIFI_802_11b] = L"Wlan11b";
	m_vectProtco[WIFI_802_11g] = L"Wlan11g";
	m_vectProtco[WIFI_802_11n] = L"Wlan11n";
	m_vectProtco[WIFI_802_11ac] = L"Wlan11ac";
	m_vectProtco[WIFI_CW_SPECTRUM] = L"WlanCW";
	m_vectProtco[WIFI_802_11a] = L"Wlan11a";
	m_vectProtco[WIFI_802_11ax] = L"Wlan11ax";

	m_vectBandWidth.resize(WIFI_BW_MAX);
	m_vectBandWidth[WIFI_BW_20M] = L"BW_20M";
	m_vectBandWidth[WIFI_BW_40M] = L"BW_40M";
	m_vectBandWidth[WIFI_BW_80M] = L"BW_80M";
	m_vectBandWidth[WIFI_BW_160M] = L"BW_160M";
}

CWlanProbeLoadXML::~CWlanProbeLoadXML(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }
}

SPRESULT CWlanProbeLoadXML::WlanLoadXmlFile( WLAN_PARAM_CONF &stWlanParamImp )
{
	if (!stWlanParamImp.VecWlanParamBandImp.empty())
	{
		stWlanParamImp.VecWlanParamBandImp.clear();
	}
	wstring strBand[WLAN_BAND_MAX] = {L"Band_24G", L"Band_50G"}; 
	wstring strAnt[ANT_MAX] = { L"", L"Ant1st", L"Ant2nd", L"MIMO" };

	std::wstring strTxCommon = L"Param:Common";
	m_stGroupParam.Init();
	int nAvgCount = m_pSpatBase->GetConfigValue((strTxCommon + L":AvgCount").c_str(), 3);

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
			for(int nProtoc=WIFI_802_11b; nProtoc<WIFI_MaxProtocol; nProtoc++)
			{
				m_stWlanParamBand.vecConfParamGroup.clear();
				m_stWlanParamBand.stSpec.Int();
				m_stWlanParamBand.eMode = (WIFI_PROTOCOL_ENUM)nProtoc;
				m_stWlanParamBand.eBand = (WLAN_BAND_ENUM)nBandIndex;

				BOOL bProtocEnable = FALSE;
				for(int nBandWith=WIFI_BW_20M; nBandWith<WIFI_BW_MAX; nBandWith++)
				{
					m_strBandSel   = L"Option:" + strBand[nBandIndex] + L":"+ m_vectProtco[nProtoc] + L":" + m_vectBandWidth[nBandWith];
					m_strParamChan = L"Param:" + strBand[nBandIndex] + L":" + m_vectProtco[nProtoc] + L":" + m_vectBandWidth[nBandWith];
					bEnable = (BOOL)m_pSpatBase->GetConfigValue((m_strBandSel).c_str(), FALSE);
					if(!bEnable)
					{
						continue;
					}
					bProtocEnable = TRUE;
					wstring strPath = m_strParamChan + L":GroupCount";
					int nGroupCount = m_pSpatBase->GetConfigValue(strPath.c_str(), 0);
					for(int nGroupIndex=0; nGroupIndex<nGroupCount; nGroupIndex++)
					{
						m_stGroupParam.Init();
						m_stGroupParam.stAlgoParamGroupSub.eAnt = (ANTENNA_ENUM)nAnt;
						m_stGroupParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
						wchar_t    szGroup[20] = {0};
						swprintf_s(szGroup, L":Group%d", nGroupIndex+1);
						strPath = m_strParamChan + szGroup ;
						wstring strVal = m_pSpatBase->GetConfigValue((strPath + L":CenChan").c_str(), L"1,7,13");
						int nCount = 0;
						INT *parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
						if(nCount < 1)
						{
							m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
							return SP_E_WCN_INVALID_XML_CONFIG;
						}
						int nFreqOff = m_pSpatBase->GetConfigValue((strPath + L":PriChan").c_str(), 0);
						int nCenChan = 0;
						int nPrimChan = 0;
						m_stGroupParam.vectChan.resize(nCount);
						for (int nChTmp=0;nChTmp<nCount;nChTmp++)
						{
							nCenChan = parrInt[nChTmp];
							m_stGroupParam.vectChan[nChTmp].nCenChan = nCenChan;
							nPrimChan = nCenChan + nFreqOff*2;
							m_stGroupParam.vectChan[nChTmp].nPriChan = nPrimChan;
						}
						//Get modulation type
						LPCTSTR lpVal = m_pSpatBase->GetConfigValue((strPath + L":Modulation").c_str(), L"MCS-7");
						E_WLAN_RATE eRate = CwcnUtility::WlanGetRate( m_pSpatBase->_W2CA(lpVal));
						if(INVALID_WLAN_RATE == eRate)
						{
							return SP_E_WCN_INVALID_XML_CONFIG;
						}
						m_stGroupParam.stAlgoParamGroupSub.eRate = eRate;

						m_stGroupParam.stAlgoParamGroupSub.nTotalPackets = m_pSpatBase->GetConfigValue((strPath + L":PacketCount").c_str(), 1000);
						m_stGroupParam.stAlgoParamGroupSub.dRxMaxLvl = m_pSpatBase->GetConfigValue((strPath + L":RXMaxLvl").c_str(), -30.0);
						if(!IN_RANGE(-100, m_stGroupParam.stAlgoParamGroupSub.dRxMaxLvl, -25))
						{
							return SP_E_WCN_INVALID_XML_CONFIG;
						}
						m_stGroupParam.stAlgoParamGroupSub.dRxMinLvl = m_pSpatBase->GetConfigValue((strPath + L":RXMinLvl").c_str(), -65.0);
						if(!IN_RANGE(-100, m_stGroupParam.stAlgoParamGroupSub.dRxMinLvl, -25))
						{
							return SP_E_WCN_INVALID_XML_CONFIG;
						}
						m_stGroupParam.stAlgoParamGroupSub.dRxProbeStep = m_pSpatBase->GetConfigValue((strPath + L":RXProbeStep").c_str(), 3.0);
						m_stGroupParam.stAlgoParamGroupSub.nCBWType = nBandWith;
						//Signal band width
						m_stGroupParam.stAlgoParamGroupSub.nSBWType = m_pSpatBase->GetConfigValue((strPath + L":SBW").c_str(), 0);
						//BLDC
						m_stGroupParam.stAlgoParamGroupSub.nChCode = m_pSpatBase->GetConfigValue((strPath + L":ChanCoding").c_str(), 0);

						int nDelta = m_stGroupParam.stAlgoParamGroupSub.nCBWType - m_stGroupParam.stAlgoParamGroupSub.nSBWType;
						if (nDelta == 1 && abs(nFreqOff) > 1)
						{
							m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT] channel configuration is not correct!");
							return SP_E_WCN_INVALID_XML_CONFIG;
						}
						m_stGroupParam.stAlgoParamGroupSub.dwMask = WIFI_PER;
						m_stWlanParamBand.vecConfParamGroup.push_back(m_stGroupParam);
					}
				}
				if(!bProtocEnable)
				{
					continue;
				}
				wstring strPath = L"Param:" + strBand[nBandIndex] + L":"+ m_vectProtco[nProtoc] + L":Specification";
				
				//PER spec
				m_stWlanParamBand.stSpec.dPer = m_pSpatBase->GetConfigValue((strPath + L":PER").c_str(), 10.0);
				stWlanParamImp.VecWlanParamBandImp.push_back(m_stWlanParamBand);
			}
		}
	}
	return SP_OK;
}
