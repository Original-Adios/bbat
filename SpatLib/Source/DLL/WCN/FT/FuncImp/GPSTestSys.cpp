#include "StdAfx.h"
#include <cassert>
#include "GPSApiAT.h"
#include "GPSTestSys.h"
#include "SimpleAop.h"

#include "CalCNR.h"

const char CGpsTestSys::g_GpsBand[GPS_MAX_BAND][8] = { "GPS_L1", "GPS_L5", "BD_B1", "BD_B2", "BD_B3", "GLO_L1" };

IMPLEMENT_RUNTIME_CLASS(CGpsTestSys)

CGpsTestSys::CGpsTestSys(void)
	:m_pclGpsApi(NULL)
{
}

CGpsTestSys::~CGpsTestSys(void)
{
}

SPRESULT CGpsTestSys::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());
	m_pclGpsApi = new CGPSApiAT(m_hDUT);

	LPCWSTR lpProjectName = GetConfigValue(L"Header:ProjectInfo:ProjectName", L"");
	m_Customize.InitRfSwitch(GetAdjParam().nTASK_ID, SP_GPS, (CSpatBase*)this, lpProjectName);
	return SP_OK;
}

BOOL CGpsTestSys::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	__super::LoadXMLConfig();

	m_vGpsParam.clear();
	m_vGpsParam.reserve(GPS_MAX_BAND);

	for (int k = 0; k < GPS_MAX_BAND; k++)
	{
		wstring strTemp = L"Option:";
		strTemp += _A2CW(g_GpsBand[k]);
		GPSMeasParamChan GpsParam;

		GpsParam.eAnt = (RF_ANT_E)_wtoi(GetConfigValue((strTemp + L":ANT").c_str(), L"0"));
		GpsParam.eAnt = (RF_ANT_E)(GpsParam.eAnt - 1);
		if (GpsParam.eAnt < 0 || GpsParam.eAnt > RF_ANT_2nd)
		{
			m_vGpsParam.push_back(GpsParam);
			continue;
		}

		GpsParam.bSNR = GetConfigValue((strTemp + L":ModuleWave:CN0").c_str(), TRUE);
		GpsParam.bClockDrift = GetConfigValue((strTemp + L":ModuleWave:Clock_Drift").c_str(), TRUE);
		GpsParam.bFreqDrift = GetConfigValue((strTemp + L":ModuleWave:Freq_Drift").c_str(), FALSE);
		if (1 == GetConfigValue((strTemp + L":CWWave:CNR").c_str(), 1))
		{
			GpsParam.bCNR = TRUE;
			GpsParam.bModuleCNR = FALSE;
		}
		else if (2 == GetConfigValue((strTemp + L":CWWave:CNR").c_str(), 1))
		{
			GpsParam.bModuleCNR = TRUE;
			GpsParam.bCNR = FALSE;
		}
		else
		{
			GpsParam.bCNR = FALSE;
			GpsParam.bModuleCNR = FALSE;
		}
		GpsParam.bFreqOffset = GetConfigValue((strTemp + L":CWWave:FreqOffset").c_str(), FALSE);

		if (FALSE == GpsParam.bSNR
			&& FALSE == GpsParam.bClockDrift
			&& FALSE == GpsParam.bFreqDrift
			&& FALSE == (GpsParam.bCNR || GpsParam.bModuleCNR)
			&& FALSE == GpsParam.bFreqOffset
			)
		{
			GpsParam.bCheck = FALSE;
			m_vGpsParam.push_back(GpsParam);
			continue;
		}

		GpsParam.bCheck = TRUE;

		GpsParam.nRfSwitchAnt = GetConfigValue(L"Option:RfSwitchAnt", 0);
		GpsParam.bForceSingleWave = (BOOL)GetConfigValue(L"Option:SingleWave", 0);


		GpsParam.nCmdSleep = GetConfigValue(L"Param:CmdSleep", 6000);
		GpsParam.nRetryCount = GetConfigValue(L"Param:RetryCount", 5);

		strTemp = L"Param:";
		strTemp += _A2CW(g_GpsBand[k]);

		//Modulation Wave
		GpsParam.dSNRLvl = GetConfigValue((strTemp + L":MODULATION:RXLVL").c_str(), -130.0);
		wstring strVal = GetConfigValue((strTemp + L":MODULATION:Specification:CN0").c_str(), L"40,44");
		INT nTokenCount = 0;
		INT* pIntRlt = CUtility::GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCount);
		GpsParam.Spec.dSNRLimit.low = pIntRlt[0];
		GpsParam.Spec.dSNRLimit.upp = (nTokenCount >= 2) ? pIntRlt[1] : NOUPPLMT;

		strVal = GetConfigValue((strTemp + L":MODULATION:Specification:Clock_Drift").c_str(), L"-3000, 3000");
		nTokenCount = 0;
		double* pDoubleRlt = CUtility::GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCount);
		if (nTokenCount < 2)
		{
			return FALSE;
		}
		GpsParam.Spec.dClockDriftLimit.low = pDoubleRlt[0];
		GpsParam.Spec.dClockDriftLimit.upp = pDoubleRlt[1];


		strVal = GetConfigValue((strTemp + L":MODULATION:Specification:Freq_Drift").c_str(), L"-7.5, 7.5");
		nTokenCount = 0;
		pDoubleRlt = CUtility::GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCount);
		if (nTokenCount < 2)
		{
			return FALSE;
		}
		GpsParam.Spec.dFreqDriftLimit.low = pDoubleRlt[0];
		GpsParam.Spec.dFreqDriftLimit.upp = pDoubleRlt[1];

		//CW Wave
		GpsParam.dCNRLvl = GetConfigValue((strTemp + L":CW:RXLVL").c_str(), -115.0);

		strVal = GetConfigValue((strTemp + L":CW:Specification:CNR").c_str(), L"53,58");
		nTokenCount = 0;
		pIntRlt = CUtility::GetTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCount);
		GpsParam.Spec.dCNRLimit.low = pIntRlt[0];
		GpsParam.Spec.dCNRLimit.upp = pIntRlt[1];

		strVal = GetConfigValue((strTemp + L":CW:Specification:FreqOffset").c_str(), L"-3000,3000");
		pDoubleRlt = CUtility::GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCount);
		if (nTokenCount < 2)
		{
			return FALSE;
		}
		GpsParam.Spec.dFreqOffsetLimit.low = pDoubleRlt[0];
		GpsParam.Spec.dFreqOffsetLimit.upp = pDoubleRlt[1];

		m_vGpsParam.push_back(GpsParam);
	}

	if (TRUE == m_vGpsParam[BAND_GPS].bCheck)
	{
		m_vGpsParam[BAND_GPS].dDlFreq = 1575.42;
		m_vGpsParam[BAND_GPS].nGpsBand = BAND_GPS;
	}

	return TRUE;
}

