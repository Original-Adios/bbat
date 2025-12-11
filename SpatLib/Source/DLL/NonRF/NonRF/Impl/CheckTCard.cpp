#include "StdAfx.h"
#include "CheckTCard.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckTCard)

///
CCheckTCard::CCheckTCard(void)
{
   
}

CCheckTCard::~CCheckTCard(void)
{

}

SPRESULT CCheckTCard::__PollAction()
{
    LPCSTR AT = "AT+SPCHKSD";
    std::string strRsp = "";
    CHKRESULT_WITH_NOTIFY(SendAT(AT, strRsp), AT);
    if (NULL != strstr(strRsp.c_str(), "+SPCHKSD: 1"))
    {
        NOTIFY("CheckTCard", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_OK;
    }
    else
    {
        NOTIFY("CheckTCard", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_E_SPAT_CHECK_T_CARD;
    }
}