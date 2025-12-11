#pragma once
#include "../../../../../../Common/Json/JsonUtil.h"
#include "SpatBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "MesDriver.h"

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

protected:
    virtual SPRESULT __InitAction(void);

    BOOL LoadJsonFile(LPCTSTR lpszJsonFile = NULL);
    BOOL SaveJsonFile(LPCTSTR lpszJsonFile = NULL);

    BOOL PopupHtmlView(const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile = NULL);

    std::wstring GetAbsFilePath(const std::wstring& path);
    int64 ComputeImeiCD(int64 nImei);
    BOOL ComputeMeidCD(char* pszMeid, char& cCheckCode);
    BOOL CheckHexString(char* pszHex, int nLength);
    BOOL CheckStringIsDigit(const char* pszdigit, int nLength);

protected:
    CJsonUtil m_Json;
    TCHAR     m_szJsonPath[MAX_PATH];
    TCHAR     m_szHtmlPath[MAX_PATH];
};

#define IS_IMEI_CODE(i)     (BC_IMEI1 == (i) || BC_IMEI2 == (i) || BC_IMEI3 == (i) || BC_IMEI4 == (i))
#define IS_MEID_CODE(i)     (BC_MEID1 == (i) || BC_MEID2 == (i))
#define IS_GEID_CODE(i)     (IS_IMEI_CODE(i) || IS_MEID_CODE(i))
#define IS_HEX_CODE(i)      (BC_BT == (i) || BC_WIFI == (i) || BC_MEID1 == (i) || BC_MEID2 == (i) || BC_ENETMAC == (i) || BC_OTTSN == (i))

