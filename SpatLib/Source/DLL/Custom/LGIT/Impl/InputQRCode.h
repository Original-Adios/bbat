#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CInputQRCode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CInputQRCode)
public:
    CInputQRCode(void);
    virtual ~CInputQRCode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    void SetJsonConfig(LPCWSTR lpszJsonFile);
    BOOL PopupHtmlView(const WINDOW_PROPERTY& Wnd, LPCTSTR lpszHtmlFile, LPCTSTR lpszJsonFile = NULL);

    struct BARCODE_ITEM 
    {
        std::string  strFormat;  // V1: 8928.800.316,355391090035308,01I_T4CHU  
        UINT32    u32MaxLength;  // 0: INFINITE
        std::string strGenType;  // ScanCode or FixCode
        std::string strFixCode;

        BARCODE_ITEM(void)
        {
            strFormat    = "V1";
            u32MaxLength = 0; 
            strGenType   = "ScanCode";
            strFixCode   = "";
        };
    } m_QRCodeConfig;
};
