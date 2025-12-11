#include "StdAfx.h"
#include "MakeGSUIS8910.h"
#include "GsmGs.h"
#include "LteGs_UIS8910.h"
#include "LteGs_UIS8850.h"
//#include "DMRGs.h"
#include "gsmUtility.h"
#include "LteUtility.h"
#include "Utility.h"
#include "wcnUtility.h"
#include "WCNGS.h"

IMPLEMENT_RUNTIME_CLASS(CMakeGSUIS8910)
CMakeGSUIS8910::CMakeGSUIS8910(void)
{
    memset(m_wifi_UeInfo, 0, 20 * sizeof(int));
}

CMakeGSUIS8910::~CMakeGSUIS8910(void)
{
}

BOOL CMakeGSUIS8910::LoadXMLConfig()
{	
    /// Option	
    //DUTType
    m_Uetype = GetConfigValue(L"Option:DUTType:UEType", 0x8910);
    /// WCDMA
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
    /// GSM
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
		m_nGsmPcl[i] = GetConfigValue(strPath.c_str(), 10);
	}

    /// LTE
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
    wstring sValue;
    sValue = GetConfigValue(L"Option:LTE:LTE:Mode", L"Manually");
    if (0 == sValue.compare(L"Automatically"))
    {
        m_bLteSelected = TRUE;
    }

	///
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

	BOOL bBandSelected = FALSE;
	BOOL bAntSelected = FALSE;
	ZeroMemory(m_bWlanBandNum, sizeof(m_bWlanBandNum));
	m_bWcnAnt = GetConfigValue(L"Option:WCN:AntennaSwitch", FALSE); //marlin2 = false
	wstring strWlanBand[2] = {L"WLAN2.4GHz", L"WLAN5.0GHz"};
	wstring strWlanAnt[MAX_RF_ANT] = {L"Ant1st", L"Ant2nd", L"Ant3rd", L"Ant4th"};
	for(int i=0; i<2; i++)
	{
		for(int k=0; k<WIFI_MaxProtocol; k++)
		{
			strPath = L"Option:WCN:WLAN:" + strWlanBand[i] + L":" + CUtility::_A2CW(CwcnUtility::WLAN_BAND_NAME[k]);
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
			strPath = L"Option:WCN:WLAN:" + strWlanBand[i] + L":" + strWlanAnt[j];
			m_bWlanAnt[i][j] = GetConfigValue(strPath.c_str(), FALSE);
			if(m_bWlanAnt[i][j])
			{
				bAntSelected = TRUE;
			}
		}
	}
	m_bWlanSelected = bBandSelected && bAntSelected;


	bBandSelected = FALSE;
	bAntSelected = FALSE;
	wstring strBTAnt[MAX_RF_ANT] = {L"Shared", L"StandAlone", L"Ant3rd", L"Ant4th"};
	
	for(int j=BDR; j<MAX_BT_TYPE; j++)
	{
		strPath = L"Option:WCN:BT:BT:";
		strPath += CUtility::_A2CW(CwcnUtility::BT_BAND_NAME[j]);
		m_bBTBandNum[j] = GetConfigValue(strPath.c_str(), FALSE);
		if(m_bBTBandNum[j])
		{
			bBandSelected = TRUE;
		}
	}
	ZeroMemory(m_bBTAnt, sizeof(m_bBTAnt));
	for(int i=0; i< MAX_RF_ANT; i++)
	{
		strPath = L"Option:WCN:BT:" + strBTAnt[i];
		m_bBTAnt[i] = GetConfigValue(strPath.c_str(), FALSE);
		if(m_bBTAnt[i])
		{
			bAntSelected = TRUE;
		}
	}
	m_bBtSelected = bBandSelected && bAntSelected;

	m_dWlanRefLvl = GetConfigValue(L"Param:WLAN:Power", -50.0);
	m_dBtRefLvl = GetConfigValue(L"Param:BT:RefLevel", 20.0);
	
	m_bGpsSelected = FALSE;
	strPath = L"Option:WCN:GPS:GPS";
	m_bGPSBandNum = GetConfigValue(strPath.c_str(), FALSE);
	if(m_bGPSBandNum)
	{
		m_bGpsSelected = TRUE;
	}
    return TRUE;
}

SPRESULT CMakeGSUIS8910::LoadWifiUeInfo(void)
{
	PC_LTE_NV_UIS8910_DATA_T TransData;

	TransData.header.eNvType = LTE_NV_UIS8910_TYPE_GET_WIFI_INFO;
	TransData.header.Position = 0;
	TransData.header.DataSize = 20 * sizeof(int);

	CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &TransData));

	memcpy(m_wifi_UeInfo, &TransData.nData[0], TransData.header.DataSize);

	return SP_OK;
}

