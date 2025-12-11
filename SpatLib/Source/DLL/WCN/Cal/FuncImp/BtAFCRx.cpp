#include "StdAfx.h"
#include "BtAFCRx.h"
#include "BTApiAT.h"
#include "WlanApiAT.h"
#include "SimpleAop.h"

IMPLEMENT_RUNTIME_CLASS(CBtAFCRx)

#define  BT_AFC_DOWNLINK_FREQ  (2402.0)

CBtAFCRx::CBtAFCRx(void)
{
	m_pBtApi = NULL;
	m_pGpsApi = NULL;
	m_bFinal  = FALSE;
	m_bStart = FALSE;
	m_bTSX = FALSE;
//	m_pWlanApi = NULL;
}


CBtAFCRx::~CBtAFCRx(void)
{
}

SPRESULT CBtAFCRx::__InitAction( void )
{
	SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());
	m_pBtApi = new CBTApiAT(m_hDUT, BDR);
	if(NULL == m_pBtApi)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pGpsApi = new CGPSApiAT(m_hDUT);
	if(NULL == m_pGpsApi)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CGPSApiAT failed!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	//m_pWlanApi = new CWlanApiAT(m_hDUT);
	//if(NULL == m_pWlanApi)
	//{
	//	LogFmtStrA(SPLOGLV_ERROR, "new CWlanApiAT failed!");
	//	return SP_E_SPAT_ALLOC_MEMORY;
	//}
	return SP_OK;
}

SPRESULT CBtAFCRx::__FinalAction( void )
{
	SimpleAop(this, __FUNCTION__);
	delete m_pBtApi;
	m_pBtApi = NULL;
	delete m_pGpsApi;
	m_pGpsApi = NULL;
	//delete m_pWlanApi;
	//m_pWlanApi = NULL;
	return SP_OK;
}

SPRESULT CBtAFCRx::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
	SetRepairMode(RepairMode_Bluetooth);
	SetRepairBand($REPAIR_BAND_B_BDR);
	SetRepairItem($REPAIR_ITEM_AFC);
	SHARE_TSX_CONF_T stTsxConf;
	SPRESULT res = GetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf));
	if(SP_OK != res)
	{
		if(!m_bStart)
		{
			LogFmtStrA(SPLOGLV_ERROR, "GetShareMemory fail, you need add TSX Start Action in seq file");
			return res;
		}
		int nType = 0;
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetTestMode(BT_NST, 10000));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_GetXOType(nType));
		if(0 == nType)
		{
			stTsxConf.eCrystal = TCXO;
		}
		else
		{
			if(m_bTSX)
			{
				stTsxConf.eCrystal = TSX;
			}
			else
			{
				stTsxConf.eCrystal = DCXO;
			}
		}
		stTsxConf.stParam = m_stCalParam;
		stTsxConf.stWcnCalData.flag = TF01;
		CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf)));
		if(TCXO == stTsxConf.eCrystal)
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetTestMode(BT_LEAVE, 10000));
		}
	}
	if(TCXO == stTsxConf.eCrystal)
	{
		LogFmtStrA(SPLOGLV_INFO, "XO = TCXO, TSX Cal no need do anything!");
		return SP_OK;
	}
	else if(DCXO == stTsxConf.eCrystal)
	{
		if(stTsxConf.bStep[0])
		{
			LogFmtStrA(SPLOGLV_INFO, "XO = DCXO, AFC is OK!");
			return SP_OK;
		}
	}
	
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetNetMode(NM_CW));
	E_NETWORK_MODE eSubMode = NM_BT;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetProperty(DP_CW_SUBNET, 0 , &eSubMode));
	U_RF_PARAM Param;
	RF_PORT rfPort;
	rfPort.eStatus = RS_OUT;
	rfPort.ePort = (stTsxConf.stParam.ePath == ANT_SHARED) ? RF_ANT_1st:RF_ANT_2nd;
	Param.pPort = &rfPort;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetParameter(PT_RF_PORT, Param));
	Param.dDlFreq = BT_AFC_DOWNLINK_FREQ;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetParameter(PT_DOWNLINK_FREQ, Param));
	Param.dOptPwr = -40.0;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetParameter(PT_OPT_PWR, Param));

	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, NULL));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, TRUE));
	Sleep(100);
	SPRESULT result = SP_OK;
	do 
	{
		//AFC Step
		result = AFC_Step0(stTsxConf);
		if(SP_OK != result)
		{
			break;
		}
		if(DCXO == stTsxConf.eCrystal)
		{
			break;
		}
		//TSX Step
		result = AFC_Step1(stTsxConf);
		if(SP_OK != result)
		{
			break;
		}
		result = AFC_Step2(stTsxConf);
		if(SP_OK != result)
		{
			break;
		}
		result = AFC_Step3(stTsxConf);
		if(SP_OK != result)
		{
			break;
		}
		if(!stTsxConf.bStep[3] && !m_bFinal)
		{
			break;
		}
		result = AFC_Step4(stTsxConf);
		if(SP_OK != result)
		{
			break;
		}
	#pragma warning(disable:4127) 
	} while (FALSE);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pRFTester->SetGen(MM_CW, FALSE));
	return result;
}

