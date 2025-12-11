#include "StdAfx.h"
#include "ModeSwitch.h"
#include <assert.h>
#include "ActionApp.h"

//
#define ShareMemory_My_BaseStation                     L"My_ModeSwitch_BSConfig"
#define ShareMemory_My_RunMode                         L"My_ModeSwitch_RunMode"
#define ShareMemory_My_ModeOptions                     L"My_ModeSwitch_Options"

#define ShareMemory_My_CurrentMode                     L"My_ModeSwitch_CurrMode"

//////////////////////////////////////////////////////////////////////////
CModeSwitch::CModeSwitch(CSpatBase* pImp)
  : m_pImp(pImp)
{
    assert(NULL != m_pImp);

    m_hDUT      = pImp->m_hDUT;
    m_pRFTester = pImp->m_pRFTester;
    m_pDCSource = pImp->m_pDCSource;
}

CModeSwitch::~CModeSwitch(void)
{
}

RM_MODE_ENUM CModeSwitch::GetCurrentMode(void)
{
    RM_MODE_ENUM eMode = RM_INVALID_MODE;
    SPRESULT res = m_pImp->GetShareMemory(ShareMemory_My_CurrentMode, (void* )&eMode, 4, NULL);
    return (SP_OK == res) ? eMode : RM_INVALID_MODE;
}

BOOL CModeSwitch::GetBSConfig(BS_CONFIG_T& bs)
{
    return (SP_OK == m_pImp->GetShareMemory(ShareMemory_My_BaseStation, (void* )&bs, sizeof(bs), NULL)) 
        ? TRUE : FALSE;
}

void CModeSwitch::SetupBSConfig(const BS_CONFIG_T& bs)
{   
    BS_CONFIG_T val;
    if (!GetBSConfig(val))
    {
        val = bs;
        m_pImp->LogFmtStrA(SPLOGLV_VERBOSE, "BSConfig network = %d, band = %d", bs.eMode, bs.param.eBand);
        m_pImp->SetShareMemory(ShareMemory_My_BaseStation, (const void* )&val, sizeof(val), IContainer::System);
    }
}

BOOL CModeSwitch::GetRunMode(RM_MODE_ENUM& eMode)
{
    return (SP_OK == m_pImp->GetShareMemory(ShareMemory_My_RunMode, (void* )&eMode, 4)) 
        ? TRUE : FALSE;
}

void CModeSwitch::SetupRunMode(RM_MODE_ENUM eMode)
{
    RM_MODE_ENUM eAuto = RM_INVALID_MODE;
    if (IS_VALID_MODE(eMode) && !GetRunMode(eAuto))
    {
        m_pImp->LogFmtStrA(SPLOGLV_VERBOSE, "DUT runtime mode = 0x%X", DIAG_MODE(eMode));
        m_pImp->SetShareMemory(ShareMemory_My_RunMode, (const void* )&eMode, 4, IContainer::System);
    }
}

SPRESULT CModeSwitch::Reboot(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS/*= NULL*/, BOOL b1stBootUp/*=FALSE*/)
{
    if (NULL == m_pImp)
    {
        assert(0);
        return SP_E_SPAT_INVALID_POINTER;  
    }

    GetOptions(m_Options);

    if (RM_INVALID_MODE == eMode)
    {
        // Automatically 
        if (!GetRunMode(eMode))
        {
            eMode = RM_CALIBRATION_MODE;
        }
    }

    return __reboot(eMode, pBS, b1stBootUp);
}

