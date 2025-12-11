#include "StdAfx.h"
#include "WriteFID.h"

//
IMPLEMENT_RUNTIME_CLASS(CWriteFID)
//////////////////////////////////////////////////////////////////////////
CWriteFID::CWriteFID(void)
{
}

CWriteFID::~CWriteFID(void)
{
}

SPRESULT CWriteFID::__PollAction(void)
{
    CONST CHAR* ITEM_NAME = "WriteFID";
    QRCODE Code;
    if (!GetQRCode(Code))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "no QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    std::string strID = "";
    if (!ParseIDFromQRCode(Code, strID))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unknown QRCode");
        return SP_E_UNKNOWN_BARCODE;
    }

    /*
    FID = hhmm + IMEI
    ¡ú hhmm : Start Time that is received by MES
    We use FID to combine the IMEI with start time in MES
    */
    CHAR hhmm[8] = {0};
    SPRESULT res = GetShareMemory(LGIT_ShareMemory_MES_StartTime, (LPVOID)hhmm, sizeof(hhmm));
    if (SP_OK != res)
    {
        // For debugging without MES environment
        LogFmtStrA(SPLOGLV_WARN, "GetShareMemory(LGIT_ShareMemory_MES_StartTime) fail");
        SYSTEMTIME    tm;
        GetLocalTime(&tm);
        sprintf_s(hhmm, "%02d%02d", tm.wHour, tm.wMinute);
    }

    // AT*FID=MS180813000001
    // OK
    /*std::string strAT  = (std::string)"AT*FID=" + hhmm + strID;
    std::string strRsp = "";
    res = SendAT(strAT.c_str(), strRsp);*/
	std::string strSN = hhmm + strID;
	res = SP_SaveSN(m_hDUT, SN1, strSN.c_str(), strSN.length());
    NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "%s%s", hhmm, strID.c_str());
	SetShareMemory(ShareMemory_SN1, (void* )strSN.c_str(), strSN.length());
	NOTIFY(DUT_INFO_IMEI1, (const char*)strSN.c_str());
    return res;
}

