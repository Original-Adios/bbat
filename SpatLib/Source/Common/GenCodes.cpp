#include "StdAfx.h"
#include "GenCodes.h"
#include "SpatBase.h"
#include "ActionApp.h"
#include "Utility.h"
#include <assert.h>
#include "CLocks.h"
#include "Utility.h"

#define   GENCODES_PREFIX_KEY		_T("PreFix")
#define   GENCODES_START_KEY		_T("Start")
#define   GENCODES_END_KEY			_T("End")
#define   GENCODES_CURR_KEY			_T("Curr")
#define   GENCODES_COMPLETE_KEY		_T("Complete")
#define   GENCODES_PREALLOC_KEY		_T("PreAlloc")
#define   GENCODES_MAXLEN_KEY		_T("MaxLen")

#define   MAX_TOKEN_STR_LENGTH      (8*1024)

extern CActionApp myApp;

//////////////////////////////////////////////////////////////////////////
CGenCodes::CGenCodes(void)
{
	m_Imp = NULL;
	ZeroMemory(m_szAPPName, sizeof(m_szAPPName));
}

CGenCodes::~CGenCodes(void) 
{

}

//
SPRESULT CGenCodes::PreAllocCodes( INPUT_CODES_T* pCodes, int nNum /*= 1*/,  BOOL bHex /*= FALSE*/ )
{
	CLocks locks(myApp.GetLock());
	if(NULL == pCodes || 0 == nNum || NULL == m_Imp)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	TCHAR szTokenBuf[MAX_TOKEN_STR_LENGTH] = {0};
	std::list<unsigned __int64>::iterator itComp;
	std::list<PRE_ALLOC_T>::iterator itPre;
	GEN_SECTION_CODES_T stCodes;
	LoadFile(stCodes);
	int nTaskID = m_Imp->GetAdjParam().nTASK_ID;
	for(int i=0; i<nNum; i++)
	{
		unsigned __int64 nSectionId = stCodes.nSectionCurr;
		if(0 != i)
		{
			nSectionId += 1;
		}
		BOOL bInComplete = FALSE;
		BOOL bInPreAlloc = FALSE;
		do 
		{
			//查询分配的ID是否已在Complete里
			bInComplete = FALSE;
			for(itComp=stCodes.SectionComplete.begin(); itComp!=stCodes.SectionComplete.end(); itComp++)
			{
				if(nSectionId == *itComp)
				{
					bInComplete = TRUE;
					break;
				}
			}
			if(!bInComplete)
			{
				//查询分配的ID是否已在PreAlloc里
				bInPreAlloc = FALSE;
				for(itPre=stCodes.SectionPreAlloc.begin(); itPre!=stCodes.SectionPreAlloc.end(); itPre++)
				{
					if(nSectionId == itPre->nSectionID)
					{
						bInPreAlloc = TRUE;
						break;
					}
				}
			}
			if(bInComplete || bInPreAlloc)
			{
				nSectionId ++;
			}
			if(nSectionId > stCodes.nSectionEnd)
			{
				return SP_E_SPAT_CODES_ALLOC_FULL;
			}
		} while (bInComplete || bInPreAlloc);

		PRE_ALLOC_T stPreAlloc;
		stPreAlloc.nTaskID = nTaskID;
		stPreAlloc.nSectionID = nSectionId;
		stCodes.SectionPreAlloc.push_back(stPreAlloc) ;

		pCodes[i].nSectionID = nSectionId;
		char strId[BARCODEMAXLENGTH + 1];
		char strFormat[16];
		int nIdLen = stCodes.nMaxLength - strlen(stCodes.szPrefix);
		sprintf_s(strFormat, "%%0%dI64%c", nIdLen, bHex? 'X':'u');
		sprintf_s(strId, strFormat, nSectionId);

		int nCodeLen = strlen(stCodes.szPrefix) + strlen(strId);
		if( nCodeLen > stCodes.nMaxLength )
		{
			m_Imp->LogFmtStrA(SPLOGLV_ERROR, "GenCode length %d > MaxLength %d", nCodeLen, stCodes.nMaxLength);
			return SP_E_SPAT_INVALID_PARAMETER;
		}

		strcpy_s(pCodes[i].szCode, stCodes.szPrefix);
		strcat_s(pCodes[i].szCode, strId);
	}
	szTokenBuf[0] = 0;
	for(itPre=stCodes.SectionPreAlloc.begin(); itPre!=stCodes.SectionPreAlloc.end(); itPre++)
	{
		TCHAR szTemp[128] = {0};
		_stprintf_s(szTemp, _countof(szTemp), _T("%d,"), itPre->nTaskID);
		_tcscat_s(szTokenBuf, _countof(szTokenBuf), szTemp);
		_stprintf_s(szTemp, _countof(szTemp), _T("%I64u,"), itPre->nSectionID);
		_tcscat_s(szTokenBuf, _countof(szTokenBuf), szTemp);
	}
	int nLen = _tcslen(szTokenBuf);
	if(0 != nLen)
	{
		szTokenBuf[nLen-1] = 0;
	}
	m_FileConfig.SaveValue(m_szAPPName, GENCODES_PREALLOC_KEY, szTokenBuf);
	

	return SP_OK;
}

