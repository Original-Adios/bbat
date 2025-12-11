#include "StdAfx.h"
#include <cassert>
#include "GPSApiAT.h"
#include "GpsTestSysBeiDou.h"
#include "SimpleAop.h"

/************************************************************************
北斗:
北斗短报文发送L  1615.68±3MHZ
北斗短报文接收S  2491.75±3MHZ
B1：1559.052MHZ - 1591.788MHZ 民用 标称频率1561.098MHZ
B2：1166.220MHZ - 1217.370MHZ 民用
B3：1250.618MHZ - 1286.423MHZ 军用

GPS
L1频段--1575.42MHZ±1.023MHZ
L2频段--1227.6MHZ±1.023MHZ
L5频段--1176.45MHZ±1.023MHZ
目前的单频定位采用的是L1频段，精度可以做到几米。
双频定位采用的是L1 + L5频段，精度可以做到1米内。
L1频段对应北斗的B1频段
L2频段对应北斗的B2I频段
L5频段对应北斗的B2A频段
L6频段对应北斗的B3I频段
************************************************************************/


IMPLEMENT_RUNTIME_CLASS(CGpsTestSysBeiDou)

CGpsTestSysBeiDou::CGpsTestSysBeiDou(void)
//:m_pclGpsApi(NULL)
{

}

CGpsTestSysBeiDou::~CGpsTestSysBeiDou(void)
{
}


BOOL CGpsTestSysBeiDou::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	__super::LoadXMLConfig();

	if (TRUE == m_vGpsParam[BAND_BD1].bCheck)
	{
		m_vGpsParam[BAND_BD1].dDlFreq = 1561.098;
		m_vGpsParam[BAND_BD1].nGpsBand = BAND_BD1;
	}

	return TRUE;
}

SPRESULT CGpsTestSysBeiDou::__PollAction(void)
{
    return __super::__PollAction();
}

SPRESULT CGpsTestSysBeiDou::GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam)
{
    if (FALSE == pGPSMeasParam->bSNR
        && FALSE == pGPSMeasParam->bClockDrift
        )
    {
        return SP_OK;
    }

    CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_SetSystemType(CPMODE_BD2_MASK));

    auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairItem($REPAIR_ITEM_COMMUNICATION);

    GPSParam instrParam;
    instrParam.dVsgLvl = pGPSMeasParam->dSNRLvl;
    instrParam.dDlFreqMHz = pGPSMeasParam->dDlFreq;
    instrParam.eBand = pGPSMeasParam->nGpsBand;
    instrParam.bForceSingleWave = pGPSMeasParam->bForceSingleWave;
    instrParam.eRfPort = pGPSMeasParam->eAnt;

    CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(TRUE, &instrParam));

	SetRepairItem($REPAIR_ITEM_COMMUNICATION);

	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_SetSystemType(CPMODE_BD2_MASK));
	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_SetBand(pGPSMeasParam->nGpsBand));

	if (pGPSMeasParam->bSNR)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));
		CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_EUT));
		CHKRESULTwithGpsLeave(TestSNR(pGPSMeasParam));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, FALSE));
	}

	if (pGPSMeasParam->bFreqDrift)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_EUT));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_MODULATION, TRUE));//Bug 2222098 - clkdrift时释放卫星信号过早
		//CHKRESULTwithGpsLeave(GsmForceMaxPower(5000));
		CHKRESULTwithGpsLeave(TestFreqDrift(pGPSMeasParam));
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

SPRESULT CGpsTestSysBeiDou::GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam)
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

	GPS_SetSystemType(CPMODE_BD2_MASK);
	GPS_SetBand(pGPSMeasParam->nGpsBand);
	GPS_SetCwFreqID(pGPSMeasParam->nGpsBand);
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



