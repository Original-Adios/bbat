#include "StdAfx.h"
#include "ImpBase.h"
#include "ExtraLogFile.h"
#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>
#include "AFCB_G3.h"
#pragma comment(lib, "setupapi.lib")

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
    //SetupDUTRunMode(RM_AUTOTEST_MODE);
}

CImpBase::~CImpBase(void)
{
}

void CImpBase::__EnterAction(void)
{
    GetG3HandleFromShareMemory();
    GetFunctionBoxHandleFromShareMemory();
    Getm_bAutoStartByJigFromShareMemory();
    Getm_bFunBoxInitFromShareMemory();
}

SPRESULT CImpBase::GetG3HandleFromShareMemory()
{
    SPRESULT sp_result = GetShareMemory(L"G3_Handle", (void*)&m_hContext, sizeof(m_hContext));
    if (sp_result == SP_OK)
    {
        if (m_hContext == nullptr)
        {
            LogFmtStrA(SPLOGLV_ERROR, "G3_Handle is nullptr.");
            sp_result = SP_E_POINTER;
        }
    }
    return sp_result;
}

SPRESULT CImpBase::GetFunctionBoxHandleFromShareMemory()
{
    SPRESULT sp_result = GetShareMemory(L"FunctionBox_Handle", (void*)&m_pObjFunBox, sizeof(m_pObjFunBox));
    if (sp_result == SP_OK)
    {
        if (m_pObjFunBox == nullptr)
        {
            LogFmtStrA(SPLOGLV_ERROR, "FunctionBox_Handle is nullptr.");
            sp_result = SP_E_POINTER;
        }
    }
    return sp_result;
}

SPRESULT CImpBase::Getm_bAutoStartByJigFromShareMemory()
{
    return GetShareMemory(L"m_bAutoStartByJig", (void*)&m_bAutoStartByJig, sizeof(m_bAutoStartByJig));
}

SPRESULT CImpBase::Getm_bFunBoxInitFromShareMemory()
{
    return GetShareMemory(L"m_bFunBoxInit", (void*)&m_bFunBoxInit, sizeof(m_bFunBoxInit));
}

SPRESULT CImpBase::GetLocationPathRoot(SPDBT_INFO& epi, wstring& strRoot)
{
    wstring strLocationPath(epi.szLocationPath);
    wstring strTemp(strLocationPath);
    int nPostion = 0, nTotalOffset = 0;

    for (int i = 0; i < 4; i++)
    {
        nPostion = strTemp.find(L"#");
        strTemp = strTemp.substr(nPostion + 1);
        nTotalOffset += (nPostion + 1);
    }
    strRoot = strLocationPath.substr(0, nTotalOffset);
    return SP_OK;
}

SPRESULT CImpBase::PortExist(int& nPort)
{
    BOOL bBoardOpen = TRUE;
    nPort = (int)GetAdjParam().ca.Com.dwPortNum;
    SPDBT_INFO epi;
    vector<SPDBT_INFO> vecTemp;
    GetPortInfo(nPort, epi, vecTemp);
    if (epi.nPortNum == 0)
    {
        bBoardOpen = FALSE;
        CHKRESULT(SetShareMemory(L"G3_Port", (void*)&bBoardOpen, sizeof(bBoardOpen), IContainer::Normal));
        PrintErrorMsg(SP_E_FAIL, "Cannot Find G3 Port", LEVEL_ITEM);
        return SP_E_FAIL;
    }

    //端口占用检查
    CHANNEL_ATTRIBUTE ca;
    ca.ChannelType = CHANNEL_TYPE_COM;
    ca.Com.dwPortNum = nPort;
    ca.Com.dwBaudRate = 115200;
    ICommChannel* m_pChannel = nullptr;
    CreateChannel(&m_pChannel, CHANNEL_TYPE_COM);
    if (!m_pChannel->Open(&ca))
    {
        bBoardOpen = FALSE;
        CHKRESULT(SetShareMemory(L"G3_Port", (void*)&bBoardOpen, sizeof(bBoardOpen), IContainer::Normal));

        PrintErrorMsg(SP_E_FAIL, "Port is Occupied", LEVEL_ITEM);
        return SP_E_FAIL;
    }
    m_pChannel->Close();

    CHKRESULT(SetShareMemory(L"G3_Port", (void*)&bBoardOpen, sizeof(bBoardOpen), IContainer::Normal));

    return SP_OK;
}

int CImpBase::GetComboxValue(LPCWSTR* lpwSource, int nSourceSize, LPCWSTR lpPath)
{
    LPCWSTR lpwTemp = GetConfigValue(lpPath, L"");
    for (int i = 0; i < nSourceSize; i++)
    {
        if (0 == _wcsicmp(lpwSource[i], lpwTemp))
        {
            return i;
        }
    }
    return nSourceSize;
}

