#include "StdAfx.h"
#include "MakeGS.h"
#include "GsmGs.h"
#include "TdGs.h"
#include "WcdmaGs.h"
#include "C2KGs.h"
#include "LteGs.h"
#include "NrAlgoGs.h"
#include "DMRGs.h"
#include "gsmUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "Utility.h"
#include "wcnUtility.h"
#include "WCNGS.h"
#include "NrUtility.h"

IMPLEMENT_RUNTIME_CLASS(CMakeGS)
CMakeGS::CMakeGS(void)
{
}

CMakeGS::~CMakeGS(void)
{
}

BOOL CMakeGS::LoadXMLConfig()
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

	BOOL ret = TRUE;
	/// GSM
	ret = ret && LoadGsmConfig();
	ret = ret && LoadWcdmaConfig();
	ret = ret && LoadTdConfig();
	ret = ret && LoadC2kConfig();
	ret = ret && LoadLteConfig();
	ret = ret && LoadNrConfig();
	ret = ret && LoadWcnConfig();
	ret = ret && LoadDmrConfig();

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
SPRESULT CMakeGS::__PollAction(void)
{
	if (!m_bGsmSelected &&
		!m_bTdSelected &&
		!m_bWcdmaSelected &&
		!m_bC2KSelected &&
		!m_bLteSelected &&
		!m_bNrSelected &&
		!m_bDMRFTSelected &&
		!m_bDMRCalSelected &&
		!m_bWlanSelected &&
		!m_bGpsSelected &&
		!m_bBtSelected)
	{
		MessageBox(NULL, _T("没有选择任何制式，请确认你的操作！"), _T("提示"), MB_OK);
		return SP_OK;
	}
	CHKRESULT(CImpBase::__PollAction());
	SPRESULT Res = RunAll();
	CHKRESULT_WITH_NOTIFY( m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, ( LPCVOID )&m_lossVal ),
						   "Set loss property" );
	return Res;
}
SPRESULT CMakeGS::__InitAction( void )
{
	m_pGsmFunc = new CGsmGs(this);
	m_pGsmFunc->Init();
	m_pWcdmaFunc = new CWcdmaGs(this);
	m_pWcdmaFunc->Init();
	m_pTdFunc = new CTdGs(this);
	m_pTdFunc->Init();
	m_pC2KFunc = new CC2KGs(this);
	m_pC2KFunc->Init();
	m_pLteFunc = new CLteGs(this);
	m_pLteFunc->Init();
	m_pNrFunc = new CNrAlgoGs(this);
	m_pNrFunc->Init();
	m_pDMRCalFunc = new CDMRGs(this);
	m_pDMRCalFunc->Init();
	m_pDMRFtFunc = new CDMRGs(this);
	m_pDMRFtFunc->Init();
	m_pWCNFunc = new CWCNGS(this);
	m_pWCNFunc->Init();
	return SP_OK;
}
BOOL CMakeGS::LoadGsmConfig()
{
	for (int i = 0; i < BI_GSM_MAX_BAND; i++)
	{
		wstring strPath = L"Option:GSM:";
		strPath += _A2CW(CgsmUtility::GSM_BAND_NAME[i]);
		m_bGsmBandNum[i] = (BOOL)GetConfigValue(strPath.c_str(), 0);
		if (m_bGsmBandNum[i])
		{
			m_bGsmSelected = TRUE;
		}
		else
		{
			continue;
		}
		strPath = L"Param:GSM:PCL:";
		strPath += _A2CW(CgsmUtility::GSM_BAND_NAME[i]);
		m_nGsmPcl[i] = GetConfigValue(strPath.c_str(), 0);
	}
	/// WCDMA
	return TRUE;
}
BOOL CMakeGS::LoadWcdmaConfig()
{
	ZeroMemory(m_bWcdmaBandNum, sizeof(m_bWcdmaBandNum));
	for (int i = 0; i < BI_W_MAX_BAND; i++)
	{
		wstring strPath = L"Option:WCDMA:WCDMA1:Band";
		char szBuf[32] = {0};
		sprintf_s(szBuf, "%d", i+1);
		strPath += _A2CW(szBuf);
		m_bWcdmaBandNum[i] = (BOOL)GetConfigValue(strPath.c_str(), 0);
		if (m_bWcdmaBandNum[i])
		{
			m_bWcdmaSelected = TRUE;
			m_eWcdmaBandAnt[i] = RF_ANT_1st;
		}
	}

	for (int i = 0; i < BI_W_MAX_BAND; i++)
	{
		wstring strPath = L"Option:WCDMA:WCDMA2:Band";
		char szBuf[32] = {0};
		sprintf_s(szBuf, "%d", i+1);
		strPath += _A2CW(szBuf);
		BOOL bValue = (BOOL)GetConfigValue(strPath.c_str(), 0);
		if (bValue && (m_bWcdmaBandNum[i] == bValue))
		{
			SendCommonCallback(L"This band(%d) has been selected at group one", (i+1));
			return FALSE;
		}

		if (bValue)
		{
			m_bWcdmaBandNum[i] = TRUE;
			m_bWcdmaSelected = TRUE;
			m_eWcdmaBandAnt[i] = RF_ANT_3rd;
		}

	}
	m_dWcdmaTargetPwr = GetConfigValue(L"Param:WCDMA:UplinkPower", 10.0);
	//TD
	return TRUE;
}
BOOL CMakeGS::LoadTdConfig()
{
	wchar_t BANDNAME[2][2] = {L"A", L"F"};
	for (int i = 0; i < BI_TD_MAX_BAND; i++)
	{
		wstring strPath = L"Option:TD:Band";
		strPath += BANDNAME[i];
		m_bTdBandNum[i] = (BOOL)GetConfigValue(strPath.c_str(), 0);
		if (m_bTdBandNum[i])
		{
			m_bTdSelected = TRUE;
		}
	}
	//C2K
	return TRUE;
}
BOOL CMakeGS::LoadLteConfig()
{
	m_bLteSelected = FALSE;
	std::wstring BaseKey = L"Option:LTE:LTE:";
	for (uint32 i = 0; i < MAX_LTE_BAND; i++)
	{
		if (GetConfigValue((BaseKey + CLteUtility::m_BandInfo[i].NameW).c_str(), FALSE))
		{
			m_bLteSelected = TRUE;
			break;
		}
	}
	return TRUE;
}
BOOL CMakeGS::LoadNrConfig()
{
	m_bNrSelected = FALSE;
	std::wstring BaseKey = L"Option:NR:";
	for (uint32 i = 0; i < MAX_NR_BAND; i++)
	{
		if (GetConfigValue((BaseKey + CNrUtility::m_BandInfo[i].NameW).c_str(), FALSE))
		{
			m_bNrSelected = TRUE;
			break;
		}
	}
	return TRUE;
}
BOOL CMakeGS::LoadC2kConfig()
{
	ZeroMemory(m_bC2KBandNum, sizeof(m_bC2KBandNum));
	for (int i = 0; i < BI_C_MAX_BAND; i++)
	{
		wstring strPath = L"Option:C2K:";
		char szBuf[32] = {0};
		sprintf_s(szBuf, "%s", CCdmaUtility::m_BandInfo[i].NameA);
		strPath += _A2CW(szBuf);
		m_bC2KBandNum[i] = (BOOL)GetConfigValue(strPath.c_str(), 0);
		if (m_bC2KBandNum[i])
		{
			m_bC2KSelected = TRUE;
		}
	}
	m_dC2KTargetPwrConf = GetConfigValue(L"Param:C2K:UplinkPower", 23.0);
	/// LTE
	return TRUE;
}
BOOL CMakeGS::LoadWcnConfig()
{
	BOOL bBandSelected = FALSE;
	BOOL bAntSelected = FALSE;
	BOOL bWlanSwitch = FALSE;
	BOOL bBtSwitch = FALSE;
	ZeroMemory(m_bWlanBandNum, sizeof(m_bWlanBandNum));
	m_bWcnAnt = GetConfigValue(L"Option:WCN:AntennaSwitch", FALSE); //marlin2 = false
	wstring strWlanBand[2] = {L"WLAN2.4GHz", L"WLAN5.0GHz"};
	wstring strWlanAnt[MAX_RF_ANT] = {L"Ant1st", L"Ant2nd", L"Ant3rd", L"Ant4th"};
	for(int i=0; i<2; i++)
	{
		for(int k=0; k<WIFI_MaxProtocol; k++)
		{
			wstring strPath = L"Option:WCN:WLAN:" + strWlanBand[i] + L":" + CUtility::_A2CW(CwcnUtility::WLAN_BAND_NAME[k]);
			m_bWlanBandNum[i][k] = GetConfigValue(strPath.c_str(), FALSE);
			if(m_bWlanBandNum[i][k])
			{
				bBandSelected = TRUE;
			}
		}
	}
	ZeroMemory(m_bWlanAnt, sizeof(m_bWlanAnt));
	for(int i=0; i<2; i++)
	{
		for(int j=0; j< MAX_RF_ANT; j++)
		{
			wstring strPath = L"Option:WCN:WLAN:" + strWlanBand[i]  + L":" + strWlanAnt[j];
			m_bWlanAnt[i][j] = GetConfigValue(strPath.c_str(), FALSE);
			if(m_bWlanAnt[i][j])
			{
				bAntSelected = TRUE;
			}
		}
	}
	m_bWlanSelected = bBandSelected && bAntSelected;
	if(m_bWlanSelected)
	{
		for(int i=0; i<2; i++)
		{
			for(int j=0; j< MAX_RF_ANT; j++)
			{
				if( i > 0 || (RF_ANT_E)j >= RF_ANT_2nd  )
				{
					if(m_bWlanAnt[i][j])
					{
						bWlanSwitch = TRUE;
					}
				}
			}
		}
	}
	bBandSelected = FALSE;
	bAntSelected = FALSE;
	wstring strBTAnt[MAX_RF_ANT] = {L"Shared", L"StandAlone", L"Ant3rd", L"Ant4th"};
	for(int j=BDR; j<MAX_BT_TYPE; j++)
	{
		wstring strPath = L"Option:WCN:BT:BT:";
		strPath += CUtility::_A2CW(CwcnUtility::BT_BAND_NAME[j]);
		m_bBTBandNum[j] = GetConfigValue(strPath.c_str(), FALSE);
		if(m_bBTBandNum[j])
		{
			bBandSelected = TRUE;
		}
	}
	if(m_bBTBandNum[BLE_EX] && m_bBTBandNum[BLE])
	{
		LogFmtStrA(SPLOGLV_ERROR, "BLE BLE5.0 cannot selected same time!");
		SendCommonCallback(L"BLE BLE5.0 cannot selected same time!");
		return FALSE;
	}
	ZeroMemory(m_bBTAnt, sizeof(m_bBTAnt));
	for(int i=0; i< MAX_RF_ANT; i++)
	{
		wstring strPath = L"Option:WCN:BT:" + strBTAnt[i];
		m_bBTAnt[i] = GetConfigValue(strPath.c_str(), FALSE);
		if(m_bBTAnt[i])
		{
			bAntSelected = TRUE;
		}
	}

	m_bBtSelected = bBandSelected && bAntSelected;
	if(m_bBtSelected)
	{
		if(m_bBTBandNum[BLE_EX] || m_bBTAnt[RF_ANT_2nd])
		{
			bBtSwitch = TRUE;
		}
		if(m_bBTBandNum[BLE])
		{
			if(bWlanSwitch || bBtSwitch)
			{
				LogFmtStrA(SPLOGLV_ERROR, "Marlin3 no BLE!");
				SendCommonCallback(L"BLE and WLAN 5G cannot selected same time!");
				return FALSE;
			}
		}
	}
	if(bWlanSwitch || bBtSwitch)
	{
		m_bWcnAnt = TRUE;
	}
	m_dWlanRefLvl = GetConfigValue(L"Param:WLAN:RefLevel", 20.0);
	m_dBtRefLvl = GetConfigValue(L"Param:BT:RefLevel", 20.0);

	m_bGpsSelected = TRUE;
	wstring strPath = L"Option:WCN:GPS";
	m_Gps_Setting[BAND_GPS].bCheck = GetConfigValue((strPath +L":GPS_L1:Check").c_str(), FALSE);
	RF_ANT_E eAnt = (RF_ANT_E) _wtoi(GetConfigValue((strPath + L":GPS_L1:ANT").c_str(), L"0"));
	m_Gps_Setting[BAND_GPS].nGPSAnt = (RF_ANT_E)(eAnt - 1);

	m_Gps_Setting[BAND_GPS5].bCheck = GetConfigValue((strPath + L":GPS_L5:Check").c_str(), FALSE);
	eAnt = (RF_ANT_E)_wtoi(GetConfigValue((strPath + L":GPS_L5:ANT").c_str(), L"1"));
	m_Gps_Setting[BAND_GPS5].nGPSAnt = (RF_ANT_E)(eAnt - 1);

	m_Gps_Setting[BAND_BD1].bCheck = GetConfigValue((strPath + L":BD_B1:Check").c_str(), FALSE);
	eAnt = (RF_ANT_E)_wtoi(GetConfigValue((strPath + L":BD_B1:ANT").c_str(), L"0"));
	m_Gps_Setting[BAND_BD1].nGPSAnt = (RF_ANT_E)(eAnt - 1);

	m_Gps_Setting[BAND_GLO1].bCheck = GetConfigValue((strPath + L":GLO_L1:Check").c_str(), FALSE);
	eAnt = (RF_ANT_E)_wtoi(GetConfigValue((strPath + L":GLO_L1:ANT").c_str(), L"0"));
	m_Gps_Setting[BAND_GLO1].nGPSAnt = (RF_ANT_E)(eAnt - 1);

	if(FALSE == m_Gps_Setting[BAND_GPS].bCheck
		&& FALSE == m_Gps_Setting[BAND_GPS5].bCheck
		&& FALSE == m_Gps_Setting[BAND_BD1].bCheck
		&& FALSE == m_Gps_Setting[BAND_GLO1].bCheck
		)
	{
		m_bGpsSelected = FALSE;
	}
	///
	return TRUE;
}
BOOL CMakeGS::LoadDmrConfig()
{
	wstring strPath = L"Option:DMR:DMR-CAL";
	m_bDMRCalSelected = (BOOL)GetConfigValue(strPath.c_str(), 0);

	strPath = L"Option:DMR:DMR-FT";
	m_bDMRFTSelected = (BOOL)GetConfigValue(strPath.c_str(), 0);

	if(m_bDMRCalSelected || m_bDMRFTSelected )
	{
		strPath = L"Option:DMR_Param:Freq";
		wstring strVal = GetConfigValue(strPath.c_str(), L"");
		int nTokenCnt = 0;
		double* pDouVal= GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nTokenCnt);
		if (nTokenCnt == 0)
		{
			return FALSE;
		}
		else
		{
			m_nDmrFreqCnt = nTokenCnt;
			for (int i = 0; i < nTokenCnt; i++)
			{
				m_dDmrFreq[i] = pDouVal[i];
			}
		}

		strPath = L"Option:DMR_Param:PCL";
		m_nDmrPcl = (int16)GetConfigValue(strPath.c_str(), 0);

		strPath = L"Option:DMR_Param:TargetPower";
		m_dDmrTargetPwr = GetConfigValue(strPath.c_str(), -8.0);

	}

	return TRUE;
}