SPRESULT CGenCodes::CompleteCodes( INPUT_CODES_T* pCodes, int nNum /*= 1*/ )
{
	CLocks locks(myApp.GetLock());
	if(NULL == pCodes || 0 == nNum || NULL == m_Imp)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	std::list<unsigned __int64>::iterator itComp;
	std::list<PRE_ALLOC_T>::iterator itPre;
	GEN_SECTION_CODES_T stCodes;
	LoadFile(stCodes);
	TCHAR szTokenBuf[MAX_TOKEN_STR_LENGTH] = {0};
	for(int i=0; i<nNum; i++)
	{
		unsigned __int64 nSectionId = pCodes[i].nSectionID;
		BOOL bPreAlloc = FALSE;
		for(itPre=stCodes.SectionPreAlloc.begin(); itPre!=stCodes.SectionPreAlloc.end();)
		{
			if(nSectionId == itPre->nSectionID)
			{
				std::list<PRE_ALLOC_T>::iterator it;
				it = itPre;
				itPre++;
				stCodes.SectionPreAlloc.erase(it);
				bPreAlloc = TRUE;
			}
			else
			{
				itPre++;
			}
		}
		if(!bPreAlloc)
		{
			return SP_E_SPAT_CODES_PRE_FIND;
		}
		stCodes.SectionComplete.push_back(nSectionId) ;
	}
	szTokenBuf[0] = 0;
	for(itPre=stCodes.SectionPreAlloc.begin(); itPre!=stCodes.SectionPreAlloc.end();itPre++)
	{
		TCHAR szTemp[128] = {0};
		_stprintf_s(szTemp, _countof(szTemp), _T("%d,"), itPre->nTaskID);
		_tcscat_s(szTokenBuf, _countof(szTokenBuf), szTemp);
		_stprintf_s(szTemp, _countof(szTemp), _T("%I64u,"), itPre->nSectionID);
		_tcscat_s(szTokenBuf, _countof(szTokenBuf), szTemp);
	}
	int nLen = _tcslen(szTokenBuf);
	if(0 != nLen)
	{
		szTokenBuf[nLen-1] = 0;
	}
	m_FileConfig.SaveValue(m_szAPPName, GENCODES_PREALLOC_KEY, szTokenBuf);


	//clean complete
	unsigned __int64 nCurrID = stCodes.nSectionCurr;
	
	for(itComp=stCodes.SectionComplete.begin(); itComp!=stCodes.SectionComplete.end();)
	{
		if(nCurrID == *itComp)
		{
			stCodes.SectionComplete.erase(itComp);
			itComp = stCodes.SectionComplete.begin();
			nCurrID++;
			stCodes.nSectionCurr = nCurrID;
		}
		else
		{
			itComp++;
		}
	}

	_stprintf_s(szTokenBuf, _countof(szTokenBuf), _T("%I64u"), stCodes.nSectionCurr);
	m_FileConfig.SaveValue(m_szAPPName, GENCODES_CURR_KEY, szTokenBuf);

	szTokenBuf[0] = 0;
	for(itComp=stCodes.SectionComplete.begin(); itComp!=stCodes.SectionComplete.end(); itComp++)
	{
		TCHAR szTemp[32] = {0};
		_stprintf_s(szTemp, _countof(szTemp), _T("%I64u,"), *itComp);
		_tcscat_s(szTokenBuf, MAX_TOKEN_STR_LENGTH, szTemp);
	}
	nLen = _tcslen(szTokenBuf);
	if(0 != nLen)
	{
		szTokenBuf[nLen-1] = 0;
	}
	m_FileConfig.SaveValue(m_szAPPName, GENCODES_COMPLETE_KEY, szTokenBuf);
	
	return SP_OK;
}

