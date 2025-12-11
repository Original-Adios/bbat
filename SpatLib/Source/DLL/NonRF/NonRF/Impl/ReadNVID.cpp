#include "stdafx.h"
#include "ReadNVID.h"
#include <time.h>
#include "ExtraLogFile.h"
#include <memory>

IMPLEMENT_RUNTIME_CLASS(CReadNVID)

CReadNVID::CReadNVID()
{
	m_nLength = 0;
	memset(m_arrID, 0, sizeof(m_arrID));
}

CReadNVID::~CReadNVID()
{
	
}

BOOL CReadNVID::LoadXMLConfig(void)
{
//	__super::LoadXMLConfig();

	LPCWSTR szNVIDs = GetConfigValue(L"Param:ID", L"0");

	INT* pArrID = GetTokenIntegerW(szNVIDs, DEFAULT_DELIMITER_W, m_nLength, 16);
	for (INT i = 0; i < m_nLength; i++)
	{
		m_arrID[i] = (uint16)pArrID[i];
	}

	return (0 != m_nLength);
}

SPRESULT CReadNVID::__PollAction(void)
{
	TCHAR szFileName[64] = { 0 };
	 
	__time32_t tt = __time32_t(NULL);
	_time32(&tt);
	
	const int NV_BUF_SIZE = 64 << 10;
	unique_ptr<BYTE[]> spData(new BYTE[NV_BUF_SIZE]);
	for (INT i = 0; i < m_nLength; i++)
	{
		uint16 usNVId = m_arrID[i];
		uint16 usDataLen = 0;
		memset(spData.get(), 0, NV_BUF_SIZE);
		
		uint32 nReadLen = 0;
		SPRESULT res = SP_ReadNV(m_hDUT, usNVId, (void*)spData.get(), NV_BUF_SIZE, &nReadLen);
		if (SP_OK != res || 0 == nReadLen)
		{
			continue;
		}
		nReadLen = ((nReadLen + 3) / 4) * 4;		// ËÄ×Ö½Ú¶ÔÆë
		usDataLen = static_cast<uint16>(nReadLen);

		_stprintf_s(szFileName, _T("DUT\\0x%X.bin"), usNVId);
		CExtraLog extApc;
		extApc.Open(this, szFileName);
		extApc.Write(&tt, sizeof(tt));
		extApc.Write(&usNVId, sizeof(usNVId));
		extApc.Write(&usDataLen, sizeof(usDataLen));
		extApc.Write(spData.get(), nReadLen);
		extApc.Close();
	}

	return SP_OK;
}
