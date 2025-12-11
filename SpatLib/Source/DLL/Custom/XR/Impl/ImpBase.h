#pragma once
#include "UnisocMesBase.h"
#include "../../../../../../Common/Json/JsonUtil.h"

//APPID
#define ShareMemory_XunRuiInputAppid          L"XunRuiInputAppid"
#define MISCDATA_FOR_APPID_LENGTH 33
#define MISCDATA_APPID_OFFSET (800*1024)
//SSN
#define MISCDATA_FOR_SSN_LENGTH 47
#define MISCDATA_SSN_OFFSET (768*1024)
//Kuda
#define ShareMemory_XunRuiInputKuda          L"XunRuiInputKuda"

//ManualCode2Compare
#define ShareMemory_XunRuiInputManualCode2Compare  L"XunRuiInputManualCode2Compare"

#define MISCDATA_KUDA_OFFSET (768*1024)
#define MISCDATA_FOR_KUDA_LENGTH 128

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CUnisocMesBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

	BOOL GetMEIDCheckCode(char*pszMeid,char&cCheckCode);

protected:
    virtual SPRESULT __InitAction(void);

    BOOL LoadJsonFile(LPCTSTR lpszJsonFile = NULL);
    BOOL SaveJsonFile(LPCTSTR lpszJsonFile = NULL);

    BOOL PopupHtmlView(const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile = NULL);

    std::wstring GetAbsFilePath(const std::wstring& path);

protected:
    CJsonUtil m_Json;
    TCHAR     m_szJsonPath[MAX_PATH];
    TCHAR     m_szHtmlPath[MAX_PATH];
};

#define IS_IMEI_CODE(i)     (BC_IMEI1 == (i) || BC_IMEI2 == (i) || BC_IMEI3 == (i) || BC_IMEI4 == (i))
#define IS_MEID_CODE(i)     (BC_MEID1 == (i) || BC_MEID2 == (i))
#define IS_GEID_CODE(i)     (IS_IMEI_CODE(i) || IS_MEID_CODE(i))
#define IS_HEX_CODE(i)      (BC_BT == (i) || BC_WIFI == (i) || BC_MEID1 == (i) || BC_MEID2 == (i))

