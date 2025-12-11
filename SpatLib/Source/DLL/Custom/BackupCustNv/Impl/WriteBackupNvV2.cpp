#include "StdAfx.h"
#include "WriteBackupNvV2.h"

//extern CONST CHAR *PNV_FILE_NAME[];
IMPLEMENT_RUNTIME_CLASS(CWriteBackupNvV2)
//////////////////////////////////////////////////////////////////////////
CWriteBackupNvV2::CWriteBackupNvV2(void)
{
	m_strFileName = L"";
	m_strFileFolder = L"";
	m_bCalinv = true;
	m_bProdnv = true;
	m_bMiscdata = true;
	m_bClearNvVerFlag = true;
}

CWriteBackupNvV2::~CWriteBackupNvV2(void)
{
}

BOOL CWriteBackupNvV2::LoadXMLConfig(void)
{
	std::wstring strFileName = GetConfigValue(L"Option:NvFile", L"");
	if (0 != strFileName.length())
	{
		m_strFileName = GetAbsoluteFilePathW(strFileName.c_str());
	}

	std::wstring strFileFolder = GetConfigValue(L"Option:NvFileFolder", L"");
	if (0 != strFileFolder.length())
	{
		m_strFileFolder = GetAbsoluteFilePathW(strFileFolder.c_str());
	}

	m_bCalinv = GetConfigValue(L"Option:Fixnv", 1);
	m_bProdnv = GetConfigValue(L"Option:Prodnv", 1);
	m_bMiscdata = GetConfigValue(L"Option:Miscdata", 1);
	m_bClearNvVerFlag = GetConfigValue(L"Option:ClearNvVerFlag", 1);
	return TRUE;
}
#pragma warning(disable:4533)
SPRESULT CWriteBackupNvV2::__PollAction(void)
{ 
	const char ITEM_NAME[] = "WriteBackupNvV2";
	SPRESULT sRet = SP_OK;
	std::wstring strOpenFileName = L"";

	if (0 == m_strFileName.length())
	{
		char  szSN1[64] = {0};
		sRet = SP_LoadSN(m_hDUT, SN1, szSN1, sizeof(szSN1));
		if (SP_OK != sRet)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SP_LoadSN");
			return sRet;
		}
		std::wstring strFileName = m_strFileFolder +L"\\" + _A2CW(szSN1) + L"_*_ok.bin";
		WCHAR szNewPath[MAX_PATH] = {0};
		uint32 nNewpathsize = MAX_PATH;
		if (!SearchFilesByWildChar(strFileName.c_str(), szNewPath, nNewpathsize))
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", _W2CA(szNewPath));
			return SP_E_FILE_NOT_EXIST;
		}
		strOpenFileName = m_strFileFolder +L"\\" + szNewPath;
	}
	else
	{
		strOpenFileName = m_strFileName;
	}

	HANDLE hFile = CreateFileW(strOpenFileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, \
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "Open NV file <%s> failed WinErr = %d", _W2CA(strOpenFileName.c_str()), ::GetLastError());
		return SP_E_OPEN_FILE;
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);
	DWORD dwNvSize = GetFileSize(hFile, NULL); 
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	uint32 u32offset = sizeof(BACKUP_FILE_HEAD);
	if (dwNvSize < u32offset)
	{
		CloseHandle(hFile);
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "Read %d size fail! small size of file head", dwNvSize);
		return SP_E_INVALID_PARAMETER;
	}

	uint8 *pu8NvBuf = (uint8*)malloc(dwNvSize);
	if (!pu8NvBuf)
	{
		CloseHandle(hFile);
		LogFmtStrA(SPLOGLV_ERROR, "pu8NvBuf malloc memory failed");
		return SP_E_ALLOC_MEMORY;
	}
	memset(pu8NvBuf, 0, dwNvSize);

	DWORD dwIOSize = 0;
	BOOL bOK = ReadFile(hFile, pu8NvBuf, dwNvSize, &dwIOSize, NULL);
	CloseHandle(hFile);
	if (!bOK || dwIOSize != dwNvSize)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "ReadFile failed; WinErr = %d", ::GetLastError());
		sRet = SP_E_FILE_IO;
		goto __END;
	}
	//fileHead
	BACKUP_FILE_HEAD fileHead = *(BACKUP_FILE_HEAD*)pu8NvBuf;
	UINT16 u16CalCrc = crc16(0, (const unsigned char *)&pu8NvBuf[sizeof(BACKUP_FILE_HEAD)], dwIOSize - sizeof(BACKUP_FILE_HEAD));
	if (u16CalCrc != fileHead.u16CalCrc)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "Fail crc %d fail cal %d", fileHead.u16CalCrc, u16CalCrc);
		sRet = SP_E_PHONE_INVALID_CRC;
		goto __END;
	}

	//partitionsize
	UINT32 u32FixnvSize = 0;
	UINT32 u32MiscnvSize = 0;
	UINT32 u32ProdnvSize = 0;
	UINT16 u16nvFileCount = 0;
	sRet = QueryPartitionSizeV2(&u32FixnvSize, &u32MiscnvSize, &u32ProdnvSize, &u16nvFileCount);
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "QueryPartitionSize");

	while (u32offset < dwNvSize)
	{
		SEGMENT_HEAD_STRUCT segmentHead = *((SEGMENT_HEAD_STRUCT*)&pu8NvBuf[u32offset]);//segmentHead
		LogFmtStrA(SPLOGLV_INFO, "%s file offset = 0x%08X", segmentHead.szFileName, u32offset);
		u32offset += sizeof(SEGMENT_HEAD_STRUCT);
		if (!strcmp(segmentHead.szFileName, GetPartionName(3)))
		{
			u32FixnvSize += 4/*uint32 nv_count*/;
			sRet = WriteCustNvFile(m_bCalinv, &pu8NvBuf[u32offset], segmentHead.size);
			if (SP_OK != sRet)
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "WriteCustNvFile WinErr = %d", sRet);
				goto __END;
			}
			u32offset += (segmentHead.size);
		}
		else if (!strcmp(segmentHead.szFileName, GetPartionName(1)))
		{
			if (segmentHead.size != u32MiscnvSize)
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "File miscnvSize %d != Phone miscnvSize %d", segmentHead.size, u32MiscnvSize);
				sRet = SP_E_INVALID_PARAMETER;
				goto __END;
			}

			sRet = WriteCustMiscData(m_bMiscdata, &pu8NvBuf[u32offset], segmentHead.size);
			if (SP_OK != sRet)
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "WriteCustMiscData WinErr = %d", sRet);
				goto __END;
			}
			u32offset += segmentHead.size;
		}
		else if (!strcmp(segmentHead.szFileName, GetPartionName(2)))
		{
			if (segmentHead.size != u32ProdnvSize)
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "File prodnvSize %d != Phone prodnvSize %d", segmentHead.size, u32ProdnvSize);
				sRet = SP_E_INVALID_PARAMETER;
				goto __END;
			}

			sRet = WriteCustProdNv(m_bProdnv, &pu8NvBuf[u32offset], segmentHead.size);
			if (SP_OK != sRet)
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "WriteCustProdNv WinErr = %d", sRet);
				goto __END;
			}
			u32offset += segmentHead.size;
		}
	}

	sRet = SP_OK;
	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);

