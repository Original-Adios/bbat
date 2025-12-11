#include "StdAfx.h"
#include "ReadBackupNV.h"


IMPLEMENT_RUNTIME_CLASS(CReadBackupNV)
//////////////////////////////////////////////////////////////////////////
CReadBackupNV::CReadBackupNV(void)
{
	m_strFileName = L"";
	m_strFileFolder = L"";
	m_u32Magic = 0;
}

CReadBackupNV::~CReadBackupNV(void)
{
}

BOOL CReadBackupNV::LoadXMLConfig(void)
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
SPRESULT CReadBackupNV::__PollAction(void)
{ 
	const char ITEM_NAME[] = "ReadBackupNV";
	SPRESULT sRet = SP_OK;

	//ReadSN1
	char  szSN1[64] = {0};
	sRet = SP_LoadSN(m_hDUT, SN1, szSN1, sizeof(szSN1));
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SP_LoadSN");
		return sRet;
	}
	
	//m_strFileName = GetAbsoluteFilePathW(L"Nvback\\");
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
	sRet = QueryPartitionSize(&u32FixnvSize, &u32MiscnvSize, &u32ProdnvoSize, &m_fileHead.u16FileCount);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "QueryPartitionSize");
		return sRet;
	}
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "QueryPartitionSize");


	uint32 nPartionSize = u32FixnvSize + u32MiscnvSize + u32ProdnvoSize + 	m_fileHead.u16FileCount * sizeof(SEGMENT_HEAD_STRUCT);
	if (0 == nPartionSize)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "No PartitionSize need to backup");
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
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, GetPartionName(0));
			return sRet;
		}
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, GetPartionName(0));
	}


	//u32MiscnvSize
	if (0 != u32MiscnvSize)
	{
		sRet = ReadCustMiscData((uint8*)&u8pPartionData[u32offset], u32MiscnvSize, &u32offset);
		if (SP_OK != sRet)
		{
			free(u8pPartionData);
			u8pPartionData = NULL;
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, GetPartionName(1));
			return sRet;
		}
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, GetPartionName(1));
	}

	//u32ProdnvoSize
	if (0 != u32ProdnvoSize)
	{
		sRet = ReadCustProdNv((uint8*)&u8pPartionData[u32offset], u32ProdnvoSize, &u32offset);
		if (SP_OK != sRet)
		{
			free(u8pPartionData);
			u8pPartionData = NULL;
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, GetPartionName(2));
			return sRet;
		}
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, GetPartionName(2));
	}


	//write2file
	u8pPartionData[u32offset] = '\0';
	if (WriteNvData2File(m_strFileName.c_str(), (uint8 *)&u8pPartionData[0], u32offset))
	{
		free(u8pPartionData);
		u8pPartionData = NULL;
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "CalcFileCRC");
		return sRet;
	}
	
	free(u8pPartionData);
	u8pPartionData = NULL;
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
	return SP_OK;
}

SPRESULT CReadBackupNV::ReadCustNvFile(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset)
{
	LogFmtStrA(SPLOGLV_INFO, "%s file offset = 0x%08X", GetPartionName(0), *u32offset + sizeof(BACKUP_FILE_HEAD)/*文件头*/);

	NVDATA_R_T nvItem;
	nvItem.data_size = 0;
	nvItem.nv_count = 0;
	nvItem.pu8Data = (uint8 *)&u8pNvData[sizeof(SEGMENT_HEAD_STRUCT) + 4/*uint32 nv_count*/];

	SPRESULT sRet = SP_fixnvFullDumpEx(m_hDUT, &nvItem);
	if (SP_OK != sRet || 0 == nvItem.data_size || 0 == nvItem.nv_count || u32Size != nvItem.data_size)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Fixnv status:%d need size:%d real size:%d", sRet, u32Size, nvItem.data_size);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	//FILE HEAD
	SEGMENT_HEAD_STRUCT segmentHead;
	memcpy(segmentHead.szFileName, GetPartionName(0), sizeof(char) * strlen(GetPartionName(0)));
	segmentHead.size = nvItem.data_size + 4/*uint32 nv_count*/;

	memcpy((void*)&u8pNvData[0], (void*)&segmentHead, sizeof(SEGMENT_HEAD_STRUCT));
	memcpy((void*)&u8pNvData[sizeof(SEGMENT_HEAD_STRUCT)], (void*)&nvItem.nv_count, sizeof(uint32));

	*u32offset += (sizeof(SEGMENT_HEAD_STRUCT) + segmentHead.size);//偏移到最后，为下一个分区准备

	return SP_OK;
}
/*/////////////////////////////////////////////////////////////////////////
 R/W Customer data of miscdata
 0 - 4K              PhaseCheck
 4K - 8K             root recorder（是否root过）
 8K~8K+512Byte       Verified Boot功能
 512K~516K           ADC数据存储
 517K~517K+1K        保存pac的创建时间
 768K- 1024K         客户自定义区                   <-- R/W
/////////////////////////////////////////////////////////////////////////*/
SPRESULT CReadBackupNV::ReadCustMiscData(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset)
{
	LogFmtStrA(SPLOGLV_INFO, "%s file offset = 0x%08X", GetPartionName(1), *u32offset + sizeof(BACKUP_FILE_HEAD)/*文件头*/);

	uint32 lpu32RecvLen = u32Size;
	SPRESULT sRet = SP_LoadMiscData(m_hDUT, MISCDATA_ALL_OFFSET, (void*)&u8pNvData[sizeof(SEGMENT_HEAD_STRUCT)], lpu32RecvLen, TIMEOUT_3S);
	if (SP_OK != sRet)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"SP_LoadMiscData,ErrCode:%d", sRet);
		return SP_E_PHONE_FILE_IO;
	}

	//FILE HEAD
	SEGMENT_HEAD_STRUCT segmentHead;
	memcpy(segmentHead.szFileName, GetPartionName(1), sizeof(char) * strlen(GetPartionName(1)));
	segmentHead.size = lpu32RecvLen;

	memcpy((void*)&u8pNvData[0], (void*)&segmentHead, sizeof(SEGMENT_HEAD_STRUCT));

	*u32offset += (sizeof(SEGMENT_HEAD_STRUCT) + segmentHead.size);//偏移到最后，为下一个分区准备
 	return SP_OK;
}

