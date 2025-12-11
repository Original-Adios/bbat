#include "StdAfx.h"
#include "EnterMode.h"
#include "CLocks.h"
#include "BarCodeUtility.h"
#include "SharedDefine.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CEnterMode)

///
CEnterMode::CEnterMode(void)
    : m_bLoadSN(TRUE)
    , m_nMaxRetryCount(1)
    , m_bLogUsingOtherPort(FALSE)
{
}

CEnterMode::~CEnterMode(void)
{
}

BOOL CEnterMode::LoadXMLConfig(void)
{
	LPCWSTR lpszMode = GetConfigValue(L"Option:TestMode", L"GSM Calibration");
	if (NULL == lpszMode)
	{
		return FALSE;
	}

	BOOL bValid = FALSE;
	for (INT i=0; i<MAX_DUT_MODE; i++)
	{
		if (0 == _wcsicmp(lpszMode, M_DUT_MODE[i].name))
		{
			bValid  = TRUE;
			m_Options.m_eMode = M_DUT_MODE[i].mode;
			break;
		}
	}

	if (!bValid)
	{
		return FALSE;
	}

    LPCWSTR lpszUsbSpeed = GetConfigValue(L"Param:UsbSpeedType", L"NotSet Speed");
    for (INT i = 0; i < MAX_USB_SPEED_MODE; i++)
    {
        if (0 == _wcsicmp(lpszUsbSpeed, M_USB_SPEED_MODE[i].name))
        {
            bValid = TRUE;
            m_Options.m_eUsbSpeedType = M_USB_SPEED_MODE[i].mode;
            break;
        }
    }

    if (!bValid)
    {
        return FALSE;
    }

    INT nValue  = GetConfigValue(L"Option:TimeOut", TIMEOUT_30S);
    m_Options.m_dwTimeOut = (0 == nValue) ? INFINITE : nValue;

    // Bug1020677
    m_Options.m_bChargeOff = (BOOL)GetConfigValue(L"Option:ChargeOff", 0);

    m_Options.m_dwSleepTime = GetConfigValue(L"Param:SleepTime", 0);

    // default
    m_Options.m_u32DelayTime = 350; 

    // Restart option
    std::wstring strOption = GetConfigValue(L"Param:RestartOption", L"Automatically");
    if (strOption == L"Command")
    {
        m_Options.m_eRestartOption = CModeOptions::COMMAND;
    }
    else if (strOption == L"Customized")
    {
        m_Options.m_eRestartOption = CModeOptions::CUSTOMIZED;
    }
    else if (strOption == L"NpiDevice")
    {
        m_Options.m_eRestartOption = CModeOptions::NPIDEVICE;
    }
    else
    {
        m_Options.m_eRestartOption = CModeOptions::AUTO;
    }

    m_Options.m_bSearchFixedBCCH = (1 == GetConfigValue(L"Param:SearchFixedBCCH", 1) ? TRUE : FALSE);

    /*
    // Bug 1162044/925444
    // Un-check modem software anymore to adapt to modem less product.
    // NonRF.dll also provides CCheckDiagConnect to replace.
    m_Options.m_bLoadModemSwVer = (1 == GetConfigValue(L"Param:LoadModemSW", 1) ? TRUE : FALSE);
    */

    // By default, load SN.
    m_bLoadSN = (1 == GetConfigValue(L"Param:LoadSN", TRUE)) ? TRUE : FALSE;

    // Max retry count
    m_nMaxRetryCount = GetConfigValue(L"Param:MaxRetryCount", 1/*default: 1*/);

    m_Options.m_fVBUSVoltage = (float)GetConfigValue(L"Param:VBUS", 5.0f);

	m_Options.m_bAuth = (float)GetConfigValue(L"Param:EnterModeAuth", 0);
    // 
    m_pModeSwitch->SetupOptions(m_Options);

    // Arm log is got from different port.
    m_bLogUsingOtherPort = (1 == GetConfigValue(L"Option:UsingOtherPort", 0)) ? TRUE : FALSE;

    return TRUE;
}

SPRESULT CEnterMode::__PollAction(void)
{
//  @JXP 20180413: ½øÄ£Ê½/CheckPreStationÊ§°Ü²»¸üÐÂÕ¾Î»
    BOOL IsUpdate = FALSE;
    SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);
    SP_SetProperty( m_hDUT, SP_ATTR_LOGEL_USING_OTHER_PORT, 0, (LPCVOID)m_bLogUsingOtherPort);

    SPRESULT    res = SP_OK;
    UINT32 u32Value = 0;
    res = GetShareMemory(ShareMemory_UBootSecureBoot, &u32Value, 4);
    if (SP_OK == res && 1 == u32Value)
    {
        m_Options.m_eMode = RM_UBOOT_CALIBRATION_MODE;
    }

    BOOL bFirst = TRUE;
    GetShareMemory(ShareMemory_AutoStart, &bFirst, 4);

    if (m_bAutoStart && bFirst)
    {
        m_Options.m_dwTimeOut = INFINITE;
    }

    for (UINT32 i=0; i<m_nMaxRetryCount; i++)
    {
        res = m_pModeSwitch->Reboot(m_Options.m_eMode, NULL, TRUE);
        if (SP_OK == res)
        {
            break;
        }
    }

    if (SP_OK == res)
    {
        if (CHANNEL_TYPE_USBMON == GetAdjParam().ca.ChannelType)
        {
            SPDBT_INFO info;
            SP_GetProperty(m_hDUT, SP_ATTR_DBT_INFO, 0, (LPVOID)&info);
            NOTIFY("USB", _W2CA(info.szFriendlyName));
        }
        else
        {
            CHAR szValue[20] = {0};
            sprintf_s(szValue, "%d", GetAdjParam().ca.Com.dwPortNum);
            NOTIFY("COM", szValue);
        }

        if (m_bLoadSN)
        {
            CHAR szSN[64] = {0};
            INPUT_CODES_T InputSN[BC_MAX_NUM];
            if (SP_OK != GetShareMemory(ShareMemory_My_UserInputSN, (void* )&InputSN, sizeof(InputSN)))  
            {
                if (SP_OK == SP_LoadSN(m_hDUT, SN1, szSN, sizeof(szSN)))
                {
                    NOTIFY(DUT_INFO_SN1, szSN);
                    SetShareMemory(ShareMemory_SN1, (const void* )&szSN[0], ShareMemory_SN_SIZE);
                }
            }
            else
            {
                strncpy_s(szSN, 64, InputSN[BC_SN1].szCode, 63);
            }
        }
    }
    
    NOTIFY("EnterMode", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Mode = 0x%X", DIAG_MODE(m_pModeSwitch->GetCurrentMode()));
    
    if (SP_OK == res)
    {
        IsUpdate = TRUE;
        SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);
    }

    return res;
}