SPRESULT CGpsTestSys::__PollAction(void)
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairMode(RepairMode_GPS);
	SPRESULT res = SP_OK;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_GPS));

	for (int k = 0; k < GPS_MAX_BAND; k++)
	{
		if (FALSE == m_vGpsParam[k].bCheck)
		{
			continue;
		}
		//Customer control RF Port
		if (m_vGpsParam[k].nRfSwitchAnt > 0)
		{
			CHKRESULT(m_Customize.SetRfSwitch((GPS_BAND)k, m_vGpsParam[k].nRfSwitchAnt, m_vGpsParam[k].nRfSwitchAnt));
		}
		//if (m_vGpsParam[m_nGpsBand].bSNR || m_vGpsParam[m_nGpsBand].bClockDrift || m_vGpsParam[m_nGpsBand].bFreqDrift)
		{
			CHKRESULT(GPS_MeasureEUT((GPSMeasParamChan*)&m_vGpsParam[k]));
		}
		//if (m_vGpsParam[m_nGpsBand].bCNR || m_vGpsParam[m_nGpsBand].bModuleCNR || m_vGpsParam[m_nGpsBand].bFreqOffset)//
		{
			CHKRESULT(GPS_MeasureCW((GPSMeasParamChan*)&m_vGpsParam[k]));
		}
	}
	return res;
}

