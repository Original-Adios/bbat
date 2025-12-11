#include "StdAfx.h"
#include "LgitMesEnd.h"
#include "SocketCon.h"

//
IMPLEMENT_RUNTIME_CLASS(CLgitMesEnd)
//////////////////////////////////////////////////////////////////////////
CLgitMesEnd::CLgitMesEnd(void)
{
    //m_strIP = "127.0.0.1";
    //m_dwPort = 8300;
    m_bStopUpload = FALSE;
}

CLgitMesEnd::~CLgitMesEnd(void)
{
}

//BOOL CLgitMesEnd::LoadXMLConfig(void)
//{
//    return TRUE;
//}

SPRESULT CLgitMesEnd::__PollAction(void)
{
    CONST CHAR* ITEM_NAME = "MES End";
    //CHAR szIP[32] = "127.0.0.1";
    //DWORD dwPort = 8300;
    //CHKRESULT(GetShareMemory(LGIT_ShareMemory_LgitMesIp, (VOID* )szIP, sizeof(dwPort)));
    //CHKRESULT(GetShareMemory(LGIT_ShareMemory_LgitMesPort, (VOID* )&dwPort, sizeof(dwPort)));

 //   CSocketCon clinet;
 //   int ret = -1;
	//ret = clinet.Connect(std::string(szIP), dwPort);
 //   if (ret != 0)
 //   {
 //       LogFmtStrA(SPLOGLV_INFO, "MES Connect Error: %d", ret);
	//	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Connect fail!");
 //       return SP_E_MES_ERROR;
 //   }

    /*
    Complete Work Message: [EDRE|MER012|ON|20130225175526.528|EIF||EIF||FID|355391090035308||1|B|1|P|P|||N||||N||PASS=P;CTQ00=1000;CTQ002=2.52||||||I02=355391090035308||||||||||]

    Seq.	MER012	EQP	작업 완료 요청 (Complete)	MER012 -> N/A	File
    0	Action ID	String	Action ID	EDRE	
    1	Massage ID	String	Message ID	MER012	
    2	On/Off Mode	String	On/Off Mode	ON	
    3	Event Time	String	Event Time : YYYYMMDDHHMISS.FFF (24시 기준)	20130225175526.528	
    4	Process Code	String	Process Code	EIF	
    5	Machine Segment	String	Machine Segment		
    6	Machine Code	String	Machine Code	EIF	
    7	Port	String	Port		
    8	ID TYPE	String	ID TYPE	FID	
    9	ID	String	LOT ID	355391090035308	IMEI
    10	Model ID	String	Model ID		
    11	Quantity	Number	작업 수량	1	
    12	Unit	String	작업 단위	B	
    13	자공정 측정 차수	String	공정에서 품질 측정 작업 차수	1	
    14	공정 착,완공 적용 여부	String	공정 착,완공 Data로 사용 여부	P	
    15	Pass / Fail Flag	String	Pass / Fail Flag	P	P(Pass), F(Fail)
    16	Link ID Type	String	Link ID Type		
    17	Link ID	String	Link ID		
    18	Request Label	String	Request Label	N	
    19	Label Type	String	Label Type		
    20	Label Information	String	Label Information		
    21	Barcode Label Key	String	바코드 Label Key		
    22	Repair move flag	String	Repair move flag	N	N/A
    23	Defect Information	String	Defect Information		N/A
    24	CTQ Data	String	CTQ Data	PASS=P;CTQ00=1000;CTQ002=2.52	"' + Item + '=' + Result + '' + ‘;’ 
    - Judge for pass / fail : P(Pass), F(Fail)"
    25	MAC ID	String	MAC ID		
    26	IMEI ID	String	IMEI ID		
    27	MEID ID	String	MEID ID		
    28	License ID	String	License ID		Link the MAC
    29	Serial No	String	Serial No 리스트		N/A
    30	Alias List	String	Alias List	I02=355391090035308	"Alias List : Unique Number
    - I02=IMEI Value"
    31	Line	String	Line		Line
    32	ATTRIBUTE02	String	ATTRIBUTE02		
    33	ATTRIBUTE03	String	ATTRIBUTE03		
    34	ATTRIBUTE04	String	ATTRIBUTE04		
    35	ATTRIBUTE05	String	ATTRIBUTE05		
    36	ATTRIBUTE06	String	ATTRIBUTE06		
    37	ATTRIBUTE07	String	ATTRIBUTE07		
    38	ATTRIBUTE08	String	ATTRIBUTE08		
    39	ATTRIBUTE09	String	ATTRIBUTE09		
    40	ATTRIBUTE10	String	ATTRIBUTE10		
    */
	std::string strSendMsg = "";
	//std::string strReceiveMsg = "";
    QRCODE Code;
    if (!GetQRCode(Code))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "no QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    std::string strID = "";
    if (!ParseIDFromQRCode(Code, strID))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unknown QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);
    
    CHAR szTime[20] = "";
    sprintf_s(szTime, "%4d%2d%2d%2d%2d%2d.%3d",
        time.wYear,
        time.wMonth,
        time.wDay,
        time.wHour,
        time.wMinute,
        time.wSecond,
        time.wMilliseconds
        );

    std::string strResult = "F";
    std::string strCTQ = "";
    std::string strFID = "";
    std::string strFilePath = "C:\\UMES\\DATA\\EQP\\";
    SPTEST_RESULT_T simba_result;
    SPRESULT sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
    if (SP_OK != sp_result)
    {   
        NOTIFY("MES_SendTestResult", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(TestResult)");
        return sp_result;
    }
    if (simba_result.errCode == SP_OK)
    {
        strResult = "P";
        strCTQ = "PASS=P;";
    }
    else
    {
        strResult = "F";
        strCTQ = "PASS=F;";
    }

    std::string strData = "";
    strSendMsg = "[EDRE|MER012|ON|";
    strSendMsg += szTime;
    strSendMsg += "|EIF||EIF||FID|";
    strSendMsg += strID;
    strSendMsg += "||1|B|1|P|";
    strSendMsg += strResult;
    strSendMsg += "|||N||||N||";
    strSendMsg += strCTQ;
    strSendMsg += GetCTQData(strData);
    strSendMsg += "||||||I02=";
    strSendMsg += strID;
    strSendMsg += "||||||||||]";

    LogFmtStrA(SPLOGLV_INFO, "MesEnd send message: %s", strSendMsg.c_str());
	//ret = clinet.Send(strSendMsg);
	//if (ret != 0)
	//{
 //       LogFmtStrA(SPLOGLV_INFO, "MES Send Message Error: %d", ret);
	//	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES Send Message Error!");
	//	return SP_E_MES_ERROR;
	//}

    CHAR hhmm[8] = {0};
    CHKRESULT(GetShareMemory(LGIT_ShareMemory_MES_StartTime, (LPVOID)hhmm, sizeof(hhmm)));
    strFID = hhmm + strID;
    strFilePath += strFID;
    strFilePath += ".txt";

    if (!SaveCompletionInfo(strFilePath, strSendMsg))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES Save Completion Information Error!");
        return SP_E_MES_ERROR;
    }

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}

