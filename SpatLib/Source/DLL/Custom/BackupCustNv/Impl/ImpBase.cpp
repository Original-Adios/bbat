#include "StdAfx.h"
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::QueryPartitionSizeV2(uint32* u32FixnvSize, uint32* u32MiscnvSize, uint32* u32ProdnvSize, uint16* u16nvFileCount)
{
	SPRESULT sRet = SP_OK;
	uint16 u16nvCount = 0;

	sRet = SP_QueryPartitionSize(m_hDUT, GetPartionName(3), u32FixnvSize);
	if (SP_OK != sRet)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Query %s Size", GetPartionName(0));
		return sRet;
	}
	if (0 != *u32FixnvSize)
	{
		u16nvCount++;
	}

	sRet = SP_QueryPartitionSize(m_hDUT, GetPartionName(1), u32MiscnvSize);
	if (SP_OK != sRet)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Query %s Size", GetPartionName(1));
		return sRet;
	}

	if (0 != *u32MiscnvSize)
	{
		u16nvCount++;
	}

	sRet = SP_QueryPartitionSize(m_hDUT, GetPartionName(2), u32ProdnvSize);
	if (SP_OK != sRet)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Query %s Size", GetPartionName(2));
		return sRet;
	}
	if (0 != *u32ProdnvSize)
	{
		u16nvCount++;
	}
	* u16nvFileCount = u16nvCount;
	return SP_OK;
}

SPRESULT CImpBase::QueryPartitionSize(uint32 *u32FixnvSize, uint32 *u32MiscnvSize, uint32 *u32ProdnvSize, uint16 *u16nvFileCount)
{
	SPRESULT sRet = SP_OK;
	uint16 u16nvCount = 0;

	sRet = SP_QueryPartitionSize(m_hDUT, GetPartionName(0), u32FixnvSize);
	if (SP_OK != sRet)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Query %s Size", GetPartionName(0));
		return sRet;
	}
	if (0 != *u32FixnvSize)
	{
		u16nvCount++;
	}


	sRet = SP_QueryPartitionSize(m_hDUT, GetPartionName(1), u32MiscnvSize);
	if (SP_OK != sRet)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Query %s Size", GetPartionName(1));
		return sRet;
	}

	if (0 != *u32MiscnvSize)
	{
		u16nvCount++;
	}

	sRet = SP_QueryPartitionSize(m_hDUT, GetPartionName(2), u32ProdnvSize);
	if (SP_OK != sRet)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Query %s Size", GetPartionName(2));
		return sRet;
	}
	if (0 != *u32ProdnvSize)
	{
		u16nvCount++;
	}

	*u16nvFileCount = u16nvCount;
	return SP_OK;
}
