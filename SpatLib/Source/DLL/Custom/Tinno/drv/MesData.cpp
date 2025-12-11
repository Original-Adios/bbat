#include "StdAfx.h"
#include "MesData.h"
#include "callback_def.h"
#include "ISpLogExport.h"
#include <algorithm>
#include <regex>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////////
CMesData::CMesData()
   :m_bStopUpload(FALSE) // Should be FALSE to enable push data,
   ,m_pFileData(NULL)
   ,m_iUploadLevel(0)
{
	ZeroMemory(m_szInitFileName,sizeof(m_szInitFileName));
	ZeroMemory(m_szNewFileName,sizeof(m_szNewFileName));
}

CMesData::~CMesData(void)
{
	ZeroMemory(m_szInitFileName,sizeof(m_szInitFileName));
	ZeroMemory(m_szNewFileName,sizeof(m_szNewFileName));
}

std::string& CMesData::replace_all(std::string& str, const std::string& old_value, const std::string& new_value)   
{   
	for (; ;)  
	{   
		std::string::size_type pos(0);   
		if ((pos = str.find(old_value)) != std::string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else 
		{
			break;
		}
	}   

	return str;   
}   

void CMesData::Clear(void)
{
    m_LockQ.clear();
}

void CMesData::PushData(CONST ITEMDATA_T& item)
{
    if (!m_bStopUpload && IS_BIT_SET(item.nLv, LEVEL_REPORT))
    {
        m_LockQ.push_back(item);
    }
}

void CMesData::run(void)
{
	std::deque<ITEMDATA_T> dqe;
	while (m_LockQ.size() > 0)   
	{ 	
		INT nCount = m_LockQ.pop_all(dqe, INFINITE);

		if (NULL != m_pFileData)
		{
			char szLogItem[1024]={0};
			char szLogData[1024]={0};

			for (INT nIndex=0;nIndex<nCount;nIndex++)
			{
				int iLogLevel = (((dqe[nIndex].nLv)<<(LEVEL_MAX_BITS-LEVEL_HOST_OFFSET)) >> (LEVEL_MAX_BITS-LEVEL_HOST_OFFSET));
				if ((m_iUploadLevel&iLogLevel)==iLogLevel || 9999 == m_iUploadLevel)
				{
					if (strstr(dqe[nIndex].szActionName,"GSM FT") != NULL || strstr(dqe[nIndex].szActionName,"GSM ANT") != NULL)
					{
						char szStr[64] = {0};
						char szStr_Seps[64] = {0};
						char seps[] = ";";
						char *pToken = NULL;
						char *pSource = dqe[nIndex].szCond;
						pToken = strstr(pSource, seps);
						if (NULL != pToken)
						{
							_snprintf_s(szStr, pToken -pSource, "%s", pSource);
							_snprintf_s(szStr_Seps, strlen(pToken), "%s_", pToken+2);
						}
						else
						{
							_snprintf_s(szStr, sizeof(szStr), "%s", pSource);
						}

						sprintf_s(szLogData,sizeof(szLogData),"\n[%s^CHAN=%d^%s]\n",dqe[nIndex].szBand,dqe[nIndex].nChannel,szStr/*dqe[nIndex].szCond*/);
						if (strcmp(szLogData, szLogItem))
						{
							fputs(szLogData, m_pFileData);
							strcpy_s(szLogItem, szLogData);
						}

						sprintf_s(szLogData,sizeof(szLogData),"%s%s=%s,%.2f,%0.2f,%.2f\n",szStr_Seps,dqe[nIndex].szItemName,
							(dqe[nIndex].dValue<dqe[nIndex].dLower || dqe[nIndex].dValue>dqe[nIndex].dUpper)?"FAIL":"PASS",
							dqe[nIndex].dValue,
							dqe[nIndex].dLower,
							dqe[nIndex].dUpper);
						fputs(szLogData,m_pFileData);
					}
					else if (strstr(dqe[nIndex].szActionName,"WCDMA FT") != NULL || strstr(dqe[nIndex].szActionName,"WCDMA ANT") != NULL)
					{
						sprintf_s(szLogData,sizeof(szLogData),"\n[WCDMA_%s^CHAN=%d^]\n",dqe[nIndex].szBand,dqe[nIndex].nChannel);
						if (strcmp(szLogData, szLogItem))
						{
							fputs(szLogData, m_pFileData);
							strcpy_s(szLogItem, szLogData);
						}

						sprintf_s(szLogData,sizeof(szLogData),"%s=%s,%.2f,%0.2f,%0.2f\n",dqe[nIndex].szItemName,
							(dqe[nIndex].dValue<dqe[nIndex].dLower || dqe[nIndex].dValue>dqe[nIndex].dUpper)?"FAIL":"PASS",
							dqe[nIndex].dValue,
							dqe[nIndex].dLower,
							dqe[nIndex].dUpper);
						fputs(szLogData,m_pFileData);
					}
					else if (strstr(dqe[nIndex].szActionName,"LTE LMT") != NULL || strstr(dqe[nIndex].szActionName,"LTE ANT") != NULL)
					{
						std::string strCond = dqe[nIndex].szCond;
						replace_all(strCond, "; ", "_");
						sprintf_s(szLogData,sizeof(szLogData),"\n[LTE_%s^CHAN=%d^%s]\n",dqe[nIndex].szBand,dqe[nIndex].nChannel,strCond.c_str());
						if (strcmp(szLogData, szLogItem))
						{
							fputs(szLogData, m_pFileData);
							strcpy_s(szLogItem, szLogData);
						}

						sprintf_s(szLogData,sizeof(szLogData),"%s=%s,%.2f,%0.2f,%0.2f\n",dqe[nIndex].szItemName,
							(dqe[nIndex].dValue<dqe[nIndex].dLower || dqe[nIndex].dValue>dqe[nIndex].dUpper)?"FAIL":"PASS",
							dqe[nIndex].dValue,
							dqe[nIndex].dLower,
							dqe[nIndex].dUpper);
						fputs(szLogData,m_pFileData);
					}
				}
			}
			fclose(m_pFileData);
			m_pFileData = NULL;
		}
	}   
	m_LockQ.clear();
}

BOOL CMesData::StartUpload(BOOL bStart)
{
    if (bStart)
    {
        m_bStopUpload = FALSE;
        return CThread::start();
    }
    else
    {
        m_bStopUpload = TRUE;
        m_LockQ.wakeup();
        CThread::join(INFINITE);
        return TRUE;
    }
}

BOOL CMesData::MES_Log_Path_Init(char*pszDirectory,char*pszNote)
{
	if(!Creat_Dir(pszDirectory))     
	{       
		return FALSE;              
	}

	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	char szTime[100]={0};
	sprintf_s(szTime,sizeof(szTime),"%04d%02d%02d%02d%02d%02d%03d",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond,sTime.wMilliseconds);
	sprintf_s(m_szInitFileName,sizeof(m_szInitFileName),"%s\\PrintDataLog_%s_%s.txt",pszDirectory,pszNote,szTime);
	return TRUE;
}

BOOL CMesData::MES_Log_Path_New(int iResult,char*pszSN,char*pszDirectory,char*pszNote)
{
	if (NULL != m_pFileData)
	{
		fclose(m_pFileData);
		m_pFileData = NULL;
	}
	if (NULL == m_szNewFileName[0])
	{
		string strPath(pszDirectory);

		if(!Creat_Dir(pszDirectory))     
		{       
			return FALSE;              
		}

		SYSTEMTIME sTime;
		GetLocalTime(&sTime);

		char szTime[100]={0};
		char szNewName[100]={0};

		sprintf_s(szTime,sizeof(szTime),"%04d%02d%02d%02d%02d%02d",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
		sprintf_s(szNewName,sizeof(szNewName),"%s_%s_%s_%s.txt",pszNote,pszSN,szTime,iResult==1?"PASS":"FAIL");
		sprintf_s(m_szNewFileName,sizeof(m_szNewFileName),"%s\\%s",strPath.data(),szNewName);
	}
	return TRUE;
}
BOOL CMesData::MES_Log_Get_New_Name(char*pszName,int iSize)
{
	if (NULL==m_szNewFileName[0])
	{
		return FALSE;
	}
	strcpy_s(pszName,iSize,m_szNewFileName);
	return TRUE;
}

BOOL CMesData::MES_Log_Path_Rename()
{
	if (NULL==m_szNewFileName[0] || NULL==m_szInitFileName[0])
	{
		return FALSE;
	}

	if (!PathFileExistsA(m_szInitFileName))
	{
		return FALSE;
	}

	CopyFileA(m_szInitFileName,m_szNewFileName,TRUE);
	DeleteFileA(m_szInitFileName);

	ZeroMemory(m_szInitFileName,sizeof(m_szInitFileName));
	ZeroMemory(m_szNewFileName,sizeof(m_szNewFileName));
	
	return TRUE;
}

BOOL CMesData::MES_Log_Head(_pMES_LOG_HEAD p_data)
{
	if (!File_Open())
	{
		return FALSE;
	}

	m_iUploadLevel = p_data->iUploadLevel;

	char szLogData[1024]={0};

	sprintf_s(szLogData,"[Test_Station]\n");
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TestName=%s\n",p_data->szTestName);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"OrderID=%s\n",p_data->szOrderID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"OperationID=%s\n",p_data->szOperationID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"WorkUnitID=%s\n",p_data->szWorkUnitID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"UserName=%s\n",p_data->szUserName);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"FixtureNO=%s\n",p_data->szFixtureNO);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"CableNO=%s\n",p_data->szCableNO);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"InstrumentNO=%s\n",p_data->szInstrumentNO);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"//=======================\n");
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"[LotInfo]\n");
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TestSN=%s\n",p_data->szSN);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TestStartTime=%s\n",p_data->szStartTime);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TestTotTime=%.2f\n",p_data->dElapsedTime);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TestResult=%s\n",p_data->iTestResult==1?"PASS":"FAIL");
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TestMsg=%s\n",p_data->szTestMsg);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Version=%s\n",p_data->szVersion);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Platform=%s\n",p_data->szPlatform);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"//=======================\n");
	fputs(szLogData,m_pFileData);

	return TRUE;
}
BOOL CMesData::Creat_Dir(char*pszDir)
{
	string strDir(pszDir),strPath="";
	const string strFname("\\");
	if(PathIsDirectoryA(strPath.data()))     
	{  
		return TRUE;
	}
#pragma warning(disable:4127)
	while(TRUE)
#pragma warning(default:4127)
	{
		string::size_type posS = strDir.find(strFname);
		if (std::string::npos != posS)
		{
			strPath += strDir.substr(0, posS);
			strPath += strFname;
			strDir = strDir.substr(posS + strFname.length(),strDir.length()-(posS + strFname.length()));  
			if(!PathIsDirectoryA(strPath.data()))     
			{   
				if (!CreateDirectoryA(strPath.data(),NULL))
				{
					return FALSE;
				}		
			} 
		}
		else
		{
			strPath +=  strDir;
			if(!PathIsDirectoryA(strPath.data()))     
			{   
				if (!CreateDirectoryA(strPath.data(),NULL))
				{
					return FALSE;
				}		
			} 
			break;
		}
	}
	return TRUE;
}

BOOL CMesData::File_Open()
{
	int iTry=0;
	for (iTry=0;iTry<5;iTry++)
	{
		m_pFileData = NULL;
		fopen_s(&m_pFileData,m_szInitFileName,"at+");
		if(NULL != m_pFileData)
		{
			break;
		}
	}
	if (5==iTry)
	{
		return FALSE;
	}
	fseek(m_pFileData,0L,2);
	return TRUE;
}