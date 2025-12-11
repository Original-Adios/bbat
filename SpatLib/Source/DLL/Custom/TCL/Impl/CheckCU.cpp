#include "stdafx.h"
#include "CheckCU.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckCU)

//
CCheckCU::CCheckCU()
: m_bCheckCU(FALSE)
{
}

CCheckCU::~CCheckCU()
{
}

BOOL CCheckCU::LoadXMLConfig(void)
{
    m_strTargetCU = _W2CA(GetConfigValue(L"Option:TargetCU", L""));
	m_bCheckCU   = (BOOL)GetConfigValue(L"Option:CheckCU", 0);
    return TRUE;
}

SPRESULT CCheckCU::__PollAction(void)
{
    std::string::size_type nLen = m_strTargetCU.length();
	if (m_bCheckCU && (nLen > TCL_CU_REF_LEN - 1 || nLen < 1))
	{
		NOTIFY("CheckCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid target CU Length = %d", nLen);
		return SP_E_FAIL;
	}

	MISCDATA_TCL misData;
	SPRESULT res = SP_LoadMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, &misData, sizeof(misData), TIMEOUT_3S);
    if (SP_OK != res)
    {
        NOTIFY("LoadCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load customized miscdata fail");
        return SP_E_LOAD_CUSTOMIZED_MISCDATA;
    }

	if (m_bCheckCU)
	{
        if (NULL == strstr(misData.CU, m_strTargetCU.c_str()))
        {
            NOTIFY("CheckCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unmatched CU %s <> %s", misData.CU, m_strTargetCU.c_str());
            return SP_E_MISMATCHED_CU;
        }
        else
        {
            NOTIFY("CheckCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "CU = %s", misData.CU);
        }
    }
	else
	{
		NOTIFY("LoadCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "CU = %s", misData.CU);
	}

    return SP_OK;
}
