#pragma once
#include "UnisocMesBase.h"
#include "../../../../../../Common/Json/JsonUtil.h"
#include "SharedDefine.h"

/*
项目			存放分区		Offset（Bytes）	最大长度（Bytes）	PhoneCommand API 
Android			miscdata		768K			256K				SP_LoadMiscData(m_hDUT, 768*1024,  (LPVOID)&wbuff, sizeof(wbuff), NULL, TIMEOUT_3S);SP_SaveMiscData(m_hDUT, 768*1024,  (LPVOID)&buff, sizeof(buff), NULL, TIMEOUT_3S)
RTOS（T117）	PHASE_CHECK		64K				64K					SP_LoadMiscData(m_hDUT, 64*1024,  (LPVOID)&wbuff, sizeof(wbuff), NULL, TIMEOUT_3S);SP_SaveMiscData(m_hDUT, 64*1024,  (LPVOID)&buff, sizeof(buff), NULL, TIMEOUT_3S)
*/
#define MISCDATA_BASE_ANDROID_OFFSET (768*1024)
#define MISCDATA_BASE_RTOS_OFFSET (64*1024)

CONST INT MAX_CUSTOMIZED_ANDROID_MISCDATA_SIZE = 256*1024;// 最大不能超过 256KB,256*1024
CONST INT MAX_CUSTOMIZED_RTOS_MISCDATA_SIZE = 64*1024;// 最大不能超过 64KB,64*1024

typedef enum
{
	ESYS_Android,
	ESYS_RTOS
}ESYSTEM;
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CUnisocMesBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

    SPRESULT SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut = TIMEOUT_3S);

protected:
    virtual SPRESULT __InitAction(void);
	BOOL LoadJsonFile(LPCTSTR lpszJsonFile = NULL);
	BOOL SaveJsonFile(LPCTSTR lpszJsonFile = NULL);
	BOOL PopupHtmlView(const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile = NULL);

	SPRESULT WriteCustMisdata(uint32 u32InBase, const void * pData, const int length, ESYSTEM eSys);
protected:
    std::string m_strRevBuf;
 	CJsonUtil m_Json;
 	TCHAR     m_szJsonPath[MAX_PATH];
 	TCHAR     m_szHtmlPath[MAX_PATH];
};

#define IS_IMEI_CODE(i)     (BC_IMEI1 == (i) || BC_IMEI2 == (i) || BC_IMEI3 == (i) || BC_IMEI4 == (i))
#define IS_MEID_CODE(i)     (BC_MEID1 == (i) || BC_MEID2 == (i))
#define IS_GEID_CODE(i)     (IS_IMEI_CODE(i) || IS_MEID_CODE(i))
#define IS_HEX_CODE(i)      (BC_BT == (i) || BC_WIFI == (i) || BC_MEID1 == (i) || BC_MEID2 == (i))