#include "StdAfx.h"
#include "WCNCLC.h"
#include "ImpBaseUIS8910.h"
#include "wcnUtility.h"


CWCNCLC::CWCNCLC(CImpBaseUIS8910 *pSpat)
	:CWCNMeasurePower(pSpat)
{
	ZeroMemory((void *)m_BtPointGroup,MAX_BT_LOSS_NUMBER*sizeof(WCN_MEASURE_T));
	ZeroMemory((void *)m_WlanPointGroup,MAX_WLAN_LOSS_NUMBER*sizeof(WCN_MEASURE_T));
}


CWCNCLC::~CWCNCLC(void)
{
}

SPRESULT CWCNCLC::Run()
{
	switch(m_eMode)
	{
	case SP_WIFI:
		return RunWlan();
		break;
	case SP_BT:
		return RunBt();
		break;
	case SP_GPS:
		return RunGps();
		break;
	default:
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "unsupported mode %d", m_eMode);
		return SP_E_SPAT_NOT_SUPPORT;
	}
}

SPRESULT CWCNCLC::Init()
{
	CHKRESULT(__super::Init());
	return SP_OK;
}

SPRESULT CWCNCLC::Release()
{
	CHKRESULT(__super::Release());
	return SP_OK;
}

SPRESULT CWCNCLC::RunWlan()
{
	int nWLanCount = 0;
	if(m_pImp->m_gs.common.nWlanCnt > MAX_WLAN_LOSS_NUMBER)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Wlan的Loss频点过多，最大%d", MAX_WLAN_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	for(int i=0; i<m_pImp->m_gs.common.nWlanCnt; i++)
	{

		m_WlanPointGroup[nWLanCount].nChannel = (int16)m_pImp->m_gs.wlan[i].nCh;
		m_WlanPointGroup[nWLanCount].nType = (int8)(m_pImp->m_gs.wlan[i].data.nProtocalType);
		for(int j=0; j< MAX_RF_ANT; j++)
		{
			m_WlanPointGroup[nWLanCount].dExpPower[j] = m_pImp->m_gs.wlan[i].data.nExpPwr /100.0;
			m_WlanPointGroup[nWLanCount].OrgPara[j].Cnt = m_pImp->m_gs.wlan[i].data.Rlt[j].Cnt;
			for (int k = 0; k < m_pImp->m_gs.wlan[i].data.Rlt[j].Cnt; k++)
			{
				m_WlanPointGroup[nWLanCount].OrgPara[j].AgcIdx[k] = m_pImp->m_gs.wlan[i].data.Rlt[j].AgcIdx[k];
				m_WlanPointGroup[nWLanCount].OrgPara[j].Power[k] = m_pImp->m_gs.wlan[i].data.Rlt[j].Power[k];
			}
			m_WlanPointGroup[nWLanCount].nPath[j] = (int8)(m_pImp->m_gs.wlan[i].data.nPath[j]);
		}
		nWLanCount++;
	}
	if(nWLanCount > 0)
	{
		int nTotalCount = 0;
		SPRESULT res = WlanMeasurePower(&m_WlanPointGroup[0], nWLanCount);
		if(SP_OK != res)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量Wlan功率失败！ %d", res);
			return res;
		}
		RF_CABLE_LOSS_UNIT_EX* pWlanLoss = &(m_pImp->m_lossVal.wlanLoss);
		ZeroMemory(pWlanLoss, sizeof(*pWlanLoss));
		for(int n=0; n<nWLanCount; n++)
		{
			pWlanLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX] = m_WlanPointGroup[n].nChannel;
			pWlanLoss->arrPoint[nTotalCount].uArfcn[RF_IO_RX] = pWlanLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX];
			pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] = CwcnUtility::WIFI_Ch2MHz(m_WlanPointGroup[n].nChannel);
			pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_RX] = pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX];
			pWlanLoss->arrPoint[nTotalCount].nBand = 0;		//2.4g
			if (pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] > 5000)
			{
				pWlanLoss->arrPoint[nTotalCount].nBand = 1;		//5g
			}
			
			for(int i=0; i<MAX_RF_ANT; i++)
			{
				ANTENNA_ENUM ePath = (ANTENNA_ENUM)m_WlanPointGroup[n].nPath[i];
				if(ANT_WLAN_INVALID != ePath)
				{
					RF_ANT_E eAnt[2] = {RF_ANT_1st, RF_ANT_1st};
					CwcnUtility::WlanGetAnt(ePath, eAnt);
					//for(int j=0; j<MAX_RF_IO; j++)
					{
						int cnt = 0;
						double dTempLoss = 0.0;
						double dLoss = 0.0;
						for (int k = 0; k < m_WlanPointGroup[n].Para[i].Cnt; k++)
						{
							for (int m = 0; m < m_WlanPointGroup[n].OrgPara[i].Cnt; m++)
							{
								if (m_WlanPointGroup[n].Para[i].AgcIdx[k] == m_WlanPointGroup[n].OrgPara[i].AgcIdx[m])
								{
									dTempLoss = (m_WlanPointGroup[n].OrgPara[i].Power[m] - m_WlanPointGroup[n].Para[i].Power[k]) / 16.0;
									m_pImp->LogFmtStrA(SPLOGLV_INFO, "WLAN Loss:Chan=%d, AgcIdx=%d, Loss=%.3f", m_WlanPointGroup[n].nChannel, m_WlanPointGroup[n].Para[i].AgcIdx[k], dTempLoss);
									if (dTempLoss > 0)
									{
										dLoss += dTempLoss;
										cnt++;
									}
									break;
								}
							}
						}
						if (0 != cnt)
						{
							dLoss = dLoss / cnt;
						}
						pWlanLoss->arrPoint[nTotalCount].dLoss[i][RF_IO_TX] = dLoss;
						m_pImp->GetLossLimit(pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX], m_pImp->m_dLossValLower, m_pImp->m_dLossValUpper);
						m_pImp->NOTIFY("WLAN Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pWlanLoss->arrPoint[nTotalCount].dLoss[i][RF_IO_TX], m_pImp->m_dLossValUpper,
							"WLAN", m_WlanPointGroup[n].nChannel, "dB");
						CHKRESULT(m_pImp->CheckLoss(pWlanLoss->arrPoint[nTotalCount].dLoss[i][RF_IO_TX], "WLAN"));

					}
				}
			}
			nTotalCount++;
		}
		pWlanLoss->nCount = nTotalCount;
	}
	return SP_OK;
}

