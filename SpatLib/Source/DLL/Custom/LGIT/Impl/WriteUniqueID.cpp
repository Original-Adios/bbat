#include "StdAfx.h"
#include "WriteUniqueID.h"

//
IMPLEMENT_RUNTIME_CLASS(CWriteUniqueID)
//////////////////////////////////////////////////////////////////////////
CWriteUniqueID::CWriteUniqueID(void)
{
}

CWriteUniqueID::~CWriteUniqueID(void)
{
}

SPRESULT CWriteUniqueID::__PollAction(void)
{
    CONST CHAR* ITEM_NAME = "WriteUniqueID";
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

    // AT+SPIMEI=<card_id>,<"imei">
    // OK
    std::string strRsp = "";
    std::string strAT  = "AT+SPIMEI=0,\"" + strID + "\"";
    SPRESULT res = SendAT(strAT.c_str(), strRsp);
    NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, strID.c_str());
	SetShareMemory(ShareMemory_IMEI1, (void* )strID.c_str(), strID.length());
	NOTIFY(DUT_INFO_IMEI1, (const char*)strID.c_str());
    return res;
}

