#include "StdAfx.h"
#include "ImplicitCallMesDll.h"



IMPLEMENT_RUNTIME_CLASS(CImplicitCallMesDll)
CImplicitCallMesDll::CImplicitCallMesDll(void)
{
	m_strServerType.clear();
	m_strServerIP.clear();
	m_strServerDatabase.clear();
	m_strServerUser.clear();
	m_strServerPassword.clear();
}


CImplicitCallMesDll::~CImplicitCallMesDll(void)
{
}

BOOL CImplicitCallMesDll::LoadXMLConfig(void)
{
	m_strServerType = GetConfigValue(L"Option:Server:Type", L"");
	m_strServerIP = GetConfigValue(L"Option:Server:Server", L"");
	m_strServerDatabase = GetConfigValue(L"Option:Server:Database", L"");
	m_strServerUser = GetConfigValue(L"Option:Server:User", L"");
	m_strServerPassword = GetConfigValue(L"Option:Server:Password", L"");

	return TRUE;
}

SPRESULT CImplicitCallMesDll::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "Implicit Call Mes Dll";

	HANDLE *hMes;
	if (FALSE == _CreateMesDllObject(GetISpLogObject(), &hMes))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES HANDLE == NULL");
		return SP_E_FAIL;
	}

	/*m_pMesDrv->SetMesHandle(hMes);*/
	BOOL bRet = _Connect(hMes, m_strServerType.c_str(), m_strServerIP.c_str(), m_strServerDatabase.c_str(), m_strServerUser.c_str(), m_strServerPassword.c_str());
	if (!bRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES_Login fail");
		return SP_E_INVALID_PARAMETER;
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}