SPRESULT CModeSwitch::Change(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS /*= NULL*/)
{
    if (NULL == m_pImp)
    {
        assert(0);
        return SP_E_SPAT_INVALID_POINTER;   
    }

    GetOptions(m_Options);

    RM_MODE_ENUM eNextMode = eMode; 
    RM_MODE_ENUM eCurrMode = GetCurrentMode();
    if (eCurrMode == eNextMode)
    {
        m_pImp->LogFmtStrA(SPLOGLV_INFO, "Current mode is already 0x%X, no need to change!", DIAG_MODE(eNextMode));
        return SP_OK;
    }

    if (IS_CALIBRATION_CELLULAR_MODE(eCurrMode) && IS_CALIBRATION_CELLULAR_MODE(eNextMode))
    {
        // no need to reboot from one calibration mode to the other calibration mode
        return __switch(eNextMode, eCurrMode);
    }
    else
    {
        return __reboot(eNextMode, pBS, FALSE);
    }
}

SPRESULT CModeSwitch::__switch(RM_MODE_ENUM eNextMode, RM_MODE_ENUM eCurrMode)
{
    LPCWSTR lpszModemVersion = m_pImp->GetConfigValue(L"GLOBAL:MODEM", L"V2", TRUE);
    m_pImp->LogFmtStrA(SPLOGLV_INFO, "Changing mode from 0x%X to 0x%X ...", DIAG_MODE(eCurrMode), DIAG_MODE(eNextMode));

#ifdef __SUPPORT_DUAL_GSM__
    if (RM_CALIBRATION_MODE == eNextMode || RM_WCDMA_GSM_CALIBR_CAL_MODE == eNextMode)
#else
    if (RM_CALIBRATION_MODE == eNextMode)
#endif
    {
        switch(eCurrMode)
        {
        case RM_WCDMA_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_wcdmaActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_hDUT, FALSE));
            }
            break;
        case RM_LTE_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_lteActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            break;
        case RM_TD_CALIBRATION_MODE:
            CHKRESULT(SP_tdActive(m_hDUT, FALSE));
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_tdCalSwitchTogsmCal(m_hDUT));
            }
            break;
        default:
            break;
        }
    }
    else if (RM_TD_CALIBRATION_MODE == eNextMode)
    {
        switch(eCurrMode)
        {
        case RM_WCDMA_CALIBRATION_MODE:
            CHKRESULT(SP_wcdmaActive(m_hDUT, FALSE));
            break;
        case RM_LTE_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_lteActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            break;
    #ifdef __SUPPORT_DUAL_GSM__
        case RM_WCDMA_GSM_CALIBR_CAL_MODE:
    #endif
        case RM_CALIBRATION_MODE:
            CHKRESULT(SP_gsmActive(m_hDUT, FALSE));
            break;
        default:
            break;
        }
		// Bug 760064: Glayer1 Talent: 
		// 每次切换到TD，都需要走一次TD到的GSM切换
        if (0 == _wcsicmp(lpszModemVersion, L"V2"))
        {
		    CHKRESULT(SP_gsmCalSwitchToTDCal(m_hDUT));
        }
    }
    else if (RM_WCDMA_CALIBRATION_MODE == eNextMode)
    {
        switch(eCurrMode)
        {
        case RM_CALIBRATION_MODE:
    #ifdef __SUPPORT_DUAL_GSM__
        case RM_WCDMA_GSM_CALIBR_CAL_MODE:
    #endif
            CHKRESULT(SP_gsmActive(m_hDUT, FALSE));
            break;
        case RM_TD_CALIBRATION_MODE:
            CHKRESULT(SP_tdActive(m_hDUT, FALSE));
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                // Bug 760064: Glayer1 Talent: 
                // 每次TD起来，都无条件走一次GSM到TD的切换，每次离开TD，都无条件走一次TD到G切换
                CHKRESULT(SP_tdCalSwitchTogsmCal(m_hDUT));
            }
            break;
        case RM_LTE_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_lteActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            break;
        }
    }
    else if (RM_LTE_CALIBRATION_MODE == eNextMode)
    {
        switch(eCurrMode)
        {
        case RM_CALIBRATION_MODE:
    #ifdef __SUPPORT_DUAL_GSM__
        case RM_WCDMA_GSM_CALIBR_CAL_MODE:
    #endif
            CHKRESULT(SP_gsmActive(m_hDUT, FALSE));
            break;
        case RM_TD_CALIBRATION_MODE:
            CHKRESULT(SP_tdActive(m_hDUT, FALSE));
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                // Bug 760064: Glayer1 Talent: 
                // 每次TD起来，都无条件走一次GSM到TD的切换，每次离开TD，都无条件走一次TD到G切换
                CHKRESULT(SP_tdCalSwitchTogsmCal(m_hDUT));
            }
            break;
        case RM_WCDMA_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_wcdmaActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_hDUT, FALSE));
            }
            break;
        }
    }
	else if (RM_C2K_CALIBRATION_MODE == eNextMode)
	{
		switch(eCurrMode)
		{
		case RM_CALIBRATION_MODE:
#ifdef __SUPPORT_DUAL_GSM__
		case RM_WCDMA_GSM_CALIBR_CAL_MODE:
#endif
			CHKRESULT(SP_gsmActive(m_hDUT, FALSE));
			break;
		case RM_TD_CALIBRATION_MODE:
			CHKRESULT(SP_tdActive(m_hDUT, FALSE));
			if (0 == _wcsicmp(lpszModemVersion, L"V2"))
			{
				// Bug 760064: Glayer1 Talent: 
				// 每次TD起来，都无条件走一次GSM到TD的切换，每次离开TD，都无条件走一次TD到G切换
				CHKRESULT(SP_tdCalSwitchTogsmCal(m_hDUT));
			}
			break;
		case RM_WCDMA_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_wcdmaActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_hDUT, FALSE));
            }
			break;
		case RM_LTE_CALIBRATION_MODE:
            if (0 == _wcsicmp(lpszModemVersion, L"V2"))
            {
                CHKRESULT(SP_lteActive(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V3"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
            else if (0 == _wcsicmp(lpszModemVersion, L"V4"))
            {
                CHKRESULT(SP_ModemV3_LTE_Active(m_hDUT, FALSE));
            }
			break;
		}
	}

    return SetCurrentMode(eNextMode);
}

