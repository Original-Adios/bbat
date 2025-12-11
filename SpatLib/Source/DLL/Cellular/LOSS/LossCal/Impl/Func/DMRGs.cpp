#include "StdAfx.h"
#include "DMRGs.h"
#include "ImpBase.h"
#include <list>
using namespace std;

CDMRGs::CDMRGs(CImpBase *pSpat)
	: CDMRLossBase(pSpat)
{
}


CDMRGs::~CDMRGs(void)
{
}

SPRESULT CDMRGs::Run()
{
	ConfigFreq();
	m_pImp->m_gs.common.nDmrCnt = 0;
	CHKRESULT(InitCal());
	int nCnt = m_pImp->m_lossVal.otherLoss.nCount;
	int nfrqIdx = 0;
	for (;nfrqIdx < nCnt; nfrqIdx++)
	{
		double dFrq = m_pImp->m_lossVal.otherLoss.arrPoint[nfrqIdx].dFreq[RF_IO_TX];
		double dPwr = 0.0;
		CHKRESULT(MeaPower(dFrq, m_pImp->m_nDmrPcl, m_pImp->m_dDmrTargetPwr, dPwr));
		m_pImp->m_gs.dmr[m_pImp->m_gs.common.nDmrCnt].nFreq = (int16)(dFrq*10);
		for (int j = 0; j < DUL_ANT; j++ )
		{
			m_pImp->m_gs.dmr[m_pImp->m_gs.common.nDmrCnt].data[j].nExpPwr = (int16)(dPwr * 100);
			m_pImp->m_gs.dmr[m_pImp->m_gs.common.nDmrCnt].data[j].nPcl    = (uint8)m_pImp->m_nDmrPcl;
		}
		m_pImp->m_gs.common.nDmrCnt++;
		m_pImp->NOTIFY("DMR GS", LEVEL_ITEM, m_pImp->m_dDmrTargetPwr - 8.0, dPwr, m_pImp->m_dDmrTargetPwr + 8.0,NULL, -1, "dBm", "Freq =%.2f MHz, PCL = %d", dFrq, m_pImp->m_nDmrPcl);
		if (!IN_RANGE(m_pImp->m_dDmrTargetPwr - 8.0, dPwr, m_pImp->m_dDmrTargetPwr + 8.0))
		{
			return SP_DMR_LOSS_FAIL;
		}
	}
	return SP_OK;
}

void CDMRGs::ConfigFreq()
{
	int nCnt = 0;
	list<double> listFreq;

	for (int i = 0; i < m_pImp->m_nDmrFreqCnt; i++)
	{ 
		listFreq.push_back(m_pImp->m_dDmrFreq[i]);
	}

	m_pImp->m_lossVal.otherLoss.nCount = listFreq.size();

	listFreq.sort();
	list<double>::iterator it = listFreq.begin();
	for(;it != listFreq.end(); ++it)
	{
		m_pImp->m_lossVal.otherLoss.arrPoint[nCnt++].dFreq[RF_IO_TX] = *it;
	}
	return ;
}
