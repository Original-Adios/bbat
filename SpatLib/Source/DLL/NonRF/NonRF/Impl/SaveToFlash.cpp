#include "StdAfx.h"
#include "SaveToFlash.h"
#include <assert.h>
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSaveToFlash)

//////////////////////////////////////////////////////////////////////////
CSaveToFlash::CSaveToFlash(void)
: m_eModule(SP_GSM)
, m_u32TimeOut(TIMEOUT_20S)
{
}

CSaveToFlash::~CSaveToFlash(void)
{
}

BOOL CSaveToFlash::LoadXMLConfig(void)
{
    std::wstring sValue = GetConfigValue(L"Option:Module", L"GSM");
    if (0 == sValue.compare(L"GSM"))
    {
        m_eModule = SP_GSM;
    }
    else if (0 == sValue.compare(L"TD-SCDMA"))
    {
        m_eModule = SP_TDSCDMA;
    }
    else if (0 == sValue.compare(L"WCDMA"))
    {
        m_eModule = SP_WCDMA;
    }
    else if (0 == sValue.compare(L"LTE"))
    {
        m_eModule = SP_LTE;
    }
    else
    {
        return FALSE;
    }

    m_u32TimeOut = GetConfigValue(L"Option:TimeOut", 20000);
    return TRUE;
}

SPRESULT CSaveToFlash::__PollAction(void)
{
    SPRESULT res = SP_OK;
    switch(m_eModule)
    {
    case SP_GSM:
		{
			int nFlagPre  = 0x0;
			int nFlagSet  = 0x0;
			if (SP_OK == GetShareMemory(ShareMemory_GSM_SaveToFlash_PreSet, (void*)&nFlagPre, sizeof(nFlagPre), NULL)
				&& SP_OK == GetShareMemory(ShareMemory_GSM_SaveToFlash_Done, (void*)&nFlagSet, sizeof(nFlagSet), NULL))
			{
				if (nFlagPre == nFlagSet && nFlagPre != 0)
				{
					break;
				}
			}
			res = SP_gsmSaveToFlash(m_hDUT, m_u32TimeOut);
		}
        break;
    case SP_TDSCDMA:
        res = SP_tdSaveToFlash(m_hDUT, m_u32TimeOut);
        break;
    case SP_WCDMA:
		res = SP_wcdmaSaveToFlash(m_hDUT, m_u32TimeOut);
        break;
    case SP_LTE:
		res = SP_lteSaveToFlash(m_hDUT, m_u32TimeOut);
        break;
    default:
        assert(0);
        break;
    }

    NOTIFY("SaveToFlash", LEVEL_UI, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, "-", CELLULAR_NAME[m_eModule]);
    return res;
}