void CGpsTestSys::__LeaveAction(void)
{
	m_Customize.ResetRfSwitch();
}

SPRESULT CGpsTestSys::__FinalAction(void)
{
	if (NULL != m_pclGpsApi)
	{
		delete m_pclGpsApi;
		m_pclGpsApi = NULL;
	}

	return SP_OK;
}

SPRESULT CGpsTestSys::GPS_SetSystemType(int sysType)
{
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_SetSystemType(sysType, TIMEOUT_3S));
	return SP_OK;
}

SPRESULT CGpsTestSys::GPS_SetBand(uint32 bandMode)
{
	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_LEAVE));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_SetBand(bandMode));
	return SP_OK;
}

SPRESULT CGpsTestSys::GPS_SetCwFreqID(uint32 bandMode)
{
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_SetCwFreqID(bandMode));
	return SP_OK;
}
SPRESULT CGpsTestSys::GPS_ModeEnable(GPS_MODE eMode)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT result = SP_E_WCN_BASE_ERROR;
	int nCurMode = -1;

	CHKRESULT(m_pclGpsApi->DUT_GetTestMode(nCurMode));
	if (nCurMode == eMode)
	{
		result = SP_OK;
	}
	else
	{
		unsigned long      ulTimeOut = 10000;//(GPS_LEAVE != m_eMode) ? 10000 : 3000;
		unsigned int          nCount = 0;
		const unsigned int MAX_RETRY = 1;

		do
		{
			if (SP_OK == m_pclGpsApi->DUT_SetTestMode(eMode, ulTimeOut))
			{
				Sleep(TIMEOUT_1S);
				result = SP_OK;
				break;
			}
			else
			{
				Sleep(200);
			}

		} while (++nCount < MAX_RETRY);
	}

	return result;
}

SPRESULT CGpsTestSys::GPS_MeasureEUT(GPSMeasParamChan* pGPSMeasParam)
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

SPRESULT CGpsTestSys::TestSNR(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);

	double snr = 0.0;
	int sv_id = 0;
	int sv_num = 0;
	int iCount = pGPSMeasParam->nRetryCount;

	SPRESULT rlt = SP_E_SPAT_TIMEOUT;
	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		/*CHKRESULT_WITH_NOTIFY_FUNNAME*/(m_pclGpsApi->DUT_GetSNR(snr, sv_id, sv_num));
		if (IN_RANGE(pGPSMeasParam->Spec.dSNRLimit.low, snr, pGPSMeasParam->Spec.dSNRLimit.upp))
		{
			snr = (int)(snr + 0.5);
			rlt = SP_OK;
			break;
		}

		Sleep(pGPSMeasParam->nCmdSleep);
	} while (--iCount > 0);


	NOTIFY("CN0", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dSNRLimit.low, snr, pGPSMeasParam->Spec.dSNRLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "dB-Hz");
	SetRepairItem($REPAIR_ITEM_SNR);
	return rlt;
}


