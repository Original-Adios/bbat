#include "StdAfx.h"
#include "MES_Connect.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//



IMPLEMENT_RUNTIME_CLASS(CMES_Connect)
//////////////////////////////////////////////////////////////////////////
CMES_Connect::CMES_Connect(void)
:  m_eUpload(SYNC)
{
}

CMES_Connect::~CMES_Connect(void)
{
}

BOOL CMES_Connect::LoadXMLConfig(void)
{
    std::wstring strUpload = GetConfigValue(L"Option:Upload", L"Sync");
    if (strUpload == L"Async")
    {
        m_eUpload = ASYNC;
    }
    else if (strUpload == L"Sync")
    {
        m_eUpload = SYNC;
    }
    else
    {
        m_eUpload = NONE;
    }

    return TRUE;
}

SPRESULT CMES_Connect::__PollAction(void)
{
	extern CActionApp  myApp;

	char szPathIni[512]={0},szNote[128]={0},szUser[128]={0},szPwd[128]={0},szDescription[1024]={0};

	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\..\\Config\\TinnoMes.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));

	GetPrivateProfileStringA("Connect", "username", "", szUser, sizeof(szUser), szPathIni);
	GetPrivateProfileStringA("Connect", "password", "", szPwd, sizeof(szPwd), szPathIni);
	GetPrivateProfileStringA("Connect", "mesnote", "", szNote, sizeof(szNote), szPathIni);


	if (TINNO_SUCC != MES_Login(szNote, szUser, szPwd, szDescription))
	{
		NOTIFY("MES_Connect", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, szDescription);
		return SP_E_SPDB_SERVER_CONNECT;
	}
	NOTIFY("MES_Connect", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");

	if (NONE != m_eUpload)
	{
		CMesData* pMesData = NULL;
		if ((SP_OK == GetShareMemory(ShareMemoryItemDataObserver, (void* )&pMesData, sizeof(pMesData))) && (NULL != pMesData))
		{
			delete pMesData;
			pMesData = NULL;
		}
		pMesData = new CMesData();
		if (ASYNC == m_eUpload)
		{
			CHKRESULT_WITH_NOTIFY(pMesData->StartUpload(TRUE) ? SP_OK : SP_E_SPDB_START_TEST, "StartUploading");
		}
		IItemDataObserver* pObs = (IItemDataObserver* )pMesData;
		CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pObs, sizeof(pObs), IContainer::System));
	}

    return SP_OK;
}