SPRESULT CModeSwitch::__reboot(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS, BOOL b1stBootup)
{
    m_pImp->LogFmtStrA(SPLOGLV_INFO, "Reboot mode 0x%X ...", DIAG_MODE(eMode));
    
    if (IS_SIGNAL_CELLULAR_MODE(eMode))
    {
        // To make sure DUT register the instrument network.
        // Before restart DUT, the instrument should be changed to signal state
        CHKRESULT(PreSetupSignal(eMode, pBS));
    }

    BOOL bRestarted = FALSE;
    if (!b1stBootup)
    {
        // reboot DUT by power supply or command
        if (CModeOptions::COMMAND == m_Options.m_eRestartOption || m_pImp->IsFakeDC())
        {
            CHKRESULT(SP_RestartPhone(m_hDUT));
            bRestarted = TRUE;
        }
    }

    SP_EndPhoneTest(m_hDUT);

    // Save .logel file of ARM/DSP trace
    uint32 nValue = 0;
    m_pImp->GetShareMemory(InternalReservedShareMemory_SetupDebugMode, (void* )&nValue, sizeof(nValue));
    BOOL bDebugMode = (1 == nValue) ? TRUE : FALSE;
    SP_SetProperty(m_hDUT, SP_ATTR_LOGEL_FILE, 0, (LPCVOID)bDebugMode);

    const SPAT_INIT_PARAM& adjParam = m_pImp->GetAdjParam();
    SPRESULT res = SP_BeginPhoneTest(m_hDUT, (CHANNEL_ATTRIBUTE* )&adjParam.ca);
    if (SP_OK != res)
    {
        return res;
    }

    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == hEvent)
    {
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "%s: CreateEvent failed, Windows Error = 0x%X", __FUNCTION__, ::GetLastError());
        return SP_E_SPAT_CREATE_OBJECT;
    }
	res = SP_SetEnterModeAuthentication(m_hDUT
		, m_Options.m_bAuth);
	if (SP_OK != res)
	{
        CloseHandle(hEvent);
		return res;
	}
    res = SP_EnterModeProcessEx(m_hDUT, (CHANNEL_TYPE_USBMON == adjParam.ca.ChannelType) ? TRUE : FALSE, \
        adjParam.ca.Com.dwPortNum, eMode, hEvent, m_Options.m_dwTimeOut, m_Options.m_eUsbSpeedType);
    if (SP_OK != res)
    {
        CloseHandle(hEvent);
        return res;
    }

    if (!bRestarted)
    {
        // Power On DUT
        switch(m_Options.m_eRestartOption)
        {
        case CModeOptions::AUTO:
            if (!m_pImp->IsFakeDC())
            {
                CHKRESULT(m_pImp->SwitchOnDC(FALSE, 0.0f, 0.0f));

                Sleep(m_Options.m_u32DelayTime);
                //由于此模块通过这里单独控制电源，bug1843054需要阶梯调压
				//只针对VBUS， 从2.5 - 5.0v之间，0.5v一个阶梯，延迟50ms		//针对 Bug 1843054 的特殊用法，故此 后3项为固定值
				CHKRESULT(m_pImp->SwitchOnDC(TRUE, adjParam.fVoltage, m_Options.m_fVBUSVoltage, 2.5, 0.5, 50));
            }
            break;
        case CModeOptions::CUSTOMIZED:
        {
            m_pImp->LogFmtStrA(SPLOGLV_INFO, "Customized start to power on. %d", (INT)adjParam.ca.Com.dwPortNum);
            if (!m_pImp->InvokeCustomizeDataCallback(CUSTOMIZED_CTRL_POWER_ON, (LPCVOID)adjParam.ca.Com.dwPortNum))
            {
                m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Customized power on failed. %d", (INT)adjParam.ca.Com.dwPortNum);
            }
            m_pImp->LogFmtStrA(SPLOGLV_INFO, "Customized power on finished. %d", (INT)adjParam.ca.Com.dwPortNum);
        }
        break;
        case CModeOptions::NPIDEVICE:
        {
            CHKRESULT(m_pImp->SwitchOnDC(FALSE, 0.0f, 0.0f));
            Sleep(200);
            CHKRESULT(m_pImp->SwitchOnDC(TRUE, adjParam.fVoltage, m_Options.m_fVBUSVoltage));
        }
        break;
        default:
            break;
        }
    }

    const HANDLE arrHandles[] = {adjParam.hStopEvent, hEvent};
    DWORD dwCode = WaitForMultipleObjects(ARRAY_SIZE(arrHandles), arrHandles, FALSE, INFINITE);
    SP_StopModeProcess(m_hDUT);
    
    CloseHandle(hEvent);
    hEvent = INVALID_HANDLE_VALUE;

    switch(dwCode)
    {
    case WAIT_TIMEOUT:
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Enter Mode Timeout!");
        res = SP_E_SPAT_TIMEOUT;
        break;
    case WAIT_OBJECT_0:
        res = SP_E_USER_ABORT;
        break;
    case WAIT_OBJECT_0 + 1:
        {
            SPRESULT resCode = SP_GetModeResultCode(m_hDUT);
            if (SP_OK != resCode)
            {
                res = resCode;
            }
            else
            {
                res = SetCurrentMode(eMode);
            }
        }
        break;
    default:
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "WaitForMultipleObjects() failed!, Windows Error = 0x%X", ::GetLastError());
        res = SP_E_SPAT_TEST_FAIL;
        break;
    }

    if (SP_OK != res)
    {
        return res;
    }

    Sleep(m_Options.m_dwSleepTime);

    /*
    // Bug1162044: 
    // Un-check modem software anymore to adapt to modem less product.
    // NonRF.dll also provides CCheckDiagConnect to replace.
    CHKRESULT(TestCommunicating());
    */

    CHKRESULT(ActiveLogels());

    if (m_Options.m_bChargeOff)
    {
        PC_CHARGE_T req;
        ZeroMemory((void* )&req, sizeof(req));
        req.cmd = CHARGE_OFF;
        CHKRESULT(SP_apChargeCmd(m_hDUT, &req));
    }

    m_pImp->SendWorkStatus();

    return SP_OK;
}

