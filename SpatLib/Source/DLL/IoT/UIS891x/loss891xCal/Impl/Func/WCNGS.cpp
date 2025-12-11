#include "StdAfx.h"
#include "WCNGS.h"
#include "ImpBaseUIS8910.h"
#include "wcnUtility.h"


CWCNGS::CWCNGS(CImpBaseUIS8910 *pSpat)
	:CWCNMeasurePower(pSpat)
{
	m_eMode = SP_INVALID;
    ZeroMemory((void *)m_BtPointGroup,MAX_BT_LOSS_NUMBER*sizeof(WCN_MEASURE_T));
    ZeroMemory((void *)m_WlanPointGroup,MAX_WLAN_LOSS_NUMBER*sizeof(WCN_MEASURE_T));
}


CWCNGS::~CWCNGS(void)
{
}

SPRESULT CWCNGS::Run()
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

SPRESULT CWCNGS::Init()
{
	CHKRESULT(__super::Init());
	return SP_OK;
}

SPRESULT CWCNGS::Release()
{
	CHKRESULT(__super::Release());
	return SP_OK;
}

void CWCNGS::ConfigWlanChannel()
{
	m_vecWlan[0].clear();
	m_vecWlan[0].push_back(1);
	m_vecWlan[0].push_back(7);
	m_vecWlan[0].push_back(13);
	//m_vecWlan[0].push_back(14);

	m_vecWlan[1].clear();
	m_vecWlan[1].push_back(36);
	m_vecWlan[1].push_back(104);
	m_vecWlan[1].push_back(165);

}

void CWCNGS::ConfigBtChannel()
{
	m_vecBt.clear();
	m_vecBt.push_back(0);
	m_vecBt.push_back(10);
	m_vecBt.push_back(40);
	m_vecBt.push_back(78);

	m_vecBtBle.clear();
	m_vecBtBle.push_back(0);
	m_vecBtBle.push_back(19);
	m_vecBtBle.push_back(38);

}

SPRESULT CWCNGS::RunWlan()
{
	int nWlanCount = 0;
	ConfigWlanChannel();
	ANTENNA_ENUM eWlanPath[MAX_RF_ANT] = {ANT_PRIMARY, ANT_SECONDARY, ANT_MAX, ANT_MAX};
	for(int i=0; i<2; i++)
	{
		WIFI_PROTOCOL_ENUM eType = WIFI_802_11b;
		if(m_pImp->m_bWlanBandNum[i][WIFI_802_11b])
		{
			eType = WIFI_802_11b;
		}
		else if(m_pImp->m_bWlanBandNum[i][WIFI_802_11g])
		{
			eType = WIFI_802_11g;
		}
		else if(m_pImp->m_bWlanBandNum[i][WIFI_802_11n])
		{
			eType = WIFI_802_11n;
		}
		else if(m_pImp->m_bWlanBandNum[i][WIFI_802_11a])
		{
			eType = WIFI_802_11a;
		}
		else if(m_pImp->m_bWlanBandNum[i][WIFI_802_11ac])
		{
			eType = WIFI_802_11ac;
		}
		else
		{
			continue;
		}
		for(int k=0; k<(int)m_vecWlan[i].size(); k++)
		{

			BOOL bAntSelected = FALSE;
			for(int j=0; j<MAX_RF_ANT; j++)
			{
				ZeroMemory((void*)&m_WlanPointGroup[nWlanCount].Para[j], sizeof(WCN_PARA_T));
				ZeroMemory((void*)&m_WlanPointGroup[nWlanCount].OrgPara[j], sizeof(WCN_PARA_T));
				if(m_pImp->m_bWlanAnt[i][j])
				{
					bAntSelected = TRUE;
					if(m_pImp->m_bWcnAnt)
					{
						m_WlanPointGroup[nWlanCount].nPath[j] = (int8)eWlanPath[j];
					}
					else
					{
						m_WlanPointGroup[nWlanCount].nPath[j] = (int8)ANT_WLAN_AUTO;
					}
					m_WlanPointGroup[nWlanCount].dExpPower[j] = m_pImp->m_dWlanRefLvl;
				}
				else
				{
					m_WlanPointGroup[nWlanCount].nPath[j] = (int8)ANT_WLAN_INVALID;
				}
			}
			if(!bAntSelected)
			{
				continue;
			}
			m_WlanPointGroup[nWlanCount].nChannel = (int16)m_vecWlan[i][k];
			m_WlanPointGroup[nWlanCount].nType = (int8)eType;
			nWlanCount++;
		}
	}
	int nTotalCount = 0;
	if(nWlanCount > 0)
	{
		SPRESULT res = WlanMeasurePower( &m_WlanPointGroup[0], nWlanCount);
		if(SP_OK != res)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量WLAN功率失败！ %d", res);
			return res;
		}
		for(int i=0; i<nWlanCount; i++)
		{
			m_pImp->m_gs.wlan[nTotalCount].nCh = (int16)m_WlanPointGroup[i].nChannel;
			m_pImp->m_gs.wlan[nTotalCount].data.nProtocalType = (int8)(m_WlanPointGroup[i].nType);
			m_pImp->m_gs.wlan[nTotalCount].data.nExpPwr = (int16)(m_pImp->m_dWlanRefLvl*100);
			for(int j=0; j<MAX_RF_ANT; j++)
			{
				m_pImp->m_gs.wlan[nTotalCount].data.Rlt[j].Cnt = m_WlanPointGroup[i].Para[j].Cnt;
				for (int k = 0; k < m_WlanPointGroup[i].Para[j].Cnt; k++)
				{
					m_pImp->m_gs.wlan[nTotalCount].data.Rlt[j].AgcIdx[k] = m_WlanPointGroup[i].Para[j].AgcIdx[k];
					m_pImp->m_gs.wlan[nTotalCount].data.Rlt[j].Power[k] = m_WlanPointGroup[i].Para[j].Power[k];

				}
				m_pImp->m_gs.wlan[nTotalCount].data.nPath[j] = int8(m_WlanPointGroup[i].nPath[j]);
			}
			nTotalCount++;
		}
	}
	m_pImp->m_gs.common.nWlanCnt = (int8)nTotalCount;
	return SP_OK;
}

