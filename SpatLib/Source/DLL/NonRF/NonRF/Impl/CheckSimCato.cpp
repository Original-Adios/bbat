#include "StdAfx.h"
#include "CheckSimCato.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckSimCato)

///
CCheckSimCato::CCheckSimCato(void)
{
   
}

CCheckSimCato::~CCheckSimCato(void)
{

}

SPRESULT CCheckSimCato::__PollAction()
{
    LPCSTR AT = "AT+SPTEST=100";
    std::string strRsp = "";
    CHKRESULT_WITH_NOTIFY(SendAT(AT, strRsp), AT);
    if (NULL != strstr(strRsp.c_str(), "+SPTEST: 1"))
    {
        NOTIFY("CheckSimCato", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_OK;
    }
    else
    {
        NOTIFY("CheckSimCato", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_E_SPAT_CHECK_T_CARD;
    }
}