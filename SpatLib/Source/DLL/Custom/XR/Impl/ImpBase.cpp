#include "StdAfx.h"
#include "ImpBase.h"
#include "ActionApp.h"
#include "CLocks.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//
extern CActionApp myApp;
//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}

BOOL CImpBase::PopupHtmlView(const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile/* = NULL */)
{
    if (NULL == lpszHtmlFile)
    {
        assert(0);
        return FALSE;
    }
	return GetCallback()->UiMsgPopupHtmlView(Wnd, lpszHtmlFile, (NULL != lpszJsonFile) ? lpszJsonFile : _T2CW(m_szJsonPath));
}

BOOL CImpBase::LoadJsonFile(LPCTSTR lpszJsonFile /* = NULL */)
{
    return m_Json.LoadJson((NULL != lpszJsonFile) ? lpszJsonFile : m_szJsonPath);
}

BOOL CImpBase::SaveJsonFile(LPCTSTR lpszJsonFile /* = NULL */)
{
    return m_Json.SaveJson((NULL != lpszJsonFile) ? lpszJsonFile : m_szJsonPath);
}

SPRESULT CImpBase::__InitAction(void)
{
    /*
        Jason file: Bin\sys\InputBarcode_1(2/3/4).Json
    */
    _stprintf_s(m_szJsonPath, _T("%s\\InputCodes_%d.Json"), myApp.GetSysPath(), GetAdjParam().nTASK_ID);

    /*
        X:\Bin\App\SpatLib\Html
    */
    _stprintf_s(m_szHtmlPath, _T("%s\\Html"), myApp.GetAppPath());

    return SP_OK;
}

std::wstring CImpBase::GetAbsFilePath(const std::wstring& path)
{
    std::wstring strAbsPath = L"";
    if (path.length() > 0)
    {
        if (PathIsRelativeW(path.c_str()))
        {
            extern CActionApp myApp;
            strAbsPath = (std::wstring)(_T2CW(myApp.GetAppPath())) + L"\\" + path;
        }
        else
        {
            strAbsPath = path;
        }

        replace_all_w(strAbsPath, L"/", L"\\");

        CONST INT SIZE = strAbsPath.length() + 1;
        LPWSTR pszFile = new WCHAR[SIZE];
        ZeroMemory(pszFile, sizeof(WCHAR)*SIZE);
        wcscpy_s(pszFile,    SIZE, strAbsPath.c_str());
        PathCanonicalizeW(pszFile, strAbsPath.c_str());  // Replace .  or ... in path
        strAbsPath = pszFile;
        delete []pszFile;
    }

    return strAbsPath;
}
BOOL CImpBase::GetMEIDCheckCode(char*pszMeid,char&cCheckCode)
{
	if (14 != strlen(pszMeid))
	{
		NOTIFY("GetMEIDCheckCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MEID Length is %d", strlen(pszMeid));
		return FALSE;
	}

	string strMeid(pszMeid);
	transform(strMeid.begin(), strMeid.end(), strMeid.begin(), ::toupper); 

	int iSum = 0;
	for (int iUnit = 0; iUnit < 14; iUnit++)
	{
		int iValue= -1;
		string strParam = strMeid.substr(iUnit, 1);
		if (strParam[0] >= 'A' && strParam[0] <= 'F')
		{
			iValue = strParam[0] - 'A' + 10 ;
		}
		else if(strParam[0] >= '0' && strParam[0] <= '9')
		{
			iValue = strParam[0] - '0';
		}
		else
		{
			NOTIFY("GetMEIDCheckCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MEID(%s) must be hexadecimal.",pszMeid);
			return FALSE;
		}
		if (iUnit % 2 == 0)
		{
			//将奇数位数字相加，再加上上一步算得的值
			iSum += iValue;
		}
		else
		{
			//将偶数位数字分别乘以2，分别计算个位数和十位数之和，注意是16进制数
			iSum +=  2 * iValue % 16;
			iSum +=  2 * iValue / 16;
		}
	}
	//如果得出的数个位是0则校验位为0，否则为10减去个位数
	if (iSum % 16 == 0)
	{
		cCheckCode = '0';
	}
	else
	{
		int iCheckCode = 16 - iSum % 16;
		if (iCheckCode > 9)
		{
			iCheckCode += 'A' - 10;
		}
		else
		{
			iCheckCode += '0';
		}
		cCheckCode = (char)iCheckCode;
	} 
	return TRUE;
}
