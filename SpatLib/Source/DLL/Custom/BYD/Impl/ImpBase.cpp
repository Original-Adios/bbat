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
	SPRESULT res = GetShareMemory(ShareMemory_BydDriver, (void* )&m_pMesDrv, sizeof(m_pMesDrv));
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
	return SetShareMemory(ShareMemory_BydDriver, (const void* )&m_pMesDrv, sizeof(m_pMesDrv),IContainer::System);
}

SPRESULT CImpBase::MES_GetSN(char*pszSN,int iSize)
{
	SPRESULT sp_result = SP_OK;
	char szSN[ShareMemory_SN_SIZE] = {0};
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	sp_result = GetShareMemory(ShareMemory_SN1, (void* )szSN, sizeof(szSN));
	if (SP_OK != sp_result || 0 == strlen(szSN))
	{
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
			return sp_result;
		}
		if (0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
		strcpy_s(szSN,sizeof(szSN),m_InputSN[BC_SN1].szCode);
	}

	strcpy_s(pszSN,iSize,szSN);
	return SP_OK;
}
BOOL CImpBase::GetInfoFromCustomData(char*pszCustomData,char*pszName,char*pszValue,UINT uSize)
{
	enum
	{
		_name_ =0,
		_value_,
		_description_
	};

	const string szFname("|");
	const string szFname1("กิ");

	string strCustomData(pszCustomData);
	string::size_type posS = strCustomData.find(szFname);

	while(posS > 0 && strCustomData.length() > 0)
	{	
		string strInfo = strCustomData.substr(0, posS);

		if (posS>strCustomData.length())
		{
			strCustomData = "";
		}
		else
		{
			strCustomData= strCustomData.substr(posS+szFname.length(), strCustomData.length()-posS-szFname.length());
		}

		vector<string> v_info;
		posS = strInfo.find(szFname1);
		while(posS > 0 && strInfo.length() > 0)
		{	
			v_info.push_back(strInfo.substr(0, posS));
			if (posS > strInfo.length())
			{
				strInfo = "";
			}
			else
			{
				strInfo= strInfo.substr(posS+szFname1.length(), strInfo.length()-posS-szFname1.length());
			}	
			posS = strInfo.find(szFname1);
		}

		if (v_info[_name_].compare(pszName)==0)
		{
			if (v_info[_value_].length()<1)
			{
				return FALSE;
			}
			strncpy_s(pszValue,uSize,v_info[_value_].data(),v_info[_value_].length());
			break;
		}
		posS = strCustomData.find(szFname);
	}

	if (strlen(pszValue)<1)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CImpBase::SetSfcKeyCollectData(char*pszNumbae,char*pszData,UINT uSize)
{
	const string szFname(";");

	string strNumber(pszNumbae);
	string::size_type posS = strNumber.find(szFname);

	while(posS > 0 && strNumber.length() > 0)
	{	
		string strInfo = strNumber.substr(0, posS);
		strcat_s(pszData,uSize,strInfo.data());
		strcat_s(pszData,uSize,":1");

		if (posS>strNumber.length())
		{
			strNumber = "";
		}
		else
		{
			strcat_s(pszData,uSize,";");
			strNumber= strNumber.substr(posS+szFname.length(), strNumber.length()-posS-szFname.length());
		}
		posS = strNumber.find(szFname);
	}
	return TRUE;
}