SPRESULT CGenCodes::Init(LPCTSTR pAppName, CSpatBase* pParent, GEN_SECTION_CODES_T *pData)
{
	CLocks locks(myApp.GetLock());
	if (NULL == pParent || NULL == pAppName)
	{
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	m_Imp = pParent;
	_tcscpy_s(m_szAPPName, pAppName);
	m_FileConfig.InitConfigFile(m_Imp, TRUE);
	GEN_SECTION_CODES_T stCodes;
	LoadFile(stCodes);
	//run input code
	if(NULL != pData)
	{
        /// 去掉=判断，允许输入前缀的长度等于最大长度，用于自动化测试模拟人工输入
        /// 
		if(strlen(pData->szPrefix) > pData->nMaxLength)
		{
			m_Imp->LogFmtStrA(SPLOGLV_ERROR, "Prefix length %s  >= Code Max Length %d", pData->szPrefix, pData->nMaxLength);
			return SP_E_SPAT_INVALID_PARAMETER;
		}
		if(0 != strcmp(stCodes.szPrefix, pData->szPrefix) 
			|| stCodes.nSectionStart != pData->nSectionStart  
			|| stCodes.nSectionEnd != pData->nSectionEnd
			|| stCodes.nMaxLength != pData->nMaxLength)
		{
			CUtility Utility;
			TCHAR szTokenBuf[MAX_TOKEN_STR_LENGTH] = {0};
		
			LPCTSTR lpString = Utility._A2CT(pData->szPrefix);
			m_FileConfig.SaveValue(m_szAPPName, GENCODES_PREFIX_KEY, lpString);

			_stprintf_s(szTokenBuf, _countof(szTokenBuf), _T("%u"), pData->nMaxLength);
			m_FileConfig.SaveValue(m_szAPPName, GENCODES_MAXLEN_KEY, szTokenBuf);

			_stprintf_s(szTokenBuf, _countof(szTokenBuf), _T("%I64u"), pData->nSectionStart);
			m_FileConfig.SaveValue(m_szAPPName, GENCODES_START_KEY, szTokenBuf);
			//curr = start
			m_FileConfig.SaveValue(m_szAPPName, GENCODES_CURR_KEY, szTokenBuf);

			_stprintf_s(szTokenBuf, _countof(szTokenBuf), _T("%I64u"), pData->nSectionEnd);
			m_FileConfig.SaveValue(m_szAPPName, GENCODES_END_KEY, szTokenBuf);
			//Clear Complete
			m_FileConfig.SaveValue(m_szAPPName, GENCODES_COMPLETE_KEY, _T(""));
		}
		//Clear preAlloc
		m_FileConfig.SaveValue(m_szAPPName, GENCODES_PREALLOC_KEY, _T(""));
		
	}
	return SP_OK;
}

void CGenCodes::LoadFile(GEN_SECTION_CODES_T &stCodes)
{
	CUtility Utility;
	TCHAR szTokenBuf[MAX_TOKEN_STR_LENGTH] = {0};
	m_FileConfig.LoadValue(m_szAPPName, GENCODES_PREFIX_KEY,  _T(""), szTokenBuf, _countof(szTokenBuf));
	LPCSTR lpVal = Utility._T2CA(szTokenBuf);
	strcpy_s(stCodes.szPrefix, lpVal);

	stCodes.nMaxLength = (UINT8)m_FileConfig.LoadValue(m_szAPPName, GENCODES_MAXLEN_KEY, 0);

	m_FileConfig.LoadValue(m_szAPPName, GENCODES_START_KEY,  _T(""), szTokenBuf, _countof(szTokenBuf));
	stCodes.nSectionStart = _tcstoui64(szTokenBuf, NULL, 10);

	m_FileConfig.LoadValue(m_szAPPName, GENCODES_END_KEY,  _T(""), szTokenBuf, _countof(szTokenBuf));
	stCodes.nSectionEnd = _tcstoui64(szTokenBuf, NULL, 10);

	m_FileConfig.LoadValue(m_szAPPName, GENCODES_CURR_KEY,  _T(""), szTokenBuf, _countof(szTokenBuf));
	stCodes.nSectionCurr = _tcstoui64(szTokenBuf, NULL, 10);

	m_FileConfig.LoadValue(m_szAPPName, GENCODES_COMPLETE_KEY,  _T(""), szTokenBuf, _countof(szTokenBuf));

	INT nCount = 0;
	LPWSTR* lpGroup = Utility.GetTokenString(szTokenBuf, DEFAULT_DELIMITER_W, nCount);
	stCodes.SectionComplete.clear();
	for(int i=0; i<nCount; i++)
	{
		lpVal = Utility._T2CA(lpGroup[i]);
		stCodes.SectionComplete.push_back(_strtoui64(lpVal, NULL, 10)) ;
	}
	
	//Load preAlloc
	m_FileConfig.LoadValue(m_szAPPName, GENCODES_PREALLOC_KEY,  _T(""), szTokenBuf, _countof(szTokenBuf));
	nCount = 0;
	lpGroup = Utility.GetTokenString(szTokenBuf, DEFAULT_DELIMITER_W, nCount);
	stCodes.SectionPreAlloc.clear();
	for(int i=0; i<nCount/2; i++)
	{
		PRE_ALLOC_T stPreAlloc;
		stPreAlloc.nTaskID = _tcstol(lpGroup[2*i], NULL, 10);
		stPreAlloc.nSectionID = _tcstoui64(lpGroup[2*i+1], NULL, 10);
		stCodes.SectionPreAlloc.push_back(stPreAlloc);
	}
}


//清理当前Task所有的PreAlloc
void CGenCodes::ClearPreAllocForTask()
{
	CLocks locks(myApp.GetLock());
	std::list<PRE_ALLOC_T>::iterator itPre;
	GEN_SECTION_CODES_T stCodes;
	LoadFile(stCodes);
	INT nTaskID = m_Imp->GetAdjParam().nTASK_ID;
	TCHAR szTokenBuf[MAX_TOKEN_STR_LENGTH] = {0};
	for(itPre=stCodes.SectionPreAlloc.begin(); itPre!=stCodes.SectionPreAlloc.end();itPre++)
	{
		if(nTaskID != itPre->nTaskID)
		{
			TCHAR szTemp[128] = {0};
			_stprintf_s(szTemp, _countof(szTemp), L"%d,", itPre->nTaskID);
			_tcscat_s(szTokenBuf, MAX_TOKEN_STR_LENGTH, szTemp);
			_stprintf_s(szTemp, _countof(szTemp), L"%I64u,", itPre->nSectionID);
			_tcscat_s(szTokenBuf, MAX_TOKEN_STR_LENGTH, szTemp);
		}
	}
	int nLen = _tcslen(szTokenBuf);
	if(0 != nLen)
	{
		szTokenBuf[nLen-1] = 0;
	}
	m_FileConfig.SaveValue(m_szAPPName, GENCODES_PREALLOC_KEY, szTokenBuf);
	//m_Imp->LogFmtStrW(SPLOGLV_INFO, L"%s:%s = %s", m_szAPPName,  GENCODES_PREALLOC_KEY, m_pstrTokenBuf);
}
