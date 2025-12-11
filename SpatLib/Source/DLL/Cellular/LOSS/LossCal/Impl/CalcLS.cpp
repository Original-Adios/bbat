#include "StdAfx.h"
#include "CalcLS.h"
#include "GsmClc.h"
#include "TdClc.h"
#include "WcdmaClc.h"
#include "C2KClc.h"
#include "LteClc.h"
#include "DMRClc.h"
#include "WCNCLC.h"
#include "NrAlgoClc.h"

IMPLEMENT_RUNTIME_CLASS( CCalcLS )
CCalcLS::CCalcLS( void )
{
}

CCalcLS::~CCalcLS( void )
{
}

SPRESULT CCalcLS::CheckResult( SPRESULT spRlt )
{
	if ( spRlt != SP_OK )
	{
		CHKRESULT_WITH_NOTIFY(m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, (LPCVOID)&m_lossOrg),
			"Set loss property");
	}
	return spRlt;
}

SPRESULT CCalcLS::__PollAction( void )
{
	if (!m_bGsmSelected &&
		!m_bTdSelected &&
		!m_bWcdmaSelected &&
		!m_bLteSelected &&
		!m_bC2KSelected &&
		!m_bDMRFTSelected &&
		!m_bDMRCalSelected &&
		!m_bWlanSelected &&
		!m_bGpsSelected &&
		!m_bBtSelected &&
		!m_bNrSelected)
	{
		MessageBox( NULL, _T( "没有选择任何制式，请确认你的操作！" ), _T( "提示" ), MB_OK );
		return SP_OK;
	}

	CHKRESULT( CImpBase::__PollAction() );
	CHKRESULT_WITH_NOTIFY( LoadFromPhone(), "Load data from phone" );

	m_lossOrg = m_lossVal;
	ZeroLossValue( m_lossVal );

	AddPreLoss( m_lossVal.gsmLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.wcdmaLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.tdLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.cdmaLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.lteLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.wlanLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.btLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.gpsLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.otherLoss, m_dPreLoss, FALSE );
	AddPreLoss( m_lossVal.nrLoss, m_dPreLoss, FALSE );
	CHKRESULT_WITH_NOTIFY( m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, ( LPCVOID )&m_lossVal ),
						   "Set loss property" );
	NOTIFY( "Pre-Config Loss", LEVEL_ITEM | LEVEL_DEBUG, 1, 1, 1, NULL, -1, "-", "Loss = %.2fdb", m_dPreLoss );


	m_strNegativeLossDetected = "";

	CHKRESULT(RunGsm());
	CHKRESULT(RunTd());
	CHKRESULT(RunWcdma());
	CHKRESULT(RunC2k());
	CHKRESULT(RunLte());
	CHKRESULT(RunNr());
	CHKRESULT(RunDmr());
	CHKRESULT(RunWcn());

	if (m_strNegativeLossDetected.length() > 0)
	{
		CHAR szText[512] = {0};
		CHAR szMsg[512] = {0};
		//Remove the last comma
		UINT nIndex = m_strNegativeLossDetected.find_last_of(",");
		if (m_strNegativeLossDetected.length() - 1 == nIndex)
		{
			m_strNegativeLossDetected = m_strNegativeLossDetected.substr(0, nIndex);
		}

		sprintf_s(szText,"%s has loss less than 0. [ %s 存在线损小于0. ]", m_strNegativeLossDetected.c_str(), m_strNegativeLossDetected.c_str());
		sprintf_s(szMsg, "%s\nPlease confirm whether to write? [ 请确认是否需要写入? ]", szText);

		if (IDNO == MessageBoxA(NULL,szMsg,"Warning",MB_ICONWARNING | MB_YESNO))
		{
			LogFmtStrA( SPLOGLV_ERROR, szText );
			return SP_E_SPAT_LOSS_OUT_OF_RANGE;
		}

		LogFmtStrA( SPLOGLV_WARN, szText );
	}

	CHKRESULT_WITH_NOTIFY( m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, ( LPCVOID )&m_lossOrg ),
						   "Set loss property" );
	CHKRESULT_WITH_NOTIFY( UpgradeLoss(), "Upgrade loss" );

	return SP_OK;
}

