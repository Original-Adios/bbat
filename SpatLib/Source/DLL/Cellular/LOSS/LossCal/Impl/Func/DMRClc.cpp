#include "StdAfx.h"
#include "DMRClc.h"
#include "ImpBase.h"

CDMRClc::CDMRClc(CImpBase *pSpat)
: CDMRLossBase(pSpat)
{
}


CDMRClc::~CDMRClc(void)
{
}

SPRESULT CDMRClc::Run()
{
	CHKRESULT(InitCal());
	RF_CABLE_LOSS_UNIT_EX* pLoss = &(m_pImp->m_lossVal.otherLoss);
	ZeroMemory(pLoss, sizeof(*pLoss));
	int nCnt = m_pImp->m_gs.common.nDmrCnt;
	for (int i = 0; i < nCnt; i++)
	{
		double dFreq = m_pImp->m_gs.dmr[i].nFreq/10.0;
		double dExp = (int16)m_pImp->m_gs.dmr[i].data[0].nExpPwr/100.0;
		int nPcl = m_pImp->m_gs.dmr[i].data[0].nPcl;
		double dPwr = 0.0;
		CHKRESULT(MeaPower(dFreq, nPcl, dExp, dPwr));
		pLoss->arrPoint[i].dFreq[RF_IO_TX] = dFreq;
		for (int j = 0; j < DUL_ANT; j++)
		{
			pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = dExp - dPwr + m_pImp->m_dPreLoss;
			pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = pLoss->arrPoint[i].dLoss[j][RF_IO_TX];
		}
		pLoss->nCount++;

		/*
		m_pImp->NOTIFY("DMR GS", LEVEL_ITEM, dExp - 8.0, dPwr, dExp + 8.0,NULL, -1, "dBm", "Freq =%.2f MHz, PCL = %d", dFreq, nPcl);
		if (!IN_RANGE(dExp - 8.0, dPwr, dExp + 8.0))
		{
			return SP_DMR_LOSS_FAIL;
		}
		*/

		m_pImp->NOTIFY("DMR Clc", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper
			,NULL, -1, "dBm", "Freq =%.2f MHz", dFreq);
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->CheckLoss(pLoss->arrPoint[i].dLoss[0][RF_IO_TX], "DMR"), "Check loss");
	}
	return SP_OK;
}