/*
// Bug 1162044
SPRESULT CModeSwitch::TestCommunicating(void)
{
    if (!m_Options.m_bLoadModemSwVer)
    {
        // Bug925444: AP only product is no need to load modem version
        return SP_OK;
    }

    SPRESULT res = SP_OK;

    UINT32 ulTimeOut = TIMEOUT_3S;
    SP_GetProperty(m_hDUT, SP_ATTR_TIMEOUT, 0, (LPVOID )&ulTimeOut);
    SP_SetProperty(m_hDUT, SP_ATTR_TIMEOUT, 0, (LPCVOID)TIMEOUT_3S);

    CHAR szSW[2500] = {0}; 
    for (INT i=0; i<5; i++)
    {
        if (m_pImp->_IsUserStop())
        {
            res = SP_E_USER_ABORT;
            break;
        }

        res = SP_GetSWVer(m_hDUT, CP, szSW, sizeof(szSW));
        if (SP_OK == res)
        {
            break;
        }
        else
        {
            Sleep(TIMEOUT_1S);
        }
    }

    SP_SetProperty(m_hDUT, SP_ATTR_TIMEOUT, 0, (LPCVOID)ulTimeOut);
    return res;
}
*/

SPRESULT CModeSwitch::ActiveLogels(void)
{
    uint32 nValue = 0;
    m_pImp->GetShareMemory(InternalReservedShareMemory_SetupDebugMode, (void* )&nValue, sizeof(nValue));
    BOOL bDebugMode = (1 == nValue) ? TRUE : FALSE;

    if (bDebugMode)
    {
        uint8 recvbuf[128] = {0};
        SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ARMLOG=1", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S);
		if (SP_OK != res)  
		{
			// If AT+ARMLOG=1 is not support, then using the old DIAG command
			SP_EnableArmLog(m_hDUT, TRUE);
		}

		// Enable DSP Log
		Sleep(50);
		if (SP_OK == SP_SendATCommand(m_hDUT, "AT+SPDSPOP=2", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S) || SP_OK == res)
		{
			nValue = 1;
			m_pImp->SetShareMemory(InternalReservedShareMemory_EnableUELogel, &nValue, sizeof(nValue));
		}

        m_pImp->NOTIFY("ActiveLogel", LEVEL_UI, 1, 1, 1);

        /// ArmLogel needs software information to playback 
        CHAR szSW[2500] = {0}; 
        CHKRESULT(SP_GetSWVer(m_hDUT, CP, szSW, sizeof(szSW)));
    }

    return SP_OK;
}