SPRESULT CMakeGSUIS8910::__PollAction(void)
{      
    uint8  rspBuf[128] = { 0 };
	//if (m_Uetype == 0x8850)
	//{
	//	CHKRESULT_WITH_NOTIFY(LoadWifiUeInfo(), "Wifi MakeGs Load Ue Info fail.");
	//}

    if (!m_bGsmSelected && !m_bTdSelected && !m_bWcdmaSelected && !m_bLteSelected && !m_bDMRFTSelected && 
		!m_bDMRCalSelected && !m_bWlanSelected && !m_bGpsSelected && !m_bBtSelected)
    {
        MessageBox(NULL, _T("没有选择任何制式，请确认你的操作！"), _T("提示"), MB_OK);
        return SP_OK;
    }

    CHKRESULT(CImpBaseUIS8910::__PollAction());
    SPRESULT spRlt = SP_OK;

    if (m_bGsmSelected)
    {
        if (m_Uetype == 0x891C)
        {
            CHKRESULT(SP_SendATCommand(m_hDUT, "AT+UHSGL=2", TRUE, rspBuf, sizeof(rspBuf), NULL, TIMEOUT_3S));
            Sleep(TIMEOUT_3S);//wait for instrument and UE reboot
        }
        else
        {
            CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");
        }

        // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
        // and the following steps will not be executed.
        // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
        CInstrumentLock rfLock(m_pRFTester);
        m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_Uetype);

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
        else if (SP_OK != spRlt)
        {
            return spRlt;
        }
    }

#if 0
    if (m_bTdSelected)
    {
        CHKRESULT_WITH_NOTIFY(ChangeMode(RM_TD_CALIBRATION_MODE), "Change TD mode");

        // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
        // and the following steps will not be executed.
        // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
        CInstrumentLock rfLock(m_pRFTester);

        CHKRESULT(m_pTdFunc->Run());
    }

    if (m_bWcdmaSelected)
    {
        CHKRESULT_WITH_NOTIFY(ChangeMode(RM_WCDMA_CALIBRATION_MODE), "Change WCDMA mode");

        // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
        // and the following steps will not be executed.
        // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
        CInstrumentLock rfLock(m_pRFTester);

        spRlt = m_pWcdmaFunc->Run();
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
        else if (SP_OK != spRlt)
        {
            return spRlt;
        }
    }
#endif
    if (m_bLteSelected)
    {
        if (m_Uetype == 0x891C)
        {
            CHKRESULT(SP_SendATCommand(m_hDUT, "AT+UHSGL=4", TRUE, rspBuf, sizeof(rspBuf), NULL, TIMEOUT_3S));
            Sleep(TIMEOUT_3S);//wait for instrument and UE reboot
        }
        else
        {
            CHKRESULT_WITH_NOTIFY(ChangeMode(RM_LTE_CALIBRATION_MODE), "Change LTE mode");
        }

        // If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
        // and the following steps will not be executed.
        // [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
        CInstrumentLock rfLock(m_pRFTester);
        m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_Uetype);

        CHKRESULT(m_pLteFunc->Run());
    }
#if 0
	if (m_bDMRCalSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_DMR_CALIBRATION_MOD), "Change DMR calibration mode");

		// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
		// and the following steps will not be executed.
		// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pDMRCalFunc->Run());
	}

	if (m_bDMRFTSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_DMR_SIG_TEST_MOD), "Change DMR FT mode");

		// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
		// and the following steps will not be executed.
		// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
		CInstrumentLock rfLock(m_pRFTester);

		CHKRESULT(m_pDMRFtFunc->Run());
	}
#endif
	if(m_bWlanSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_LTE_CALIBRATION_MODE), "Change LTE mode");

		// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
		// and the following steps will not be executed.
		// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
		CInstrumentLock rfLock(m_pRFTester);
		m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_Uetype);
		m_pWCNFunc->SetMode(SP_WIFI);
		CHKRESULT(m_pWCNFunc->Run());
	}
#if 0
	if(m_bBtSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");

		// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
		// and the following steps will not be executed.
		// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
		CInstrumentLock rfLock(m_pRFTester);
		m_pWCNFunc->SetMode(SP_BT);
		CHKRESULT(m_pWCNFunc->Run());
	}
	if(m_bGpsSelected)
	{
		CHKRESULT_WITH_NOTIFY(ChangeMode(RM_CALIBRATION_MODE), "Change GSM mode");

		// If fails to get the instrument resource in TIMEOUT, an exception will be throw to terminate the whole test process
		// and the following steps will not be executed.
		// [如果仪器资源在指定时间内获取失败，抛出异常，整个测试过程将被终止，不再继续执行]
		CInstrumentLock rfLock(m_pRFTester);
		m_pWCNFunc->SetMode(SP_GPS);
		CHKRESULT(m_pWCNFunc->Run());
	}
#endif
    Sleep(1000);
    CHKRESULT_WITH_NOTIFY(SaveToPhone(), "Save to phone");
    CHKRESULT_WITH_NOTIFY( m_pRFTester->SetProperty( DP_CABLE_LOSS_EX, 0, ( LPCVOID )&m_lossVal ),
                           "Set loss property" );
    Sleep(5000);//wait for UE save data

    return SP_OK;
}

SPRESULT CMakeGSUIS8910::__InitAction( void )
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

	m_pGsmFunc = new CGsmGs(this);
    //m_pTdFunc = new CTdGs(this);
    //m_pWcdmaFunc = new CWcdmaGs(this);

	if (m_Uetype == 0x8850)
	{
		m_pLteFunc = new CLteGs_UIS8850(this);
	}
	else
	{
		m_pLteFunc = new CLteGs_UIS8910(this);
	}

	//m_pDMRCalFunc = new CDMRGs(this);
	//m_pDMRFtFunc = new CDMRGs(this);

	m_pWCNFunc = new CWCNGS(this);

   m_pGsmFunc->Init();
    //m_pTdFunc->Init();
    //m_pWcdmaFunc->Init();
    m_pLteFunc->Init();
	//m_pDMRCalFunc->Init();
	//m_pDMRFtFunc->Init();
	m_pWCNFunc->Init();

    return SP_OK;
}