SPRESULT CImpBase::PrintSuccessMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level)
{
    if (Result == SP_OK)
    {
        _UiSendMsg(Msg,
            Level,
            1,
            1,
            1,
            nullptr,
            -1,
            "",
            "");
    }

    return Result;
}

SPRESULT CImpBase::PrintErrorMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level)
{
    if (Result != SP_OK)
    {
        _UiSendMsg(Msg,
            Level,
            1,
            0,
            1,
            nullptr,
            -1,
            "",
            "");
    }

    return Result;
}
DEFINE_GUID(GUID_CLASS_PORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
SPRESULT CImpBase::GetPortInfo(UINT  nPortNum, SPDBT_INFO& epi, std::vector<SPDBT_INFO>& vecPortInfo)
{

    GUID* guidDev = (GUID*)&GUID_CLASS_PORT;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

    hDevInfo = SetupDiGetClassDevsW(guidDev,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
    );

    if (INVALID_HANDLE_VALUE == hDevInfo)
    {
        LogFmtStrA(SPLOGLV_ERROR, "SetupDiGetClassDevsW() failed, WinErr = 0x%X", ::GetLastError());
        return SP_E_FAIL;
    }

    SP_DEVICE_INTERFACE_DATA ifcData;
    ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD dwIndex = 0;
    SPDBT_INFO tempEpi;

    // Enumerate port and modem class device interfaces
    do
    {
        memset(&tempEpi, 0, sizeof(tempEpi));

        if (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guidDev, dwIndex, &ifcData))
        {
            SP_DEVINFO_DATA devData;
            devData.cbSize = sizeof(SP_DEVINFO_DATA);

            if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &ifcData, NULL, 0, NULL, &devData))
            {
                if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
                {
                    // Can not get detail interface info
                    continue;
                }
            }
            
            WCHAR szHardwareID[SPDBT_NAME_MAX_LEN] = { 0 };

            // Get Friendly name from registry
            SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)tempEpi.szFriendlyName, SPDBT_NAME_MAX_LEN * sizeof(WCHAR), NULL);
            // Get port description from registry
            SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devData, SPDRP_DEVICEDESC, NULL, (PBYTE)tempEpi.szDescription, SPDBT_NAME_MAX_LEN * sizeof(WCHAR), NULL);
            // Get location path from registry
            std::vector<byte> arrLocationPaths(2000);
            tempEpi.bGetUsbInfo = SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devData, SPDRP_LOCATION_PATHS, NULL, (PBYTE)arrLocationPaths.data(), arrLocationPaths.size(), NULL);
            memcpy(tempEpi.szLocationPath, arrLocationPaths.data(), sizeof(tempEpi.szLocationPath));
            tempEpi.szLocationPath[SPDBT_NAME_MAX_LEN - 1] = L'\0';
            // Get Hardware ID from registry
            SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devData, SPDRP_HARDWAREID, NULL, (PBYTE)szHardwareID, SPDBT_NAME_MAX_LEN * sizeof(WCHAR), NULL);

            HKEY hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

            LogFmtStrW(SPLOGLV_VERBOSE, L"Friendly Name:%s; location:%s; HardwareID: %s", tempEpi.szFriendlyName, tempEpi.szLocationPath, szHardwareID);

            if (INVALID_HANDLE_VALUE != hDevKey)
            {
                // Get port name
                DWORD dwCount = SPDBT_NAME_MAX_LEN;
                RegQueryValueExW(hDevKey, L"PortName", NULL, NULL, (BYTE*)tempEpi.szPortName, &dwCount);
                RegCloseKey(hDevKey);
            }

            CheckPortInfo(tempEpi);
            if (tempEpi.nPortNum != nPortNum)
            {
                InsertPortInfo(tempEpi, vecPortInfo);
            }
            

            else
            {
                memcpy_s(&epi, sizeof(tempEpi), &tempEpi, sizeof(tempEpi));
                Sleep(100);
            }

            dwIndex++;
        }
        else
        {
            Sleep(100);

            SetupDiDestroyDeviceInfoList(hDevInfo);
            hDevInfo = SetupDiGetClassDevsW(guidDev,
                NULL,
                NULL,
                DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
            );

            memset(&ifcData, 0, sizeof(SP_DEVICE_INTERFACE_DATA));
            ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

            break;
        }

    } while (INVALID_HANDLE_VALUE != hDevInfo);

    if (INVALID_HANDLE_VALUE != hDevInfo)
    {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        hDevInfo = INVALID_HANDLE_VALUE;
    }

    return SP_OK;
}