SPRESULT CModeSwitch::SetCampParam(const BS_CONFIG_T& bs)
{
    SP_MODE_INFO eMode = SP_INVALID;
    SP_BAND_INFO eBand = bs.param.eBand;
    int         nArfcn = bs.param.nDlChan;
    switch(bs.eMode)
    {
    case NM_GSM:
    case NM_EDGE:
        eMode = SP_GSM;
        break;
    case NM_WCDMA:
        eMode = SP_WCDMA;
        break;
    case NM_TD:
        eMode = SP_TDSCDMA;
        break;
	case NM_CDMA:
		eMode = SP_C2K;
		break;
    case NM_LTE:
        eMode = SP_LTE;
        break;
    default:
        break;
    }

    return SP_SetCampParam(m_hDUT, eMode, eBand, nArfcn);
}

SPRESULT CModeSwitch::SetCurrentMode(RM_MODE_ENUM eMode)
{
    uint32 nValue = eMode;
    return m_pImp->SetShareMemory(ShareMemory_My_CurrentMode, (const void* )&nValue, 4);
}

void CModeSwitch::SetupOptions(const CModeOptions& op)
{
    const CModeOptions* pVal = &op;
    m_pImp->SetShareMemory(ShareMemory_My_ModeOptions, (const void* )&pVal, sizeof(pVal), IContainer::System);
}

