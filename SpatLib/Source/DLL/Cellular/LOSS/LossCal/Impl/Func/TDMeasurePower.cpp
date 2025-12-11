#include "StdAfx.h"
#include "TDMeasurePower.h"
#include "ImpBase.h"
#include "TDUtility.h"

CTDMeasurePower::CTDMeasurePower(CImpBase *pSpat)
: ILossFunc(pSpat)
{
	ZeroMemory(&m_tdTxDutParam, sizeof(m_tdTxDutParam));
	ZeroMemory(&m_tdTxInsParam, sizeof(m_tdTxInsParam));
	ZeroMemory(&m_tdInsResult, sizeof(m_tdInsResult));
}

CTDMeasurePower::~CTDMeasurePower(void)
{
}


SPRESULT CTDMeasurePower::MeasurePower(MEASURE_T *pList, int nListCount)
{
	if(NULL == pList || 0 == nListCount)
	{
		m_pImp->LogFmtStrA(SPLOGLV_ERROR, "[%s]InValid input Param", __FUNCTION__);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	m_tdTxDutParam.nTrigNum = 0;
	m_tdTxDutParam.nTxFlag = 0x2A40;  ////0x0010 1010 0100 0000 
	m_tdTxInsParam.nTxSlot = 3;
	m_tdTxDutParam.txFDTpara.nChanelNum = nListCount;
	m_tdTxInsParam.nSequece = nListCount;
	for(int i=0; i<nListCount;i++)
	{
		m_tdTxDutParam.txFDTpara.Arfcn[i].nApcArrNum = 1;
		m_tdTxDutParam.txFDTpara.Arfcn[i].ApcTxArr[0].nStartFactor = pList[i].nGainIndex;
		m_tdTxDutParam.txFDTpara.Arfcn[i].ApcTxArr[0].cal_apc_step = 8;
		m_tdTxDutParam.txFDTpara.Arfcn[i].ApcTxArr[0].nFram = 1;
		m_tdTxDutParam.txFDTpara.Arfcn[i].nArfcn = pList[i].nChannel;

		m_tdTxInsParam.m_sqcdata[i].nArfcn = pList[i].nChannel;
		m_tdTxInsParam.m_sqcdata[i].nGainNum = 1;
		m_tdTxInsParam.m_sqcdata[i].nSubFrame[0] = 1;
		m_tdTxInsParam.m_sqcdata[i].nRefPow[0] = pList[i].dExpPower;
		m_tdTxInsParam.m_sqcdata[i].fPowerStep[0] = 1.0;
	}
	
    CHKRESULT(m_pImp->m_pRFTester->SetNetMode(NM_TD));
	CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TX_FDT, NULL));

	RF_PORT rfPort = {RF_ANT_1st, RS_INOUT};
    U_RF_PARAM param;
    param.pPort = &rfPort;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));
	CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_FDT, &m_tdTxInsParam));

	CHKRESULT(SP_tdActive(m_pImp->m_hDUT, TRUE));
	CHKRESULT(SP_tdFDTTX(m_pImp->m_hDUT, &m_tdTxDutParam));

    CHKRESULT(m_pImp->m_pRFTester->FetchResult(TI_FDT, &m_tdInsResult));
	for (int i=0; i<nListCount; i++)
	{
		pList[i].dPower = m_tdInsResult.fTxPower[3*i];
		int nBandIndex = 0;
		if (pList[i].nChannel < 10054)
		{
			nBandIndex = 1;
		}
		m_pImp->NOTIFY("TD Power", LEVEL_ITEM, -80.0, pList[i].dPower, 33.0, CTDUtility::TD_BAND_NAME[nBandIndex],
			pList[i].nChannel, "dBm", "Index = 0x%X", pList[i].nGainIndex);
	}
	
    CHKRESULT(SP_tdActive(m_pImp->m_hDUT, FALSE));
	return SP_OK;
}