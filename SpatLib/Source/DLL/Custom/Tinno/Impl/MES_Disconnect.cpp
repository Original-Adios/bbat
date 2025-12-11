#include "StdAfx.h"
#include "MES_Disconnect.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_Disconnect)
//////////////////////////////////////////////////////////////////////////
CMES_Disconnect::CMES_Disconnect(void)
{
}

CMES_Disconnect::~CMES_Disconnect(void)
{
}

BOOL CMES_Disconnect::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_Disconnect::__PollAction(void)
{
	extern CActionApp  myApp;

	char szPathIni[512]={0},szNote[128]={0},szUser[128]={0},szPwd[128]={0},szDescription[1024]={0};

	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Config\\TinnoMes.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));

	GetPrivateProfileStringA("Connect", "username", "", szUser, sizeof(szUser), szPathIni);
	GetPrivateProfileStringA("Connect", "password", "", szPwd, sizeof(szPwd), szPathIni);
	GetPrivateProfileStringA("Connect", "mesnote", "", szNote, sizeof(szNote), szPathIni);


	if (TINNO_SUCC != MES_Login_out(szNote, szUser, szPwd, szDescription))
	{
		NOTIFY("MES_Disconnect", LEVEL_ITEM, 1, 0, 1, NULL, -1, szDescription);
		return SP_E_SPDB_DISCONNECT_SERVER_FAIL;
	}

	NOTIFY("MES_Disconnect", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	return SP_OK;
}