__END:
	free(pu8NvBuf);
	pu8NvBuf = NULL;
	return sRet;
}

SPRESULT CWriteBackupNvV2::WriteCustNvFile(bool bWrite, uint8 *pBuff, uint32 u32BytesToWrite)
{
	if (!bWrite)
	{
		return SP_OK;
	}

	SPRESULT sRet = SP_OK;
	char szLogNvIdList[MAX_DIAG_BUFF_SIZE] = {0};

	//Remove Send ReadBackupNvList Diag Command to DUT. Bug.2700010 Runde.Qiu 2024/6/21 
	/*uint8 byteNvListBuff[MAX_DIAG_BUFF_SIZE] = {0};
	uint32 lpu32NvListLen = MAX_DIAG_BUFF_SIZE;
	sRet = SP_ReadBackupNvList(m_hDUT, byteNvListBuff, &lpu32NvListLen);
	if (SP_OK != sRet)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"ReadBackupNvList,ErrCode:%d", sRet);
		return sRet;
	}*/

	uint32 u32RealCount = 0;
	uint32 u32offset = 4/*uint32 nv_count*/ + 4/*timestamp*/;
	while (u32offset < u32BytesToWrite && *((uint16*)&pBuff[u32offset])!=65535)
	{
		uint16 u16NvID = *((uint16*)&pBuff[u32offset]);
		u32offset += 2;
		uint16 u16BytesToWrite = *((uint16*)&pBuff[u32offset]);
		u16BytesToWrite = ((u16BytesToWrite + 3) / 4) * 4;//align to 4 byte
		u32offset += 2;

		uint16 lpData[MAX_DIAG_BUFF_SIZE] = {0};
		uint32 lpu32RecvLen = 0;
		sRet = SP_ReadNV(m_hDUT, u16NvID, lpData, MAX_DIAG_BUFF_SIZE, &lpu32RecvLen);
		uint32 u32RecvLen4align = ((lpu32RecvLen + 3) / 4) * 4;//align to 4 byte
		if (SP_OK != sRet || u32RecvLen4align != u16BytesToWrite) 
		{
			LogFmtStrW(SPLOGLV_ERROR, L"SP_ReadNV, ErrCode:%d", sRet);
			return sRet;
		}

		sRet = SP_WriteNV(m_hDUT, u16NvID, pBuff + u32offset, lpu32RecvLen/*(uint32)u16BytesToWrite*/);//刘颖 (Ying Liu/9254),要求实际长度2020/10/30 (周五) 15:33
		if (SP_OK != sRet)
		{
			LogFmtStrW(SPLOGLV_ERROR, L"SP_WriteNV, ErrCode:%d", sRet);
			return sRet;
		}
		sprintf_s(szLogNvIdList + strlen(szLogNvIdList), 3 * sizeof(uint16)/*NVID*/ + 1/*,*/, "%d,", u16NvID);
		u32RealCount ++;
		u32offset += u16BytesToWrite;
	}

	if (m_bClearNvVerFlag)
	{
		sRet = SP_ClearNvVerFlag(m_hDUT);//Bug1506419
		if (SP_OK != sRet)
		{
			LogFmtStrW(SPLOGLV_ERROR, L"SP_ClearNvVerFlag, ErrCode:%d", sRet);
			return sRet;
		}
	}


	return SP_OK;
}
