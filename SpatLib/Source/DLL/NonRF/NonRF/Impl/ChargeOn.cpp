#include "StdAfx.h"
#include "ChargeOn.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CChargeOn)

///
CChargeOn::CChargeOn(void)
: m_lMaxChargeCurrent(500)
{
   
}

CChargeOn::~CChargeOn(void)
{

}

BOOL CChargeOn::LoadXMLConfig(void)
{
    m_lMaxChargeCurrent = GetConfigValue(L"Option:MaxChargeCurrent", 500);
    return TRUE;
}

SPRESULT CChargeOn::__PollAction()
{
    PC_CHARGE_T req;
    ZeroMemory((void* )&req, sizeof(req));
    req.cmd = CHARGE_ON;
    SPRESULT res = SP_apChargeCmd(m_hDUT, &req);
    if (SP_OK != res)
    {
        NOTIFY("ChargeOn", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return res;
    }

    CONST long MAX_USB_CHARGE_CURRENT = 500;
    if (m_lMaxChargeCurrent > MAX_USB_CHARGE_CURRENT)
    {
        PC_CHARGE_T req;
        ZeroMemory((void* )&req, sizeof(req));
        req.cmd = CHARGE_CURRENT_CHANGE;
        req.charging_current = m_lMaxChargeCurrent;
        res = SP_apChargeCmd(m_hDUT, &req);
        if (SP_OK != res)
        {
            NOTIFY("ChangeChargeCurrent", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MaxChargeCurrent = %dmA", m_lMaxChargeCurrent);
            return res;
        }
        else
        {
            NOTIFY("ChargeOn", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "MaxChargeCurrent = %dmA", m_lMaxChargeCurrent);
        }
    }
    else
    {
        NOTIFY("ChargeOn", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
    }

    return res;
}