SPRESULT CImpBase::CheckPortInfo(SPDBT_INFO& epi)
{
    int nFriendlyName = wcslen(epi.szFriendlyName) * sizeof(wchar_t);
    int nPortName = wcslen(epi.szPortName) * sizeof(wchar_t);

    if (nPortName)
    {
        // Get port number from port name
        swscanf_s(epi.szPortName, L"COM%d", &epi.nPortNum);
        if (0 == epi.nPortNum)
        {
            // Not a valid port name
            nPortName = 0;
        }
    }

    if (!nPortName && nFriendlyName && SPDBT_PORT == epi.eType)
    {
        // Try to get port number from friendly name
        swscanf_s(epi.szFriendlyName, L"(COM%d)", &epi.nPortNum);
        if (0 == epi.nPortNum)
        {
            return SP_E_FAIL;
        }
    }

    if (0 == epi.nPortNum)
    {
        // Can not get port number
        return SP_E_FAIL;
    }

    if (!nPortName)
    {
        // compose a valid port name
        nPortName = swprintf_s(epi.szPortName, L"COM%d", epi.nPortNum) * sizeof(wchar_t);
    }

    if (!nFriendlyName)
    {
        // compose a void friendly name
        nFriendlyName = swprintf_s(epi.szFriendlyName, L"COM%d", epi.nPortNum) * sizeof(wchar_t);
    }
    else
    {
        if (SPDBT_MODEM == epi.eType)
        {
            // Because the friendly name of the modem has no com port information,
            // so we append it to the end of the friendly name,if the name is too
            // long,it will be changed to "part_of_friendly_name...(COMX)"
            bool bCat = true;
            if (nPortName + nFriendlyName + 2 * sizeof(wchar_t) > SPDBT_NAME_MAX_LEN - sizeof(wchar_t))
            {
                nFriendlyName = SPDBT_NAME_MAX_LEN - sizeof(wchar_t) - nPortName - 5 * sizeof(wchar_t);
                if (nFriendlyName > 0)
                {
                    epi.szFriendlyName[nFriendlyName] = L'\0';
                    wcscat_s(epi.szFriendlyName, L"...");
                }
                else
                {
                    bCat = SP_E_FAIL;
                }
            }

            if (bCat)
            {
                wcscat_s(epi.szFriendlyName, L"(");
                wcscat_s(epi.szFriendlyName, epi.szPortName);
                wcscat_s(epi.szFriendlyName, L")");
            }
        }
    }

    return SP_OK;
}

SPRESULT CImpBase::InsertPortInfo(SPDBT_INFO& epi, std::vector<SPDBT_INFO>& vecPortInfo)
{
    if (0 == epi.nPortNum)
    {
        return SP_E_FAIL;
    }

    int nCount = vecPortInfo.size();
    int i = 0;
    for (i = 0; i < nCount; i++)
    {
        if (vecPortInfo[i].nPortNum == epi.nPortNum)
        {
            vecPortInfo[i].bExist = TRUE;
            return SP_E_FAIL;
        }
    }

    epi.bExist = TRUE;
    vecPortInfo.push_back(epi);

    return SP_OK;
}

void CImpBase::GenerateSpeakerData(int* pData)
{
    double Freq = 1000;
    double SampleRate = 44100;  //Speaker
    int   NumPeriod = 1;
    int   SampleNum;
    double Sinwave[1024] = { 0 };
    double Tp, Ts;
    Tp = 1 / Freq;
    Ts = 1 / SampleRate;
    SampleNum = (int)(NumPeriod * Tp / Ts);
    SinWavGenerate(Freq, Ts, SampleNum, Sinwave, pData);
}

void CImpBase::CalcMicData(BYTE* pData, BOOL bMic8k, AudioResult& tResult)
{
    double arrReceivedWave[1024] = { 0.0 };
    double dPeak = 0.0;
    for (int i = 0; i < 800; i++)
    {
        int nCount = i * 2;
        short sTemp = MAKEWORD(pData[nCount], pData[nCount + 1]);
        arrReceivedWave[i] = (double)sTemp;
        dPeak = max(dPeak, arrReceivedWave[i]);
    }
    tResult.dPeak = dPeak;
    //1k
    double dSampleRate = 16000;
    if (bMic8k)
    {
        dSampleRate = 8000;
    }
    double dTemp_1k = SNR_Wave_Verification(1000, dSampleRate, 800, arrReceivedWave);
    double dTemp_2k = SNR_Wave_Verification(2000, dSampleRate, 800, arrReceivedWave);
    double dMaxSnr = max(dTemp_1k, dTemp_2k);
    for (int i = 1; i < 6; i++)
    {
        dTemp_1k = SNR_Wave_Verification(1000, dSampleRate, 160, &arrReceivedWave[(i - 1) * 160]);
        dMaxSnr = max(dTemp_1k, dMaxSnr);
        dTemp_2k = SNR_Wave_Verification(2000, dSampleRate, 160, &arrReceivedWave[(i - 1) * 160]);
        dMaxSnr = max(dTemp_2k, dMaxSnr);
    }
    tResult.dSNR = dMaxSnr;
}

