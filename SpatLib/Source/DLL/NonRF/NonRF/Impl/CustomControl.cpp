#include "StdAfx.h"
#include "CustomControl.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCustomControl)

///
CCustomControl::CCustomControl(void)
: m_bMessageBox(FALSE)
{
    m_strMsgBoxCap.clear();
    m_strMsgBoxText.clear();
}

CCustomControl::~CCustomControl(void)
{
}

BOOL CCustomControl::LoadXMLConfig(void)
{
    m_bMessageBox = GetConfigValue(L"Option:MessageBox:Select", 0);
    if (m_bMessageBox)
    {
        LPCWSTR strTemp = GetConfigValue(L"Option:MessageBox:Text", L"");
        m_strMsgBoxText = _W2CA(strTemp);

        strTemp = GetConfigValue(L"Option:MessageBox:Caption", L"");
        m_strMsgBoxCap = _W2CA(strTemp);
    }

    return TRUE;
}

SPRESULT CCustomControl::__PollAction(void)
{
    if (m_bMessageBox)
    {
        MessageBoxA(NULL, m_strMsgBoxText.c_str(), m_strMsgBoxCap.c_str(), 0);
    }

    return SP_OK;
}

