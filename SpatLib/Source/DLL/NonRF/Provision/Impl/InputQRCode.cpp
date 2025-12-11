#include "StdAfx.h"
#include "InputQRCode.h"
#include "CLocks.h"
#include "../../../../../../Common/Json/JsonUtil.h"
#include "ActionApp.h"
#include "ShareMemoryDefine.h"
extern CActionApp myApp;
//
IMPLEMENT_RUNTIME_CLASS(CInputQRCode)
//////////////////////////////////////////////////////////////////////////
CInputQRCode::CInputQRCode(void)
{
}

CInputQRCode::~CInputQRCode(void)
{
}

BOOL CInputQRCode::LoadXMLConfig(void)
{
    m_QRCodeConfig.strFormat    = _W2CA(GetConfigValue(L"Option:QRCode:Format", L"V1"));
    m_QRCodeConfig.u32MaxLength =  GetConfigValue(L"Option:QRCode:Length", 0);
    m_QRCodeConfig.strGenType   = _W2CA(GetConfigValue(L"Option:QRCode:GenerateType", L"ScanCode"));
    m_QRCodeConfig.strFixCode   = _W2CA(GetConfigValue(L"Option:QRCode:FixCode", L""));

    return TRUE;
}

SPRESULT CInputQRCode::__PollAction(void)
{
    // Lock resource while multi-thread 
    CLocks Lock(myApp.GetLock());

    // JSON file is used to transfer config parameter of html and code scanned.
    WCHAR szFile[MAX_PATH] = {0};
    swprintf_s(szFile, L"QRCode%d.Json", GetAdjParam().nTASK_ID);
    std::wstring strJsonFile = (std::wstring)_T2CW(myApp.GetSysPath()) + L"\\" + szFile;
    SetJsonConfig(strJsonFile.c_str());

    string_t strHtmlFile = (string_t)myApp.GetAppPath() + _T("\\Html\\InputQRCode\\InputCodes.html");
    
    WINDOW_PROPERTY Wnd;
    ZeroMemory((void *)&Wnd, sizeof(Wnd));
    Wnd.lpszTitle = L"ScanQRCode";
    Wnd.u32WndStyle = POPUP_WINDOW_CENTER;
    Wnd.cx = 600;
    Wnd.cy = 400;
    BOOL bApply = PopupHtmlView(Wnd, strHtmlFile.c_str(), strJsonFile.c_str());
    if (!bApply)
    {
        if (m_bAutoStart)
        {
            return SP_E_USER_ABORT;
        }
        else
        {
            LogRawStrA(SPLOGLV_ERROR, "User not input!");
            return SP_E_SPAT_USER_NOT_INPUT;
        }
    }
    else
    {
        SendWorkStatus();
    }

    QRCODE QRCode;
    if (!GetQRCode(QRCode, strJsonFile.c_str()))
    {
        NOTIFY("GetQRCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "no QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    if (!ParseCode(QRCode))
    {
        NOTIFY("ParseQRCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return SP_E_UNKNOWN_BARCODE;
    }

    CHKRESULT(SetShareMemory(ShareMemory_My_UserInputSN, (const void *)&m_InputSN[0], sizeof(m_InputSN)));
    return SP_OK;
}

void CInputQRCode::SetJsonConfig(LPCTSTR lpszJsonFile)
{
    /* JSON format of configuration
    {
        "Codes": [
            {
                "CodeType":"QRCode",        // QRCode or BarCode
                "Label":"QR",               // Label, if this changed, do not forget change CImpBase::GetQRCode()    
                "Enable":1                  // 1: Show  
                "Format":"V1",              // QRCode format to analyse which is used in CImpBase::ParseIDFromQRCode()
                "Length":0,                 // 0: INFINITE, others means inputted code length must be equal to this setting
                "GenerateType":"ScanCode",  // ScanCode or FixCode, FixCode is usually for debugging
                "FixCode":""                // FixCode string
            }
        ],
        "DUT":"DUT1-Port1"
    }
    */
    
    CJsonUtil Json;
    Document::AllocatorType &allocator = Json.m_document.GetAllocator();
    Json.m_document.SetObject();

    Value vArray(kArrayType);
    vArray.Clear();

    Value vObject(kObjectType);
    Value vString(kStringType);

    // QR fixed type
    vString.SetString("QRCode", allocator);
    vObject.AddMember("CodeType", vString, allocator);

    vString.SetString("QR", allocator);
    vObject.AddMember("Label", vString, allocator);

    // Fixed enable
    vObject.AddMember("Enable", 1, allocator);

    vString.SetString(m_QRCodeConfig.strFormat.c_str(), allocator);
    vObject.AddMember("Format", vString, allocator);

    vObject.AddMember("Length",  m_QRCodeConfig.u32MaxLength, allocator);

    vString.SetString(m_QRCodeConfig.strGenType.c_str(), allocator);
    vObject.AddMember("GenerateType", vString, allocator);

    vString.SetString(m_QRCodeConfig.strFixCode.c_str(), allocator);
    vObject.AddMember("FixCode", vString, allocator);

    vArray.PushBack(vObject, allocator);

    Json.m_document.AddMember("Codes", vArray, allocator);

    // Add DUT No. and Port No. to UI
    CHAR szBuf[64] = "";
    sprintf_s(szBuf, "DUT%d-PORT%d", GetAdjParam().nTASK_ID, GetAdjParam().ca.Com.dwPortNum);
    vString.SetString(szBuf, allocator);
    Json.m_document.AddMember("DUT", vString, allocator);

    Json.SaveJson(lpszJsonFile);
}

BOOL CInputQRCode::GetQRCode(QRCODE& Code, LPCWSTR lpszJsonFile)
{
    if (NULL == lpszJsonFile)
    {
        assert(0);
        return FALSE;
    }

    /* JSON file of QRCode Scanned
    {
        "QR": {  
            "Code":"xxxxxxxxxxx",
            "Format":"xxx"
        }
    }
    */
    CJsonUtil Json;
    Json.m_document.SetObject();
    if (Json.LoadJson(_W2CT(lpszJsonFile)))
    {
        LPCSTR QRLabel = "QR"; // This is the label of QR code scan text box which is shown on HTML.
        if (NULL == Json.m_document.HasMember(QRLabel))
        {
            return FALSE;
        }
        Value& QR = Json.m_document[QRLabel];

        LPCSTR CODE = "Code"; // Fix tag saved by HTML javascript file (Submit.js)
        if (NULL == QR.HasMember(CODE) || !QR[CODE].IsString())
        {
            return FALSE;
        }
        else
        {
            Code.strQRCode = QR[CODE].GetString();
        }

        LPCSTR FORMAT = "Format"; // Fix tag saved by HTML javascript file (Submit.js)
        if (NULL == QR.HasMember(FORMAT) || !QR[FORMAT].IsString())
        {
            return FALSE;
        }
        else
        {
            Code.strFormat = QR[FORMAT].GetString();
        }
       
        return TRUE;
    }
    
    return FALSE;
}

BOOL CInputQRCode::ParseCode(const QRCODE& QRCode)
{
    // Initialize code memory
    for (INT iCode=0; iCode<BC_MAX_NUM; iCode++)
    {
        m_InputSN[iCode].bEnable = 0;
        strncpy_s(m_InputSN[iCode].szCodeName, CBarCodeUtility::m_BarCodeInfo[iCode].nameA, sizeof(m_InputSN[iCode].szCodeName) - 1);
        m_InputSN[iCode].nMaxLength   = (uint8)CBarCodeUtility::m_BarCodeInfo[iCode].SNlength;
        m_InputSN[iCode].eGenCodeType = E_GENCODE_MAN;
    }

    if (0 == QRCode.strFormat.compare("V1"))
    {
        return ParseV1(QRCode.strQRCode);
    }
    else if (0 == QRCode.strFormat.compare("V2"))  
    {
        return ParseV2(QRCode.strQRCode);
    }
    else if (0 == QRCode.strFormat.compare("V3"))  
    {
        return ParseV3(QRCode.strQRCode);
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "UnSupport QR Code: %s", QRCode.strFormat.c_str());
        return FALSE;
    }
}

BOOL CInputQRCode::ParseV1(const std::string& strQRCode)
{
    /*
        V1 format: Key1=Value1;Key2=Value2;...  such as IMEI1=11111111111;IMEI2=222222222222
    */
    INT nMapCodesCount = 0;
    LPSTR* lppMapCodes = GetTokenStringA(strQRCode.c_str(), ";", nMapCodesCount);
    if (NULL == lppMapCodes || nMapCodesCount < 1)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Invalid QR Code: %s", strQRCode.c_str());
        return FALSE;
    }

    CHAR szItemName[64] = {0};
    INT nCodesCountToWrite = 0;
    for (INT i=0; i<nMapCodesCount; i++)
    {
        CUtility Spliter;
        INT nCount = 0;
        LPSTR* lppCode = Spliter.GetTokenStringA(lppMapCodes[i], "=", nCount);
        if (NULL == lppCode || 2 != nCount)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Invalid QR Code: %s", strQRCode.c_str());
            return FALSE;
        }

        for (INT jCode=0; jCode<BC_MAX_NUM; jCode++)
        {                
            std::string strKey = lppCode[0];
            std::string strVal = lppCode[1];
            trimA(strKey);
            trimA(strVal);
            if (strKey == CBarCodeUtility::m_BarCodeInfo[jCode].nameA)
            {
                m_InputSN[jCode].bEnable = 1;
                strncpy_s(m_InputSN[jCode].szCode, strVal.c_str(), sizeof(m_InputSN[jCode].szCode) - 1);

                sprintf_s(szItemName, "Input %s", m_InputSN[jCode].szCodeName);
                NOTIFY(szItemName, LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, m_InputSN[jCode].szCode);
                if (NULL != CBarCodeUtility::m_BarCodeInfo[i].lpDutInfoKey)
                {
                    NOTIFY(CBarCodeUtility::m_BarCodeInfo[jCode].lpDutInfoKey, m_InputSN[jCode].szCode);
                }
                nCodesCountToWrite++;
                break;
            }
        }
    }

    if (nCodesCountToWrite < 1)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Unknown QR Code: %s", strQRCode.c_str());
        return FALSE;
    }

    return TRUE;
}

