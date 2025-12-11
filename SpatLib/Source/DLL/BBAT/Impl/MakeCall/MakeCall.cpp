#include "StdAfx.h"
#include "MakeCall.h"

//
IMPLEMENT_RUNTIME_CLASS(CMakeCall)

//////////////////////////////////////////////////////////////////////////
CMakeCall::CMakeCall(void)
{

}

CMakeCall::~CMakeCall(void)
{
}
SPRESULT CMakeCall::__PollAction(void)
{
    int nRssi = 0;
    CHKRESULT(OpenSimCard());
    Sleep(m_nSleep1);
    if (m_nGsmOnly&&(!m_bIOT))
    {
        GsmOnly();//有些项目此命令一定会失败，因此不对此命令是否执行成功进行判断
    }
    else 
    {
        char szRecv[128] = { 0 };
        CHKRESULT(SP_SendATCommand(m_hDUT, "AT+SPTESTMODEM=6,6", TRUE, szRecv, sizeof(szRecv), nullptr, 10000));
    }

    CHKRESULT(OpenProtocolStack());
    Sleep(m_nSleep2);
    CHKRESULT(MakeCall());
    Sleep(m_nSleep);
    CHKRESULT(ReadStatus());
    CHKRESULT(ReadRssi(nRssi));
    if (nRssi > MAX_CSQ_VALUE|| m_bIOT)
    {
        CHKRESULT(ReadLteRssi());
    }
    CHKRESULT(FinishCall());
    return SP_OK;

}
BOOL CMakeCall::LoadXMLConfig(void)
{
	string strUp = _W2CA(GetConfigValue(L"Option:Spec_Up", L"31"));
	string strDown = _W2CA(GetConfigValue(L"Option:Spec_Down", L"5"));
	m_nRssiUp = stoi(strUp);
	m_nRssiDown = stoi(strDown);

    m_bIOT = GetConfigValue(L"Option:IOT", 0);
    m_strPhoneNumber = _W2CA(GetConfigValue(L"Option:PhoneNumber", L"112"));
    m_nGsmOnly = GetConfigValue(L"Option:Gsm_Only", 1);
    m_nSleep = GetConfigValue(L"Option:Sleep", 0);
    m_nSleep1 = GetConfigValue(L"Param:Sleep1_ms", 0);
    m_nSleep2 = GetConfigValue(L"Param:Sleep2_ms", 0);


	return TRUE;
}