SPRESULT CGpsTestSys::GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam)
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
	instrParam.eRfPort = pGPSMeasParam->eAnt;
	instrParam.dVsgLvl = pGPSMeasParam->dCNRLvl;
	instrParam.dDlFreqMHz = pGPSMeasParam->dDlFreq;
	instrParam.eBand = pGPSMeasParam->nGpsBand;

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitTest(TRUE, &instrParam));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, TRUE));
	SetRepairItem($REPAIR_ITEM_COMMUNICATION);
	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_CW));
	if (pGPSMeasParam->bCNR)
	{
		CHKRESULTwithGpsLeave(TestCNR(pGPSMeasParam));
	}

	if (m_vGpsParam[pGPSMeasParam->nGpsBand].bModuleCNR)
	{
		CHKRESULTwithGpsLeave(TestModuleCNR(pGPSMeasParam));
	}

	if (m_vGpsParam[pGPSMeasParam->nGpsBand].bFreqOffset)
	{
		CHKRESULTwithGpsLeave(TestFreqOffset(pGPSMeasParam));
	}

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, FALSE));
	CHKRESULT_WITH_NOTIFY_FUNNAME(GPS_ModeEnable(GPS_LEAVE));
	return SP_OK;
}

SPRESULT CGpsTestSys::TestClockDrift(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairItem($REPAIR_ITEM_CLOCK);
	double dFer = -9999999.0;
	double dFerTmp = 0;
	int iCount = pGPSMeasParam->nRetryCount * 10;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetClockDrift(dFerTmp));
	Sleep(100);
	SPRESULT rlt = SP_E_SPAT_TIMEOUT;
	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetClockDrift(dFerTmp));
		//invalid data
		if (fabs(dFerTmp) < 0.00001)
		{
			Sleep(pGPSMeasParam->nCmdSleep * 2);
			continue;
		}
		else
		{
			if (fabs(dFerTmp) < fabs(dFer))
			{
				dFer = dFerTmp;
			}
		}
		if (IN_RANGE(pGPSMeasParam->Spec.dClockDriftLimit.low, dFer, pGPSMeasParam->Spec.dClockDriftLimit.upp))
		{
			rlt = SP_OK;
			break;
		}
		Sleep(pGPSMeasParam->nCmdSleep * 2);
	} while (--iCount > 0);
	NOTIFY("Clock Drift", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dClockDriftLimit.low, dFer, pGPSMeasParam->Spec.dClockDriftLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "Hz");

	return rlt;
}

SPRESULT CGpsTestSys::TestFreqDrift(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairItem($REPAIR_ITEM_CLOCK);
	double dFer = -9999999.0;
	double dFerTmp = 0;
	int iCount = pGPSMeasParam->nRetryCount * 10;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetFreqDrift(dFerTmp));
	Sleep(100);
	SPRESULT rlt = SP_E_SPAT_TIMEOUT;
	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetFreqDrift(dFerTmp));
		//invalid data
		if (fabs(dFerTmp) < 0.00001)
		{
			Sleep(pGPSMeasParam->nCmdSleep*2);
			continue;
		}
		else
		{
			if (fabs(dFerTmp) < fabs(dFer))
			{
				dFer = dFerTmp;
			}
		}
		if (IN_RANGE(pGPSMeasParam->Spec.dFreqDriftLimit.low, dFer, pGPSMeasParam->Spec.dFreqDriftLimit.upp))
		{
			rlt = SP_OK;
			break;
		}
		Sleep(pGPSMeasParam->nCmdSleep*2);
	} while (--iCount > 0);

	NOTIFY("Freq Drift", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dFreqDriftLimit.low, dFer, pGPSMeasParam->Spec.dFreqDriftLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "Hz");

	return rlt;
}