BOOL CInputQRCode::ParseV2(const std::string& strQRCode)
{
    /*  Bug 1171288: 
        按照中移写号的规则，通过扫描枪扫出来的二维码使用DataMatrix，信息内容为：
        IMEI号（15位）；SN号（20位），两个号码中间用分号隔离
        例如：
        86486702999650;2009117T0102N0000001
    */
    INT nCount = 0;
    LPSTR* lppCodes = GetTokenStringA(strQRCode.c_str(), ";", nCount);
    if (NULL == lppCodes || 2 != nCount)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Invalid QR Code: %s", strQRCode.c_str());
        return FALSE;
    }

    CHAR szItemName[64] = {0};
    for (INT i=0; i<nCount; i++)
    {
        INT iCode = ((0 == i) ? BC_IMEI1 : BC_SN1);

        m_InputSN[iCode].bEnable = 1;
        strncpy_s(m_InputSN[iCode].szCode, lppCodes[i], sizeof(m_InputSN[iCode].szCode) - 1);

        sprintf_s(szItemName, "Input %s", m_InputSN[iCode].szCodeName);
        NOTIFY(szItemName, LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, m_InputSN[iCode].szCode);
        NOTIFY(CBarCodeUtility::m_BarCodeInfo[iCode].lpDutInfoKey, m_InputSN[iCode].szCode);
    }

    return TRUE;
}

