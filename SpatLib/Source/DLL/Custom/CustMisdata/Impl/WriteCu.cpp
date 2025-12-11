#include "StdAfx.h"
#include "WriteCu.h"

IMPLEMENT_RUNTIME_CLASS(CWriteCu)
//////////////////////////////////////////////////////////////////////////
CWriteCu::CWriteCu(void)
{
	m_strCu = "";
}

CWriteCu::~CWriteCu(void)
{
}

BOOL CWriteCu::LoadXMLConfig(void)
{
	m_strCu = _W2CA(GetConfigValue(L"Option:CuEdit:Cu", L""));
	if (0 == m_strCu.length() || m_strCu.length() > TCT_CU_REF_LEN)
	{
		return FALSE;
	}

    return TRUE;
}

SPRESULT CWriteCu::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "WriteCu";
	MISCDATA_CU miscCu;
	memcpy(miscCu.CU, m_strCu.c_str(), sizeof(char)*m_strCu.length());

	SPRESULT sRet = WriteCustMisdata(MISCDATA_BASE_RTOS_OFFSET + 154/*Cu offset*/, (LPCVOID)&miscCu, sizeof(miscCu), ESYS_RTOS);
	if (SP_OK != sRet)
	{
		return sRet;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Cu:%s", m_strCu.c_str());
	return SP_OK;

}

