#pragma once
#include "IItemDataObserver.h"
#include "../../../../../../Common/thread/Thread.h"
#include "LockQueue.h"
#include <string>
typedef CLockQueue<IItemDataObserver::ITEMDATA_T> CDataLockQueue;

typedef struct _STRUCT_START_TIME
{
	double dStartTime;
	char szStartTime[64];
	char szEndTime[64];
	_STRUCT_START_TIME()
	{
		ZeroMemory(this, sizeof(*this));
	}
}_START_TIME,*_pSTART_TIME;

typedef struct _STRUCT_MES_LOG_HEAD
{
	int iTestResult;
	int iUploadLevel;
	double dElapsedTime;
	char szSN[64];
	char szTestName[64];
	char szOrderID[64];//MONO
	char szOperationID[64];//Note
	char szWorkUnitID[64];//PC Name
	char szUserName[64];//User
	char szFixtureNO[64];
	char szCableNO[64];
	char szInstrumentNO[256];
	char szStartTime[64];
	char szTestMsg[512];
	char szVersion[64];
	char szPlatform[64];
	_STRUCT_MES_LOG_HEAD()
	{
		ZeroMemory(this, sizeof(*this));
	}
}_MES_LOG_HEAD,*_pMES_LOG_HEAD;

//////////////////////////////////////////////////////////////////////////
class CMesData : public IItemDataObserver
              , public CThread
{
public:
    CMesData();
    virtual ~CMesData(void);

    BOOL StartUpload(BOOL bStart);

    // Interface of <IItemDataObserver>
    virtual void Release(void) { delete this; };
    virtual void Clear(void);
    virtual void PushData(CONST ITEMDATA_T& item);
	std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);
    // Interface of <CThread>
    virtual void run(void);

public:
	BOOL MES_Log_Path_Init(char*pszDirectory,char*pszNote);
	BOOL MES_Log_Path_New(int iResult,char*pszSN,char*pszDirectory,char*pszNote);
	BOOL MES_Log_Path_Rename();

	BOOL MES_Log_Head(_pMES_LOG_HEAD p_data);
	BOOL MES_Log_Get_New_Name(char*pszName,int iSize);

	BOOL  Creat_Dir(char*pszDir);

	BOOL	File_Open();

	char m_szInitFileName[512];
	char m_szNewFileName[512];

	FILE*m_pFileData;

	int m_iUploadLevel;
    CDataLockQueue m_LockQ;
    volatile BOOL  m_bStopUpload;  // Stop to upload
};