SPRESULT CWCNGS::RunBt()
{
	int nBtCount = 0;
	ConfigBtChannel();
	BT_RFPATH_ENUM eBtPath[MAX_RF_ANT] = {ANT_SHARED, ANT_SINGLE, ANT_BT_MAX, ANT_BT_MAX};
	BT_TYPE eType = MAX_BT_TYPE;
	if(m_pImp->m_bBTBandNum[BDR])
	{
		eType = BDR;
	}
	else if(m_pImp->m_bBTBandNum[EDR])
	{
		eType = EDR;
	}
	else if(m_pImp->m_bBTBandNum[BLE])
	{
		eType = BLE;
	}
	else if(m_pImp->m_bBTBandNum[BLE_EX])
	{
		eType = BLE_EX;
	}
	else
	{
		m_pImp->LogFmtStrA(SPLOGLV_WARN, "No Bt type Selected!");
		return SP_OK;
	}
	std::vector<int> vectCh = m_vecBt;
	if(BLE_EX == eType || BLE == eType)
	{
		vectCh = m_vecBtBle;
	}
	for(int i=0; i<(int)vectCh.size(); i++)
	{
		BOOL bAntSelected = FALSE;
		for(int j=0; j< MAX_RF_ANT; j++)
		{
			if(m_pImp->m_bBTAnt[j])
			{
				bAntSelected = TRUE;
				if(m_pImp->m_bWcnAnt)
				{
					m_BtPointGroup[nBtCount].nPath[j] = (int8)eBtPath[j];
				}
				else
				{
					m_BtPointGroup[nBtCount].nPath[j] = (int8)ANT_BT_AUTO;
				}
				m_BtPointGroup[nBtCount].dExpPower[j] = m_pImp->m_dBtRefLvl;
			}
			else
			{
				m_BtPointGroup[nBtCount].nPath[j] = (int8)ANT_BT_INVALID;
			}
		}
		if(!bAntSelected)
		{
			continue;
		}
		m_BtPointGroup[nBtCount].nType = (int8)eType;
		m_BtPointGroup[nBtCount].nChannel = (int8)vectCh[i];
		nBtCount++;
	}
	int nTotalCount = 0;
	if(nBtCount > 0)
	{
		SPRESULT res = BTMeasurePower( &m_BtPointGroup[0], nBtCount);
		if(SP_OK != res)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量BT功率失败！ %d", res);
			return res;
		}
		for(int i=0; i<nBtCount; i++)
		{
			m_pImp->m_gs.bt[nTotalCount].nCh = (int16)m_BtPointGroup[i].nChannel;
			m_pImp->m_gs.bt[nTotalCount].data.nProtocalType = (int8)(m_BtPointGroup[i].nType);
			for(int j=0; j< MAX_RF_ANT; j++)
			{
				m_pImp->m_gs.bt[nTotalCount].data.nPower[j] = (int16)(m_BtPointGroup[i].dPower[j]*100.0);
				m_pImp->m_gs.bt[nTotalCount].data.nPath[j] = (int8)(m_BtPointGroup[i].nPath[j]);
			}
			nTotalCount++;
		}
	}
	m_pImp->m_gs.common.nBtCnt = (uint8)nTotalCount;
	return SP_OK;
}

SPRESULT CWCNGS::RunGps()
{
	int nTotalCount = 0;
	if(m_pImp->m_bGPSBandNum)
	{
		double dSnr = 0;
		SPRESULT res = GetGPSSNR(dSnr);
		if(SP_OK != res)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量GPS SNR 失败！ %d", res);
			return res;
		}
		m_pImp->m_gs.gps[nTotalCount].nCh = 0;
		m_pImp->m_gs.gps[nTotalCount].data.nPower[0] = (int16)(dSnr*100);
		nTotalCount++;
	}
	m_pImp->m_gs.common.nGpsCnt = (uint8)nTotalCount;
	return SP_OK;
}
