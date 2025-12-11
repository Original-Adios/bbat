#include "StdAfx.h"
#include "CalcLSUIS8910.h"
#include "GsmClc.h"
#include "LteClc_UIS8910.h"
#include "LteClc_UIS8850.h"
//#include "DMRClc.h"
#include "WCNCLC.h"
#include "LteUtility.h"

IMPLEMENT_RUNTIME_CLASS(CCalcLSUIS8910)
CCalcLSUIS8910::CCalcLSUIS8910(void)
{
    memset(m_wifi_UeInfo, 0, 20 * sizeof(int));
}

CCalcLSUIS8910::~CCalcLSUIS8910(void)
{
}

SPRESULT CCalcLSUIS8910::CheckResult(SPRESULT spRlt)
{
    if (spRlt != SP_OK)
    {
        CHKRESULT_WITH_NOTIFY(m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, (LPCVOID)&m_lossOrg),
            "Set loss property");
    }
    return spRlt;
}

SPRESULT CCalcLSUIS8910::LoadWifiUeInfo(void)
{
    PC_LTE_NV_UIS8910_DATA_T TransData;

    TransData.header.eNvType = LTE_NV_UIS8910_TYPE_GET_WIFI_INFO;
    TransData.header.Position = 0;
    TransData.header.DataSize = 20 * sizeof(int);

    CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &TransData));

    memcpy(m_wifi_UeInfo, &TransData.nData[0], TransData.header.DataSize);

    return SP_OK;
}