BOOL CBtAFCRx::LoadXMLConfig( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_stCalParam.dFerLimit	= GetConfigValue(L"Option:FERLimit", 300);
	m_stCalParam.nStep2TimeOut	= GetConfigValue(L"Option:STEP2_Timeout", 60000);
	m_stCalParam.nStep3TimeOut	= GetConfigValue(L"Option:STEP3_Timeout", 60000);
	m_stCalParam.nStep4TimeOut	= GetConfigValue(L"Option:STEP4_Timeout", 60000);
	m_stCalParam.nStartDac = (uint16)GetConfigValue(L"Param:CDAC_START", 0);
	m_stCalParam.nEndDac = (uint16)GetConfigValue(L"Param:CDAC_END", 63);
	wstring strVal = GetConfigValue(L"Param:RFPath", L"Shared");
	if(strVal.compare(L"StandAlone"))
	{
		m_stCalParam.ePath = ANT_SINGLE;
	}
	else if(strVal.compare(L"Shared"))
	{
		m_stCalParam.ePath = ANT_SHARED;
	}
	else
	{
		m_stCalParam.ePath = ANT_BT_AUTO;
	}
	m_stCalParam.dS1S2Delta = GetConfigValue(L"Param:Limit:STEP1_2_OSC", 3.0);
	m_stCalParam.dS2S3Delta = GetConfigValue(L"Param:Limit:STEP2_3_TSX", 1.0);
	m_stCalParam.dS2S3Delta_2 = GetConfigValue(L"Param:Limit:STEP2_3_DELTA", 2.5);
	m_stCalParam.dS3S4Delta = GetConfigValue(L"Param:Limit:STEP3_4_DELTA", 2.5);
	strVal = GetConfigValue(L"Param:Limit:C1_TSX", L"-0.45,-0.1");
	int nCount = 0;
	double* parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
	if(nCount < 2 || parrDouble[0] > parrDouble[1])
	{
		return FALSE;
	}
	m_stCalParam.dC1TsxLow = parrDouble[0];
	m_stCalParam.dC1TsxUpp = parrDouble[1];
	strVal = GetConfigValue(L"Param:Limit:C1_OSC", L"-0.03,0.03");
	parrDouble = GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
	if(nCount < 2 || parrDouble[0] > parrDouble[1])
	{
		return FALSE;
	}
	m_stCalParam.dC1OscLow = parrDouble[0];
	m_stCalParam.dC1OscUpp = parrDouble[1];
	m_bFinal = GetConfigValue(L"Param:TSX_Final", FALSE);
	m_bStart = GetConfigValue(L"Param:TSX_Start", FALSE);

	m_bTSX = GetConfigValue(L"Param:TSX_Enable", FALSE);
	return TRUE;
}

SPRESULT CBtAFCRx::AFC_Step0( SHARE_TSX_CONF_T &stTsxConf )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if(stTsxConf.bStep[0])
	{
		return SP_OK;
	}
	int32 nFer = 0;
	uint16 nDac = 0;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetRfPath(stTsxConf.stParam.ePath));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_AFCCalibrate(stTsxConf.stParam.nStartDac, stTsxConf.stParam.nEndDac, nFer, nDac));
	double dFer = nFer/1000.0;
	NOTIFY_CAL_RESULT_WITH_ARG("BT AFC FER", LEVEL_ITEM, -stTsxConf.stParam.dFerLimit, dFer, stTsxConf.stParam.dFerLimit, CwcnUtility::BT_BAND_NAME[BDR], 
		-1, "KHZ", "DAC=%d", nDac);
	if(!IN_RANGE(-1.0*stTsxConf.stParam.dFerLimit, dFer, stTsxConf.stParam.dFerLimit))
	{
		return SP_E_WCN_AFC_OVERRANGE;
	}
	stTsxConf.stWcnCalData.dac = nDac;
	CHKRESULT_WITH_NOTIFY_FUNNAME(SP_apSaveRawWcnData(m_hDUT, &stTsxConf.stWcnCalData, sizeof(stTsxConf.stWcnCalData)));
	stTsxConf.bStep[0] = TRUE;
	CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf)));
	if(DCXO == stTsxConf.eCrystal)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetTestMode(BT_LEAVE, 10000));
	}
	return SP_OK;
}