SPRESULT CMakeGS::RunGsm()
{
	if (!m_bGsmSelected)
	{
		return SP_OK;
	}

	SPRESULT spRlt = SP_OK;
	CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");
	CInstrumentLock rfLock(m_pRFTester);

	spRlt = m_pGsmFunc->Run();
	if (SP_E_SPAT_LOSS_FREQ_IVALID == spRlt)
	{
		int nRlt = MessageBox(NULL, _T("Loss中配置的频点都不在所选择的band频段内，建议重新设置或者使用系统推荐设置。\
										   选择yes使用系统推荐设置，选择no则自己重新配置。"), _T("警告信息"), MB_YESNO);
		if (IDYES == nRlt)
		{
			m_pGsmFunc->ConfigFreq();
			CHKRESULT(m_pGsmFunc->Run());
		}
		else if (IDNO == nRlt)
		{
			return spRlt;
		}
	}

	return spRlt;
}

SPRESULT CMakeGS::RunWcdma()
{
	if (!m_bWcdmaSelected)
	{
		return SP_OK;
	}

	CHKRESULT_WITH_NOTIFY(ChangeMode(RM_WCDMA_CALIBRATION_MODE), "Change WCDMA mode");
	CInstrumentLock rfLock(m_pRFTester);

	SPRESULT spRlt = m_pWcdmaFunc->Run();
		if (SP_E_SPAT_LOSS_FREQ_IVALID == spRlt)
		{
			int nRlt = MessageBox(NULL, _T("Loss中配置的频点都不在所选择的band频段内，建议重新设置或者使用系统推荐设置。\
选择yes使用系统推荐设置，选择no则自己重新配置。"), _T("警告信息"), MB_YESNO);
			if (IDYES == nRlt)
			{
				m_pWcdmaFunc->ConfigFreq();
				CHKRESULT(m_pWcdmaFunc->Run());
			}
			else if (IDNO == nRlt)
			{
				return spRlt;
			}
		}

	return spRlt;
}

