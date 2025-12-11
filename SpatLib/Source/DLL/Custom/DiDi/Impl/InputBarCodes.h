#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
typedef enum
{
    ManualInput = 0,
    AutoGenerate
} CodeGenType_E;
typedef struct _tagBarCode_T
{
    BOOL bActived;          // Active input or not
    UINT nMaxLen;
    CHAR szLabel[32];    // Barcode label
    CodeGenType_E eGenType;
    CHAR szPrefix[32];
    CHAR szCode[32];

    STRUCT_INITIALIZE(_tagBarCode_T);
} BarCode_T;
#define MAX_BARCODE_NUM     (5) // DeviceName/SN1/IMEI1/WIFI/BT

// ShareMemory for BarCode input
#define ShareMemory_DiDiInputCodes          L"DiDiInputCodes"
#define ShareMemory_DiDiDeviceName          L"DiDiDeviceName"
//////////////////////////////////////////////////////////////////////////
class CInputBarCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CInputBarCodes)
public:
    CInputBarCodes(void);
    virtual ~CInputBarCodes(void);

protected:
    string_t GetJsonFile(void);
    BOOL PopupHtmlView(const WINDOW_PROPERTY& Wnd, LPCTSTR lpszHtmlFile, LPCTSTR lpszJsonFile = NULL);

    // Save BarCode setting into JSON file for HTML view
    BOOL SaveJsonFile(LPCTSTR lpszJsonFile);
    BOOL LoadJsonFile(LPCTSTR lpszJsonFile);

protected:
    BOOL    LoadXMLConfig(void);
    SPRESULT __PollAction(void);

private:
    BarCode_T m_lstCodes[MAX_BARCODE_NUM];
};
