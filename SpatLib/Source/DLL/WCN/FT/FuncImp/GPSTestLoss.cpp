#include "StdAfx.h"
#include <cassert>
#include "GPSApiAT.h"
#include "GPSTestLoss.h"
#include "wcnUtility.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CGPSTestLoss);

	CGPSTestLoss::CGPSTestLoss(void)
{
	m_Tolernece = 0.0;
	m_dPreLoss = 0.0;
	m_RxLoss_Low =0.0;
	m_RxLoss_Up = 0.0;
	m_LossHelp.m_NetMode = NM_GPS;
}

CGPSTestLoss::~CGPSTestLoss(void)
{
}

SPRESULT CGPSTestLoss::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());
	return SP_OK;
}

BOOL CGPSTestLoss::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	__super::LoadXMLConfig();

	if (TRUE == m_vGpsParam[BAND_GPS].bCheck)
	{
		m_vGpsParam[BAND_GPS].dDlFreq = 1575.42;
		m_vGpsParam[BAND_GPS].nGpsBand = BAND_GPS;
	}

	if (TRUE == m_vGpsParam[BAND_GPS5].bCheck)
	{
		m_vGpsParam[BAND_GPS5].dDlFreq = 1176.45;
		m_vGpsParam[BAND_GPS5].nGpsBand = BAND_GPS5;
	}

	m_dPreLoss = GetConfigValue(L"Option:PreLoss", 10.5);
	m_Tolernece = GetConfigValue(L"Option:Tolernece", 6);
	wstring sLossValue = GetConfigValue(L"Param:Specification:RxLoss", L"0,50");
	INT nTokenCount = 0;


	double *pDoubleLossRlt = CUtility::GetTokenDoubleW(sLossValue.c_str(), DEFAULT_DELIMITER_W, nTokenCount);
	if(nTokenCount < 2)
	{
		return FALSE;
	}
	m_RxLoss_Low = pDoubleLossRlt[0];
	m_RxLoss_Up = pDoubleLossRlt[1];

	return TRUE;
}

SPRESULT CGPSTestLoss::__PollAction( void )
{
	__super::__PollAction();
	m_LossHelp.UpdateLoss(LOSS_MODE_GPS);

	return SP_OK;
}

SPRESULT CGPSTestLoss::GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam)
{
	if (FALSE == pGPSMeasParam->bCNR
		&& FALSE == pGPSMeasParam->bModuleCNR
		&& FALSE == pGPSMeasParam->bFreqOffset)
	{
		return SP_OK;
	}

	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairItem($REPAIR_ITEM_INSTRUMENT);

	GPSParam instrParam;
	instrParam.dVsgLvl = pGPSMeasParam->dCNRLvl;
	instrParam.dDlFreqMHz = pGPSMeasParam->dDlFreq;
	instrParam.eBand = pGPSMeasParam->nGpsBand;
	instrParam.bForceSingleWave = pGPSMeasParam->bForceSingleWave;
	instrParam.eRfPort = pGPSMeasParam->eAnt;

	m_LossHelp.SetLoss(0, 0, pGPSMeasParam->dDlFreq, RF_IO_RX, m_dPreLoss, pGPSMeasParam->eAnt);
	int nRetryCount = 0;
	BOOL bNeedRetry = TRUE;
	int nCnr = -999999;

	do
	{
		if (nRetryCount++ > 20)
		{
			LogRawStrA(SPLOGLV_ERROR, "Gps Over Loss Adjust");
			return SP_E_WCN_GPS_CNR_FAIL;
		}

		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(TRUE, &instrParam));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, TRUE));
		SetRepairItem($REPAIR_ITEM_COMMUNICATION);

		GPS_SetBand((int)pGPSMeasParam->nGpsBand);
		CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_CW));
		if (pGPSMeasParam->bCNR)
		{
			CHKRESULTwithGpsLeave(TestCNR(bNeedRetry, pGPSMeasParam, nCnr));
			LogFmtStrA(SPLOGLV_INFO, "Gps Loss = %d", nCnr);
		}

		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, FALSE));
		CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_LEAVE));

		LogFmtStrA(SPLOGLV_INFO, "Gps Loss = %d", m_LossHelp.m_Loss);
	} while (bNeedRetry);

	NOTIFY("CNR",
		LEVEL_ITEM | LEVEL_FT,
		pGPSMeasParam->Spec.dCNRLimit.low,
		nCnr,
		pGPSMeasParam->Spec.dCNRLimit.upp,
		g_GpsBand[pGPSMeasParam->nGpsBand],
		pGPSMeasParam->dDlFreq,
		"dB");

	NOTIFY("GPS Loss",
		LEVEL_ITEM | LEVEL_FT,
		m_RxLoss_Low,
		m_LossHelp.m_Loss,
		m_RxLoss_Up,
		g_GpsBand[pGPSMeasParam->nGpsBand],
		pGPSMeasParam->dDlFreq,
		"dB");

	if (!IN_RANGE(m_RxLoss_Low,
		m_LossHelp.m_Loss,
		m_RxLoss_Up))
	{
		LogFmtStrA(SPLOGLV_ERROR, "GPS Loss Over");
		return SP_E_WCN_GPS_CNR_FAIL;
	}
	return SP_OK;
}

SPRESULT CGPSTestLoss::TestCNR(BOOL& bNeedRetry, GPSMeasParamChan* pGPSMeasParam, int &nCnr)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	int realsnr = 0;
	int iCount = pGPSMeasParam->nRetryCount;
	//Need send AT+SPGPSTEST=SNR? for starting to search satellite
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetCNR(realsnr));
	Sleep(100);

	if (_IsUserStop())
	{
		return SP_E_USER_ABORT;
	}

	double rSnr = 50;
	nCnr = realsnr;

	CHKRESULT(m_LossHelp.AdjustLoss(rSnr, nCnr, RF_IO_RX, bNeedRetry, m_Tolernece));

	Sleep(pGPSMeasParam->nCmdSleep);

	SetRepairItem($REPAIR_ITEM_CNR);
	return SP_OK;
}