#include "stdafx.h"
#include "LoadZCPropVersion.h"

IMPLEMENT_RUNTIME_CLASS(CLoadZCPropVersion)

//////////////////////////////////////////////////////////////////////////
CLoadZCPropVersion::CLoadZCPropVersion()
{
	m_bCompareVer = TRUE;
}

CLoadZCPropVersion::~CLoadZCPropVersion()
{

}

BOOL CLoadZCPropVersion::LoadXMLConfig(void)
{
    m_bCompareVer = (BOOL)GetConfigValue(L"Option:Check", 1);
    m_strVersion = _W2CA(GetConfigValue(L"Option:Version", L""));
	m_strVerDate = _W2CA(GetConfigValue(L"Option:VerDate", L""));
    return TRUE;
}

SPRESULT CLoadZCPropVersion::__PollAction(void)
{
	const char ITEM_NAME[] = "ZC-VERS";
    LPCSTR VER_CMD = "AT+PROP=0,[ro.zx.itl.sw.verno]";
    SPRESULT res = SendATCommand(VER_CMD);
    if (SP_OK != res)
    {
        NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, VER_CMD);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
		if (!m_bCompareVer)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
		}
		else
		{
			if (NULL == strstr(m_strRevBuf.c_str(), m_strVersion.c_str()))
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Dismatch Version: %s", m_strRevBuf.c_str());
				return SP_E_SPAT_INVALID_DATA;
			}
			else
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
			}
		}
    }

	LPCSTR VERD_CMD = "AT+PROP=0,[ro.zx.date.ydmhs]";
	res = SendATCommand(VERD_CMD);
	if (SP_OK != res)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, VERD_CMD);
		return SP_E_PHONE_AT_EXECUTE_FAIL;
	}
	else
	{
		if (!m_bCompareVer)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
		}
		else
		{
			if (NULL == strstr(m_strRevBuf.c_str(), m_strVerDate.c_str()))
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Dismatch date: %s", m_strRevBuf.c_str());
				return SP_E_SPAT_INVALID_DATA;
			}
			else
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
			}
		}
	}
	
    return SP_OK;
}
