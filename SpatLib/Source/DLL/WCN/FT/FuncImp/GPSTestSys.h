#pragma once
#include "../ImpBase.h"
#include "CustomizedBase.h"


struct GPSMeasSpec
{
	DOUBLE_LIMIT dSNRLimit;
	DOUBLE_LIMIT dCNRLimit;
	DOUBLE_LIMIT dClockDriftLimit;
	DOUBLE_LIMIT dFreqDriftLimit;
	DOUBLE_LIMIT dFreqOffsetLimit;
	void Init()
	{
		dSNRLimit.Init();
		dCNRLimit.Init();
		dClockDriftLimit.Init();
		dFreqDriftLimit.Init();
		dFreqOffsetLimit.Init();
	}
	GPSMeasSpec()
	{
		Init();
	}
};

struct GPSMeasParamChan
{
	int32 nCmdSleep;
	int32 nRetryCount;

	BOOL bCheck;
	BOOL bSNR;
	BOOL bCNR;
	BOOL bModuleCNR;
	BOOL bClockDrift;
	BOOL bFreqDrift;
	BOOL bFreqOffset;
	BOOL  bForceSingleWave;

	RF_ANT_E eAnt;
	int  nRfSwitchAnt;
	GPS_BAND nGpsBand;
	double dDlFreq;

	double dSNRLvl;
	double dCNRLvl;

	GPSMeasSpec Spec;

	void Init()
	{
		nCmdSleep = 1000;
		nRetryCount = 5;

		bCheck = FALSE;
		bSNR = FALSE;
		bCNR = FALSE;
		bModuleCNR = FALSE;
		bClockDrift = FALSE;
		bFreqDrift = FALSE;
		bFreqOffset = FALSE;
		bForceSingleWave = FALSE;

		eAnt = RF_ANT_INVALID;
		nRfSwitchAnt = 0;
		nGpsBand = BAND_GPS;
		dDlFreq = 1575.42;

		dSNRLvl = -130;
		dCNRLvl = -115;
	}
	GPSMeasParamChan()
	{
		Init();
	}
};

class CGpsTestSys : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CGpsTestSys)

protected:
	CGpsTestSys(void);
	virtual ~CGpsTestSys(void);
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __FinalAction(void);

	SPRESULT GPS_ModeEnable(GPS_MODE eMode);
	virtual SPRESULT GPS_MeasureEUT(GPSMeasParamChan *pGPSMeasParam);
	virtual SPRESULT GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam);

	SPRESULT TestSNR(GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestClockDrift(GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestFreqDrift(GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestCNR(GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestModuleCNR(GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestFreqOffset(GPSMeasParamChan* pGPSMeasParam);
//	SPRESULT GPS_MeasureClockDrift();
	SPRESULT GPS_SetBand(uint32 bandMode);
	SPRESULT GPS_SetSystemType(int sysType);
	SPRESULT GPS_SetCwFreqID(uint32 bandMode);
	//MSLT
	SPRESULT GPS_EnableMslt(GPS_MSLT_TYPE eType, GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestMsltSNR(GPSMeasParamChan* pGPSMeasParam);
	//
	SPRESULT GsmForceMaxPower(int nSleepTime);
	SPRESULT GpsGetCPMode(int& nCpMode);

	CGPSApiAT   *m_pclGpsApi;
	CCustomized m_Customize;

	static const char g_GpsBand[GPS_MAX_BAND][8];
	vector<GPSMeasParamChan> m_vGpsParam;
};