SPRESULT CBtAFCRx::AFC_Step1( SHARE_TSX_CONF_T &stTsxConf )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if(stTsxConf.bStep[1])
	{
		return SP_OK;
	}
	double dTsxTmp = 0.0;
	double dOscTmp = 0.0;
	int32 nFer = 0;
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pGpsApi->DUT_SetTestMode(GPS_TSX, 20000));
	CHKRESULT(GetTemp(dTsxTmp, dOscTmp));
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_GetFer(nFer));
	NOTIFY_CAL_RESULT_WITH_ARG("TSX FER1", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
		-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, dOscTmp);
	double dFerppm = nFer/BT_AFC_DOWNLINK_FREQ;
	stTsxConf.stTsxData[0].freq_error = int32(dFerppm*10000);//ppm*10000
	stTsxConf.stTsxData[0].tsxTemp = uint32(dTsxTmp*100);
	stTsxConf.stTsxData[0].oscTemp = uint32(dOscTmp*100);
	LogFmtStrA(SPLOGLV_INFO, "TSX FER(ppm) %.5f, TSX Temp=%.2f;OSC Temp=%.2f", dFerppm, dTsxTmp, dOscTmp);
	CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetWlanPA(TRUE));
	stTsxConf.bStep[1] = TRUE;
	CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf)));
	return SP_OK;
}

SPRESULT CBtAFCRx::AFC_Step2( SHARE_TSX_CONF_T &stTsxConf )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if(stTsxConf.bStep[2])
	{
		return SP_OK;
	}
	double dTsxTmp = 0.0;
	double doscTmp = 0.0;
	int32 nFer = 0;
	double T12 = (int32)stTsxConf.stTsxData[0].oscTemp/100.0;
	double dOscTmpDelta = 0;
	CSPTimer timer;
	do 
	{
		if (_IsUserStop())
		{ 
			return SP_E_USER_ABORT;
		}
		CHKRESULT(GetTemp(dTsxTmp, doscTmp));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_GetFer(nFer));
		dOscTmpDelta = doscTmp - T12;
		LogFmtStrA(SPLOGLV_INFO, "TSX FER %d, TSX Temp=%.2f;OSC Temp=%.2f, Delta=%.2f", nFer, dTsxTmp, doscTmp, dOscTmpDelta);
		if(IN_RANGE(stTsxConf.stParam.dS1S2Delta, dOscTmpDelta, NOUPPLMT))
		{
			NOTIFY_CAL_RESULT_WITH_ARG("TSX FER2", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
				-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, doscTmp);
			NOTIFY_CAL_RESULT("TSX Step2", LEVEL_ITEM, stTsxConf.stParam.dS1S2Delta, dOscTmpDelta, NOUPPLMT, NULL, -1, NULL);
			double dFerppm = nFer/BT_AFC_DOWNLINK_FREQ;
			stTsxConf.stTsxData[1].freq_error = int32(dFerppm*10000);//ppm*10000
			stTsxConf.stTsxData[1].tsxTemp = uint32(dTsxTmp*100);
			stTsxConf.stTsxData[1].oscTemp = uint32(doscTmp*100);
			LogFmtStrA(SPLOGLV_INFO, "TSX FER(ppm) %.5f, TSX Temp=%.2f;OSC Temp=%.2f", dFerppm, dTsxTmp, doscTmp);
			stTsxConf.bStep[2] = TRUE;
			CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf)));
			break;
		}
		/*if(!m_bFinal)
		{
		break;
		}*/
		Sleep(300);
	} while (!timer.IsTimeOut(stTsxConf.stParam.nStep2TimeOut));
	//if(m_bFinal)
	{
		if(!IN_RANGE(stTsxConf.stParam.dS1S2Delta, dOscTmpDelta, NOUPPLMT))
		{
			NOTIFY_CAL_RESULT_WITH_ARG("TSX FER2", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
				-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, doscTmp);
			NOTIFY_CAL_RESULT("TSX Step2", LEVEL_ITEM, stTsxConf.stParam.dS1S2Delta, dOscTmpDelta, NOUPPLMT, NULL, -1, NULL);
			LogFmtStrA(SPLOGLV_ERROR, "TSX Cal Step2 osc temperature rise less than %.2f C", stTsxConf.stParam.dS1S2Delta);
			return SP_E_WCN_TSX_FAIL;
		}
	}
	return SP_OK;
}

