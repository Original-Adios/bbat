#include "StdAfx.h"
#include "ScanConfig.h"
#include "SpatBase.h"
#include "ActionApp.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
CScanConfig::CScanConfig(void)
{
}

CScanConfig::~CScanConfig(void) 
{
}

void CScanConfig::InitConfigFile(CSpatBase* pParent, BOOL bNoTaskID)
{
    if (NULL == pParent)
    {
        assert(0);
        return ;
    }

    extern CActionApp myApp;
    const SPAT_INIT_PARAM& param = pParent->GetAdjParam();
    if (NULL == param.pSeqParse)
    {
        assert(0);
        return ;
    }

    WCHAR szSeqFile[MAX_PATH] = {0};
    ((ISeqParse* )param.pSeqParse)->GetProperty((LONG)MAX_PATH, Property_SeqFile, (LPVOID)szSeqFile);

    TCHAR szPath[MAX_PATH] = {0};
#if defined (UNICODE) ||  defined (_UNICODE)
	if(bNoTaskID)
	{
		swprintf_s(szPath, MAX_PATH, L"%s\\%s_ALL.scan", myApp.GetSysPath(), szSeqFile);
	}
	else
	{
		swprintf_s(szPath, MAX_PATH, L"%s\\%s_%d.scan", myApp.GetSysPath(), szSeqFile, param.nTASK_ID);
	}
    
#else
    CHAR szConvPath[MAX_PATH] = {0};
    WideCharToMultiByte(CP_ACP, 0, szSeqFile, -1, szConvPath, MAX_PATH, 0, 0);
	if(bNoTaskID)
	{
		sprintf_s(szPath, MAX_PATH,   "%s\\%s_ALL.scan", myApp.GetSysPath(), szConvPath);
	}
	else
	{
		sprintf_s(szPath, MAX_PATH,   "%s\\%s_%d.scan", myApp.GetSysPath(), szConvPath, param.nTASK_ID);
	}
#endif 
    m_strFilePath = szPath;
}

void CScanConfig::DeleteConfigFile()
{
    DeleteFile( m_strFilePath.c_str() );
}

BOOL CScanConfig::isDeleteConfigFile()
{
    extern CActionApp myApp;
    std::wstring strIniPath = myApp.GetSysPath();
    strIniPath += L"\\..\\Setting\\ScanConfig.ini";
    UINT nLv = GetPrivateProfileIntW( L"Setting", L"DeleteScanFile", 0, strIniPath.c_str() );
    return 1 == nLv ? TRUE : FALSE;
}

BOOL CScanConfig::LoadValue(
    LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD dwNbrOfCharacters
    )
{
    return ::GetPrivateProfileString(
        lpAppName, lpKeyName, lpDefault, lpReturnedString, dwNbrOfCharacters, GetFilePath()
        );
}

BOOL CScanConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString)
{
    return ::WritePrivateProfileString(
        lpAppName, lpKeyName, lpString, GetFilePath()
        );
}

INT CScanConfig::LoadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault)
{
    return ::GetPrivateProfileInt(
        lpAppName, lpKeyName, nDefault, GetFilePath()
        );
}

BOOL CScanConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nValue)
{
    TCHAR szValue[20] = {0};
    _stprintf_s(szValue, _T("%d"), nValue);
    return SaveValue(lpAppName, lpKeyName, szValue);
}

double CScanConfig::LoadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, double dDefault)
{
    TCHAR szDefault[20] = {0};
    _stprintf_s(szDefault, _T("%f"), dDefault);
    TCHAR szValue[20] = {0};
    LoadValue(lpAppName, lpKeyName, szDefault, szValue, ARRAY_SIZE(szValue));
    return _tcstod(szValue, NULL);
}

BOOL CScanConfig::SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, double dValue)
{
    TCHAR szValue[20] = {0};
    _stprintf_s(szValue, _T("%f"), dValue);
    return SaveValue(lpAppName, lpKeyName, szValue);
}