SPRESULT CMakeCall::OpenSimCard()
{
	char szRecv[128] = { 0 };
    if (m_bIOT)
    {
        CHKRESULT(PrintErrorMsg(
            SP_SendATCommand(m_hDUT, "AT+SETVOLTE=1", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
            "Open Sim Card",
            LEVEL_ITEM));
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_SendATCommand(m_hDUT, "AT+SFUN=2", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
            "Open Sim Card",
            LEVEL_ITEM));
    }

	string strReturn = szRecv;

	if (-1 == strReturn.find("OK"))
	{
		PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Open Sim Card", LEVEL_ITEM);
		return SP_E_BBAT_VALUE_FAIL;
	}
	PrintSuccessMsg(SP_OK, "Open Sim Card", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CMakeCall::GsmOnly()
{
    SPRESULT Res = SP_OK;
    char szRecv[128] = { 0 };
    Res = SP_SendATCommand(m_hDUT, "AT+SPTESTMODEM=10,10", TRUE, szRecv, sizeof(szRecv), nullptr, 10000);
    if (SP_OK != Res)
    {
        LogFmtStrA(SPLOGLV_INFO, "AT+SPTESTMODEM=10,10 Command Failed");
    }
    else
    {
        LogFmtStrA(SPLOGLV_INFO, "AT+SPTESTMODEM=10,10 Command Success");
    }
    string strReturn = szRecv;
    if (-1 == strReturn.find("OK"))
    {
        LogFmtStrA(SPLOGLV_INFO, "Gsm Only Failed");
        Res = SP_E_BBAT_VALUE_FAIL;
    }
    else
    {
        LogFmtStrA(SPLOGLV_INFO, "Gsm Only Pass");
        Res = SP_OK;
    }

    return Res;
}

SPRESULT CMakeCall::OpenProtocolStack()
{
    if (m_bIOT)
    {
        return SP_OK;
    }
	char szRecv[128] = { 0 };
	CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+SFUN=4", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
		"Open Protocol Stack",
		LEVEL_ITEM));

	string strReturn = szRecv;
	if (-1 == strReturn.find("OK"))
	{
		PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Open Protocol Stack", LEVEL_ITEM);
		return SP_E_BBAT_VALUE_FAIL;
	}
	PrintSuccessMsg(SP_OK, "Open Protocol Stack", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CMakeCall::MakeCall()
{
    char szRecv[128] = { 0 };
    char szPhoneNumber[128] = { 0 };
    char szInfo[128] = { 0 };
    sprintf_s(szPhoneNumber, sizeof(szPhoneNumber), "ATD%s", m_strPhoneNumber.c_str());
    sprintf_s(szInfo, sizeof(szInfo), "Make Call %s", m_strPhoneNumber.c_str());

    CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, szPhoneNumber, TRUE, szRecv, sizeof(szRecv), nullptr, 10000), szInfo, LEVEL_ITEM));

    string strReturn = szRecv;
    if (-1 == strReturn.find("OK"))
    {
        PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, szInfo, LEVEL_ITEM);
        return SP_E_BBAT_VALUE_FAIL;
    }
    PrintSuccessMsg(SP_OK, szInfo, LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CMakeCall::ReadStatus()
{
    char szRecv[128] = { 0 };
    CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+CPAS", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
        "Call Status",
        LEVEL_ITEM));

    string strReturn = szRecv;
    if (-1 == strReturn.find("OK"))
    {
        PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Call Status", LEVEL_ITEM);
        return SP_E_BBAT_VALUE_FAIL;
    }

    PrintSuccessMsg(SP_OK, "Call Status", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CMakeCall::ReadRssi(int &nRssi)
{
    if (m_bIOT)
    {
        return SP_OK;
    }
    char szRecv[128] = { 0 };
    for (int i = 0; i < 5; i++)
    {
        CHKRESULT(PrintErrorMsg(
            SP_SendATCommand(m_hDUT, "AT+CSQ", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
            "Read Rssi",
            LEVEL_ITEM));

        string strReturn = szRecv;
        //  Return: +CSQ:<rssi>,<ber>

        int nRssiStart = strReturn.find(":");
        int nRssiEnd = strReturn.find(",");

        if (nRssiStart == -1 && nRssiEnd == -1)
        {
            PrintErrorMsg(SP_E_BBAT_CMD_FAIL, "Read Rssi", LEVEL_ITEM);
            return SP_E_BBAT_CMD_FAIL;
        }

        string strRssi = strReturn.substr(nRssiStart + 1, nRssiEnd);
        nRssi = stoi(strRssi);
        if (nRssi >=m_nRssiDown)
        {
            break;
        }
        Sleep(1000);
    }

    if (m_nGsmOnly)
    {
        _UiSendMsg("Read Rssi", LEVEL_ITEM, m_nRssiDown, nRssi, m_nRssiUp, nullptr, -1, nullptr);
        if (nRssi<m_nRssiDown || nRssi>m_nRssiUp)
        {
            return SP_E_BBAT_VALUE_FAIL;
        }
    }
    else
    {
        if (nRssi < m_nRssiDown)
        {
            _UiSendMsg("Read Rssi", LEVEL_ITEM, m_nRssiDown, nRssi, m_nRssiUp, nullptr, -1, nullptr);
            return SP_E_BBAT_VALUE_FAIL;
        }
        else if (nRssi <= m_nRssiUp)
        {
            _UiSendMsg("Read Rssi", LEVEL_ITEM, m_nRssiDown, nRssi, m_nRssiUp, nullptr, -1, nullptr);
        }else
        {
            _UiSendMsg("Read Rssi", LEVEL_ITEM, m_nRssiDown, nRssi, 99, nullptr, -1, nullptr);
        }
    }

    return SP_OK;
}

SPRESULT CMakeCall::ReadLteRssi()
{
    char szRecv[128] = { 0 };
    string strRssi;
    int nLteRssi;
    CUtility utility;
    for (int i = 0; i < 5; i++)
    {
        CHKRESULT(PrintErrorMsg(
            SP_SendATCommand(m_hDUT, "AT+CESQ", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
            "Read LTE Rssi",
            LEVEL_ITEM));

        //Return: AT+CESQ=<RSSI>,<BER>,255,255,255
        string strReturn = szRecv;
        size_t pos = strReturn.find(',');
        if (pos == -1)
        {
            PrintErrorMsg(SP_E_BBAT_CMD_FAIL, "Read LTE Rssi", LEVEL_ITEM);
            return SP_E_BBAT_CMD_FAIL;
        }

        std::vector<string> strReurnVec = utility.s_split(strReturn.substr(pos),",");
        strRssi = strReurnVec[5];

        nLteRssi = stoi(strRssi);
        if (nLteRssi < 99)
        {
            break;
        }
        Sleep(1000);
    }

    _UiSendMsg("Read Rssi", LEVEL_ITEM, 
        1, 1, 1, 
        nullptr, -1, nullptr, 
        "Value = %d", stoi(strRssi));

    return SP_OK;
}

SPRESULT CMakeCall::FinishCall()
{
	char szRecv[128] = { 0 };
	CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "ATH", TRUE, szRecv, sizeof(szRecv), nullptr, 10000),
		"Finish Call",
		LEVEL_ITEM));

	string strReturn = szRecv;
	if (-1 == strReturn.find("OK"))
	{
		PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "Finish Call", LEVEL_ITEM);

		return SP_E_BBAT_VALUE_FAIL;
	}
	PrintSuccessMsg(SP_OK, "Finish Call", LEVEL_ITEM);

	return SP_OK;
}