BOOL CCalcLS::LoadXMLConfig()
{
	/// Option
	wstring strVal = GetConfigValue( L"GLOBAL:MODEM", L"V2", TRUE );
	if ( 0 == strVal.compare( L"V2" ) )
	{
		m_eModemVer = MV_V2;
	}
	else if ( 0 == strVal.compare( L"V3" ) )
	{
		m_eModemVer = MV_V3;
	}

	wstring strPath = L"Option:NetMode:";
	m_bGsmSelected = ( BOOL )GetConfigValue( ( strPath + L"GSM" ).c_str(), 0 );
	m_bTdSelected = ( BOOL )GetConfigValue( ( strPath + L"TD" ).c_str(), 0 );
	m_bWcdmaSelected = ( BOOL )GetConfigValue( ( strPath + L"WCDMA" ).c_str(), 0 );
	m_bC2KSelected = ( BOOL )GetConfigValue( ( strPath + L"C2K" ).c_str(), 0 );
	m_bLteSelected = ( BOOL )GetConfigValue( ( strPath + L"LTE" ).c_str(), 0 );
	m_bDMRCalSelected = ( BOOL )GetConfigValue( ( strPath + L"DMR-CAL" ).c_str(), 0 );
	m_bDMRFTSelected = ( BOOL )GetConfigValue( ( strPath + L"DMR-FT" ).c_str(), 0 );
	m_bWlanSelected = ( BOOL )GetConfigValue( ( strPath + L"WLAN" ).c_str(), 0 );
	m_bBtSelected = ( BOOL )GetConfigValue( ( strPath + L"BT" ).c_str(), 0 );
	m_bGpsSelected = ( BOOL )GetConfigValue( ( strPath + L"GPS" ).c_str(), 0 );
	m_bNrSelected = (BOOL)GetConfigValue((strPath + L"NR").c_str(), 0);

	m_bLossCheck = ( BOOL )GetConfigValue( L"Option:LossCheck", 0 );
	m_dLossCheckSpec = GetConfigValue( L"Option:LossCheckSpec", 0.3 );

	/// Parameter
	m_dPreLoss      = GetConfigValue( L"Param:PreLoss", 0.0 );

	//Loss_Val_Spec can be configured with one parameter or two parameters
	//one parameter represents the upper limit, and two represents the lower limit and the upper limit.
	int nTokenCnt = 0;
	double*pdVal = NULL;
	strVal = GetConfigValue( L"Param:Loss_Val_Spec", L"" );
	pdVal = GetTokenDoubleW( strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCnt );
	if (nTokenCnt > 1)
	{
		m_dLossValLower = pdVal[0];
		m_dLossValUpper = pdVal[1];
	}
	else
	{
		double dSpec = 3.0;
		if (nTokenCnt == 1)
		{
			dSpec = pdVal[0];
		}
		m_dLossValLower = ( m_dPreLoss - dSpec ) < 0 ? -m_dPreLoss : -dSpec;
		m_dLossValUpper = dSpec;
	}
	if (m_dLossValUpper <= m_dLossValLower)
	{
		return FALSE;
	}

	if(m_bGsmSelected)
	{
		SetupDUTRunMode(RM_CALIBRATION_MODE);
	}
	else if(m_bWcdmaSelected)
	{
		SetupDUTRunMode(RM_WCDMA_CALIBRATION_MODE);
	}
	else if(m_bTdSelected)
	{
		SetupDUTRunMode(RM_TD_CALIBRATION_MODE);
	}
	else if(m_bC2KSelected)
	{
		SetupDUTRunMode(RM_C2K_CALIBRATION_MODE);
	}
	else if(m_bLteSelected)
	{
		SetupDUTRunMode(RM_LTE_CALIBRATION_MODE);
	}
	else if(m_bNrSelected)
	{
		SetupDUTRunMode(RM_NR_CALIBRATION_MODE);
	}

	return TRUE;
}