void CLgitMesEnd::Clear(void)
{
    m_LockQ.clear();
}

void CLgitMesEnd::PushData(CONST ITEMDATA_T& item)
{
    if (!m_bStopUpload 
        && IS_BIT_SET(item.nLv, LEVEL_FT))
    {
        m_LockQ.push_back(item);
    }
}

std::string& CLgitMesEnd::GetCTQData(std::string &strCTQData)
{
    std::deque<ITEMDATA_T> dqe;
    strCTQData = "";
    std::string strItem = "";
    while (m_LockQ.size() > 0)   
    { 
        INT nCount = m_LockQ.pop_all(dqe, INFINITE);

        INT nIndex = 0;
        while (nIndex < nCount)
        {
            ITEMDATA_T head = dqe[nIndex++];
            strItem += head.szActionName;
            strItem += "_";
            strItem += head.szItemName;
            strItem += "_";
            strItem += head.szCond;
            strItem += "_";
            strItem += head.szBand;
            strItem += "_";
            strItem += std::to_string((unsigned long long)head.nChannel);
            strItem += "=";
            strItem += std::to_string((long long) head.dValue);
            strItem += ";";

            strCTQData += strItem;
        } // end of while (nIndex < nCount)
    } // end of while (m_LockQ.size() > 0)   

    m_LockQ.clear();

    return strCTQData;
}

BOOL CLgitMesEnd::SaveCompletionInfo(const std::string filePath, const std::string strInfo)
{
    FILE* fFile = NULL;
    //BOOL bRet = FALSE;
    int nErr = fopen_s(&fFile, filePath.c_str(), "w+");
    if (nErr != 0)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Open file(%s) fail!", filePath.c_str());
        return FALSE;
    }

    nErr = fputs(strInfo.c_str(), fFile);
    fclose(fFile);

    if (nErr == EOF)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Write completion information to file(%s) fail!", filePath.c_str());
        return FALSE;
    }    

    return TRUE;
}