#include "stdafx.h"
#include "WriteCU.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CWriteCU)

//
CWriteCU::CWriteCU()
{
    m_strCU = "";
}

CWriteCU::~CWriteCU()
{
}

BOOL CWriteCU::LoadXMLConfig(void)
{
    m_strCU = _W2CA(GetConfigValue(L"Option:CU", L""));
    return TRUE;
}

SPRESULT CWriteCU::__PollAction(void)
{
	if (m_strCU.length() > TCL_CU_REF_LEN - 1 || m_strCU.length() < 1)
	{
		NOTIFY("WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid CU Length = %d", m_strCU.length());
		return SP_E_FAIL;
	}

	MISCDATA_TCL misData;
	SPRESULT res = SP_LoadMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, &misData, sizeof(misData), TIMEOUT_3S);
    if (SP_OK != res)
    {
        NOTIFY("WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load customized miscdata fail");
        return SP_E_LOAD_CUSTOMIZED_MISCDATA;
    }
    else
    {
		strcpy_s(misData.CU, m_strCU.data());
        res = SP_SaveMiscData(m_hDUT, MISCDATA_CUSTOMER_OFFSET, &misData, sizeof(misData), TIMEOUT_3S);
		if (SP_OK != res)
		{
			NOTIFY("WriteCU", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Save customized miscdata fail");
			return SP_E_SAVE_CUSTOMIZED_MISCDATA;
		}
		else
		{
			NOTIFY("WriteCU", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strCU.c_str());
		}
    }

    return SP_OK;
}
