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
	m_strDLL = L"";
	m_strServerType = L"";
	m_strServerIP = L"";
	m_strServerDatabase = L"";
	m_strServerUser = L"";
	m_strServerPassword = L"";
	BatchName = "";
	StationName = "";
}

CMES_Connect::~CMES_Connect(void)
{
}

BOOL CMES_Connect::LoadXMLConfig(void)
{
	std::wstring DllPath = GetConfigValue(L"Option:DllPath", L"");
	m_strDLL = GetAbsoluteFilePathW(DllPath.c_str());
	if (0 == m_strDLL.length())
	{
		return FALSE;
	}

	m_strServerType = GetConfigValue(L"Option:Server:Type", L"");	
	m_strServerIP = GetConfigValue(L"Option:Server:Server", L"");
	m_strServerDatabase = GetConfigValue(L"Option:Server:Database", L"");
	m_strServerUser = GetConfigValue(L"Option:Server:User", L"");
	m_strServerPassword = GetConfigValue(L"Option:Server:Password", L"");


    std::wstring strUpload = GetConfigValue(L"Option:DataLog:Upload", L"Sync");
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

    std::wstring strLevel = GetConfigValue(L"Option:DataLog:Level", L"");
	if (!strLevel.compare(L"BASIC_1"))
	{
		m_eLoglevel = LEVLE_BASIC;
	}
	else if (!strLevel.compare(L"FT_CURRENT_2"))
	{
		m_eLoglevel = LEVLE_FT_CURRENT;
	}
	else if (!strLevel.compare(L"CAL_4"))
	{
		m_eLoglevel = LEVLE_CAL;
	}
	else if (!strLevel.compare(L"NV_8"))
	{
		m_eLoglevel = LEVLE_NV;
	}
	else if (!strLevel.compare(L"MISC_16"))
	{
		m_eLoglevel = LEVLE_MISC;
	}
	else
	{
		m_eLoglevel = LEVLE_ALL;
	}

    BatchName = _W2CA(GetConfigValue(L"Option:DataLog:BatchName", L""));
	if (0 == BatchName.length())
	{
		return FALSE;
	}
    StationName = _W2CA(GetConfigValue(L"Option:DataLog:StationName", L""));
	if (0 == StationName.length())
	{
		return FALSE;
	}
    return TRUE;
}

SPRESULT CMES_Connect::__PollAction(void)
{
	CONST CHAR ITEM_NAME[] = "MES_Connect";
	extern CActionApp  myApp;
	//获取or创建CMesDriver指针对象m_pMesDrv
	if (SP_OK != GetMesDriverFromShareMemory())
	{
		m_pMesDrv = new CMesDriver();
		CHKRESULT(SetMesDriverIntoShareMemory());
	}


	//调用CMesDriver类里的Startup，完成CustomerSolutionMes.dll的加载
	CHKRESULT_WITH_NOTIFY(m_pMesDrv->Startup(m_strDLL) ?  FALSE : TRUE, _W2CA(m_strDLL.c_str()));

	//调用CMesDriver类里MES_Handle_Create，实现调用CustomerSolutionMes.dll的MES_Handle_Create API创建句柄
	HANDLE *hMes;
	if (FALSE == m_pMesDrv->_Handle_Create(GetISpLogObject(), &hMes))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES HANDLE == NULL");
		return SP_E_FAIL;
	}
	//保存CustomerSolutionMes.dll创建的句柄到m_pMesDrv中
	m_pMesDrv->SetMesHandle(hMes);

	//调用CMesDriver类里MES_Login，实现调用CustomerSolutionMes.dll的MES_Login API登录MES
	if (TRUE != m_pMesDrv->_Login(hMes, m_strServerType.c_str(), m_strServerIP.c_str(),
		m_strServerDatabase.c_str(), m_strServerUser.c_str(), m_strServerPassword.c_str()))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "");
		return SP_E_SPDB_SERVER_CONNECT;
	}
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");

	double dStartTime = GetTickCount();
	CHKRESULT(SetShareMemory(ShareMemory_MES_Start_Time, (const void*)&dStartTime, sizeof(dStartTime)));

	PROJINFOR ProjInfor;
	memcpy(ProjInfor.m_szBatchName, (CHAR*)BatchName.c_str(), sizeof(CHAR)*BatchName.length());
	memcpy(ProjInfor.m_szStationName, (CHAR*)StationName.c_str(), sizeof(CHAR)*StationName.length());
	CHKRESULT(SetShareMemory(ShareMemory_MES_V1_Batch_info, (const void*)&ProjInfor, sizeof(ProjInfor)));
	//以下代码用于存储Simba界面显示的内部，具体存在在ShareMemoryItemDataObserver
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
		pMesData->m_iCsvLogUploadLevel = m_eLoglevel;
		memcpy(pMesData->szBatch, (char*)BatchName.c_str(), BatchName.length());
		memcpy(pMesData->szStation, (char*)StationName.c_str(), StationName.length());

		pMesData->szBatch[BatchName.length()] = '\0';
		pMesData->szStation[StationName.length()] = '\0';
        IItemDataObserver* pObs = (IItemDataObserver* )pMesData;
        CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pObs, sizeof(pObs), IContainer::System));

    }
    return SP_OK;
}

