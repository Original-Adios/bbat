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

SPRESULT CMES_Disconnect::__PollAction(void)
{
	CONST CHAR ITEM_NAME[] = "MES_Disconnect";
	//获取m_pMesDrv
	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");
	//获取CustomerSolutionMes.dll句柄
	HANDLE hMes = m_pMesDrv->GetMesHandle(); 
	if (NULL == hMes)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "MES HANDLE == NULL");
		return SP_E_FAIL;
	}
	//调用CMesDriver类里MES_Logout，实现调用CustomerSolutionMes.dll的MES_Logout API退出MES
	if (TRUE != m_pMesDrv->_Logout(hMes))
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, "");
		return SP_E_FAIL;
	}
	//释放CustomerSolutionMes.dll创建的句柄
	m_pMesDrv->_Handle_ReleaseAll(hMes);
	//释放已加载的CustomerSolutionMes.dll
	m_pMesDrv->Cleanup();
	//删除m_pMesDrv指针
	delete m_pMesDrv;
	//m_pMesDrv置空并把共享内存中置空
	m_pMesDrv = NULL;
	CHKRESULT(SetMesDriverIntoShareMemory());

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	return SP_OK;
}

