#include "StdAfx.h"
#include <cassert>
#include "GPSApiAT.h"
#include "GpsAutoModeTestSys.h"
#include "SimpleAop.h"


IMPLEMENT_RUNTIME_CLASS(CGpsAutoModeTestSys)
CGpsAutoModeTestSys::CGpsAutoModeTestSys(void)
{
}

CGpsAutoModeTestSys::~CGpsAutoModeTestSys(void)
{
}

BOOL CGpsAutoModeTestSys::LoadXMLConfig(void)
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

	if (TRUE == m_vGpsParam[BAND_BD1].bCheck)
	{
		m_vGpsParam[BAND_BD1].dDlFreq = 1561.098;
		m_vGpsParam[BAND_BD1].nGpsBand = BAND_BD1;
	}

	if (TRUE == m_vGpsParam[BAND_GLO1].bCheck)
	{
		m_vGpsParam[BAND_GLO1].dDlFreq = 1601.7188;
		m_vGpsParam[BAND_GLO1].nGpsBand = BAND_GLO1;
	}

	return TRUE;
}

SPRESULT CGpsAutoModeTestSys::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());
	m_pclGpsApi = new CGPSApiAT(m_hDUT);

	LPCWSTR lpProjectName = GetConfigValue(L"Header:ProjectInfo:ProjectName", L"");
	m_Customize.InitRfSwitch(GetAdjParam().nTASK_ID, SP_GPS, (CSpatBase*)this, lpProjectName);


	return SP_OK;
}

SPRESULT CGpsAutoModeTestSys::__PollAction(void)
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairMode(RepairMode_GPS);
	SPRESULT res = SP_OK;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_GPS));
	int nCpMode = 0;
	CHKRESULT(GpsGetCPMode(nCpMode));

    if (GNSS_CONSTELLATION_GPS & nCpMode)
	{
		CGpsTestSysV3 GpsTestSys;
		CHKRESULT(GpsTestSys.GPS_MeasureEUT((GPSMeasParamChan*)&m_vGpsParam[BAND_GPS]));
		CHKRESULT(GpsTestSys.GPS_MeasureCW((GPSMeasParamChan*)&m_vGpsParam[BAND_GPS]));
	}

	if (GNSS_CONSTELLATION_GPS_L5 & nCpMode)
	{
		CGpsTestSysV3 GpsTestSys;
		CHKRESULT(GpsTestSys.GPS_MeasureEUT((GPSMeasParamChan*)&m_vGpsParam[BAND_GPS5]));
		CHKRESULT(GpsTestSys.GPS_MeasureCW((GPSMeasParamChan*)&m_vGpsParam[BAND_GPS5]));
	}

	if (GNSS_CONSTELLATION_BD2 & nCpMode)
	{
		CGpsTestSysBeiDou GpsTestSys;
		CHKRESULT(GpsTestSys.GPS_MeasureEUT((GPSMeasParamChan*)&m_vGpsParam[BAND_BD1]));
		CHKRESULT(GpsTestSys.GPS_MeasureCW((GPSMeasParamChan*)&m_vGpsParam[BAND_BD1]));
	}
	if (GNSS_CONSTELLATION_GLO & nCpMode)
	{
		CGpsTestSysGlonass GpsTestSys;
		CHKRESULT(GpsTestSys.GPS_MeasureEUT((GPSMeasParamChan*)&m_vGpsParam[BAND_GLO1]));
		CHKRESULT(GpsTestSys.GPS_MeasureCW((GPSMeasParamChan*)&m_vGpsParam[BAND_GLO1]));

	}

	return SP_OK;
}