SPRESULT CGpsTestSys::TestCNR(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);

	int snr = 0;
	double snrAver = 0.0;
	double snrTotal = 0;
	int iCount = pGPSMeasParam->nRetryCount;
	int iPassCount = 0;

	SPRESULT rlt = SP_E_SPAT_TIMEOUT;
	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		/*CHKRESULT_WITH_NOTIFY_FUNNAME*/(m_pclGpsApi->DUT_GetCNR(snr));
		if (IN_RANGE(pGPSMeasParam->Spec.dCNRLimit.low, snr, pGPSMeasParam->Spec.dCNRLimit.upp))
		{
			iPassCount++;
			snrAver += snr;
		}
		snrTotal += snr;

		snr = 0;

		if (iCount > 1)
		{
			Sleep(pGPSMeasParam->nCmdSleep);
		}
	} while (--iCount > 0);


	if (0 != iPassCount)
	{
		snr = int(snrAver / iPassCount + 0.5);
	}
	else
	{
		snr = (int)(snrTotal / pGPSMeasParam->nRetryCount + 0.5);
	}

	if (IN_RANGE(pGPSMeasParam->Spec.dCNRLimit.low, snr, pGPSMeasParam->Spec.dCNRLimit.upp))
	{
		rlt = SP_OK;
	}

	NOTIFY("CNR", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dCNRLimit.low, snr, pGPSMeasParam->Spec.dCNRLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "dB");
	SetRepairItem($REPAIR_ITEM_CNR);
	return rlt;
}


SPRESULT CGpsTestSys::TestModuleCNR(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);

	UINT32 nDataCount = 0;
	switch (pGPSMeasParam->nGpsBand)
	{
	case BAND_GPS:
		nDataCount = MAX_L1_DATA_COUNT;
		break;
	case BAND_GPS5:
		nDataCount = MAX_L5_DATA_COUNT;
		break;
	case BAND_BD1:
	case BAND_BD2:
	case BAND_BD3:
	case BAND_GLO1:
		LogFmtStrA(SPLOGLV_INFO, "%s", "Don't Support BeiDou or Glonass System");
		return SP_OK;
	default:
		break;
	}

	int snr = -999999;
	int iCount = pGPSMeasParam->nRetryCount;
	uint32 u32BufSize = MAX_DIAG_GPS_MODULE_CNR_BUFF_SIZE;
	vector<char> vecCnrBuff(u32BufSize);
	vecCnrBuff.clear();

	uint32 lpu32RecvBytes = 0;
	Sleep(100);
	SPRESULT rlt = SP_E_SPAT_TIMEOUT;
	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}

		rlt = m_pclGpsApi->DUT_GetModuleCNR(vecCnrBuff.data(), u32BufSize, &lpu32RecvBytes);
		if (SP_OK != rlt)
		{
			continue;
		}

		CUtility ut;
		std::string strTmp = vecCnrBuff.data();
		std::string strCnr = ut.trimA(strTmp);
		replace_all_distinct(strCnr, "\r\n", "");
		replace_all_distinct(strCnr, " ", "");

		//calculate
		CCalCNR CalCnr;
		CalCnr.Init(this, nDataCount);
		snr = CalCnr.Calculate_Cw_Data_Capture(strCnr.c_str());

		if (IN_RANGE(pGPSMeasParam->Spec.dCNRLimit.low, snr, pGPSMeasParam->Spec.dCNRLimit.upp))
		{
			rlt = SP_OK;
			break;
		}
		else
		{
			rlt = SP_E_WCN_GPS_CNR_FAIL;
		}
		Sleep(pGPSMeasParam->nCmdSleep);
	} while (--iCount > 0);

	NOTIFY("CNR", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dCNRLimit.low, snr, pGPSMeasParam->Spec.dCNRLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "dB");
	SetRepairItem($REPAIR_ITEM_CNR);
	return rlt;
}


