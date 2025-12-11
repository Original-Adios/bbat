#include "StdAfx.h"
#include "DMRLossBase.h"
#include "ImpBase.h"

CDMRLossBase::CDMRLossBase(CImpBase *pSpat)
	: ILossFunc(pSpat)
{
}


CDMRLossBase::~CDMRLossBase(void)
{
}

SPRESULT CDMRLossBase::MeaPower( double dFreq, int nPcl ,double dTargetPwr, double &dPwr )
{
	if (m_pImp->m_bDMRCalSelected)
	{
		CHKRESULT(MeaPowerByCal(dFreq, nPcl, dTargetPwr, dPwr));
	}
	else 
	{
		CHKRESULT(MeaPowerByFt(dFreq, nPcl, dTargetPwr, dPwr));
	}
	return SP_OK;
}

SPRESULT CDMRLossBase::MeaPowerByCal( double dFreq, int nPcl ,double dTargetPwr, double &dPwr )
{
	CHKRESULT(SP_dmrStartTxRx(m_pImp->m_hDUT, TRUE, TRUE, DMR_SIGNAL_CW, (UINT)(dFreq * 1000000) , nPcl));
	U_RF_PARAM param;
    param.dUlFreq = dFreq;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param));
	double dExp = dTargetPwr + 5.0;
    param.dExpPwr = dExp;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));
	CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_PWR, NULL));
	PWR_RLT rlt = {0};
	CHKRESULT(m_pImp->m_pRFTester->FetchResult(TI_PWR, (LPVOID)&rlt));
	dPwr = rlt.dAvgPwr;
	CHKRESULT(SP_dmrStartTxRx(m_pImp->m_hDUT, TRUE, FALSE, DMR_SIGNAL_CW, (UINT)(dFreq * 1000000) , nPcl));

	return SP_OK;
}

SPRESULT CDMRLossBase::MeaPowerByFt( double dFreq, int nPcl ,double dTargetPwr, double &dPwr )
{
#define RETRY_CNT (5)
	SPRESULT res = SP_E_DMR_FT_FAIL;
    U_RF_PARAM param;
    param.dUlFreq = dFreq;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param));

	CHKRESULT(SP_dmrStartTxRx(m_pImp->m_hDUT, TRUE, TRUE, DMR_SIGNAL_MODULATION, (UINT)(dFreq * 1000000)  , nPcl));
	Sleep(1000);

	CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_PWR , NULL));
	Sleep(2000);

	PWR_RLT PwrRlt;
	ZeroMemory(&PwrRlt, sizeof(PwrRlt));
	PwrRlt.dAvgPwr = INVALID_NEGATIVE_DOUBLE_VALUE;
	for (int k = 0; k < RETRY_CNT; k++)
	{
		res = m_pImp->m_pRFTester->FetchResult(TI_PWR, (LPVOID)&PwrRlt);
		if (res == SP_OK && IN_RANGE(dTargetPwr - 8, PwrRlt.dAvgPwr, dTargetPwr + 8))
		{
			dPwr  = PwrRlt.dAvgPwr;
			break;
		}
		Sleep(1000);
	}

	CHKRESULT(SP_dmrStartTxRx(m_pImp->m_hDUT, TRUE, FALSE, DMR_SIGNAL_MODULATION, (UINT)(dFreq * 1000000)  , nPcl));

	if ( IN_RANGE(dTargetPwr - 8, PwrRlt.dAvgPwr, dTargetPwr + 8))
	{
		res = SP_OK;
	}
	else
	{
		res = SP_E_DMR_FT_FAIL;
	}

	return res;
}

SPRESULT CDMRLossBase::InitCal()
{
	if (m_pImp->m_bDMRCalSelected)
	{
		CHKRESULT(InitCalByCal());
	}
	else 
	{
		CHKRESULT(InitCalByFt());
	}
	return SP_OK;
}

SPRESULT CDMRLossBase::InitCalByCal()
{
	CHKRESULT(m_pImp->m_pRFTester->SetNetMode(NM_CW));

	CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, NULL));

	RF_PORT rfPort = {RF_ANT_1st, RS_IN};
    U_RF_PARAM param;
    param.pPort = &rfPort;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));

	return SP_OK;
}

SPRESULT CDMRLossBase::InitCalByFt()
{
	CHKRESULT(m_pImp->m_pRFTester->SetNetMode(NM_DMR));

	CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_SIGNAL, SUB_CAL_COMMON, NULL));

	RF_PORT rfPort = {RF_ANT_1st, RS_IN};
    U_RF_PARAM param;
    param.pPort = &rfPort;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));

	RF_PORT rfPort1 = {RF_ANT_2nd, RS_OUT};
    param.pPort = &rfPort1;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));

	return SP_OK;
}

SPRESULT CDMRLossBase::Init()
{

	return SP_OK;
}

 SPRESULT CDMRLossBase::Release()
{

	return SP_OK;
}



