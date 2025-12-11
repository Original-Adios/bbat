#include "StdAfx.h"
#include "GsmClc.h"
#include "ImpBase.h"
#include "gsmUtility.h"

CGsmClc::CGsmClc(CImpBase* pSpat)
	: CGsmBase(pSpat)
{
}

CGsmClc::~CGsmClc(void)
{
}

SPRESULT CGsmClc::Run()
{
	CHKRESULT(InitCal());
	RF_CABLE_LOSS_UNIT_EX* pLoss = &(m_pImp->m_lossVal.gsmLoss);//{0};
	ZeroMemory(pLoss, sizeof(*pLoss));
	int nCnt = m_pImp->m_gs.common.nGsmCnt;
	for (int i = 0; i < nCnt; i++)
	{
		int nBand = m_pImp->m_gs.gsm[i].nBand;
		int nArfcn = m_pImp->m_gs.gsm[i].nArfcn;
		int nPcl = m_pImp->m_gs.gsm[i].data[0].nPCL;
		double dExp = m_pImp->m_gs.gsm[i].data[0].nExpPwr / 100.0;
		double dPwr = 0.0;

		RF_ANT_E eAnt = RF_ANT_1st;
		CHKRESULT(LoadBandAntInfo(nBand, eAnt));
		CHKRESULT(AntSwitch(eAnt));

		CHKRESULT(MeaPower((SP_BAND_INFO)nBand, nArfcn, nPcl, dPwr));
		pLoss->arrPoint[i].nBand = nBand;
		pLoss->arrPoint[i].uArfcn[RF_IO_TX] = nArfcn;
		pLoss->arrPoint[i].uArfcn[RF_IO_RX] = nArfcn;
		pLoss->arrPoint[i].dFreq[RF_IO_TX] = CgsmUtility::Arfcn2MHz((SP_BAND_INFO)nBand, TRUE, nArfcn);
		pLoss->arrPoint[i].dFreq[RF_IO_RX] = pLoss->arrPoint[i].dFreq[RF_IO_TX];
		for (int j = 0; j < (m_pImp->m_eModemVer == MV_V3 ? MAX_RF_ANT : DUL_ANT); j++)
		{
			pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = dExp - dPwr + m_pImp->m_dPreLoss;
			pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = pLoss->arrPoint[i].dLoss[j][RF_IO_TX];
		}
		pLoss->nCount++;
		m_pImp->NOTIFY("GSM Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper, CgsmUtility::GSM_BAND_NAME[nBand], nArfcn, "db", "PCL = %d;Freq = %.1lf", nPcl, pLoss->arrPoint[i].dFreq);//加个频点打印
		CHKRESULT(m_pImp->CheckLoss(pLoss->arrPoint[i].dLoss[0][RF_IO_TX], "GSM"));
	}

	CHKRESULT(UninitCal());
	return SP_OK;
}

SPRESULT CGsmClc::MeaPower(SP_BAND_INFO nBand, int nArfcn, int nPcl, double& dPwr)
{
	dPwr = INVALID_NEGATIVE_DOUBLE_VALUE;

	CHKRESULT(TxOn(nBand, nArfcn, nPcl, TRUE));
	U_RF_PARAM param;
	param.eBand = SP_BAND_INFO(nBand);
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param));
	param.nUlChan = nArfcn;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_CHAN, param));

	double dExp = CgsmUtility::PCL2dBm(nBand, nPcl);
	param.dExpPwr = dExp;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));

	Sleep(100);

	SPRESULT spRes = SP_OK;
	PWR_RLT txp;
	ZeroMemory((void*)& txp, sizeof(PWR_RLT));
	for (int i = 0; i < 5; i++)
	{
		CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_PWR, NULL));

		spRes = m_pImp->m_pRFTester->FetchResult(TI_PWR, (LPVOID)& txp);

		if (SP_E_RF_OVER_RANGE == spRes)
		{
			dExp += 5.0;
			param.dExpPwr = dExp;
			CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));
			continue;
		}
		else if (SP_E_RF_UNDER_RANGE == spRes)
		{
			dExp -= 5.0;
			param.dExpPwr = dExp;
			CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));
			continue;
		}
		else
		{
			break;
		}
	}

	CHKRESULT(TxOn(nBand, nArfcn, nPcl, FALSE));

	CHKRESULT(m_pImp->m_pRFTester->EndTest(TI_PWR));

	dPwr = txp.dAvgPwr;

	return SP_OK;
}

SPRESULT CGsmClc::InitCal()
{
	CHKRESULT(m_pImp->m_pRFTester->SetNetMode(NM_GSM));

	SP_BAND_INFO nBand = BI_EGSM;
	int nArfcn = 975;
	int nPcl = 5;

	INIT_PARAM  rf;
	rf.eBand = nBand;
	rf.dBcchPower = -70.0;
	rf.dCellPower = -70.0;
	rf.nDlChan = (uint16)nArfcn;
	rf.nUlChan = (uint16)nArfcn;
	rf.dTargetPower = CgsmUtility::PCL2dBm(BI_EGSM, nPcl);
	CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, &rf));
	U_RF_PARAM param;
	param.nUlChan = nArfcn;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_CHAN, param));
	param.nPcl = (uint16)nPcl;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_PCL, param));

	/// Setup DUT
	//     CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, FALSE));
	//     Sleep(100);
	CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, TRUE));

	CHKRESULT(LoadAndCheckFeatureSupport());

	PC_TX_PARAM_T pc;
	pc.training_sequence = 0;
	pc.coding_scheme = TOOL_TX_CODE_MCS1;
	pc.data_type = DSP_TX_TYPE_RANDOM;
	pc.puncture_type = 0;
	SP_gsmSetTxParam(m_pImp->m_hDUT, pc);

	return SP_OK;
}

SPRESULT CGsmClc::TxOn(SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn)
{
	if (bOn)
	{
		CHKRESULT(SP_gsmSetArfcn(m_pImp->m_hDUT, nBand, (uint16)nArfcn));
		CHKRESULT(SP_gsmSetPCL(m_pImp->m_hDUT, nBand, (uint16)nPcl));
		CHKRESULT(SP_gsmTxOn(m_pImp->m_hDUT, nBand, TRUE));
		Sleep(200);
	}
	else
	{
		CHKRESULT(SP_gsmTxOn(m_pImp->m_hDUT, nBand, FALSE));
	}
	return SP_OK;
}

SPRESULT CGsmClc::UninitCal()
{
	CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, FALSE));
	return SP_OK;
}

SPRESULT CGsmClc::Init()
{
	return SP_OK;
}

SPRESULT CGsmClc::Release()
{
	return SP_OK;
}