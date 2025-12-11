#pragma once
#include "SpatBase.h"
#include "../drv/MesDriver.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "fstream"
#include "regex"

#include "shellapi.h"
#include <shlwapi.h>
using namespace std;
#pragma comment(lib,"shlwapi.lib")


typedef struct _TAG_PROJ_INFOR
{
	CHAR m_szBatchName[64];
	CHAR m_szStationName[64];

	_TAG_PROJ_INFOR()
	{
		ZeroMemory(this, sizeof(*this));
	}
}PROJINFOR;

const uint16 MAX_BUFF_LENGTH = 1024;
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);


protected:
	SPRESULT GetMesDriverFromShareMemory(void);
	SPRESULT SetMesDriverIntoShareMemory(void);

	SPRESULT SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut = TIMEOUT_3S);
	SPRESULT Get_Tool_Info(LPSTR lpwToolName, LPSTR lpwToolVersion);
	SPRESULT MES_GetSN(char*pszSN,int iSize);

	BOOL GetHostIP(LPSTR lpIP);
	BOOL ParseMac(LPCSTR lpcFile, LPCSTR lpcIP, LPSTR lpMacOut);
	BOOL GetHostMAC(LPSTR lpMac, LPCSTR lpcIP);

	std::string m_strRevBuf;
	CHAR m_szToolName[MAX_BUFF_LENGTH];
	CHAR m_szToolVersion[MAX_BUFF_LENGTH];

	CHAR m_szMac[MAX_BUFF_LENGTH];
	CHAR m_szIp[MAX_BUFF_LENGTH];

	ISpLog* pSpLog;



protected:
	CMesDriver*m_pMesDrv;


	INPUT_CODES_T m_InputSN[BC_MAX_NUM];
};

///////////////////Create TestValue_Table.sql///////////////////////////////////////////////////////
/*
USE [DBTMTS]
GO

SET ANSI_NULLS ON
	GO

	SET QUOTED_IDENTIFIER ON
	GO

	SET ANSI_PADDING ON
	GO

	CREATE TABLE [dbo].[TestValue_Table](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[BatchName] [varchar](50) NOT NULL,
	[SN] [varchar](64) NOT NULL,
	[Item] [varchar](64) NULL,
	[Result] [varchar](16) NOT NULL,
	[Station] [varchar](64) NULL,
	[TestValue] [varchar](32) NULL,
	[LowLimit] [varchar](32) NULL,
	[UpLimit] [varchar](32) NULL,
	[Unit] [varchar](16) NULL,
	[UploadTime] [datetime] NOT NULL,
	CONSTRAINT [PK_TestValue_Table] PRIMARY KEY CLUSTERED 
	(
	[ID] ASC
	)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

	SET ANSI_PADDING OFF
	GO

	ALTER TABLE [dbo].[TestValue_Table] ADD  CONSTRAINT [DF_TestValue_Table_Result]  DEFAULT ((1)) FOR [Result]
GO

	ALTER TABLE [dbo].[TestValue_Table] ADD  CONSTRAINT [DF_TestValue_Table_UploadTime]  DEFAULT (getdate()) FOR [UploadTime]
GO
*/

///////////////////Create TestResult_Table.sql///////////////////////////////////////////////////////
/*USE [DBTMTS]
GO

	SET ANSI_NULLS ON
	GO

	SET QUOTED_IDENTIFIER ON
	GO

	SET ANSI_PADDING ON
	GO

	CREATE TABLE [dbo].[TestResult_Table](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[BatchName] [varchar](50) NOT NULL,
	[SN] [varchar](64) NOT NULL,
	[Result] [tinyint] NOT NULL,
	[ToolsVersion] [varchar](64) NULL,
	[ErrCode] [tinyint] NULL,
	[ErrMsg] [varchar](128) NULL,
	[IP] [varchar](32) NULL,
	[Elapsed] [int] NULL,
	[StationID] [tinyint] NOT NULL,
	[Operator] [tinyint] NOT NULL,
	[UploadTime] [datetime] NOT NULL,
	CONSTRAINT [PK_TestResult_Table] PRIMARY KEY CLUSTERED 
	(
	[ID] ASC
	)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

	SET ANSI_PADDING OFF
	GO

	ALTER TABLE [dbo].[TestResult_Table] ADD  CONSTRAINT [DF_TestResult_Table_Result]  DEFAULT ((1)) FOR [Result]
GO

	ALTER TABLE [dbo].[TestResult_Table] ADD  CONSTRAINT [DF_TestResult_Table_UploadTime]  DEFAULT (getdate()) FOR [UploadTime]
GO
*/