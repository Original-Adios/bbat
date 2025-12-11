#include "StdAfx.h"
#include "ImpBase.h"
#include <shlwapi.h>
#include <atltime.h>
#include <comutil.h>
#include <ObjBase.h>
#include <Nb30.h>
#include <winsock2.h> 
#include <winver.h>
#include <regex>
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib,"Version.lib")
//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}


SPRESULT CImpBase::GetMesDriverFromShareMemory(void)
{
	SPRESULT res = GetShareMemory(ShareMemory_MesDriver, (void* )&m_pMesDrv, sizeof(m_pMesDrv));
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
	return SetShareMemory(ShareMemory_MesDriver, (const void* )&m_pMesDrv, sizeof(m_pMesDrv),IContainer::System);
}

SPRESULT CImpBase::MES_GetSN(char*pszSN,int iSize)
{
	SPRESULT sp_result = SP_OK;
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	char szSN[64] = {0};
	//获取SN1（手机中读取的SN1）
	sp_result = GetShareMemory(ShareMemory_SN1, (void* )szSN, sizeof(szSN));
	if (SP_OK != sp_result || 0 == strlen(szSN))
	{
		//获取SN1（InputCodes扫描获取）
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
			return sp_result;
		}
		if (0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory(SN1 Length == 0)");
			NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
		strcpy_s(szSN,sizeof(szSN),m_InputSN[BC_SN1].szCode);
	}
	strcpy_s(pszSN,iSize,szSN);
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
	TCHAR szPath[512]={0};
	GetModuleFileName(NULL,szPath,sizeof(szPath));  
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


SPRESULT CImpBase::WriteCustMisdata(uint32 u32InBase, const void * pData, int length)
{
	if (!IN_RANGE(1, length, MAX_CUSTOMIZED_MISCDATA_SIZE))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid CustMisdata code, length = \'%d\'", length);
		return SP_E_INVALID_PARAMETER;
	}

	//写入miscdata
	CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, u32InBase, (uint8*)pData, length, TIMEOUT_3S), "SaveCustomerMiscData");

	Sleep(100);
	// 读取出来和写入的数据进行比较
	uint8 rbuff[MAX_CUSTOMIZED_MISCDATA_SIZE] = {0};
	CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, u32InBase, rbuff, length, TIMEOUT_3S), "LoadCustomerMiscData");

	if (0 != memcmp(pData, rbuff, length))
	{
		NOTIFY("VerifyCustomerMiscData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Verify after write fails");
		return SP_E_MISMATCHED_CU;
	}

	return SP_OK;
}