#include "StdAfx.h"
#include "ImpBase.h"
#include "JsonUtil.h"

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{

}

CImpBase::~CImpBase(void)
{
}

string_t CImpBase::GetJsonFile(void)
{
    TCHAR szFileName[MAX_PATH] = {0};
    _stprintf_s(szFileName, _T("LGIT_BarCode_%d.Json"), GetAdjParam().nTASK_ID);
    return (string_t)myApp.GetSysPath() + _T("\\") + szFileName;
}

BOOL CImpBase::GetQRCode(QRCODE& Code, LPCTSTR lpszJsonFile /* = NULL */)
{
    /* JSON file of QRCode Scanned
    {
        "QR": {  
            "Code":"8928.800.316,355391090035308,01I_T4CHU",
            "Format":"V1"
        }
    }
    */
    CJsonUtil Json;
    Json.m_document.SetObject();
    if (Json.LoadJson((NULL != lpszJsonFile) ? lpszJsonFile : GetJsonFile().c_str()))
    {
        LPCSTR QRLabel = "QR"; // This is the label of QR code scan text box which is shown on HTML.
        if (NULL == Json.m_document.HasMember(QRLabel))
        {
            return FALSE;
        }
        Value& QR = Json.m_document[QRLabel];

        LPCSTR CODE = "Code"; // Fix tag saved by HTML javascript file (Submit.js)
        if (NULL == QR.HasMember(CODE) || !QR[CODE].IsString())
        {
            return FALSE;
        }
        else
        {
            Code.strQRCode = QR[CODE].GetString();
        }

        LPCSTR FORMAT = "Format"; // Fix tag saved by HTML javascript file (Submit.js)
        if (NULL == QR.HasMember(FORMAT) || !QR[FORMAT].IsString())
        {
            return FALSE;
        }
        else
        {
            Code.strFormat = QR[FORMAT].GetString();
        }
       
        return TRUE;
    }
    
    return FALSE;
}

BOOL CImpBase::ParseIDFromQRCode(const QRCODE& Code,std::string& strID)
{
    if (Code.strFormat == "V1")
    {
        /*
        Example Scan: 8928.800.316,355391090035308,01I_T4CHU
        ¡ú 8928.800.316 : Customer P/N
        ¡ú 355391090035308 : IMEI
        ¡ú 01I_T4CHU : FW Version
        */
        INT nCount = 0;
        LPSTR* lppToken = GetTokenStringA(Code.strQRCode.c_str(), ",", nCount);
        if (3 != nCount)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Invalid QRCode format: %s", Code.strQRCode.c_str());
            return FALSE;
        }
        else
        {
            strID = lppToken[1];

            // TODO: You can add extra verify code here or add in javascript file (VerifyCode.js)
            //       Such as the valid format of IMEI

            return TRUE;
        }
    }
    else
    {
        LogFmtStrA(SPLOGLV_ERROR, "Unknown QR format! %s", Code.strFormat.c_str());
        return FALSE;
    }
}

SPRESULT CImpBase::SendAT(
    LPCSTR lpszAT, 
    std::string& strRsp, 
    uint32 u32RetryCount /* = 1 */, 
    uint32 u32Interval /* = 200 */,
    uint32 u32TimeOut /* = TIMEOUT_3S */
    )
{
    SPRESULT res = SP_OK;
    CHAR   recvBuf[4096] = {0};
    uint32 recvSize = 0;
    for (uint32 i=0; i<u32RetryCount; i++)
    {
        res = SP_SendATCommand(m_hDUT, lpszAT, TRUE, recvBuf, sizeof(recvBuf), &recvSize, u32TimeOut);
        if (SP_OK == res)
        {
            if (NULL != strstr(recvBuf, "+CME ERROR"))
            {
                res = SP_E_PHONE_AT_EXECUTE_FAIL;
            }
            else
            {
                strRsp = recvBuf;
                replace_all(strRsp, "\r", "");
                replace_all(strRsp, "\n", "");
                replace_all(strRsp, "OK", "");
                trimA(strRsp);

                return SP_OK;
            }
        }

        Sleep(u32Interval);
    }

    return res;
}
