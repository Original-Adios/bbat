#include "stdafx.h"
#include "EnterMode.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CEnterModeG3)

#pragma comment(lib, "FunctionBox.lib")
//////////////////////////////////////////////////////////////////////////
CEnterModeG3::CEnterModeG3(void)
{

}

CEnterModeG3::~CEnterModeG3(void)
{
}

SPRESULT CEnterModeG3::__InitAction(void)
{
    SPRESULT spRlt = SP_OK;
    do
    {
        m_pObjFunBox = CreateFunBoxObject(GetCallback(), GetISpLogObject());
        if (NULL == m_pObjFunBox)
        {
            m_nEnterModeInitResult |= ERR_FUNBOX_OBJECT;
            break;
        }
        spRlt = m_pObjFunBox->FunBoxCreatHandle((LPVOID*)&m_hContext);
        if (SP_OK != spRlt)
        {
            m_nEnterModeInitResult |= ERR_FUNBOX_HANDLE;
            break;
        }
        spRlt = m_pObjFunBox->FunBoxOpen(m_nG3Port);
        if (SP_OK != spRlt)
        {
            m_nEnterModeInitResult |= ERR_FUNBOX_OPENPORT;
            break;
        }
        if (m_bAutoStartByJig)
        {
            spRlt = m_pObjFunBox->FunBoxStartMonitor();
            if (SP_OK != spRlt)
            {
                m_nEnterModeInitResult |= ERR_FUNBOX_MONIT0R;
                break;
            }
        }
    } while (false);
    if (SP_OK != spRlt)
    {
        TCHAR szInfo[128] = { 0 };
        _stprintf_s(szInfo, _T("TASK%d : Init BBAT:AFCB_G3 Jig Fail.\nDo you want to go to the main UI?"), GetAdjParam().nTASK_ID);
        if (IDYES == MessageBox(NULL, szInfo, _T("ERROR INFO"), MB_YESNO | MB_SYSTEMMODAL))
        {
            return SP_OK;
        }
    }
    return spRlt;
}


SPRESULT CEnterModeG3::__FinalAction(void)
{
    m_nEnterModeInitResult = 0;
    m_pObjFunBox->FunBoxStopMonitor();
    m_pObjFunBox->FunBoxClose();
    m_pObjFunBox->FunBoxReleaseHandle();
    m_pObjFunBox->Release();
    return SP_OK;
}

SPRESULT CEnterModeG3::AFCB_Headmic(HEADSET_TYPE nType)
{
    LPCSTR lpName[MAX_TYPE_HEADSET] =
    {
        "",
        "G3: Select Europe",
        "G3: Select America",
    };
    RETURNSPRESULT(m_pObjFunBox->FB_HeadMic_Ctrl_Select_X(nType), lpName[nType]);
    return SP_OK;
}


SPRESULT CEnterModeG3::__PollAction(void)
{
    CHKRESULT(InitCheck());
    m_pObjFunBox->TestStart(TRUE);
    CHKRESULT(AFCB_Headmic(m_eHeadsetType));
    _UiSendMsg("EnterMode Start", LEVEL_ITEM, 1, 1, 1);
    CHKRESULT(G3_EnterMode());
    return SP_OK;
}

BOOL CEnterModeG3::LoadXMLConfig(void)
{
    m_nEnterModeTimeout = GetConfigValue(L"Option:EnterModeTimeout", 30000);
    LPCWSTR lpwVoltage[2] =
    {
        L"4V",
        L"12V"
    };
    int nIndex = GetComboxValue(lpwVoltage, 2, L"Option:PowerVoltage");
    if (nIndex == 2)
    {
        return FALSE;
    }
    m_eVoltage = (ChargeVoltage)nIndex;
    m_bAutoStartByJig = GetConfigValue(L"Param:AutoStartByJig", 0);
    wchar_t    szGroup[128] = { 0 };
    swprintf_s(szGroup, L"Param:FunctionBoxPort%d", GetAdjParam().nTASK_ID);
    m_nG3Port = GetConfigValue(szGroup, -1);

    LPCWSTR lpwHeadsetName[MAX_TYPE_HEADSET] =
    {
        L"Dummy",
        L"EUROPE",
        L"AMERICA",
    };

    m_eHeadsetType = (HEADSET_TYPE)GetComboxValue(lpwHeadsetName, MAX_TYPE_HEADSET, L"Param:Headset");
    if ( m_eHeadsetType == MAX_TYPE_HEADSET )
    {
        return FALSE;
    }
  
    return TRUE;
}

SPRESULT CEnterModeG3::InitCheck(void)
{
    if ((m_nEnterModeInitResult & ERR_FUNBOX_OPENPORT) != 0)
    {
        LogFmtStrA(SPLOGLV_ERROR, "FunBox Open Port Fail!");
        return SP_E_BBAT_FUNBOX_OPENPORT_FAIL;
    }

    if ((m_nEnterModeInitResult & ERR_FUNBOX_HANDLE) != 0)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Creat FunBox Handle Fail!");
        return SP_E_SPAT_INVALID_POINTER;
    }

    if ((m_nEnterModeInitResult & ERR_FUNBOX_OBJECT) != 0)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Creat FunBox Object Fail!");
        return SP_E_SPAT_INVALID_POINTER;
    }

    if (m_bAutoStartByJig)
    {
        if ((m_nEnterModeInitResult & ERR_FUNBOX_MONIT0R) != 0)
        {
            LogFmtStrA(SPLOGLV_ERROR, "FunBox Monitor Fail!");
            return SP_E_BBAT_JIG_MONITOR_FAIL;
        }
    }
    m_bFunBoxInit = TRUE;

    CHKRESULT(SetShareMemory(L"m_bAutoStartByJig", (void*)&m_bAutoStartByJig, sizeof(m_bAutoStartByJig), IContainer::Normal));
    CHKRESULT(SetShareMemory(L"FunctionBox_Handle", (void*)&m_pObjFunBox, sizeof(m_pObjFunBox), IContainer::Normal));
    CHKRESULT(SetShareMemory(L"G3_Handle", (void*)&m_hContext, sizeof(m_hContext), IContainer::Normal));
    CHKRESULT(SetShareMemory(L"m_bFunBoxInit", (void*)&m_bFunBoxInit, sizeof(m_bFunBoxInit), IContainer::Normal));

    return SP_OK;
}