SPRESULT CGpsTestSys::TestFreqOffset(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);

	double dFer = -9999999.0;
	double dFerTmp = 0;
	int iCount = pGPSMeasParam->nRetryCount;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetFreqOffset(dFerTmp));
	Sleep(100);
	SPRESULT rlt = SP_E_SPAT_TIMEOUT;
	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetFreqOffset(dFerTmp));
		//invalid data
		if (fabs(dFerTmp) < 0.00001)
		{
			Sleep(pGPSMeasParam->nCmdSleep);
			continue;
		}
		else
		{
			if (fabs(dFerTmp) < fabs(dFer))
			{
				dFer = dFerTmp;
			}
		}
		if (IN_RANGE(pGPSMeasParam->Spec.dFreqOffsetLimit.low, dFer, pGPSMeasParam->Spec.dFreqOffsetLimit.upp))
		{
			rlt = SP_OK;
			break;
		}
		Sleep(pGPSMeasParam->nCmdSleep);
	} while (--iCount > 0);
	NOTIFY("Freq Offset", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dFreqOffsetLimit.low, dFer, pGPSMeasParam->Spec.dFreqOffsetLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "Hz");
	SetRepairItem($REPAIR_ITEM_CLOCK);
	return rlt;
}

SPRESULT CGpsTestSys::GPS_EnableMslt(GPS_MSLT_TYPE eType, GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	int snr = -999999;
	int sv_id = 0;
	int sv_num = 0;

	if (MSLT_OPEN != eType && MSLT_STOP != eType)
	{
		LogFmtStrA(SPLOGLV_ERROR, "GSLT_TYPE Set eType = %d fail", eType);
		return SP_E_WCN_GPS_MODE_ERROR;
	}

	do
	{
		if (MSLT_OPEN == eType)
		{
			CHKRESULT(m_pclGpsApi->DUT_Mslt_Func(snr, sv_id, sv_num, MSLT_INIT));
			CHKRESULT(m_pclGpsApi->DUT_Mslt_Func(snr, sv_id, sv_num, MSLT_OPEN));
		}
		else
		{
			CHKRESULT(m_pclGpsApi->DUT_Mslt_Func(snr, sv_id, sv_num, MSLT_STOP));
		}


	} while (0);

	return SP_OK;
}

SPRESULT CGpsTestSys::TestMsltSNR(GPSMeasParamChan* pGPSMeasParam)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	int snr = -999999;
	int sv_id = 0;
	int sv_num = 0;
	int iCount = pGPSMeasParam->nRetryCount;
	SPRESULT rlt = SP_E_SPAT_TIMEOUT;

	do
	{
		if (_IsUserStop())
		{
			return SP_E_USER_ABORT;
		}
		SPRESULT sRet = m_pclGpsApi->DUT_Mslt_Func(snr, sv_id, sv_num, MSLT_SNR);
		if (SP_OK == sRet && IN_RANGE(pGPSMeasParam->Spec.dSNRLimit.low, snr, pGPSMeasParam->Spec.dSNRLimit.upp))
		{
			rlt = SP_OK;
			break;
		}
		Sleep(pGPSMeasParam->nCmdSleep);
	} while (--iCount > 0);
	NOTIFY("MsltCN0", LEVEL_ITEM | LEVEL_FT, pGPSMeasParam->Spec.dSNRLimit.low, snr, pGPSMeasParam->Spec.dSNRLimit.upp, g_GpsBand[pGPSMeasParam->nGpsBand], (int)pGPSMeasParam->dDlFreq, "dB-Hz");
	SetRepairItem($REPAIR_ITEM_SNR);
	return rlt;
}

SPRESULT CGpsTestSys::GsmForceMaxPower(int nSleepTime)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_SetNetwork(0));
	CHKRESULTwithNetWorkSet(m_pclGpsApi->DUT_CloseProtocolStack(5));
	CHKRESULTwithNetWorkSet(m_pclGpsApi->DUT_GsmMaxPower(1, TRUE, 62));
	Sleep(nSleepTime);
	CHKRESULTwithNetWorkSet(m_pclGpsApi->DUT_GsmMaxPower(1, FALSE, 0));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_SetNetwork(1));
	return SP_OK;
}

SPRESULT CGpsTestSys::GpsGetCPMode(int &nCpMode)
{
	auto _function = SimpleAop(this, __FUNCTION__);

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pclGpsApi->DUT_GetCPMode(nCpMode));

	return SP_OK;
}