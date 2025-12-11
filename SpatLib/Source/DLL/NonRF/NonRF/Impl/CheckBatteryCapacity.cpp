#include "StdAfx.h"
#include "CheckBatteryCapacity.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckBatteryCapacity)

///
CCheckBatteryCapacity::CCheckBatteryCapacity(void)
: m_dLowLimit(0.0f)
, m_dUppLimit(0.0f)
{
}

CCheckBatteryCapacity::~CCheckBatteryCapacity(void)
{
    
}

BOOL CCheckBatteryCapacity::LoadXMLConfig(void)
{
    m_dLowLimit = GetConfigValue(L"Option:LowLimit",   0.0f);
    m_dUppLimit = GetConfigValue(L"Option:UppLimit", 100.0f);

    return TRUE;
}

SPRESULT CCheckBatteryCapacity::__PollAction()
{
    uint32 u32Capacity = 0;
    CHKRESULT_WITH_NOTIFY(SP_apGetBatteryCapacity(m_hDUT, &u32Capacity), "SP_apGetBatteryCapacity");
    NOTIFY("BatteryCapacity", LEVEL_ITEM|LEVEL_INFO, m_dLowLimit, u32Capacity, m_dUppLimit, NULL, -1, "%");
    if (!IN_RANGE(m_dLowLimit, u32Capacity, m_dUppLimit))
    {
        return SP_E_SPAT_BATTERY_CAPACITY_OUTOF_RANGE;
    }

    return SP_OK;
}
