#include "StdAfx.h"
#include "LgitMesStart.h"
#include "SocketCon.h"

//
IMPLEMENT_RUNTIME_CLASS(CLgitMesStart)
//////////////////////////////////////////////////////////////////////////
CLgitMesStart::CLgitMesStart(void)
{
    m_strIP = "127.0.0.1";
    m_dwPort = 8300;
}

CLgitMesStart::~CLgitMesStart(void)
{
}

BOOL CLgitMesStart::LoadXMLConfig(void)
{
    m_dwPort = (DWORD)GetConfigValue(L"Option:Port", 0);
    m_strIP = _W2CA(GetConfigValue(L"Option:IP", L""));

    return TRUE;
}

SPRESULT CLgitMesStart::__PollAction(void)
{
    CONST CHAR* ITEM_NAME = "MES Start";


    CSocketCon clinet;
    int ret = -1;
	ret = clinet.Connect(m_strIP, m_dwPort);
    if (ret != 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "MES Connect Error: %d", ret);
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES Connect Error!");
        return SP_E_MES_ERROR;
    }

    CHKRESULT(SetShareMemory(LGIT_ShareMemory_LgitMesIp, (CONST VOID* )m_strIP.c_str(), sizeof(m_strIP.length())));
    CHKRESULT(SetShareMemory(LGIT_ShareMemory_LgitMesPort, (CONST VOID* )&m_dwPort, sizeof(m_dwPort)));

    /*
    Send Message: [STRE|MER011|ON|20140307155212.528|EIF||EIF||FID|355391090035308||1|B|Y|P|||N||0|0|0|0|0||||||||||]
    20140307155212.528: start time
    355391090035308: IMEI
    */
    /*
    Reveive Message: [STSE|MES011|ON|20140307155212.5|||||FID|355391090035308|WDHT-KT114-F.KXX|1|1|Y|||N|||||||||||||||||]
    Action ID = STSE
    Message ID = MES011
    Event Time = hhmm check to make FID
    ID = IMEI
    Whether to start work flag = Y (If N, Stop the test and show and write reason for issue
    */
	std::string strSendMsg = "";
	std::string strReceiveMsg = "";
    
    QRCODE Code;
    if (!GetQRCode(Code))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "No QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    //std::string strID = "355391090035308"; //debug
    
    std::string strID = "";
    if (!ParseIDFromQRCode(Code, strID))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unknown QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);
    
    CHAR szTime[20] = "";
    sprintf_s(szTime, "%04d%02d%02d%02d%02d%02d.%03d",
        time.wYear,
        time.wMonth,
        time.wDay,
        time.wHour,
        time.wMinute,
        time.wSecond,
        time.wMilliseconds
        );

    strSendMsg = "[STSE|MER011|ON|";
    strSendMsg += szTime;
    strSendMsg += "|EIF||EIF||FID|";
    strSendMsg += strID;
    strSendMsg += "||1|B|Y|P|||N||0|0|0|0|0||||||||||]";

    LogFmtStrA(SPLOGLV_INFO, "MesStart send message: %s", strSendMsg.c_str());
	ret = clinet.Send(strSendMsg);
	if (ret != 0)
	{
        LogFmtStrA(SPLOGLV_INFO, "MES Send Message Error: %d", ret);
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES Send Message Error!");
		return SP_E_MES_ERROR;
	}
    
    ret = clinet.Receive(strReceiveMsg);
    
    //strReceiveMsg = "[STSE|MES011|ON|20140307155212.5|||||FID|355391090035308|WDHT-KT114-F.KXX|1|1|Y|||N|||||||||||||||||]"; //debug

    LogFmtStrA(SPLOGLV_INFO, "MesStart receive message: %s", strReceiveMsg.c_str());
    if (ret != 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "MES Receive Message Error: %d", ret);
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES Receive Message fail!");
        return SP_E_MES_ERROR;
    }

    INT nCount = 0;
    LPSTR* lppToken = GetTokenStringA(strReceiveMsg.c_str(), "|", nCount);
    if (nCount != 34)
    {
        //LogFmtStrA(SPLOGLV_ERROR, "Invalid Receive Message format: %s", strReceiveMsg.c_str());
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Receive Message formate error!");
        return SP_E_MES_ERROR;
    }
    std::string strStartFlag = lppToken[13]; //Whether to start work flag
    if (strStartFlag.compare("Y") != 0)
    {
        //LogFmtStrA(SPLOGLV_ERROR, "MES Start Work Flag is not Y: %s", strReceiveMsg.c_str());
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES Start Fail!");
        return SP_E_MES_ERROR;
    }

    CHAR szStartTime[4] = "";
    sprintf_s(szStartTime, "%2d%2d", time.wHour, time.wMinute);
    CHKRESULT(SetShareMemory(LGIT_ShareMemory_MES_StartTime, szStartTime, sizeof(szStartTime)));

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}

