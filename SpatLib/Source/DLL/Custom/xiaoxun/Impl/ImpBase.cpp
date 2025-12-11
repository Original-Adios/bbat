#include "StdAfx.h"
#include "ImpBase.h"


//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::WriteCustMisdata(uint32 u32InBase, const void * pData, int length)
{
	if (!IN_RANGE(1, length, MISC_DATA_FOR_OTP))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid CustMisdata code, length = \'%d\'", length);
		return SP_E_INVALID_PARAMETER;
	}

	//写入miscdata
	CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, u32InBase, (uint8*)pData, length, TIMEOUT_3S), "SaveCustomerMiscData");

	Sleep(100);
	// 读取出来和写入的数据进行比较
	uint8 rbuff[MISC_DATA_FOR_OTP] = {0};
	CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, u32InBase, rbuff, length, TIMEOUT_3S), "LoadCustomerMiscData");

	if (0 != memcmp(pData, rbuff, length))
	{
		NOTIFY("VerifyCustomerMiscData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Verify after write fails");
		return SP_E_MISMATCHED_CU;
	}

	return SP_OK;
}
