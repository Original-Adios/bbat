#include "StdAfx.h"
#include "GsmGs.h"
#include "ImpBase.h"
#include "gsmUtility.h"
#include <list>
#include <algorithm>
using namespace std;

CGsmGs::CGsmGs(CImpBase* pSpat)
	: CGsmBase(pSpat)
{
}

CGsmGs::~CGsmGs(void)
{
}

SPRESULT CGsmGs::Run()
{
	ConfigFreq();
	m_pImp->m_gs.common.nGsmCnt = 0;
	CHKRESULT(InitCal());
	BOOL bFindAnyBand = FALSE;
	list<double>::iterator it = m_listFreq.begin();
	for (; it != m_listFreq.end(); ++it)
	{
		double dFreq = *it;
		
		bFindAnyBand = FALSE;
		int nTargetBand = BI_GSM_850;
		for (int nBand = BI_GSM_850; nBand < BI_GSM_MAX_BAND; nBand++)
		{
			if (IN_RANGE(CgsmUtility::nFreq[nBand][0][0],
				dFreq,
				CgsmUtility::nFreq[nBand][0][1]))
			{
				bFindAnyBand = TRUE;
				nTargetBand = nBand;
				break;
			}
		}

		if (!bFindAnyBand)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "The frequency table is invalid");
			return SP_E_SPAT_LOSS_FREQ_TABLE_IVALID;
		}

		RF_ANT_E eAnt = RF_ANT_1st;
		CHKRESULT(LoadBandAntInfo(nTargetBand, eAnt));
		CHKRESULT(AntSwitch(eAnt));

		double dPwr = 0.0;
		int nArfcn = CgsmUtility::MHz2Arfch((SP_BAND_INFO)nTargetBand, TRUE, dFreq);
		MeaPower((SP_BAND_INFO)nTargetBand, nArfcn, m_pImp->m_nGsmPcl[nTargetBand], dPwr);

		m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].nBand = (int16)nTargetBand;
		m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].nArfcn = (int16)nArfcn;
		for (int j = 0; j < DUL_ANT; j++)
		{
			m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].data[j].nExpPwr = (int16)(dPwr * 100);
			m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].data[j].nPCL = (uint8)m_pImp->m_nGsmPcl[nTargetBand];
		}
		m_pImp->m_gs.common.nGsmCnt++;
		m_pImp->NOTIFY("GSM GS", LEVEL_ITEM, 0, dPwr, 35, CgsmUtility::GSM_BAND_NAME[nTargetBand], nArfcn, "dBm", "PCL = %d;Freq = %.1lf", m_pImp->m_nGsmPcl[nTargetBand], dFreq);//金板制作添加频点打印 by zzq 2021.8.17  ;Freq = %.1l
		if (dPwr > 35.0 || dPwr < 0)
		{
			return SP_E_SPAT_OUT_OF_RANGE;
		}
	}
	CHKRESULT(UninitCal());

	if (!bFindAnyBand)
	{
		return SP_E_SPAT_LOSS_FREQ_IVALID;
	}
	return SP_OK;
}

void CGsmGs::ConfigFreq()
{
	m_listFreq.clear();
	for (int i = 0; i < BI_GSM_MAX_BAND; i++)
	{
		if (!m_pImp->m_bGsmBandNum[i])
		{
			continue;
		}
		double dLow = CgsmUtility::nFreq[i][0][0];
		double dHigh = CgsmUtility::nFreq[i][0][1];
		double dMid = (dLow + dHigh) / 2;

		m_listFreq.push_back(dLow);
		m_listFreq.push_back(dMid);
		m_listFreq.push_back(dHigh);
	}
	m_listFreq.sort();
}

SPRESULT CGsmGs::MeaPower(SP_BAND_INFO nBand, int nArfcn, int nPcl, double& dPwr)
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

	if (spRes == SP_OK)
	{
		dPwr = txp.dAvgPwr;
	}

	CHKRESULT(TxOn(nBand, nArfcn, nPcl, FALSE));

	CHKRESULT(m_pImp->m_pRFTester->EndTest(TI_PWR));

	return SP_OK;
}

SPRESULT CGsmGs::InitCal()
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

SPRESULT CGsmGs::TxOn(SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn)
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

SPRESULT CGsmGs::UninitCal()
{
	CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, FALSE));
	return SP_OK;
}

SPRESULT CGsmGs::Init()
{
	return SP_OK;
}

SPRESULT CGsmGs::Release()
{
	return SP_OK;
}