SPRESULT CMakeGS::RunTd()
{
	if (m_bTdSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_TD_CALIBRATION_MODE), "Change TD mode");
		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pTdFunc->Run());
	}

	return SP_OK;
	}

SPRESULT CMakeGS::RunC2k()
{
	if (!m_bC2KSelected)
	{
		return SP_OK;
	}

	CHKRESULT_WITH_NOTIFY(ChangeMode(RM_C2K_CALIBRATION_MODE), "Change c2k mode");
	CInstrumentLock rfLock(m_pRFTester);

	SPRESULT spRlt = m_pC2KFunc->Run();
		if (SP_E_SPAT_LOSS_FREQ_IVALID == spRlt)
		{
			int nRlt = MessageBox(NULL, _T("Loss中配置的频点都不在所选择的band频段内，建议重新设置或者使用系统推荐设置。\
										   选择yes使用系统推荐设置，选择no则自己重新配置。"), _T("警告信息"), MB_YESNO);
			if (IDYES == nRlt)
			{
				m_pC2KFunc->ConfigFreq();
				CHKRESULT(m_pC2KFunc->Run());
			}
			else if (IDNO == nRlt)
			{
				return spRlt;
			}
		}

			return spRlt;
		}

SPRESULT CMakeGS::RunLte()
{
	if (m_bLteSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_LTE_CALIBRATION_MODE), "Change LTE mode");
		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pLteFunc->Run());
		m_arrLteInfo.clear();
		m_pLteFunc->Serialization( &m_arrLteInfo );
		m_gs.common.usLteCnt = ( uint16 )m_arrLteInfo.size();
	}
	else
	{
		 m_gs.common.usLteCnt = 0;
	}

	return SP_OK;
}