SPRESULT CEnterModeG3::G3_EnterMode()
{
    BOOL IsUpdate = FALSE;
    SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);

    CHKRESULT(PowerOn(FALSE));
    CHKRESULT(ChargeOn(FALSE, m_eVoltage));
    _UiSendMsg("FunctionBox Port", LEVEL_ITEM, 0, 0, 0, 0, -1, 0, "FunctionBox Port Num = %d", m_nG3Port);
    if (_IsUserStop())
    {
        return SP_E_BBAT_CMD_FAIL;
    }
    SP_EndPhoneTest(m_hDUT);
    const SPAT_INIT_PARAM& adjParam = GetAdjParam();

    CHKRESULT(SP_BeginPhoneTest(m_hDUT, (CHANNEL_ATTRIBUTE*)&(adjParam.ca)));
    _UiSendMsg("DUT Port", LEVEL_ITEM, 0, 0, 0, 0, -1, 0, "DUT Port Num = %d", adjParam.ca.Com.dwPortNum);


    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == hEvent)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: CreateEvent failed, Windows Error = 0x%X", __FUNCTION__, ::GetLastError());
        return SP_E_SPAT_CREATE_OBJECT;
    }

    RM_MODE_ENUM ePhoneMode;
    ePhoneMode = RM_AUTOTEST_MODE; //default

    SPRESULT res = SP_EnterModeProcessEx(m_hDUT, (CHANNEL_TYPE_USBMON == adjParam.ca.ChannelType) ? TRUE : FALSE,
        adjParam.ca.Com.dwPortNum, ePhoneMode, hEvent, m_nEnterModeTimeout, 0);

    if (res != SP_OK)
    {
        CloseHandle(hEvent);
        return res;
    }

    CHKRESULT(PowerOn(TRUE));
    CHKRESULT(ChargeOn(TRUE, m_eVoltage));

    PrintSuccessMsg(SP_OK, "Wait for port...", LEVEL_ITEM);

    const HANDLE arrHandles[] = { adjParam.hStopEvent, hEvent };
    DWORD dwCode = WaitForMultipleObjects(ARRAY_SIZE(arrHandles), arrHandles, FALSE, INFINITE);
    SP_StopModeProcess(m_hDUT);

    CloseHandle(hEvent);
    hEvent = INVALID_HANDLE_VALUE;


    switch (dwCode)
    {
    case WAIT_TIMEOUT:
        LogFmtStrA(SPLOGLV_ERROR, "Enter Mode Timeout!");
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
            res = m_pModeSwitch->SetCurrentMode(RM_AUTOTEST_MODE);
        }
    }
    break;
    default:
        LogFmtStrA(SPLOGLV_ERROR, "WaitForMultipleObjects() failed!, Windows Error = 0x%X", ::GetLastError());
        res = SP_E_SPAT_TEST_FAIL;
        break;
    }

    if (SP_OK == res)
    {
        IsUpdate = TRUE;
        SetShareMemory(InternalReservedShareMemory_IsUpdateStation, (LPCVOID)&IsUpdate, 4);
    }

    return res;

}

SPRESULT CEnterModeG3::CheckVBAT(void)
{
    float Voltage[2] = { 0 };
    if (Charge_4V == m_eVoltage)
    {
        RETURNSPRESULT(m_pObjFunBox->FB_Power_Supply_GetVoltage_X(Voltage), "Get Voltage of VBAT 4V");
        if (Voltage[0] < 3600)
        {
            _UiSendMsg("VBAT Fail", LEVEL_ITEM, 0, Voltage[0], 4300, nullptr, -1, "mV", "whether power is on ?");
            return SP_E_BBAT_VALUE_FAIL;
        }
        else
        {
            _UiSendMsg("VBAT 4V", LEVEL_ITEM, 3600, Voltage[0], 4300, nullptr, -1, "mV", "");
        }
    }
    else if (Charge_12V == m_eVoltage)
    {
        RETURNSPRESULT(m_pObjFunBox->FB_Charge_GetVoltage12_X(Voltage), "Get Voltage of VBAT 12V");
        if (Voltage[0] < 11600)
        {
            _UiSendMsg("VBAT Fail", LEVEL_ITEM, 0, Voltage[0], 12400, nullptr, -1, "mV", "whether power is on ?");
            return SP_E_BBAT_VALUE_FAIL;
        }
        else
        {
            _UiSendMsg("VBAT 12V", LEVEL_ITEM, 11600, Voltage[0], 12400, nullptr, -1, "mV", "");
        }
    }

    return SP_OK;
}
