#include "StdAfx.h"
#include "PLink_InputBarCodes.h"
#include "JsonUtil.h"
#include "CLocks.h"
#include "ActionApp.h"
extern CActionApp myApp;

//
IMPLEMENT_RUNTIME_CLASS(CInputBarCodes)

//////////////////////////////////////////////////////////////////////////
CInputBarCodes::CInputBarCodes(void)
{
	m_bReInputSmo = FALSE;
}

CInputBarCodes::~CInputBarCodes(void)
{
}

string_t CInputBarCodes::GetJsonFile(void)
{
    TCHAR szFileName[MAX_PATH] = {0};
    _stprintf_s(szFileName, _T("PLink_BarCode_%d.Json"), GetAdjParam().nTASK_ID);
    return (string_t)myApp.GetSysPath() + _T("\\") + szFileName;
}

BOOL CInputBarCodes::PopupHtmlView( const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile/* = NULL */ )
{
    if ( NULL == lpszHtmlFile || NULL == lpszJsonFile )
    {
        assert( 0 );
        return FALSE;
    }
    return GetCallback()->UiMsgPopupHtmlView( Wnd, lpszHtmlFile, lpszJsonFile );
}

BOOL CInputBarCodes::LoadXMLConfig(void)
{
    std::wstring strKey = L"";
    std::wstring strVal = L"";
    LPCWSTR SUPPORT_LABELS[MAX_BARCODE_NUM] = {L"SN1", L"CustFixed"};
    for (INT i=0; i<MAX_BARCODE_NUM; i++)
    {
        strcpy_s(m_lstCodes[i].szLabel, _W2CA(SUPPORT_LABELS[i]));

        strKey = (std::wstring)L"Option:" + SUPPORT_LABELS[i] + L":Active";
        m_lstCodes[i].bActived = (BOOL)GetConfigValue(strKey.c_str(), 0);

        strKey = (std::wstring)L"Option:" + SUPPORT_LABELS[i] + L":GenerateType";
        strVal = GetConfigValue(strKey.c_str(), L"ManualInput");
        m_lstCodes[i].eGenType = (strVal == L"AutoGenerate") ? AutoGenerate : ManualInput;
       
        strKey = (std::wstring)L"Option:" + SUPPORT_LABELS[i] + L":MaxLength";
        m_lstCodes[i].nMaxLen = (INT)GetConfigValue(strKey.c_str(), 0);
        if (0 == m_lstCodes[i].nMaxLen)
        {
            assert(0);
            return FALSE;
        }

        strKey = (std::wstring)L"Option:" + SUPPORT_LABELS[i] + L":Prefix";
        strVal = GetConfigValue(strKey.c_str(), L"");
        if (strVal.length() > 0)
        {
            strncpy_s(m_lstCodes[i].szPrefix, _W2CA(strVal.c_str()), CopySize(m_lstCodes[i].szPrefix));
        }
    }

    return TRUE;
}

