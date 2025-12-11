#include "StdAfx.h"
#include "ReadUniqueID.h"

//
IMPLEMENT_RUNTIME_CLASS(CReadUniqueID)
//////////////////////////////////////////////////////////////////////////
CReadUniqueID::CReadUniqueID(void)
{
}

CReadUniqueID::~CReadUniqueID(void)
{
}

SPRESULT CReadUniqueID::__PollAction(void)
{
    CONST CHAR* ITEM_NAME = "ReadUniqueID";

    // AT+SPIMEI=<card_id>,<"imei">
    // OK
    std::string strID = "";
    std::string strAT  = "AT+SPIMEI?";
    SPRESULT res = SendAT(strAT.c_str(), strID);
    NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, strID.c_str());
	SetShareMemory(ShareMemory_IMEI1, (void* )strID.c_str(), strID.length());
	NOTIFY(DUT_INFO_IMEI1, (const char*)strID.c_str());
    return res;
}

