#pragma once
#include "InputCodes.h"

//////////////////////////////////////////////////////////////////////////
class CCustomizeQRCode : public CInputCodes
{
    DECLARE_RUNTIME_CLASS(CCustomizeQRCode)
public:
    CCustomizeQRCode(void);
    virtual ~CCustomizeQRCode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    void SetJsonConfig(LPCWSTR lpszJsonFile);

    // QR Code
    typedef struct
    {
        std::string strQRCode;
        std::string strFormat;
    } QRCODE;

    BOOL ParseCode(const QRCODE& QRCode, std::string &strUsedCode);
    BOOL GetQRCode(QRCODE& Code, LPCWSTR lpszJsonFile);
    void GetXMLConfig(INPUT_CODES_T* pInput, std::wstring strCodeName);

    struct BARCODE_ITEM 
    {
        std::string  strFormat;  // V1: IMEI1=xxxxxx;SN1=yyyyyy  
        UINT32    u32MaxLength;  // 0: INFINITE
        std::string strGenType;  // ScanCode or FixCode
        std::string strFixCode;

        BARCODE_ITEM(void)
        {
            strFormat    = "V4";
            u32MaxLength = 0; 
            strGenType   = "ScanCode";
            strFixCode   = "";
        };
    } m_QRCodeConfig;
};