SPRESULT CWCNCLC::RunBt()
{
	int nBtCount = 0;
	if(m_pImp->m_gs.common.nBtCnt > MAX_BT_LOSS_NUMBER)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "BT的Loss频点过多，最大%d", MAX_BT_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	for(int i=0; i<m_pImp->m_gs.common.nBtCnt; i++)
	{
		m_BtPointGroup[nBtCount].nChannel = m_pImp->m_gs.bt[i].nCh;
		m_BtPointGroup[nBtCount].nType = (int8)(m_pImp->m_gs.bt[i].data.nProtocalType);
		for(int j=0; j< MAX_RF_ANT; j++)
		{
			m_BtPointGroup[nBtCount].nPath[j] = (int8)(m_pImp->m_gs.bt[i].data.nPath[j]);
			m_BtPointGroup[nBtCount].dOrigPower[j] = m_pImp->m_gs.bt[i].data.nPower[j]/100.0;
			m_BtPointGroup[nBtCount].dExpPower[j] = m_BtPointGroup[nBtCount].dOrigPower[j]  + 10.0;
		}
		nBtCount++;
	}
	if(nBtCount > 0)
	{
		int nTotalCount = 0;
		SPRESULT res = BTMeasurePower(&m_BtPointGroup[0], nBtCount);
		if(SP_OK != res)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量BT功率失败！ %d", res);
			return res;
		}
		RF_CABLE_LOSS_UNIT_EX* pBtLoss = &(m_pImp->m_lossVal.btLoss);
		ZeroMemory(pBtLoss, sizeof(RF_CABLE_LOSS_UNIT_EX));
		for(int n=0; n<nBtCount; n++)
		{
			pBtLoss->arrPoint[nTotalCount].nBand = m_BtPointGroup[n].nType;
			pBtLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX] = m_BtPointGroup[n].nChannel;
			pBtLoss->arrPoint[nTotalCount].uArfcn[RF_IO_RX] = m_BtPointGroup[n].nChannel;
			pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] = CwcnUtility::BT_Ch2MHz((BT_TYPE)m_BtPointGroup[n].nType, m_BtPointGroup[n].nChannel);
			pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_RX] = pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX];

			for(int i=0; i<MAX_RF_ANT; i++)
			{
				BT_RFPATH_ENUM ePath = (BT_RFPATH_ENUM)m_BtPointGroup[n].nPath[i];
				if(ANT_BT_INVALID != ePath)
				{
					RF_ANT_E eAnt = CwcnUtility::BTGetAnt(ePath);
					for(int j=0; j<MAX_RF_IO; j++)
					{
						pBtLoss->arrPoint[nTotalCount].dLoss[eAnt][j] = m_BtPointGroup[n].dOrigPower[i] - m_BtPointGroup[n].dPower[i];
					}
					m_pImp->GetLossLimit(pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX], m_pImp->m_dLossValLower, m_pImp->m_dLossValUpper);
					m_pImp->NOTIFY("BT Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pBtLoss->arrPoint[nTotalCount].dLoss[eAnt][RF_IO_TX], m_pImp->m_dLossValUpper, 
						"BT", m_BtPointGroup[n].nChannel , "dB", "%s", CwcnUtility::BT_ANT_NAME[ePath]);
					CHKRESULT(m_pImp->CheckLoss(pBtLoss->arrPoint[nTotalCount].dLoss[eAnt][RF_IO_TX],"WLAN"));
				}
			}
			nTotalCount++;
		}
		pBtLoss->nCount = nTotalCount;
	}
	return SP_OK;
}