SPRESULT CCalcLS::__InitAction( void )
{
	m_pGsmFunc = new CGsmClc( this );
	m_pGsmFunc->Init();

	m_pTdFunc = new CTdClc( this );
	m_pTdFunc->Init();

	m_pWcdmaFunc = new CWcdmaClc( this );
	m_pWcdmaFunc->Init();

	m_pC2KFunc = new CC2KClc( this );
	m_pC2KFunc->Init();

	m_pLteFunc = new CLteClc( this );
	m_pLteFunc->Init();

	m_pNrFunc = new CNrAlgoClc(this);
	m_pNrFunc->Init();
	m_pDMRCalFunc = new CDMRClc( this );
	m_pDMRCalFunc->Init();

	m_pDMRFtFunc = new CDMRClc( this );
	m_pDMRFtFunc->Init();

	m_pWCNFunc = new CWCNCLC( this );
	m_pWCNFunc->Init();

	return SP_OK;
}

void CCalcLS::AddPreLoss( RF_CABLE_LOSS_UNIT_EX& loss, double dPreLoss, BOOL bCheckZero )
{
	for ( int i = 0; i < loss.nCount; i++ )
	{
		for ( int j = 0; j < MAX_RF_ANT; j++ )
		{
			for ( int k = 0; k < MAX_RF_IO; k++ )
			{
				if ( bCheckZero && 0.0 == loss.arrPoint[i].dLoss[j][k] )
				{
					;
				}
				else
				{
					loss.arrPoint[i].dLoss[j][k] += dPreLoss;
				}
			}
		}
	}
}

SPRESULT CCalcLS::RunGsm()
{
	if (m_bGsmSelected)
	{
		if (m_gs.common.nGsmCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pGsmFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no gsm golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunWcdma()
{
	if (m_bWcdmaSelected)
	{
		if (m_gs.common.nWcdmaCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_WCDMA_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change WCDMA mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pWcdmaFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no wcdma golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunTd()
{
	if (m_bTdSelected)
	{
		if (m_gs.common.nTdCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_TD_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change TD mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pTdFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no td golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunC2k()
{
	if (m_bC2KSelected)
	{
		if (m_gs.common.nC2KCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_C2K_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change C2K mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pC2KFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no c2k golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunLte()
{
	if (m_bLteSelected)
	{
		if (m_gs.common.usLteCnt > 0)
		{
			m_pLteFunc->LoadBandInfo();
			m_pLteFunc->Deserialization(&m_arrLteInfo);
			SPRESULT spRlt = ChangeMode(RM_LTE_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change LTE mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pLteFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no lte golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunNr()
{
	if (m_bNrSelected)
	{
		if (m_gs.common.usNrCnt > 0)
		{
			m_pNrFunc->Deserialization(&m_arrNrInfo);
			SPRESULT spRlt = SP_OK;
			CHKRESULT(SP_NR_CAL_Active(m_hDUT));
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pNrFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no lte golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunWcn()
{
	if (m_bWlanSelected)
	{
		if (m_gs.common.nWlanCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");
			CInstrumentLock rfLock(m_pRFTester);
			m_pWCNFunc->SetMode(SP_WIFI);
			spRlt = m_pWCNFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no wlan golden calibration data.");
			NOTIFY("Check wlan golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	if (m_bBtSelected)
	{
		if (m_gs.common.nBtCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");
			CInstrumentLock rfLock(m_pRFTester);
			m_pWCNFunc->SetMode(SP_BT);
			spRlt = m_pWCNFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no bt golden calibration data.");
			NOTIFY("Check bt golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	if (m_bGpsSelected)
	{
		if (m_gs.common.nGpsCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");
			CInstrumentLock rfLock(m_pRFTester);
			m_pWCNFunc->SetMode(SP_GPS);
			spRlt = m_pWCNFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no gps golden calibration data.");
			NOTIFY("Check gps golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
SPRESULT CCalcLS::RunDmr()
{
	if (m_bDMRCalSelected)
	{
		if (m_gs.common.nDmrCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_DMR_CALIBRATION_MOD);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change DMR calibration mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pDMRCalFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no dmr golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	if (m_bDMRFTSelected)
	{
		if (m_gs.common.nDmrCnt > 0)
		{
			SPRESULT spRlt = ChangeMode(RM_DMR_SIG_TEST_MOD);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change DMR FT mode");
			CInstrumentLock rfLock(m_pRFTester);
			spRlt = m_pDMRFtFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no dmr golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		}
	}
	return SP_OK;
}