SPRESULT CBtAFCRx::AFC_Step3( SHARE_TSX_CONF_T &stTsxConf )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if(stTsxConf.bStep[3])
	{
		return SP_OK;
	}

	double dTsxTmp = 0.0;
	double doscTmp = 0.0;
	int32 nFer = 0;
	double T21 = (int32)stTsxConf.stTsxData[1].tsxTemp/100.0;
	double T22 = (int32)stTsxConf.stTsxData[1].oscTemp/100.0;
	double T11 = (int32)stTsxConf.stTsxData[0].tsxTemp/100.0;
	double T12 = (int32)stTsxConf.stTsxData[0].oscTemp/100.0;
	CSPTimer timer;
	double dTsxTmpDelta = 0;
	double dDelta = 0;
	do 
	{
		if (_IsUserStop())
		{ 
			return SP_E_USER_ABORT;
		}
		CHKRESULT(GetTemp(dTsxTmp, doscTmp));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_GetFer(nFer));
		dTsxTmpDelta = dTsxTmp - T21;
		dDelta = abs((T21-T11)*(doscTmp-T22) - (T22-T12)*(dTsxTmp-T21));
		LogFmtStrA(SPLOGLV_INFO, "TSX FER %d, TSX Temp=%.2f;OSC Temp=%.2f, Delta=%.2f", nFer, dTsxTmp, doscTmp, dTsxTmpDelta);
		LogFmtStrA(SPLOGLV_INFO, "T11=%.2f, T12=%.2f, T21=%.2f, T22=%.2f, T31=%.2f, T32=%.2f, Delta=%.2f;", 
			T11, T12, T21, T22, dTsxTmp, doscTmp, dDelta);

		if(IN_RANGE(stTsxConf.stParam.dS2S3Delta, dTsxTmpDelta, NOUPPLMT) &&
			IN_RANGE(stTsxConf.stParam.dS2S3Delta_2, dDelta, NOUPPLMT))
		{
			NOTIFY_CAL_RESULT_WITH_ARG("TSX FER3", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
				-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, doscTmp);
			NOTIFY_CAL_RESULT("TSX Step3_1", LEVEL_ITEM, stTsxConf.stParam.dS2S3Delta, dTsxTmpDelta, NOUPPLMT, NULL, -1, NULL);
			NOTIFY_CAL_RESULT("TSX Step3_2", LEVEL_ITEM, stTsxConf.stParam.dS2S3Delta_2, dDelta, NOUPPLMT, NULL, -1, NULL);
			double dFerppm = nFer/BT_AFC_DOWNLINK_FREQ;
			stTsxConf.stTsxData[2].freq_error = int32(dFerppm*10000);//ppm*10000
			stTsxConf.stTsxData[2].tsxTemp = uint32(dTsxTmp*100);
			stTsxConf.stTsxData[2].oscTemp = uint32(doscTmp*100);
			LogFmtStrA(SPLOGLV_INFO, "TSX FER(ppm) %.5f, TSX Temp=%.2f;OSC Temp=%.2f", dFerppm, dTsxTmp, doscTmp);
			stTsxConf.bStep[3] = TRUE;
			CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf)));
			CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetWlanPA(FALSE));
			break;
		}
		if(!m_bFinal)
		{
			break;
		}
		Sleep(300);
	} while (!timer.IsTimeOut(stTsxConf.stParam.nStep3TimeOut));
	if(m_bFinal)
	{
		if(!IN_RANGE(stTsxConf.stParam.dS2S3Delta, dTsxTmpDelta, NOUPPLMT) || 
			!IN_RANGE(stTsxConf.stParam.dS2S3Delta_2, dDelta, NOUPPLMT))
		{
			NOTIFY_CAL_RESULT_WITH_ARG("TSX FER3", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
				-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, doscTmp);
			NOTIFY_CAL_RESULT("TSX Step3_1", LEVEL_ITEM, stTsxConf.stParam.dS2S3Delta, dTsxTmpDelta, NOUPPLMT, NULL, -1, NULL);
			NOTIFY_CAL_RESULT("TSX Step3_2", LEVEL_ITEM, stTsxConf.stParam.dS2S3Delta_2, dDelta, NOUPPLMT, NULL, -1, NULL);
			LogFmtStrA(SPLOGLV_ERROR, "TSX Cal Step3 tsx temperature rise less than %.2f C or delta less than %.2f", 
				stTsxConf.stParam.dS2S3Delta, stTsxConf.stParam.dS2S3Delta_2);
			return SP_E_WCN_TSX_FAIL;
		}
	}

	return SP_OK;
}