BOOL CInputQRCode::ParseV3(const std::string& strQRCode)
{
    /*
		V3: Bug1189339
		sprintf(param, "%s,%s,%d", sn, ipAddr, port)
		以逗号分隔,sn, ipaddr, port
	*/
    INT nCount = 0;
    LPSTR* lppCodes = GetTokenStringA(strQRCode.c_str(), ",-", nCount);
    if (NULL == lppCodes || 3 != nCount)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Invalid QR Code: %s", strQRCode.c_str());
        return FALSE;
    }

    CHAR szSN[ShareMemory_SN_SIZE] = {0};
    memcpy_s(szSN, sizeof(szSN), lppCodes[0], ShareMemory_SN_SIZE-1);
    if (SP_OK != SetShareMemory(ShareMemory_SN1, szSN, sizeof(szSN)))
    {
        return FALSE;
    }

    IP_T ip;
    memcpy_s(ip.szIP, sizeof(ip.szIP), lppCodes[1], sizeof(ip.szIP)-1);
    ip.dwPort = atoi(lppCodes[2]);
    NOTIFY("Input IP", LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, "IP = %s; Port = %d", ip.szIP, ip.dwPort);
    if (SP_OK != SetShareMemory(ShareMemory_IP, &ip, sizeof(ip)))
    {
        return FALSE;
    }

    return TRUE;
}