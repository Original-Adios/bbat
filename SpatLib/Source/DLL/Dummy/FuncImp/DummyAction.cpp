#include "StdAfx.h"
#include "DummyAction.h"
#include "ExtraLogFile.h"
#include "ScanConfig.h"
#include "ActionApp.h"
#include "CLocks.h"
#include <assert.h>

/// 
IMPLEMENT_RUNTIME_CLASS(CDummyAction)
//////////////////////////////////////////////////////////////////////////
CDummyAction::CDummyAction(void)
: m_bAssertDUT(FALSE)
, m_bUID(FALSE)
, m_bSN1(FALSE)
, m_bSN2(FALSE)
, m_bCPSwVer(FALSE)
, m_bAPSwVer(FALSE)
, m_bIMEI1(FALSE)
, m_bIMEI2(FALSE)
, m_bBT(FALSE)
, m_bWIFI(FALSE)
, m_nItemCount(10000)
{
}

CDummyAction::~CDummyAction(void)
{
}

BOOL CDummyAction::LoadXMLConfig(void)
{
    m_bAssertDUT = (1 == GetConfigValue(L"Option:ASSERT", 0)) ? TRUE : FALSE;
    LPCWSTR lpszValue  = GetConfigValue(L"Option:UID", L"OFF");
    m_bUID = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;
   
    lpszValue  = GetConfigValue(L"Option:SN1", L"OFF");
    m_bSN1 = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:SN2", L"OFF");
    m_bSN2 = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:CP", L"OFF");
    m_bCPSwVer = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:AP", L"OFF");
    m_bAPSwVer = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:IMEI1", L"OFF");
    m_bIMEI1 = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:IMEI2", L"OFF");
    m_bIMEI2 = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:BT", L"OFF");
    m_bBT = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

    lpszValue  = GetConfigValue(L"Option:WIFI", L"OFF");
    m_bWIFI = (0 == _wcsicmp(lpszValue, L"ON")) ? TRUE : FALSE;

	const int nDefCount = -1;
	SpatParamsPath OldPaths = g_SpatParamsContainer.FindAllPath( L"CDummyAction", L"Param:ItemCount" );
	for ( auto it : OldPaths )
	{
	    m_nItemCount = GetConfigValue( it.c_str(), nDefCount );
	    if ( nDefCount != m_nItemCount )
	    {
	        break;
	    }
	}
	if ( nDefCount == m_nItemCount )
	{
	    m_nItemCount = GetConfigValue( L"Param:ItemCount", 10000 );
	}

    m_strSN1 = _W2CA(GetConfigValue(L"Param:SN1", L"2111012345678901"));

    return TRUE;
}

