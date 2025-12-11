#include "StdAfx.h"
#include "WriteSN_MGB_GSM.h"
#include "ShareMemoryDefine.h"
#include "CLocks.h"
#include <atlconv.h>
#include "GenCodes.h"
#include "UeHelper.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CWriteSN_MGB_GSM)

    /// 
CWriteSN_MGB_GSM::CWriteSN_MGB_GSM(void)
{
}

CWriteSN_MGB_GSM::~CWriteSN_MGB_GSM(void)
{
}

BOOL CWriteSN_MGB_GSM::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CWriteSN_MGB_GSM::__PollAction(void)
{
	SPRESULT res = SP_OK;
	res = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
	if (SP_OK != res)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Get share memory < %s > failure!", ShareMemory_My_UserInputSN);
		return res;
	}

	CUeHelper ue(this);
	if(m_InputSN[BC_SN1].bEnable)
	{
		CHKRESULT(ue.WriteMGBSN(m_InputSN[BC_SN1].szCode ));
	}
	else
	{
		NOTIFY("Write SN MGB GSM", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN not be selected!");
	}
	
	NOTIFY(DUT_INFO_SN1 , m_InputSN[BC_SN1].szCode);
	CHKRESULT(SetShareMemory(ShareMemory_SN1, (const void* )&m_InputSN[BC_SN1].szCode, ShareMemory_SN_SIZE));
	CHKRESULT(SaveDataIntoLocal());
	CHKRESULT(UpdateSectionAllocCodes());

    return SP_OK;
}