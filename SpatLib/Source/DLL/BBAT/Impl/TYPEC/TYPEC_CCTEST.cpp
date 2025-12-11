#include "StdAfx.h"
#include "TYPEC_CCTEST.h"

//
IMPLEMENT_RUNTIME_CLASS(CTYPEC_CCTEST)

//////////////////////////////////////////////////////////////////////////
CTYPEC_CCTEST::CTYPEC_CCTEST(void)
: m_eCCType(CC0)
, m_eUsbType(USB2_0)
{
}

CTYPEC_CCTEST::~CTYPEC_CCTEST(void)
{
}

SPRESULT CTYPEC_CCTEST::__PollAction(void)
{
	CHKRESULT(RunTypeC());

    return SP_OK;
}

BOOL CTYPEC_CCTEST::LoadXMLConfig(void)
{
    std::wstring strMode = GetConfigValue(L"Option:TESTMODE", L"Single-sided");
    if (strMode == L"Single-sided")
    {
        m_eTESTMODE = SINGAL_SIDED;
    }
    else
    {
        m_eTESTMODE = DOUBLE_SIDED;
    }

    std::wstring strCC = GetConfigValue(L"Option:CC", L"CC0");
    if (strCC == L"CC0")
    {
        m_eCCType = CC0;
    }
    else
    {
        m_eCCType = CC1;
    }

    std::wstring strUSB = GetConfigValue(L"Option:USB", L"USB2.0");
    if (strUSB == L"USB2.0")
    {
        m_eUsbType = USB2_0;
    }
    else
    {
        m_eUsbType = USB3_0;
    }

    m_nSleepTime = GetConfigValue(L"Option:SleepTime", 15000);
    return TRUE;
}

SPRESULT CTYPEC_CCTEST::RunTypeC()
{
    TypeCData DataRecv;
    memset(&DataRecv, 0, sizeof(DataRecv));
    CHKRESULT(PrintErrorMsg(SP_BBAT_TypeC(m_hDUT, &DataRecv), "BBAT TypeC Fail", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "BBAT TypeC 1st Pass", LEVEL_ITEM);
    CHKRESULT(CheckCCType_1st(DataRecv.iCCValue));
    CHKRESULT(CheckUsbType(DataRecv.iUSBValue));

    if (DOUBLE_SIDED == m_eTESTMODE)
    {
        CHKRESULT(GetPhonePort());
        CHKRESULT(MessageTips());
        _UiSendMsg("Wait For USB Plug Out And In", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, "Wait Time: %d ms", m_nSleepTime);
        Sleep(m_nSleepTime);
        CHKRESULT(ConnectPhone());
        CHKRESULT(PrintErrorMsg(SP_BBAT_TypeC(m_hDUT, &DataRecv), "BBAT TypeC Fail", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "BBAT TypeC 2nd Pass", LEVEL_ITEM);
        CHKRESULT(CheckCCType_2nd(DataRecv.iCCValue));
    }

	return SP_OK;
}

SPRESULT CTYPEC_CCTEST::MessageTips(void)
{
    TCHAR szInfo[128] = { 0 };
    _stprintf_s(szInfo, _T("Plugout the usb cable,rolling-over and plugin."));
    if (IDYES == MessageBox(NULL, szInfo, _T("ERROR INFO"), MB_YESNO | MB_SYSTEMMODAL))
    {
        return SP_OK;
    }
    else
    {
        _UiSendMsg("Test Stop", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "Setting Wrong", m_nSleepTime);
        return SP_E_FAIL;
    }
}

SPRESULT CTYPEC_CCTEST::ConnectPhone(void)
{
    std::string strTemp;
    SP_EndPhoneTest(m_hDUT);

    SPAT_INIT_PARAM& adjParam = const_cast<SPAT_INIT_PARAM&>(GetAdjParam());

    CHANNEL_ATTRIBUTE   ca;
    ca.ChannelType = CHANNEL_TYPE_COM;
    ca.Com.dwPortNum = m_nSecondEnumPort;
    ca.Com.dwBaudRate = adjParam.ca.Com.dwBaudRate;

    CHKRESULT(SP_BeginPhoneTest(m_hDUT, &ca));
    _UiSendMsg("DUT Port", LEVEL_ITEM, 1, 1, 1, 0, -1, 0, "DUT Port Num = %d", adjParam.ca.Com.dwPortNum);

    return SP_OK;
}

SPRESULT CTYPEC_CCTEST::GetPhonePort(void)
{
    m_nSecondEnumPort = SP_GetUsbPort(m_hDUT);
    if (0 == m_nSecondEnumPort)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Get Second Enum Port Fail");
        return SP_E_FAIL;
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "Second Enum Port Is:%d", m_nSecondEnumPort);
        return SP_OK;
    }
}

SPRESULT CTYPEC_CCTEST::CheckCCType_1st(int8 iCCValue)
{
    if (SINGAL_SIDED == m_eTESTMODE && (iCCValue != m_eCCType))
    {
        _UiSendMsg("CCType 1st", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "CC = %d", iCCValue);
        return SP_E_BBAT_TYPEC_READ_VALUE_FAIL;
    }
    else
    {
        m_eCCType = (CC0 == iCCValue) ? CC1 : CC0;
        _UiSendMsg("CCType 1st", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, "CC = %d", iCCValue);
        return SP_OK;
    }
}

SPRESULT CTYPEC_CCTEST::CheckCCType_2nd(int8 iCCValue)
{
    if (iCCValue != m_eCCType)
    {
        _UiSendMsg("CCType 2nd", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "CC = %d", iCCValue);
        return SP_E_BBAT_TYPEC_READ_VALUE_FAIL;
    }
    else
    {
        _UiSendMsg("CCType 2nd", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, "CC = %d", iCCValue);
        return SP_OK;
    }
}

SPRESULT CTYPEC_CCTEST::CheckUsbType(int8 iUSBValue)
{
	if (m_eUsbType == iUSBValue)
	{
		_UiSendMsg("UsbType", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, nullptr);
		return SP_OK;
	}
	
	_UiSendMsg("UsbType", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, "USB = %d", iUSBValue);
	return SP_E_BBAT_TYPEC_READ_VALUE_FAIL;
	
}