SPRESULT CDummyAction::__PollAction(void)
{
    LogFmtStrA(SPLOGLV_VERBOSE, "%s", __FUNCTION__);

    CExtraLog ExtraSample;
    ExtraSample.Open(this, _T("Example\\DummyExtra.Log"));
    ExtraSample.WriteString(_T("This is a test, %d"), 1);
    ExtraSample.WriteStringW(L"This is a test, 0x%X", 2);
    ExtraSample.WriteString(_T("This is a test, %s"), _T("HelloWorld"));
    ExtraSample.WriteStringA("This is a test");
    ExtraSample.Close();

    CScanConfig ScanSample;
    INT       nValue = 1000;
    double    dValue = 99.9;
    string_t strText = _T("Config");
    ScanSample.InitConfigFile(this);
    ScanSample.SaveValue(_T("DUMMY"), _T("INT"),    nValue);
    ScanSample.SaveValue(_T("DUMMY"), _T("DOUBLE"), dValue);
    ScanSample.SaveValue(_T("DUMMY"), _T("STRING"), strText.c_str());
    assert(nValue == ScanSample.LoadValue(_T("DUMMY"), _T("INT"), INVALID_POSITIVE_INTEGER_VALUE));
    assert(dValue == ScanSample.LoadValue(_T("DUMMY"), _T("DOUBLE"), INVALID_POSITIVE_DOUBLE_VALUE));
    TCHAR szValue[100] = {0};
    ScanSample.LoadValue(_T("DUMMY"), _T("STRING"), _T(""), szValue, ARRAY_SIZE(szValue));
    assert(strText == szValue);

    LPCWSTR GLOBAL_SHAREMEMORIES[] = {
        InternalReservedShareMemory_FailStop,
        InternalReservedShareMemory_SaveFinalTestFlag,
        InternalReservedShareMemory_MaxRetryCount,
		InternalReservedShareMemory_SetupDebugMode
    };

    for (INT i=0; i<ARRAY_SIZE(GLOBAL_SHAREMEMORIES); i++)
    {
        INT32 i32Value = -1;
        SPRESULT res = GetShareMemory(GLOBAL_SHAREMEMORIES[i], (void* )&i32Value, 4);
        NOTIFY(_W2CA(GLOBAL_SHAREMEMORIES[i]), LEVEL_UI, NOLOWLMT, i32Value, NOUPPLMT, NULL, -1, NULL, "Error Code = %d", res);
    }

    for (uint32 i = 0; i <= 100; i++)
    {
        Sleep(100);
        SendProgress(i);
    }

    WCHAR szSeqFile[MAX_PATH] = {0};
    WCHAR szSeqVer[32] = {0};
    GetSeqParse()->GetProperty(MAX_PATH, Property_SeqFile, (LPVOID)szSeqFile);
    GetSeqParse()->GetProperty(32, Property_SeqVersion, (LPVOID)szSeqVer);
    std::string  strSeqFile = _W2CA(szSeqFile); 
    strSeqFile = strSeqFile + " R" + _W2CA(szSeqVer); 
    NOTIFY("SeqFile", LEVEL_UI, 1, 1, 1, NULL, -1, NULL, strSeqFile.c_str());

    DWORD dwStart = GetTickCount();
    if (m_bSN1)
    {
        CLocks Lock(*(GetAdjParam().pLocker));

        static LONGLONG i64SN1 = 0;
        CHAR SN1[ShareMemory_SN_SIZE] = {0};
        if (0 == i64SN1)
        {
            if (m_strSN1.length() > 0)
            {
                i64SN1 = _atoi64(m_strSN1.c_str());
            }
            else
            {
                i64SN1 = 10000000000 + GetAdjParam().nTASK_ID * 1000000000;
            }
        }
        i64SN1++;       
        sprintf_s(SN1, "%I64u", i64SN1);
        NOTIFY(DUT_INFO_SN1, (const char*)SN1);
        SetShareMemory(ShareMemory_SN1, (const void* )&SN1, sizeof(SN1));
    }

    if (m_bAssertDUT)
    {
        SP_AssertUE(m_hDUT);
    }

    if (m_bSN2)
    {
        static LONGLONG i64SN2 = 20000000000 + GetAdjParam().nTASK_ID * 1000000000;
        i64SN2++;
        CHAR SN2[64] = {0};
        sprintf_s(SN2, "%I64u", i64SN2);
        NOTIFY(DUT_INFO_SN2, (const char*)SN2);
    }

    NOTIFY(DUT_INFO_IMEI1, (const char*)"111111111111119");
    NOTIFY(DUT_INFO_IMEI2, (const char*)"222222222222228");

    if (m_bUID)
    {
        CHAR* UID = "UIDSC9860-77777";
        NOTIFY(DUT_INFO_UID, (const char*)UID);
    }

    if (m_bCPSwVer)
    {
        CHAR* CP = "Platform Version: MOCORTM_W16.27_P1_Debug.Project Version:   whale2_pub.BASE  Version:    FM_BASE_W16.27_P1.HW Version:        sc9860_modem.07-01-2016 10:55:24";
        NOTIFY(DUT_INFO_CP, (const char*)CP);
    }

    if (m_bAPSwVer)
    {
        CHAR* AP = "Android 7.0";
        NOTIFY(DUT_INFO_AP, (const char*)AP);
    }

    if (m_bBT)
    {
        CHAR* BT = "6D:8C:C2:67:86:DC";
        NOTIFY(DUT_INFO_BT, (const char*)BT);
    }

    if (m_bWIFI)
    {
        CHAR* WIFI = "6D:8C:C2:67:86:DD";
        NOTIFY(DUT_INFO_WIFI, (const char*)WIFI);
    }

    
    for (UINT i=0; i<m_nItemCount; i++)
    {
        if (_IsUserStop())
        {
            return SP_E_USER_ABORT;
        }
        char szItem[20] = {0};
        sprintf_s(szItem, "SubItem %d", i % 100);
        NOTIFY(szItem, LEVEL_ITEM, 0, i%100, 100, "DummyBand", i, NULL, "Index = %d", i);

        if (0 == i%5)
        {
            NOTIFY("trace", LEVEL_UI, NOLOWLMT, 0.0, NOUPPLMT, "DummyBand", 10, NULL, "Index = %d", i);
        }
    }
    DWORD dwEnd = GetTickCount() - dwStart;
    LogFmtStrA(SPLOGLV_VERBOSE, "SendUIMsg elapsed = %d", dwEnd);

    if (m_bAssertDUT)
    {
        CHKRESULT(SP_gsmActive(m_hDUT, TRUE));
    }

    return SP_OK;
}
