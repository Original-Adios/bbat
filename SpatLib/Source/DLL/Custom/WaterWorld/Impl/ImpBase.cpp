#include "StdAfx.h"
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
	:m_pSql_Imp(NULL)
{
}

CImpBase::~CImpBase(void)
{
}


SPRESULT CImpBase::SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut /* = TIMEOUT_3S */)
{
	CHAR   szRevBuf[4096] = {0};
	uint32 u32RevSize = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, lpszCmd, TRUE, szRevBuf, sizeof(szRevBuf), &u32RevSize, u32TimeOut);
	if (SP_OK != res)
	{
		return res;
	}

	if (NULL != strstr(szRevBuf, "CME ERROR"))
	{
		return SP_E_PHONE_AT_EXECUTE_FAIL;
	}

	m_strRevBuf = szRevBuf;
	replace_all(m_strRevBuf, "\r", "");
	replace_all(m_strRevBuf, "\n", "");
	replace_all(m_strRevBuf, "OK", "");

	return SP_OK;
}

SPRESULT CImpBase::GetMesDriverFromShareMemory(void)
{
	SPRESULT res = GetShareMemory(ShareMemory_SQLDriver, (void* )&m_pSql_Imp, sizeof(m_pSql_Imp));
	if (SP_OK == res)
	{
		if (NULL == m_pSql_Imp)
		{
			res = SP_E_POINTER;
		}
	}
	return res;
}

SPRESULT CImpBase::SetMesDriverIntoShareMemory(void)
{
	return SetShareMemory(ShareMemory_SQLDriver, (const void* )&m_pSql_Imp, sizeof(m_pSql_Imp),IContainer::System);
}

BOOL CImpBase::WriteInputFile(const char*pszPath,std_que_str&input_info)
{
	FILE *pFileData = NULL;
	int n=0;
	do 
	{
		fopen_s(&pFileData,pszPath,"at+");
		n++;
	} while (pFileData==NULL && n<5);
	if (5==n)
	{
		return FALSE;
	}

	fseek(pFileData,0,SEEK_SET);

	while(input_info.size()>0)
	{
		string strData = input_info.front();
		input_info.pop();
		fputs(strData.data(),pFileData);
	}

	fclose(pFileData);
	return TRUE;
}

BOOL CImpBase::ReadInputFile(const char*pszPath,std_que_str&input_info)
{
	FILE *pFileData = NULL;
	char szData[1024]={0};

	int n=0;
	do 
	{
		fopen_s(&pFileData,pszPath,"at+");
		n++;
	} while (pFileData==NULL && n<5);
	if (5==n)
	{
		return FALSE;
	}

	if((fgets (szData,sizeof(szData), pFileData)==NULL))
	{
		fclose(pFileData);
		return FALSE;
	}

	do 
	{	
		string strData(szData);
		string::size_type pos(0);     
		if ((pos=strData.find("MAC_"))!=string::npos 
			|| (pos=strData.find("BT_MAC_"))!=string::npos 
			|| (pos=strData.find("SerNr_"))!=string::npos
			|| (pos=strData.find("IMEI1_"))!=string::npos
			|| (pos=strData.find("IMEI2_"))!=string::npos)
		{
			input_info.push(strData);
		}
	} while (fgets (szData,sizeof(szData), pFileData)!=NULL);
	fclose(pFileData);
	return TRUE;
}