BOOL CModeSwitch::GetOptions(CModeOptions& op)
{
    CModeOptions* pVal = NULL;
    SPRESULT res = m_pImp->GetShareMemory(ShareMemory_My_ModeOptions, (void* )&pVal, sizeof(pVal));
    if (SP_OK == res && NULL != pVal)
    {
        op = *pVal;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CModeSwitch::__defaultBSConfig(RM_MODE_ENUM eMode, BS_CONFIG_T& bs)
{
    switch(eMode)
    {
    case RM_CALIBR_POST_MODE:
#ifdef __SUPPORT_DUAL_GSM__
    case RM_WCDMA_GSM_CALIBR_POST_MODE:
#endif
        {
            bs.eMode = NM_GSM;

            bs.param.eBand   = BI_EGSM;
            bs.param.nUlChan = 57;
            bs.param.nDlChan = 15;
            bs.param.dBcchPower   = -50.0;
            bs.param.dCellPower   = -70.0;
            bs.param.dTargetPower =  33.0;
        }
        break;
    case RM_TD_CALIBR_POST_MODE:
        {
            bs.eMode = NM_TD;

            bs.param.eBand   = BI_TD_20G;
            bs.param.nUlChan = 10096;
            bs.param.nDlChan = 10096;
            bs.param.dBcchPower   = -50.0;
            bs.param.dCellPower   = -70.0;
            bs.param.dTargetPower =  23.0;
        }
        break;
    case RM_WCDMA_CALIBR_POST_MODE:
        {
            bs.eMode = NM_WCDMA;

            bs.param.eBand   = BI_W_B1;
            bs.param.nUlChan =  9612;
            bs.param.nDlChan = 10562;
            bs.param.dBcchPower   = -50.0;
            bs.param.dCellPower   = -70.0;
            bs.param.dTargetPower =  23.0;
        }
        break;
	case RM_C2K_CALIBR_POST_MODE:
		{
			bs.eMode = NM_CDMA;
			bs.param.eBand   = BI_C_BC0;
			bs.param.nUlChan =   384;
			bs.param.nDlChan =	 384;
			bs.param.dBcchPower   = -60.0;
			bs.param.dCellPower   = -60.0;
			bs.param.dTargetPower =  23.0;
			bs.param.eRc		  = CDMA_RC3;	
		}
		break;
    case RM_LTE_CALIBR_POST_MODE:
        {
            bs.eMode = NM_LTE;

            bs.param.eBand   = BI_LTE_FDD;
            bs.param.nUlChan =   300;
            bs.param.nDlChan = 18300;
            bs.param.dBcchPower   = -85.0;
            bs.param.dCellPower   = -85.0;
            bs.param.dTargetPower =  23.0;
        }
        break;
    case RM_NR_CALIBR_POST_MODE:
        {
            bs.eMode = NM_NR;
            bs.param.eBand = BI_NR_TDD;
            bs.param.nUlChan = 636666;
            bs.param.nDlChan = 636666;
            bs.param.dBcchPower   = -70.0;
            bs.param.dCellPower   = -70.0;
            bs.param.dTargetPower =  23.0;
        }
        break;

    default:
        {
            bs.eMode = NM_INVALID;
        }
        break;
    }
}

SPRESULT CModeSwitch::DeActiveLogels(void)
{
    // AT+SPDSPOP=2会改变NV值，所以测试结束后回复NV设置, 忽略返回值
    UINT32 nValue = 0;
    SPRESULT res = m_pImp->GetShareMemory(InternalReservedShareMemory_EnableUELogel, &nValue, sizeof(nValue));
    if (SP_OK == res && 1 == nValue)
    {
        //Add load SystemConfig.ini  2021.12.20    
        extern CActionApp myApp;
        std::wstring strIniPath = myApp.GetSysPath();
        strIniPath += L"\\..\\Setting\\SystemConfig.ini";

        //Armlog TimeOut  2021.12.20
        m_nArmlogTimeOut = GetPrivateProfileIntW(L"ArmLog", L"TIMEOUT", TIMEOUT_30S, strIniPath.c_str());

        // Wait some time for logel dump out of DUT buffer  
        CONST DWORD LOGEL_DUMP_TIME = m_pImp->GetConfigValue(L"GLOBAL:LOGEL_DUMP_TIME", m_nArmlogTimeOut, TRUE);

        m_pImp->NOTIFY("LogelDumping", LEVEL_UI, 1, 1, 1, NULL, -1, NULL, "Waiting %d secs", LOGEL_DUMP_TIME/1000);
        
        if (NULL != m_pImp->GetAdjParam().hStopEvent)
        {
            if (WAIT_OBJECT_0 == WaitForSingleObject(m_pImp->GetAdjParam().hStopEvent, LOGEL_DUMP_TIME))
            {
                return SP_E_USER_ABORT;
            }
        }
        else
        {
            Sleep(LOGEL_DUMP_TIME);
        }

        uint8 recvbuf[64] = {0};
        SP_SendATCommand(m_hDUT, "AT+SPDSPOP=0", TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S);
        SP_SendATCommand(m_hDUT, "AT+ARMLOG=0",  TRUE, recvbuf, sizeof(recvbuf), NULL, TIMEOUT_2S);
        nValue = 0;
        m_pImp->SetShareMemory(InternalReservedShareMemory_EnableUELogel, &nValue, sizeof(nValue));

        m_pImp->NOTIFY("DeActiveLogel", LEVEL_UI, 1, 1, 1);
    }

    return SP_OK;
}

SPRESULT CModeSwitch::PowerOff(BOOL bAuto/* =TRUE */)
{
	BOOL bOff = FALSE;
    if (RM_INTERNAL_POWEROFF_MODE == GetCurrentMode())
    {
        m_pImp->LogRawStrA(SPLOGLV_INFO, "Current DUT is already powered off!");
        bOff = TRUE;
    }
	if(!bOff)
	{
		CHKRESULT(DeActiveLogels());
	}
    if (!m_pImp->IsFakeDC() && bAuto)
    {
    //  Safe remove device: 
    //  Make sure port is closed before device is removed
    //  Otherwise PC USB host perhaps hang dead and restore unless reboot PC.

		if(!bOff)
		{
			SP_EndPhoneTest(m_hDUT);
		}
        

        CHKRESULT(m_pDCSource->SetVoltage((float)-1.0));

        // Bug1024842: Switch off VBUS 
        IDCS* pVBUS = NULL;
        if (   (SP_OK == m_pImp->GetShareMemory(ShareMemory_VBusPowerSupplyObject, (LPVOID)&pVBUS, sizeof(IDCS*))) 
            && (NULL != pVBUS)
            )
        {
            // 独立的VBUS电源
            pVBUS->SetVoltage(static_cast<float>(-1.0));
        }
        else
        {
			int32 nVbusChan = 0;
			DCS_CHANNEL_SPECIAL stChan;
			stChan.nChannelType = DC_CHANNEL_VBUS;
			if (SP_OK == m_pDCSource->GetProperty(DCP_CHANNEL_SPECIAL, NULL, &stChan))
			{
				nVbusChan = stChan.nChannel;
			}
            if (nVbusChan > 0)
            {
                m_pDCSource->SetVoltage(static_cast<float>(-1.0), DC_CHANNEL_VBUS);
            }
        }
    }
    else
    {
		if(!bOff)
		{
			CHKRESULT(SP_PowerOff(m_hDUT));
		}
        
    }

    return SetCurrentMode(RM_INTERNAL_POWEROFF_MODE);
}

SPRESULT CModeSwitch::DeepSleep(DWORD delayTime)
{
    if (RM_INTERNAL_DEEPSLEEP_MODE == GetCurrentMode())
    {
        m_pImp->LogRawStrA(SPLOGLV_INFO, "Current DUT is already under DeepSleep mode!");
        return SP_OK;
    }

    CHKRESULT(DeActiveLogels());

    CHKRESULT(SP_DeepSleep(m_hDUT));
    
    //Bug #2846483 Phone EndPhoneTest too fast ,Need Sleep (1s);
    m_pImp->LogFmtStrA(SPLOGLV_INFO, "DelayEndPhoneTest: %d usec",delayTime);
    Sleep(delayTime);

    SP_EndPhoneTest(m_hDUT);

    return SetCurrentMode(RM_INTERNAL_DEEPSLEEP_MODE);
}

SPRESULT CModeSwitch::LDO_VDDWIFIPA(BOOL bOn)
{
	return SP_Active_LDO_VDDWIFIPA(m_hDUT, bOn);
}

SPRESULT CModeSwitch::RestartPhone(void)
{
    CHKRESULT(DeActiveLogels());
 /*
    @JXP
        PC immediately close port without waiting for a reply from the device side.
        It may cause the power off and restart not work well, 
        the reason is if there is an ack package of restart/power off needs to send back to PC,
        meanwhile if PC port is closed, DUT cannot send the ack package successfully and then 
        USB driver code of DUT will be in dead loop, 
        the result is restart and power off not be executed. 
*/
    // Here we should want ack response at first and then close port
    CHAR recvBuf[64] = {0};
    /*
    AT+SPWIQ=<iq_mode> handle by AP side
    0: restart to normal mode
    1: restart to WCDMA IQ capture mode
    2: restart to WCDMA IQ replay mode
    */
    CHKRESULT(SP_SendATCommand(m_hDUT, "AT+SPREBOOTCMD="/*"AT+SPWIQ=0"*/, TRUE, recvBuf, sizeof(recvBuf), NULL, TIMEOUT_3S));

    SP_EndPhoneTest(m_hDUT);
    if (NULL != strstr(recvBuf, "+CME ERROR"))
    {
        return SP_E_FAIL;
    }

    return SetCurrentMode(RM_INVALID_MODE);
}

SPRESULT CModeSwitch::PreSetupSignal(RM_MODE_ENUM eMode, const BS_CONFIG_T* pBS)
{
    if (!IS_SIGNAL_CELLULAR_MODE(eMode))
    {
        return SP_OK;
    }

    BS_CONFIG_T bs;
    if (m_Options.m_bSearchFixedBCCH && !m_pImp->IsFakeRFTester())
    {
        if (NULL != pBS)
        {
            bs = *pBS;
        }
        else 
        {
            if (!GetBSConfig(bs))
            {
                __defaultBSConfig(eMode, bs);
            }
        }

        // Reset equipment to signal 
        CHKRESULT(m_pRFTester->SetNetMode(bs.eMode));
        CHKRESULT(m_pRFTester->InitDev(TM_SIGNAL, SUB_INVALID, &bs.param));
        RF_PORT rfPort = {RF_ANT_1st, RS_INOUT};
        U_RF_PARAM rfParam;
        rfParam.pPort = &rfPort;
        CHKRESULT_WITH_NOTIFY1(m_pImp, m_pRFTester->SetParameter(PT_RF_PORT, rfParam), "SetParameter(PT_RF_PORT)");
    }
    else
    {
        // Switch off fixed broadcast channel 
        bs.eMode = NM_INVALID;
        bs.param.eBand = BI_INVALID;
    }

    // Set network search parameters
    CHKRESULT(SetCampParam(bs));
    return SP_OK;
}