SPRESULT CBtAFCRx::AFC_Step4( SHARE_TSX_CONF_T &stTsxConf )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	if(stTsxConf.bStep[4])
	{
		return SP_OK;
	}

	double dTsxTmp = 0.0;
	double doscTmp = 0.0;
	int32 nFer = 0;
	double T21 = (int32)stTsxConf.stTsxData[1].tsxTemp/100.0;
	double T22 = (int32)stTsxConf.stTsxData[1].oscTemp/100.0;
	double T31 = (int32)stTsxConf.stTsxData[2].tsxTemp/100.0;
	double T32 = (int32)stTsxConf.stTsxData[2].oscTemp/100.0;
	double dDelta = 0;
	CSPTimer timer;
	do 
	{
		if (_IsUserStop())
		{ 
			return SP_E_USER_ABORT;
		}
		CHKRESULT(GetTemp(dTsxTmp, doscTmp));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_GetFer(nFer));

		dDelta = abs((T31-T21)*(doscTmp-T32)-(T32-T22)*(dTsxTmp-T31));
		LogFmtStrA(SPLOGLV_INFO, "TSX FER %d, TSX Temp=%.2f;OSC Temp=%.2f", nFer, dTsxTmp, doscTmp);
		LogFmtStrA(SPLOGLV_INFO, "T21=%.2f, T22=%.2f, T31=%.2f, T32=%.2f, T41=%.2f, T42=%.2f, Delta=%.2f;", 
			T21, T22, T31, T32, dTsxTmp, doscTmp, dDelta);

		if( IN_RANGE(stTsxConf.stParam.dS3S4Delta, dDelta, NOUPPLMT))
		{
			NOTIFY_CAL_RESULT_WITH_ARG("TSX FER4", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
				-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, doscTmp);
			NOTIFY_CAL_RESULT("TSX Step4", LEVEL_ITEM, stTsxConf.stParam.dS3S4Delta, dDelta, NOUPPLMT, NULL, -1, NULL);
			double dFerppm = nFer/BT_AFC_DOWNLINK_FREQ;
			stTsxConf.stTsxData[3].freq_error = int32(dFerppm*10000);//ppm*10000
			stTsxConf.stTsxData[3].tsxTemp = uint32(dTsxTmp*100);
			stTsxConf.stTsxData[3].oscTemp = uint32(doscTmp*100);
			LogFmtStrA(SPLOGLV_INFO, "TSX FER(ppm) %.5f, TSX Temp=%.2f;OSC Temp=%.2f", dFerppm, dTsxTmp, doscTmp);
			stTsxConf.bStep[4] = TRUE;
			CHKRESULT_WITH_NOTIFY_FUNNAME(SetShareMemory(ShareMemory_TSX_CONFIG, &stTsxConf, sizeof(stTsxConf)));
			break;
		}
		//if(!m_bFinal)
		//{
		//	break;
		//}
		Sleep(300);
	} while (!timer.IsTimeOut(stTsxConf.stParam.nStep4TimeOut));

