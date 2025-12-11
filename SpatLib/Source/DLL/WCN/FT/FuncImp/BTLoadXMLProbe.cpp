#include "StdAfx.h"
#include "BTLoadXMLProbe.h"
#include <map>

CBTLoadXMLProbe::CBTLoadXMLProbe(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
}

CBTLoadXMLProbe::~CBTLoadXMLProbe(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }
}

SPRESULT CBTLoadXMLProbe::BTLoadXmlFile(vector<BTMeasParamBand> &VecBTParamBandImp)
{
    if (!VecBTParamBandImp.empty())
    {
        VecBTParamBandImp.clear();
    }

    BOOL bEnable = FALSE;
    int nCount = 0;
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
				 double dMinLevel = m_pSpatBase->GetConfigValue((strGroup + L":RXMinLvl").c_str(), -65.0);
				 double dMaxLevel = m_pSpatBase->GetConfigValue((strGroup + L":RXMaxLvl").c_str(), 10);
				 double dStep = m_pSpatBase->GetConfigValue((strGroup + L":RXStep").c_str(), 10);
				 
				 int nPacketCount = m_pSpatBase->GetConfigValue((strGroup + L":RxCount").c_str(), 1000);
					 
				 DWORD dwMask = (stBTParamBand.eMode == BLE || stBTParamBand.eMode == BLE_EX) ? BLE_PER:BDR_BER;
			
				 for (int nChTmp=0;nChTmp<(int)ChGroup.size();nChTmp++)
				 {
					 for(double dRxleve=dMaxLevel; dRxleve>dMinLevel; dRxleve-=dStep)
					 {
						 BTMeasParamChan stChanParam;
						 stChanParam.dRefLvl = 20;
						 stChanParam.dVsgLvl = dRxleve;
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
			 }
			 wstring strSpec = L"Param:" + strMode[nMode] + L":Specification";
			 if(nMode == BDR)
			 {
				 //BER spec
				 stBTParamBand.stSpec.stBdrSpec.dBer = m_pSpatBase->GetConfigValue((strSpec + L":BER").c_str(), 0.1);
				 VecBTParamBandImp.push_back(stBTParamBand);
			 }
			 else if(nMode == EDR)
			 {
				 //BER spec
				 stBTParamBand.stSpec.stEdrSpec.dBer = m_pSpatBase->GetConfigValue((strSpec + L":BER").c_str(), 0.1);

				 VecBTParamBandImp.push_back(stBTParamBand);
			 }
			 else
			 {
				 //PER spec
				 stBTParamBand.stSpec.stBleExSpec.dBer = m_pSpatBase->GetConfigValue((strSpec + L":BER").c_str(), 30.8);
				 VecBTParamBandImp.push_back(stBTParamBand);
			 }
			 
		}
	}
    return SP_OK;
}
