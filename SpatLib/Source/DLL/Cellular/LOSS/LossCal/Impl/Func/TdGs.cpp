#include "StdAfx.h"
#include "TdGs.h"
#include "ImpBase.h"
#include "TDUtility.h"

CTdGs::CTdGs(CImpBase *pSpat)
: CTDMeasurePower(pSpat)
{
}

CTdGs::~CTdGs(void)
{
}

SPRESULT CTdGs::Run()
{
	SPRESULT res = SP_OK;
	PC_TD_NV_T stNv;
	stNv.eNvType = TD_NV_CAL_APC_CONTROL_WORD_INDEX;
	stNv.nDataNum = sizeof(m_tApcNv)/2;
	res = SP_tdLoadNV(m_pImp->m_hDUT, &stNv);
	if (SP_OK != res)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "SP_tdLoadNV(TD_NV_CAL_APC_CONTROL_WORD_INDEX) fail,err=%d", res);
		return res;
	}
	memcpy_s(&m_tApcNv, sizeof(m_tApcNv), stNv.nData, sizeof(m_tApcNv));
	ConfigChannel();
	uint8 nTotalCount = 0;
	std::list<int>::iterator it = m_listChannel.begin();
	for(; it != m_listChannel.end(); ++it)
	{
		uint16 nChan = (uint16)*it;
		double dFreq = CTDUtility::TDChan2MHz(nChan);
		uint8 nLoss = (uint8)(m_pImp->m_lossVal.tdLoss.arrPoint[nTotalCount].dLoss[0][RF_IO_TX]*10);
		BOOL bValidChan = FALSE;
		for(int nBand=BI_TD_20G; nBand<BI_TD_MAX_BAND;nBand++)
		{
			if(m_pImp->m_bTdBandNum[nBand])
			{
				bValidChan = CTDUtility::CheckValidChannel((SP_BAND_INFO)nBand, nChan);
				if(bValidChan)
				{
					break;
				}
			}
		}
		if(!bValidChan)
		{
			continue;
		}
		m_PointGroup[nTotalCount].nChannel = nChan;
		m_PointGroup[nTotalCount].dExpPower = 23;
		m_PointGroup[nTotalCount].nGainIndex = FindGainIndexFromNv(nChan);

		m_pImp->m_gs.td[nTotalCount].nFreq = uint16(dFreq*10);
		m_pImp->m_gs.td[nTotalCount].data[0].nLoss = nLoss;
		m_pImp->m_gs.td[nTotalCount].data[0].nIndex = m_PointGroup[nTotalCount].nGainIndex;
		nTotalCount++;
		if(nTotalCount > MAX_TD_LOSS_NUMBER)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "TD的Loss频点过多，需小于%d", MAX_TD_LOSS_NUMBER);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
	}
	if(0 == nTotalCount)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "有效的频点等于0,请请检查TD的band选择和losss设置！");
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	res = MeasurePower(m_PointGroup, nTotalCount);
	if(SP_OK != res)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量功率失败！ %d", res);
		return res;
	}
	m_pImp->m_gs.common.nTdCnt = nTotalCount;
	for(int i=0; i<nTotalCount; i++)
	{
		m_pImp->m_gs.td[i].data[0].nExpPwr = uint16(m_PointGroup[i].dPower*100);
	}
    return SP_OK;
}

SPRESULT CTdGs::Init()
{
    return SP_OK;
}

SPRESULT CTdGs::Release()
{
    return SP_OK;
}

uint16 CTdGs::FindGainIndexFromNv(uint16 nChan)
{
	uint32 nIndex = 0;
	if(nChan >= 10054)
	{
		double dStep = (10121-10054)/2.0;
		nIndex = int((10121-nChan)/dStep+0.5);
		if(nIndex > 2)
		{
			nIndex = 2;
		}
		return m_tApcNv.WordBand2G[nIndex][0];
	}
	else
	{
		double dStep = (9596-9404)/7.0;
		nIndex = int((9596-nChan)/dStep+0.5);
		if(nIndex > 7)
		{
			nIndex = 7;
		}
		return m_tApcNv.WordBand19G[nIndex][0];
	}
}

void CTdGs::ConfigChannel()
{
	m_listChannel.clear();
	if(m_pImp->m_bTdBandNum[BI_TD_19G])
	{
		m_listChannel.push_back(9404);
		m_listChannel.push_back(9596);
	}
	
	if(m_pImp->m_bTdBandNum[BI_TD_20G])
	{
		m_listChannel.push_back(10054);
		m_listChannel.push_back(10087);
	}
	
}
