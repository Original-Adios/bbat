#include "StdAfx.h"
#include <cassert>
#include "GPSApiAT.h"
#include "GpsTestSysMslt.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CGpsTestSysMslt)

CGpsTestSysMslt::CGpsTestSysMslt(void)
{
}

CGpsTestSysMslt::~CGpsTestSysMslt(void)
{
}

BOOL CGpsTestSysMslt::LoadXMLConfig(void)
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

SPRESULT CGpsTestSysMslt::__PollAction(void)
{
    return __super::__PollAction();
}

SPRESULT CGpsTestSysMslt::GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam)
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
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));
	SetRepairItem($REPAIR_ITEM_COMMUNICATION);

	GPS_SetBand(pGPSMeasParam->nGpsBand);//待开发实现

	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_EnableMslt(MSLT_STOP, pGPSMeasParam));
	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_EnableMslt(MSLT_OPEN, pGPSMeasParam));

	CHKRESULT(TestMsltSNR(pGPSMeasParam));

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_EnableMslt(MSLT_STOP, pGPSMeasParam));
	return SP_OK;
}



