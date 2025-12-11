#include "StdAfx.h"
#include "ImpBase.h"



//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
	 : m_pMesDrv(NULL)
{
}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::GetMesDriverFromShareMemory(void)
{
	SPRESULT res = GetShareMemory(ShareMemory_TinnoDriver, (void* )&m_pMesDrv, sizeof(m_pMesDrv));
	if (SP_OK == res)
	{
		if (NULL == m_pMesDrv)
		{
			res = SP_E_POINTER;
		}
	}
	return res;
}

SPRESULT CImpBase::SetMesDriverIntoShareMemory(void)
{
	return SetShareMemory(ShareMemory_TinnoDriver, (const void* )&m_pMesDrv, sizeof(m_pMesDrv),IContainer::System);
}

SPRESULT CImpBase::Get_Host_PCName(char*pszName,int iSize)
{
	char szBuffer[512] = {0};
	_COMPUTER_NAME_FORMAT e_name_type = ComputerNameDnsHostname;
	DWORD dwSize = sizeof(szBuffer); 
	if (!GetComputerNameExA(e_name_type, szBuffer, &dwSize)) 
	{ 
		return SP_E_FAIL; 
	} 
	else
	{
		strcpy_s(pszName,iSize,szBuffer);
	}
	return SP_OK; 
}

SPRESULT CImpBase::Get_Tool_Info(char*pszToolName,char*pszToolVersion)
{
	CString strVersion;
	struct st_Language 
	{
		WORD wLanguageID;
		WORD wCodePage;
	};

	TCHAR szPath[MAX_PATH]={0};
	GetModuleFileName(NULL,szPath,MAX_PATH);  
	DWORD dwSize = GetFileVersionInfoSize(szPath,NULL);
	if (dwSize>0)
	{
		LPBYTE pBlock = (BYTE*)malloc(dwSize);
		if (GetFileVersionInfo(szPath,0,dwSize,pBlock))
		{
			WCHAR*pszValue = NULL;
			UINT uiLen1 = 0;
			VerQueryValue(pBlock,_T("VarFileInfo\\Translation"),(LPVOID*)&pszValue,&uiLen1);
			if (uiLen1>0)
			{
				st_Language stlang = *((st_Language*)pszValue);
				CString strSubBlock,strTranslation,strTemp; 
				strTemp.Format(_T("000%x"),stlang.wLanguageID); 
				strTranslation = strTemp.Right(4); 
				strTemp.Format(_T("000%x"),stlang.wCodePage); 
				strTranslation += strTemp.Right(4); 

				WCHAR*pszVerValue=NULL,*pszFileName=NULL;
				UINT uiLen2 = 0;

				strSubBlock.Format(_T("\\StringFileInfo\\%s\\OriginalFilename"),strTranslation);
				VerQueryValue(pBlock,strSubBlock,(LPVOID*)&pszFileName,&uiLen2);
				int iLen = WideCharToMultiByte(CP_ACP,0,pszFileName,-1,NULL,0,NULL,NULL);
				WideCharToMultiByte(CP_ACP,0,pszFileName,-1,pszToolName,iLen,NULL,NULL);
				strSubBlock.ReleaseBuffer();

				strSubBlock.Format(_T("\\StringFileInfo\\%s\\ProductVersion"),strTranslation);
				VerQueryValue(pBlock,strSubBlock,(LPVOID*)&pszVerValue,&uiLen2); 

				CString strVer;
				strVer.Format(_T("%s"),pszVerValue);
				strSubBlock.ReleaseBuffer();

				strVer.Remove(_T(' '));
				strVer.Replace( _T(","),_T(".") );	
				int iLastDot = strVer.ReverseFind(_T('.')); 
				CString strBuild = strVer.Right(strVer.GetLength() - iLastDot-1);		
				strVer = strVer.Left(iLastDot);

				strVer.Remove(_T(' '));
				strVer.Replace( _T(","),_T(".") );	
				iLastDot = strVer.ReverseFind(_T('.')); 
				CString strBuild1 = strVer.Right(strVer.GetLength() - iLastDot-1);		
				strVer = strVer.Left(iLastDot);

				int iBuild = _ttoi(strBuild1);
				strBuild1.Format(_T("%02d"),iBuild);
				strVer += ".";
				strVer += strBuild1;	

				iBuild = _ttoi(strBuild);
				strBuild.Format(_T("%02d"),iBuild);	
				strVer += strBuild;

				iLen = WideCharToMultiByte(CP_ACP,0,strVer,-1,NULL,0,NULL,NULL);
				WideCharToMultiByte(CP_ACP,0,strVer,-1,pszToolVersion,iLen,NULL,NULL);

				free(pBlock);
				return SP_OK;
			}
		}
		free(pBlock); 
		return SP_E_FAIL;
	}
	else
	{
		return SP_E_FAIL;
	}
}
void CImpBase::StringReplace(char*pszData)
{												   
	int iLen=strlen(pszData); 
	for(int i = 0;i < iLen ; i++)
	{
		if(pszData[i]=='/')
		{
			pszData[i]='_'; 
		}
	} 
}
int CImpBase::StringToArray(int iCount,char cTok,char*pszData, char*pszArray,int iStrLen)
{
	int iTemp=0;
	char szTemp[200]={0};
	char szData[200]={0};  
	char*psz_e=NULL;
	char*psz_b=pszData;
	int iDatelen =0;

	for (int i=0 ; i<iCount ;i++)
	{
		iDatelen = i + 1;
		psz_e = strchr (psz_b,cTok);
		if (psz_e !=NULL)
		{
			memset (szTemp, 0, sizeof(szTemp));
			iTemp=psz_e-psz_b;
			strncpy_s(szData,sizeof(szData), psz_b, iTemp);
			memcpy(pszArray,szData,iTemp);
			pszArray+=iStrLen;
			psz_b=psz_e+1;
		}
		else
		{
			memset (szTemp, 0, sizeof(szTemp));
			strncpy_s(szData,sizeof(szData),psz_b,strlen(psz_b));
			memcpy(pszArray,szData,strlen(psz_b));
			pszArray+=iStrLen;
			break;
		}
	}
	return iDatelen;
}