/*/////////////////////////////////////////////////////////////////////////
//prodNV
SP_EXPORT SPRESULT SP_API SP_QueryPartitionSize(SP_HANDLE hDiagPhone, LPCSTR lpszPartitionName, uint64 *lpu64RecvLen);
SP_EXPORT SPRESULT SP_API SP_ReadPartition(SP_HANDLE hDiagPhone,	LPCSTR lpszPartitionName, LPVOID lpBuff,	uint64 u64BytesToRead, uint32 u32TimeOut);
SP_EXPORT SPRESULT SP_API SP_WritePartition(SP_HANDLE hDiagPhone, LPCSTR lpszPartitionName, LPCVOID lpBuff, uint64 u64BytesToWrite, uint32 u32TimeOut);
/////////////////////////////////////////////////////////////////////////*/
SPRESULT CReadBackupNV::ReadCustProdNv(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset)
{
	LogFmtStrA(SPLOGLV_INFO, "%s file offset = 0x%08X", GetPartionName(2), *u32offset + sizeof(BACKUP_FILE_HEAD)/*文件头*/);

	uint32 lpu32RecvLen = u32Size;
	SPRESULT sRet = SP_ReadPartition(m_hDUT, GetPartionName(2), (void*)&u8pNvData[sizeof(SEGMENT_HEAD_STRUCT)], lpu32RecvLen, TIMEOUT_10S);
	if (SP_OK != sRet)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"SP_LoadMiscData,ErrCode:%d", sRet);
		return SP_E_PHONE_FILE_IO;
	}

	//FILE HEAD
	SEGMENT_HEAD_STRUCT segmentHead;
	memcpy(segmentHead.szFileName, GetPartionName(2), sizeof(char) * strlen(GetPartionName(2)));
	segmentHead.size = lpu32RecvLen;

	memcpy((void*)&u8pNvData[0], (void*)&segmentHead, sizeof(SEGMENT_HEAD_STRUCT));

	*u32offset += (sizeof(SEGMENT_HEAD_STRUCT) + segmentHead.size);//偏移到最后，为下一个分区准备
	return SP_OK;
}

SPRESULT CReadBackupNV::WriteNvData2File(LPCWSTR lpcwFileName, uint8 *u8pNvData, uint32 u32Filesize)
{
	//CRC
	UINT16 u16CalCrc = crc16(0, (const unsigned char *)&u8pNvData[0], u32Filesize);
	if (0 == u16CalCrc)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Backup data crc = %d", u16CalCrc);
		return SP_E_PHONE_INVALID_CRC;
	}

	m_fileHead.u16CalCrc = u16CalCrc;
	m_fileHead.magic = m_u32Magic;

	//add file
	HANDLE hFile = CreateFileW(lpcwFileName, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, \
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Create NV file <%s> failed WinErr = %d", lpcwFileName, ::GetLastError());
		return SP_E_PHONE_OPEN_FILE;
	}

	//write file head
	DWORD dwByteWritten = 0;
	BOOL bOK = WriteFile(hFile, (void*)&m_fileHead, sizeof(BACKUP_FILE_HEAD), &dwByteWritten, NULL);
	if (!bOK || sizeof(BACKUP_FILE_HEAD) != dwByteWritten)
	{
		CloseHandle(hFile);
		LogFmtStrW(SPLOGLV_ERROR, L"Write file head to file fail WinErr = %d", ::GetLastError());
		return SP_E_PHONE_FILE_IO;
	}
	FlushFileBuffers(hFile);

	//write file data
	bOK = WriteFile(hFile, (void*)&u8pNvData[0], u32Filesize, &dwByteWritten, NULL);
	if (!bOK || u32Filesize != dwByteWritten)
	{
		CloseHandle(hFile);
		LogFmtStrW(SPLOGLV_ERROR, L"Write total file data fail WinErr = %d", ::GetLastError());
		return SP_E_PHONE_FILE_IO;
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	//rename file
	std::wstring newFileName = lpcwFileName;
	replace_all_w(newFileName, L"temp", L"ok");
	int errCode = _wrename(lpcwFileName, newFileName.c_str());
	if (0 != errCode)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Rename file fail errCode:%d", errCode);
		return SP_E_FILE_NOT_EXIST;
	}

	SetShareMemory(ShareMemory_BackupNVFilePath, (const void*)newFileName.c_str(), newFileName.size() * sizeof(wchar_t));

	return SP_OK;
}