//	if(m_bFinal)
	{
		if(!IN_RANGE(stTsxConf.stParam.dS3S4Delta, dDelta, NOUPPLMT))
		{
			NOTIFY_CAL_RESULT_WITH_ARG("TSX FER4", LEVEL_ITEM, NOLOWLMT, nFer/1000.0, NOUPPLMT, CwcnUtility::BT_BAND_NAME[BDR], 
				-1, "KHZ", "TSX Temp=%.2f;OSC Temp=%.2f", dTsxTmp, doscTmp);
			NOTIFY_CAL_RESULT("TSX Step4", LEVEL_ITEM, stTsxConf.stParam.dS3S4Delta, dDelta, NOUPPLMT, NULL, -1, NULL);
			LogFmtStrA(SPLOGLV_ERROR, "TSX Cal Step4 delta less than %.2f C", stTsxConf.stParam.dS2S3Delta);
			return SP_E_WCN_TSX_FAIL;
		}
	}
	if(stTsxConf.bStep[4])
	{
		double T41 = (int32)stTsxConf.stTsxData[3].tsxTemp/100.0;
		double T42 = (int32)stTsxConf.stTsxData[3].oscTemp/100.0;
		double F2 = (int32)stTsxConf.stTsxData[1].freq_error/10000.0;
		double F3 = (int32)stTsxConf.stTsxData[2].freq_error/10000.0;
		double F4 = (int32)stTsxConf.stTsxData[3].freq_error/10000.0;
		double DET = T31*T42 + T22*T41 + T21*T32 - T31*T22 - T21*T42 - T41*T32;
		double A12 = T32 - T42;
		double A13 = T41 - T31;
		double A22 = T42 - T22;
		double A23 = T21 - T41;
		double A32 = T22 - T32;
		double A33 = T31 - T21;
		double C1_TSX = (A12*F2 + A22*F3 + A32*F4)/DET;
		double C1_OSC = (A13*F2 + A23*F3 + A33*F4)/DET;
		LogFmtStrA(SPLOGLV_INFO, "T21=%.2f, T22=%.2f, T31=%.2f, T32=%.2f, T41=%.2f, T42=%.2f;", 
			T21, T22, T31, T32, T41, T42);
		LogFmtStrA(SPLOGLV_INFO, "F2=%.5f, F3=%.5f, F4=%.5f;", F2, F3, F4);
		LogFmtStrA(SPLOGLV_INFO, "A12=%.2f, A13=%.2f, A22=%.2f, A23=%.2f, A32=%.2f A33=%.2f;", 
			A12, A13, A22, A23, A32, A33);

		NOTIFY_CAL_RESULT("C1_TSX", LEVEL_ITEM, stTsxConf.stParam.dC1TsxLow, C1_TSX, stTsxConf.stParam.dC1TsxUpp, NULL, -1, NULL);
		NOTIFY_CAL_RESULT("C1_OSC", LEVEL_ITEM, stTsxConf.stParam.dC1OscLow, C1_OSC, stTsxConf.stParam.dC1OscUpp, NULL, -1, NULL);

		if(!IN_RANGE(stTsxConf.stParam.dC1TsxLow, C1_TSX, stTsxConf.stParam.dC1TsxUpp))
		{
			LogFmtStrA(SPLOGLV_ERROR, "TSX Cal Step4 C1_TSX over range: %.2f, %.2f, %.2f",stTsxConf.stParam.dC1TsxLow, C1_TSX, stTsxConf.stParam.dC1TsxUpp);
			return SP_E_WCN_TSX_FAIL;
		}
		if(!IN_RANGE(stTsxConf.stParam.dC1OscLow, C1_OSC, stTsxConf.stParam.dC1OscUpp))
		{
			LogFmtStrA(SPLOGLV_ERROR, "TSX Cal Step4 C1_OSC over range: %.2f, %.2f, %.2f",stTsxConf.stParam.dC1OscLow, C1_OSC, stTsxConf.stParam.dC1OscUpp);
			return SP_E_WCN_TSX_FAIL;
		}
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pBtApi->DUT_SetTestMode(BT_LEAVE, 10000));
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pGpsApi->DUT_SetTestMode(GPS_LEAVE, 10000));
		CHKRESULT_WITH_NOTIFY_FUNNAME(SP_apSaveTsxDataV2(m_hDUT, &stTsxConf.stTsxData[0]));
	}
	
	return SP_OK;
}

SPRESULT CBtAFCRx::GetTemp(double &dTsxTmp, double &dOscTmp)
{
	SimpleAop(this, __FUNCTION__);
	int nRetry = 3;
	BOOL bOk = FALSE;
	do 
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(m_pGpsApi->DUT_GetTemperature(dTsxTmp, dOscTmp));
		if(IN_RANGE(5.0, dTsxTmp, 100.0) && IN_RANGE(5.0, dOscTmp, 100.0))
		{
			bOk = TRUE;
			break;
		}
		Sleep(100);
	} while (nRetry--);
	if(!bOk)
	{
		NOTIFY_CAL_RESULT("TSX Temp", LEVEL_ITEM, 5.0, dTsxTmp, 100.0, NULL, -1, NULL);
		NOTIFY_CAL_RESULT("OSC Temp", LEVEL_ITEM, 5.0, dOscTmp, 100.0, NULL, -1, NULL);
		return SP_E_WCN_TSX_FAIL;
	}
	return SP_OK;
}