SPRESULT CMakeGS::RunNr()
{
	if (m_bNrSelected)
	{
#ifndef Internal_Debug
		//CHKRESULT_WITH_NOTIFY(ChangeMode(RM_NR_CALIBRATION_MODE), "Change NR mode");

		CHKRESULT(SP_NR_CAL_Active(m_hDUT));
#endif
		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pNrFunc->Run());
		m_arrNrInfo.clear();
		m_pNrFunc->Serialization(&m_arrNrInfo);
		m_gs.common.usNrCnt = (uint16)m_arrNrInfo.size();
	}
	else
	{
		m_gs.common.usNrCnt = 0;
	}

	return SP_OK;
	}

SPRESULT CMakeGS::RunWcn()
{
	if(m_bWlanSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");
		CInstrumentLock rfLock(m_pRFTester);
		m_pWCNFunc->SetMode(SP_WIFI);
		CHKRESULT(m_pWCNFunc->Run());
	}

	if(m_bBtSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");
		CInstrumentLock rfLock(m_pRFTester);
		m_pWCNFunc->SetMode(SP_BT);
		CHKRESULT(m_pWCNFunc->Run());
	}
	if(m_bGpsSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");
		CInstrumentLock rfLock(m_pRFTester);
		m_pWCNFunc->SetMode(SP_GPS);
		CHKRESULT(m_pWCNFunc->Run());
	}

	return SP_OK;
}

SPRESULT CMakeGS::RunDmr()
{
	if (m_bDMRCalSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_DMR_CALIBRATION_MOD), "Change DMR calibration mode");

		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pDMRCalFunc->Run());
	}

	if (m_bDMRFTSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_DMR_SIG_TEST_MOD), "Change DMR FT mode");
		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pDMRFtFunc->Run());
	}

	return SP_OK;
}

SPRESULT CMakeGS::RunAll()
{
	CHKRESULT(RunGsm());
	CHKRESULT(RunWcdma());
	CHKRESULT(RunTd());
	CHKRESULT(RunC2k());
	CHKRESULT(RunLte());
	CHKRESULT(RunNr());
	CHKRESULT(RunWcn());
	CHKRESULT(RunDmr());
	CHKRESULT_WITH_NOTIFY(SaveToPhone(), "Save to phone");
	return SP_OK;
}