BOOL CInputBarCodes::SaveJsonFile(LPCTSTR lpszJsonFile)
{
    if (NULL == lpszJsonFile)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: Invalid JSON file!", __FUNCTION__);
        assert(0);
        return FALSE;
    }

    CJsonUtil Json;
    Document::AllocatorType &allocator = Json.m_document.GetAllocator();
    Json.m_document.SetObject();
    Value vArray(kArrayType);
    for (INT i=0; i<MAX_BARCODE_NUM; i++)
    {  
        Value vObject(kObjectType);
        Value vString(kStringType);

        vString.SetString(m_lstCodes[i].szLabel, allocator);
        vObject.AddMember("Name", vString, allocator);

        vObject.AddMember("Enable", m_lstCodes[i].bActived, allocator);

        vObject.AddMember("MaxLength", m_lstCodes[i].nMaxLen, allocator);

		if (m_bReInputSmo && 0 == _stricmp(m_lstCodes[i].szLabel, "CustFixed"))
		{
			m_lstCodes[i].eGenType = AutoGenerate;
		}
	    vString.SetString((AutoGenerate == m_lstCodes[i].eGenType) ? "AutoGenerate" : "ManualInput", allocator);

        vObject.AddMember("GenerateType", vString, allocator);

        vString.SetString(m_lstCodes[i].szPrefix, allocator);
        vObject.AddMember("Prefix", vString, allocator);

        vString.SetString("", allocator);
        vObject.AddMember("Validation", vString, allocator);

		if (m_bReInputSmo && 0 == _stricmp(m_lstCodes[i].szLabel, "CustFixed"))
		{
			vString.SetString(m_lstCodes[i].szCode, allocator);
		}
		else
		{
			vString.SetString("", allocator);
		}
		vObject.AddMember("InitBarCode", vString, allocator);

        vObject.AddMember("EnableInputCheck", 1, allocator);

        vArray.PushBack(vObject, allocator);
    }

    Json.m_document.AddMember("Barcodes", vArray, allocator);

    Value vDut(kStringType);
    char szBuf[64] = "";
    SPAT_INIT_PARAM spat_init_param = GetAdjParam();
    sprintf_s(szBuf, "DUT%d-Port%d", spat_init_param.nTASK_ID, spat_init_param.ca.Com.dwPortNum);
    vDut.SetString(szBuf, allocator);
    Json.m_document.AddMember("DutNo", vDut, allocator);

    return Json.SaveJson(lpszJsonFile);
}

BOOL CInputBarCodes::LoadJsonFile(LPCTSTR lpszJsonFile)
{
    if (NULL == lpszJsonFile)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: Invalid JSON file!", __FUNCTION__);
        assert(0);
        return FALSE;
    }

    CJsonUtil Json;
    Json.m_document.SetObject();
    if (!Json.LoadJson(lpszJsonFile))
    {
        return FALSE;
    }
    for (INT i=0; i<MAX_BARCODE_NUM; i++)
    {
        if (!m_lstCodes[i].bActived)
        {
            continue;
        }

        strncpy_s(m_lstCodes[i].szCode, Json.GetString(m_lstCodes[i].szLabel), CopySize(m_lstCodes[i].szCode));
		if (!m_bReInputSmo && 0 == _stricmp(m_lstCodes[i].szLabel, "CustFixed"))
		{
			m_bReInputSmo = TRUE;
		}
    }

    return TRUE;
}

SPRESULT CInputBarCodes::__PollAction(void)
{
    INT nActivedCodesCount = 0;
    for (INT i=0; i<MAX_BARCODE_NUM; i++)
    {  
        if (m_lstCodes[i].bActived)
        {
            nActivedCodesCount++;
        }
    }
    if (0 == nActivedCodesCount)
    {
        // No BarCode is active
        NOTIFY("0 BarCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return SP_E_INVALID_PARAMETER;
    }

    // Sync. for multi-thread, once time only one input code view is popped up.
    CLocks Lock(myApp.GetLock());

    string_t strJsonFile = GetJsonFile();
    if (!SaveJsonFile(strJsonFile.c_str()))
    {
        NOTIFY("Save Json", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return SP_E_SPAT_TEST_FAIL;
    }

    // Popup HTML view for scan or input BarCode.
    string_t strHtmlView =  (string_t)myApp.GetAppPath() + _T("\\Html\\InputCodes\\InputCodes.html");
    WINDOW_PROPERTY Wnd;
    ZeroMemory((void *)&Wnd, sizeof(Wnd));
    Wnd.lpszTitle   = L"请输入条码";
    Wnd.u32WndStyle = POPUP_WINDOW_CENTER;
    Wnd.cx = 530;
    Wnd.cy = 340 + 40 * nActivedCodesCount;
    BOOL bApply = PopupHtmlView(Wnd, strHtmlView.c_str(), strJsonFile.c_str());
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
        //  AutoStart模式下启动界面刷新和显示
        SendWorkStatus();
    }

    if (!LoadJsonFile(strJsonFile.c_str()))
    {
        NOTIFY("Load Json", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return SP_E_OPEN_FILE;
    }

    CHKRESULT(SetShareMemory(ShareMemory_PLinkInputCodes, &m_lstCodes, sizeof(BarCode_T)*MAX_BARCODE_NUM));
    return SP_OK;
}