SPRESULT CImpBase::AnalysisG3Audio(unsigned short* pWave, BYTE byCh, double& dSNR, double& dPeak)
{
    int SampleRate = 47619;//16000;//48K暂时
    double uAverage = 0;
    double dMaxWave = 0.0;
    double dWaveData[BYSEND_BUFF_SIZE] = { NULL };


    for (int k = 0; k < BYSEND_BUFF_SIZE; k++)
    {
        uAverage += (double)pWave[k];
    }

    uAverage = uAverage / BYSEND_BUFF_SIZE;
    for (int k = 0; k < BYSEND_BUFF_SIZE; k++)
    {
        dWaveData[k] = (double)pWave[k] - uAverage;
        dMaxWave = (dMaxWave >= abs(dWaveData[k])) ? dMaxWave : abs(dWaveData[k]);
    }
    dPeak = dMaxWave;
    double dSNR1K = SNR_Wave_Verification(1000, SampleRate, BYSEND_BUFF_SIZE, dWaveData);
    double dSNR2K = SNR_Wave_Verification(2000, SampleRate, BYSEND_BUFF_SIZE, dWaveData);

    _UiSendMsg("G3:ADC Wave Cmd", LEVEL_ITEM, 1, 1, 1, 0, -1, 0,
        "Ch%d: SNR1K %.2lf, SNR2K %.2lf, Peak %.2lf",
        byCh, dSNR1K, dSNR2K, dMaxWave);

    double dSNR_Temp = 0;
    double dSNR_HeadSet_1K = 0;
    if (byCh == Head_Mic)
    {
        dSNR_Temp = dSNR2K;
        dSNR_HeadSet_1K = dSNR1K;
        dSNR_Temp = max(dSNR_Temp, dSNR_HeadSet_1K);
    }
    else
    {
        dSNR_Temp = dSNR1K;

    }
    dSNR = dSNR_Temp;

    return SP_OK;
}

SPRESULT CImpBase::ChargeOn(BOOL bStatus, ChargeVoltage eVoltage)
{
    char szInfo[128] = { 0 };
    if (Charge_4V == eVoltage)
    {
        sprintf_s(szInfo, sizeof(szInfo), ("Charge %s VBAT 4V"), bStatus ? "On" : "Off");
        RETURNSPRESULT(m_pObjFunBox->FB_Power_Supply_X(bStatus), szInfo);
    }
    else if (Charge_12V == eVoltage)
    {
        sprintf_s(szInfo, sizeof(szInfo), ("Charge %s VBAT 4V"), bStatus ? "On" : "Off");
        RETURNSPRESULT(m_pObjFunBox->FB_Power_Supply_X(bStatus), szInfo);
        sprintf_s(szInfo, sizeof(szInfo), ("Charge %s VBAT 12V"), bStatus ? "On" : "Off");
        RETURNSPRESULT(m_pObjFunBox->FB_Charge_Supply_12_X(bStatus), szInfo);
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "Charge Voltage setting error");
        return SP_E_BBAT_CMD_FAIL;
    }
    Sleep(200);
    return SP_OK;
}

SPRESULT CImpBase::PowerOn(BOOL bStatus)
{
    char szInfo[128] = { 0 };
    sprintf_s(szInfo, sizeof(szInfo), ("Power %s VBUS 5V"), bStatus ? "On" : "Off");
    RETURNSPRESULT(m_pObjFunBox->FB_Charge_Supply_X(bStatus), szInfo);

    Sleep(200);
    return SP_OK;
}

SPRESULT CImpBase::GetAppPath(void)
{
    TCHAR szSModulePath[_MAX_PATH];
    DWORD dwRet = ::GetModuleFileName(nullptr, szSModulePath, _MAX_PATH);
    if (!dwRet)
    {
        return SP_E_BBAT_BASE_ERROR;
    }
    TCHAR* pResult = _tcsrchr(szSModulePath, _T('\\'));
    if (pResult == nullptr)
    {
        return SP_E_BBAT_BASE_ERROR;
    }
    *pResult = 0;
    _tcscpy_s(m_szAppPath, szSModulePath);

    return SP_OK;
}