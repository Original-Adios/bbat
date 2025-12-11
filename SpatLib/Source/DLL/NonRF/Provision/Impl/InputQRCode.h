#pragma once
#include "InputCodes.h"

//////////////////////////////////////////////////////////////////////////
class CInputQRCode : public CInputCodes
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

    // QR Code
    typedef struct
    {
        std::string strQRCode;
        std::string strFormat;
    } QRCODE;

    BOOL ParseCode(const QRCODE& QRCode);
    BOOL ParseV1(const std::string& strQRCode);
    BOOL ParseV2(const std::string& strQRCode);
    BOOL ParseV3(const std::string& strQRCode);
    BOOL GetQRCode(QRCODE& Code, LPCWSTR lpszJsonFile);

    struct BARCODE_ITEM 
    {
        std::string  strFormat;  // V1: IMEI1=xxxxxx;SN1=yyyyyy  
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