SPRESULT CWCNCLC::RunGps()
{
	if(m_pImp->m_gs.common.nGpsCnt > MAX_GPS_LOSS_NUMBER)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "GPS的Loss频点过多，最大%d", MAX_GPS_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	if(m_pImp->m_gs.common.nGpsCnt > 0)
	{
		double dExpectSnr = (double)m_pImp->m_gs.gps[0].data.nPower[0]/100.0;
		int nTotalCount = 0;
		RF_CABLE_LOSS_UNIT_EX* pGpsLoss = &(m_pImp->m_lossVal.gpsLoss);
		ZeroMemory(pGpsLoss, sizeof(*pGpsLoss));
		double dSnr = 0;
		SPRESULT res = GetGPSSNR(dSnr);
		if(SP_OK != res)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量GPS SNR 失败！ %d", res);
			return res;
		}
		pGpsLoss->arrPoint[nTotalCount].nBand = 0;
		pGpsLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] = 1575.42;
		pGpsLoss->arrPoint[nTotalCount].dFreq[RF_IO_RX] = 1575.42;
		for(int i=0; i<MAX_RF_ANT; i++)
		{
			for(int j=0; j<MAX_RF_IO; j++)
			{
				pGpsLoss->arrPoint[nTotalCount].dLoss[i][j] = dExpectSnr - dSnr;
			}
		}
		m_pImp->GetLossLimit(pGpsLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX], m_pImp->m_dLossValLower, m_pImp->m_dLossValUpper);
		m_pImp->NOTIFY("GPS Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pGpsLoss->arrPoint[nTotalCount].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper, 
			"GPS", -1 , "dB");
		CHKRESULT(m_pImp->CheckLoss(pGpsLoss->arrPoint[nTotalCount].dLoss[0][RF_IO_TX],"WLAN"));
		pGpsLoss->nCount = 1;
	}
	return SP_OK;
}
