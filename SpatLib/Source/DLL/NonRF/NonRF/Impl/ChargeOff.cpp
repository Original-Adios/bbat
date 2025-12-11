#include "StdAfx.h"
#include "ChargeOff.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CChargeOff)

///
CChargeOff::CChargeOff(void)
{
   
}

CChargeOff::~CChargeOff(void)
{

}

SPRESULT CChargeOff::__PollAction()
{
    PC_CHARGE_T req;
    ZeroMemory((void* )&req, sizeof(req));
    req.cmd = CHARGE_OFF;
    SPRESULT res = SP_apChargeCmd(m_hDUT, &req);
    NOTIFY("ChargeOff", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL);
    return res;
}