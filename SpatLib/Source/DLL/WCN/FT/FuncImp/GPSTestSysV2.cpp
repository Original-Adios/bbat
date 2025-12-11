#include "StdAfx.h"
#include <cassert>
#include "GPSApiAT.h"
#include "GPSTestSysV2.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CGpsTestSysV2)

CGpsTestSysV2::CGpsTestSysV2(void)
//:m_pclGpsApi(NULL)
{

}

CGpsTestSysV2::~CGpsTestSysV2(void)
{
}


BOOL CGpsTestSysV2::LoadXMLConfig(void)
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

    return TRUE;
}

SPRESULT CGpsTestSysV2::__PollAction(void)
{
    return __super::__PollAction();
}

SPRESULT CGpsTestSysV2::GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam)
{
	if (FALSE == pGPSMeasParam->bSNR
		&& FALSE == pGPSMeasParam->bClockDrift
        )
	{
		return SP_OK;
	}

    auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairItem($REPAIR_ITEM_INSTRUMENT);

    GPSParam instrParam;
    instrParam.dVsgLvl = pGPSMeasParam->dSNRLvl;
    instrParam.dDlFreqMHz = pGPSMeasParam->dDlFreq;
    instrParam.eBand = pGPSMeasParam->nGpsBand;
    instrParam.bForceSingleWave = pGPSMeasParam->bForceSingleWave;
    instrParam.eRfPort = pGPSMeasParam->eAnt;

    CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(TRUE, &instrParam));

    SetRepairItem($REPAIR_ITEM_COMMUNICATION);
    CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_SetBand(pGPSMeasParam->nGpsBand));

    if (pGPSMeasParam->bSNR)
    {
        CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));
        CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_EUT));
        CHKRESULTwithGpsLeave(TestSNR(pGPSMeasParam));
        CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
    }
    if (pGPSMeasParam->bClockDrift)
    {
        CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_EUT));
        CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));//Bug 2222098 - clkdrift时释放卫星信号过早
        CHKRESULTwithGpsLeave(TestClockDrift(pGPSMeasParam));
        CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
    }

    CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_LEAVE));


    return SP_OK;
}

SPRESULT CGpsTestSysV2::GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam)
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

    CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(TRUE, &instrParam));
    CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, TRUE));
    SetRepairItem($REPAIR_ITEM_COMMUNICATION);

    GPS_SetBand((int)pGPSMeasParam->nGpsBand);
    CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_CW));
    if (pGPSMeasParam->bCNR)
    {
        CHKRESULTwithGpsLeave(TestCNR(pGPSMeasParam));
    }

    if (pGPSMeasParam->bModuleCNR)
    {
        CHKRESULTwithGpsLeave(TestModuleCNR(pGPSMeasParam));
    }

    if (pGPSMeasParam->bFreqOffset)
    {
        CHKRESULTwithGpsLeave(TestFreqOffset(pGPSMeasParam));
    }
    CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, FALSE));
    CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_LEAVE));

    return SP_OK;
}