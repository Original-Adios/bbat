#include "StdAfx.h"
#include "CheckNonTCard.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckNonTCard)

///
CCheckNonTCard::CCheckNonTCard(void)
{
   
}

CCheckNonTCard::~CCheckNonTCard(void)
{

}

SPRESULT CCheckNonTCard::__PollAction()
{
	LPCSTR ITEM_NAME = "CheckNonTCard";
    LPCSTR AT = "AT+SPCHKSD";
    std::string strRsp = "";
    CHKRESULT_WITH_NOTIFY(SendAT(AT, strRsp), AT);
    if (NULL == strstr(strRsp.c_str(), "+SPCHKSD: 1"))
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "T Card No Inserted");
        return SP_OK;
    }
    else
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "T Card Inserted");
        return SP_E_SPAT_CHECK_T_CARD;
    }
}