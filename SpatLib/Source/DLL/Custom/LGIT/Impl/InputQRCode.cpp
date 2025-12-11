#include "StdAfx.h"
#include "InputQRCode.h"
#include "CLocks.h"
#include "JsonUtil.h"

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
    string_t strJsonFile = GetJsonFile();
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

BOOL CInputQRCode::PopupHtmlView( const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile/* = NULL */ )
{
    if ( NULL == lpszHtmlFile || NULL == lpszJsonFile )
    {
        assert( 0 );
        return FALSE;
    }
    return GetCallback()->UiMsgPopupHtmlView( Wnd, lpszHtmlFile, lpszJsonFile );
}

