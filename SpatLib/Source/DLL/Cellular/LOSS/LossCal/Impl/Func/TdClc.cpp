#include "StdAfx.h"
#include "TdClc.h"
#include "ImpBase.h"
#include "TDUtility.h"

CTdClc::CTdClc(CImpBase *pSpat)
: CTDMeasurePower(pSpat)
{
}

CTdClc::~CTdClc(void)
{
}

SPRESULT CTdClc::Run()
{
	RF_CABLE_LOSS_UNIT_EX* pLoss = &(m_pImp->m_lossVal.tdLoss);
	ZeroMemory(pLoss, sizeof(*pLoss));
	if(m_pImp->m_gs.common.nTdCnt > MAX_TD_LOSS_NUMBER)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "TD的Loss频点过多，需小于%d", MAX_TD_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	for (int i = 0; i < m_pImp->m_gs.common.nTdCnt; i++)
	{
		double dFreq = m_pImp->m_gs.td[i].nFreq / 10.0;
		pLoss->arrPoint[i].dFreq[RF_IO_TX] = dFreq;
		uint16 nChan = CTDUtility::TDMHz2Chan(dFreq);
		BOOL bValidChan = FALSE;
		for(int nBand=BI_TD_20G; nBand<BI_TD_MAX_BAND;nBand++)
		{
			bValidChan = CTDUtility::CheckValidChannel((SP_BAND_INFO)nBand, nChan);
			if(bValidChan)
			{
				break;
			}
		}
		if(!bValidChan)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "TD频点错误%.1f", dFreq);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
		m_PointGroup[i].nChannel = nChan;
		m_PointGroup[i].dExpPower = (double)m_pImp->m_gs.td[i].data[0].nExpPwr/100.0;
		m_PointGroup[i].nGainIndex = m_pImp->m_gs.td[i].data[0].nIndex;
		int nBandIndex = 0;
		if(m_PointGroup[i].nChannel < 10054)
		{
			nBandIndex = 1;
		}
		m_pImp->NOTIFY("TargetPower", LEVEL_UI, -80.0, m_PointGroup[i].dExpPower, 33.0, CTDUtility::TD_BAND_NAME[nBandIndex],
			m_PointGroup[i].nChannel,"dBm", "Index = 0x%X", m_PointGroup[i].nGainIndex);
	}
	SPRESULT res = MeasurePower(m_PointGroup, m_pImp->m_gs.common.nTdCnt);
	if(SP_OK != res)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量功率失败！ %d", res);
		return res;
	}
	for (int i = 0; i < m_pImp->m_gs.common.nTdCnt; i++)
	{
		int nBandIndex = 0;
		if(m_PointGroup[i].nChannel < 10054)
		{
			nBandIndex = 1;
		}
		pLoss->arrPoint[i].dLoss[0][RF_IO_TX] = m_PointGroup[i].dExpPower - m_PointGroup[i].dPower + m_pImp->m_dPreLoss;
		pLoss->arrPoint[i].dLoss[0][RF_IO_RX] = pLoss->arrPoint[i].dLoss[0][RF_IO_TX];
		
		m_pImp->NOTIFY("TD Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper, 
			CTDUtility::TD_BAND_NAME[nBandIndex],m_PointGroup[i].nChannel, "dBm", "Index = 0x%X", m_PointGroup[i].nGainIndex);
		CHKRESULT(m_pImp->CheckLoss( pLoss->arrPoint[i].dLoss[0][RF_IO_TX] , "TDSCDMA"));
        pLoss->nCount++;
	}
	return SP_OK;
}

SPRESULT CTdClc::Init()
{
    return SP_OK;
}

SPRESULT CTdClc::Release()
{
    return SP_OK;
}
