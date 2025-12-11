#include "StdAfx.h"
#include "ReadBackupNvV2.h"


IMPLEMENT_RUNTIME_CLASS(CReadBackupNvV2)
//////////////////////////////////////////////////////////////////////////
CReadBackupNvV2::CReadBackupNvV2(void)
{
	m_strFileName = L"";
	m_strFileFolder = L"";
	m_u32Magic = 0;
}

CReadBackupNvV2::~CReadBackupNvV2(void)
{
}

BOOL CReadBackupNvV2::LoadXMLConfig(void)
{
	//magic
	m_u32Magic = GetConfigValue(L"Option:Magic", 0);
	if (0 == m_u32Magic)
	{
		return FALSE;
	}
	std::wstring strFileName = GetAbsoluteFilePathW(L"Nvback\\");
	std::wstring strFileFolder = GetConfigValue(L"Option:SavePath", strFileName.c_str());
	m_strFileFolder = GetAbsoluteFilePathW(strFileFolder.c_str());

	return TRUE;
}
//For CQ:SPCSS00639069
SPRESULT CReadBackupNvV2::__PollAction(void)
{ 
	LPCWSTR strItem = GetConfigValue(L"Option:Item", L"");
	std::string strItemName = _W2CA(strItem);

	SPRESULT sRet = SP_OK;

	//ReadSN1
	char  szSN1[64] = {0};
	sRet = SP_LoadSN(m_hDUT, SN1, szSN1, sizeof(szSN1));
	if (SP_OK != sRet)
	{
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SP_LoadSN");
		return sRet;
	}
	
	m_strFileName = GetAbsoluteFilePathW(L"Nvback\\");
	CreateMultiDirectory(m_strFileFolder.c_str(), NULL);
	SYSTEMTIME    tm;
	GetLocalTime(&tm);
	WCHAR szTime[32] = {0};
	swprintf_s(szTime, _T("%04d%02d%02d%02d%02d%02d"), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

	m_strFileName = m_strFileFolder;
	m_strFileName += L"\\";
	m_strFileName += _A2CW(szSN1);
 	m_strFileName += L"_";
 	m_strFileName += szTime;
 	m_strFileName += L"_temp.bin";/*if write file successfully,"Temp" will revise "OK"*/

	m_fileHead.u16FileCount = 0;

	//partitionsize
	UINT32 u32FixnvSize = 0;
	UINT32 u32MiscnvSize = 0;
	UINT32 u32ProdnvoSize = 0;
	sRet = QueryPartitionSizeV2(&u32FixnvSize, &u32MiscnvSize, &u32ProdnvoSize, &m_fileHead.u16FileCount);
	if (SP_OK != sRet)
	{
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "QueryPartitionSize");
		return sRet;
	}
	NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "QueryPartitionSize");


	uint32 nPartionSize = u32FixnvSize + u32MiscnvSize + u32ProdnvoSize + 	m_fileHead.u16FileCount * sizeof(SEGMENT_HEAD_STRUCT);
	if (0 == nPartionSize)
	{
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "No PartitionSize need to backup");
	}
	nPartionSize += 12/*uint32 nv_count*/;
	uint8 *u8pPartionData = (uint8 *)malloc(nPartionSize);
	if (!u8pPartionData)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"malloc Buffer fail");
		return SP_E_ALLOC_MEMORY;
	}
	memset(u8pPartionData, 0, nPartionSize);

	uint32 u32offset = 0;
	//u32FixnvSize
	if (0 != u32FixnvSize)
	{
		sRet = ReadCustNvFile((uint8*)&u8pPartionData[u32offset], u32FixnvSize, &u32offset);
		if (SP_OK != sRet)
		{
			free(u8pPartionData);
			u8pPartionData = NULL;
			NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, GetPartionName(0));
			return sRet;
		}
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, GetPartionName(0));
	}


	//u32MiscnvSize
	if (0 != u32MiscnvSize)
	{
		sRet = ReadCustMiscData((uint8*)&u8pPartionData[u32offset], u32MiscnvSize, &u32offset);
		if (SP_OK != sRet)
		{
			free(u8pPartionData);
			u8pPartionData = NULL;
			NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, GetPartionName(1));
			return sRet;
		}
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, GetPartionName(1));
	}

	//u32ProdnvoSize
	if (0 != u32ProdnvoSize)
	{
		sRet = ReadCustProdNv((uint8*)&u8pPartionData[u32offset], u32ProdnvoSize, &u32offset);
		if (SP_OK != sRet)
		{
			free(u8pPartionData);
			u8pPartionData = NULL;
			NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, GetPartionName(2));
			return sRet;
		}
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, GetPartionName(2));
	}


	//write2file
	u8pPartionData[u32offset] = '\0';
	if (WriteNvData2File(m_strFileName.c_str(), (uint8 *)&u8pPartionData[0], u32offset))
	{
		free(u8pPartionData);
		u8pPartionData = NULL;
		NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "CalcFileCRC");
		return sRet;
	}
	
	free(u8pPartionData);
	u8pPartionData = NULL;
	NOTIFY(strItemName.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}

SPRESULT CReadBackupNvV2::ReadCustNvFile(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset)
{
	LogFmtStrA(SPLOGLV_INFO, "%s file offset = 0x%08X", GetPartionName(3), *u32offset + sizeof(BACKUP_FILE_HEAD)/*文件头*/);

	NVDATA_R_T nvItem;
	nvItem.data_size = 0;
	nvItem.nv_count = 0;
	nvItem.pu8Data = (uint8 *)&u8pNvData[sizeof(SEGMENT_HEAD_STRUCT) + 4/*uint32 nv_count*/];

	SPRESULT sRet = SP_fixnvFullDumpExV2(m_hDUT, &nvItem);
	if (SP_OK != sRet || 0 == nvItem.data_size || 0 == nvItem.nv_count || u32Size != nvItem.data_size)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Fixnv status:%d need size:%d real size:%d", sRet, u32Size, nvItem.data_size);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	//FILE HEAD
	SEGMENT_HEAD_STRUCT segmentHead;
	memcpy(segmentHead.szFileName, GetPartionName(3), sizeof(char) * strlen(GetPartionName(3)));
	segmentHead.size = nvItem.data_size + 4/*uint32 nv_count*/;

	memcpy((void*)&u8pNvData[0], (void*)&segmentHead, sizeof(SEGMENT_HEAD_STRUCT));
	memcpy((void*)&u8pNvData[sizeof(SEGMENT_HEAD_STRUCT)], (void*)&nvItem.nv_count, sizeof(uint32));

	*u32offset += (sizeof(SEGMENT_HEAD_STRUCT) + segmentHead.size);//偏移到最后，为下一个分区准备

	return SP_OK;
}