SPRESULT CCalcLSUIS8910::__PollAction(void)
{   
    uint8  rspBuf[128] = { 0 };
    //if (m_Uetype == 0x8850)
    //{
    //    CHKRESULT_WITH_NOTIFY(LoadWifiUeInfo(), "Wifi CalcLS Load Ue Info fail.");
    //}

    if (!m_bGsmSelected && !m_bTdSelected && !m_bWcdmaSelected && !m_bLteSelected && !m_bDMRFTSelected && 
        !m_bDMRCalSelected && !m_bWlanSelected && !m_bGpsSelected && !m_bBtSelected)
    {
        MessageBox(NULL, _T("没有选择任何制式，请确认你的操作！"), _T("提示"), MB_OK);
        return SP_OK;
    }

    CHKRESULT(CImpBaseUIS8910::__PollAction());    
    CHKRESULT_WITH_NOTIFY(LoadFromPhone(), "Load data from phone");

    m_lossOrg = m_lossVal;
    ZeroLossValue(m_lossVal);
    CHKRESULT_WITH_NOTIFY(m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, (LPCVOID)&m_lossVal),
        "Set loss property");

    SPRESULT spRlt = SP_OK;

    m_strNegativeLossDetected = "";

    if (m_bGsmSelected)
    {
        if (m_gs.common.nGsmCnt > 0)
        {
            if (m_Uetype == 0x891C)
            {
                spRlt = SP_SendATCommand(m_hDUT, "AT+UHSGL=2", TRUE, rspBuf, sizeof(rspBuf), NULL, TIMEOUT_3S);
                Sleep(TIMEOUT_3S);//wait for instrument and UE reboot
            }
            else
            {
                spRlt = ChangeMode(RM_CALIBRATION_MODE);
            }
            CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");

            // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
            // and the following steps will not be executed.
            // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
            CInstrumentLock rfLock(m_pRFTester);
            m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_Uetype);

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

#if 0
    if (m_bTdSelected)
    {
        if (m_gs.common.nTdCnt > 0)
        {
            spRlt = ChangeMode(RM_TD_CALIBRATION_MODE);
            CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change TD mode");

            // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
            // and the following steps will not be executed.
            // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
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

    if (m_bWcdmaSelected)
    {
        if (m_gs.common.nWcdmaCnt > 0)
        {
            spRlt = ChangeMode(RM_WCDMA_CALIBRATION_MODE);
            CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change WCDMA mode");

            // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
            // and the following steps will not be executed.
            // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
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
#endif
    if (m_bLteSelected)
    {
        if (m_gs.common.usLteCnt > 0)
        {
            if (m_Uetype == 0x891C)
            {
                spRlt = SP_SendATCommand(m_hDUT, "AT+UHSGL=4", TRUE, rspBuf, sizeof(rspBuf), NULL, TIMEOUT_3S);
                Sleep(TIMEOUT_3S);//wait for instrument and UE reboot
            }
            else
            {
                spRlt = ChangeMode(RM_LTE_CALIBRATION_MODE);
            }
            CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change LTE mode");

            // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
            // and the following steps will not be executed.
            // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
            CInstrumentLock rfLock(m_pRFTester);
            m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_Uetype);
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
#if 0
	if (m_bDMRCalSelected)
	{
		if (m_gs.common.nDmrCnt > 0)
		{
            spRlt = ChangeMode(RM_DMR_CALIBRATION_MOD);
            CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change DMR calibration mode");

            // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
            // and the following steps will not be executed.
            // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
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
            spRlt = ChangeMode(RM_DMR_SIG_TEST_MOD);
            CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change DMR FT mode");

            // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
            // and the following steps will not be executed.
            // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
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
#endif

	if (m_bWlanSelected)
	{
		if (m_gs.common.nWlanCnt > 0)
		{
			spRlt = ChangeMode(RM_LTE_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change LTE mode");

			// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
			// and the following steps will not be executed.
			// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
			CInstrumentLock rfLock(m_pRFTester);
			m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_Uetype);
			m_pWCNFunc->SetMode(SP_WIFI);
			spRlt = m_pWCNFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}        
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no wlan golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		} 
	}

#if 0
	if (m_bBtSelected)
	{
		if (m_gs.common.nBtCnt > 0)
		{
			spRlt = ChangeMode(RM_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");

			// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
			// and the following steps will not be executed.
			// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
			CInstrumentLock rfLock(m_pRFTester);
			m_pWCNFunc->SetMode(SP_BT);
			spRlt = m_pWCNFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}        
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no bt golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		} 
	}

	if (m_bGpsSelected)
	{
		if (m_gs.common.nGpsCnt > 0)
		{
			spRlt = ChangeMode(RM_CALIBRATION_MODE);
			CHKRESULT_WITH_NOTIFY(CheckResult(spRlt), "Change GSM mode");

			// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
			// and the following steps will not be executed.
			// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
			CInstrumentLock rfLock(m_pRFTester);
			m_pWCNFunc->SetMode(SP_GPS);
			spRlt = m_pWCNFunc->Run();
			CHKRESULT(CheckResult(spRlt));
		}        
		else
		{
			LogFmtStrA(SPLOGLV_ERROR, "There has no gps golden calibration data.");
			NOTIFY("Check golden data", LEVEL_UI, 1, 0, 1);
			return CheckResult(SP_E_SPAT_LOSS_NO_GS_DATA);
		} 
	}
#endif

    if (m_strNegativeLossDetected.length() > 0)
    {
        CHAR szText[512] = { 0 };
        CHAR szMsg[512] = { 0 };
        //Remove the last comma
        UINT nIndex = m_strNegativeLossDetected.find_last_of(",");
        if ( m_strNegativeLossDetected.length() -1 == nIndex)
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
    CHKRESULT_WITH_NOTIFY(UpgradeLoss(), "Upgrade loss");   

    return SP_OK;
}

BOOL CCalcLSUIS8910::LoadXMLConfig()
{	
    /// Option	
    m_Uetype = GetConfigValue(L"Option:DUTType:UEType", 0x8910);
    wstring strPath = L"Option:NetMode:";
    m_bGsmSelected = (BOOL)GetConfigValue((strPath+L"GSM").c_str(), 0);
    m_bTdSelected = (BOOL)GetConfigValue((strPath+L"TD").c_str(), 0);
    m_bWcdmaSelected = (BOOL)GetConfigValue((strPath+L"WCDMA").c_str(), 0);
    m_bLteSelected = (BOOL)GetConfigValue((strPath+L"LTE").c_str(), 0);
	m_bDMRCalSelected = (BOOL)GetConfigValue((strPath+L"DMR-CAL").c_str(), 0);
	m_bDMRFTSelected = (BOOL)GetConfigValue((strPath+L"DMR-FT").c_str(), 0);
	m_bWlanSelected = (BOOL)GetConfigValue((strPath+L"WLAN").c_str(), 0);
	m_bBtSelected = (BOOL)GetConfigValue((strPath+L"BT").c_str(), 0);
	m_bGpsSelected = (BOOL)GetConfigValue((strPath+L"GPS").c_str(), 0);
    m_bLossCheck = ( BOOL )GetConfigValue( L"Option:LossCheck", 0 );
    m_dLossCheckSpec = GetConfigValue( L"Option:LossCheckSpec", 0.3 );

    /// Parameter
    m_dLossValLower = GetConfigValue(L"Param:PreLoss", 0.0);
    m_dLossValUpper = GetConfigValue(L"Param:Loss_Val_Spec", 5.0);
    m_dLossValLower_bk = m_dLossValLower;
    m_dLossValUpper_bk = m_dLossValUpper;
    //Loss limit list
    m_vecLossLimit.clear();
    m_vecFreq.clear();
    m_vecLoss.clear();
    std::wstring valueFreq = GetConfigValue(L"Param:FrequencyList", L"");
    std::wstring valueLoss = GetConfigValue(L"Param:LossLimitList", L"");
    int nCount = 0;
    int nCountLoss = 0;
    LPWSTR* token = GetTokenStringW(valueFreq.c_str(), DEFAULT_DELIMITER_W, nCount);
    for (int i = 0; i < nCount; i++)
    {
        m_vecFreq.push_back(_wtof(token[i]));
    }
    LPWSTR* tokenLoss = GetTokenStringW(valueLoss.c_str(), DEFAULT_DELIMITER_W, nCountLoss);
    for (int i=0;i<nCountLoss;i++)
    {
        m_vecLoss.push_back(_wtof(tokenLoss[i]));
    }
    if (m_vecFreq.size() >0)
    {
        if (m_vecLoss.size() <= 0)
        {
            return TRUE;
        }
        uint32 iVecFreqSize = m_vecFreq.size();
        for (uint32 i = 0; i < iVecFreqSize; i++)
        {
            LOSS_LIMIT loss_limit;
            loss_limit.dFreq = m_vecFreq[i];
            if (i >= m_vecLoss.size())
            {
                loss_limit.dLossUpperLimit = m_dLossValUpper;
            }
            else
            {
                loss_limit.dLossUpperLimit = m_vecLoss[i];
            }
            m_vecLossLimit.push_back(loss_limit);
        }
    }
    return TRUE;
}


SPRESULT CCalcLSUIS8910::__InitAction( void )
{

	for(int m=0; m<MAX_LTE_BAND; m++)
	{
	    if(CLteUtility::m_BandInfo[m].nNumeral == 28)
		{
			if(CLteUtility::m_BandInfo[m].nIndicator == 0xa)
			{
				CLteUtility::m_BandInfo[m].UlChannel.usBegin = 27210;
				CLteUtility::m_BandInfo[m].UlChannel.usEnd = 27435;

				CLteUtility::m_BandInfo[m].DlChannel.usBegin = 9210;
				CLteUtility::m_BandInfo[m].DlChannel.usEnd = 9435;
			}
			if(CLteUtility::m_BandInfo[m].nIndicator == 0xb)
			{
				CLteUtility::m_BandInfo[m].UlChannel.usBegin = 27436;
				CLteUtility::m_BandInfo[m].UlChannel.usEnd = 27659;

				CLteUtility::m_BandInfo[m].DlChannel.usBegin = 9436;
				CLteUtility::m_BandInfo[m].DlChannel.usEnd = 9659;
			}
		}
	    if(CLteUtility::m_BandInfo[m].nNumeral == 66)
		{
			CLteUtility::m_BandInfo[m].UlChannel.usBegin = 131972;
			CLteUtility::m_BandInfo[m].UlChannel.usEnd = 132671;

			CLteUtility::m_BandInfo[m].DlChannel.usBegin = 66436;
			CLteUtility::m_BandInfo[m].DlChannel.usEnd = 67135;
		}
	}	
	m_pGsmFunc = new CGsmClc(this);
    //m_pTdFunc = new CTdClc(this);
    //m_pWcdmaFunc = new CWcdmaClc(this);
    if (m_Uetype == 0x8850)
    {
        m_pLteFunc = new CLteClc_UIS8850(this);
    }
    else
    {
        m_pLteFunc = new CLteClc_UIS8910(this);
    }

	
	//m_pDMRCalFunc = new CDMRClc(this);
	//m_pDMRFtFunc = new CDMRClc(this);

	m_pWCNFunc = new CWCNCLC(this);

    m_pGsmFunc->Init();
    //m_pTdFunc->Init();
    //m_pWcdmaFunc->Init();
    m_pLteFunc->Init();
	//m_pDMRCalFunc->Init();
	//m_pDMRFtFunc->Init();
	m_pWCNFunc->Init();

    return SP_OK;
}

