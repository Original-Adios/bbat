#include "StdAfx.h"
#include "MesDriver.h"
#include <assert.h>

#ifdef _DEBUG
//#define _DUMMY_TINNO_MES_     
#endif

#ifdef _DUMMY_TINNO_MES_
#pragma warning(disable:4100)
#endif
//////////////////////////////////////////////////////////////////////////
CMesDriver::CMesDriver(void)
    : m_hDLL(NULL)
{
}

CMesDriver::~CMesDriver(void)
{
}

void CMesDriver::Cleanup(void)
{
    if (NULL != m_hDLL)
    {
        FreeLibrary(m_hDLL);
        m_hDLL = NULL;
        m_drv.Reset();
    }
}

BOOL CMesDriver::Startup(std::wstring& strDLL)
{
    m_hDLL = LoadLibraryExW(strDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (NULL == m_hDLL)
    {
        return FALSE;
    }

    m_drv.MES_Login = (pMES_Login)GetProcAddress(m_hDLL, "MES_Login");
	m_drv.MES_Login_out = (pMES_Login_out)GetProcAddress(m_hDLL, "MES_Login_out");
	m_drv.MES_GetinfoSN = (pMES_GetinfoSN)GetProcAddress(m_hDLL, "MES_GetinfoSN");
	m_drv.MES_SaveTestRecord = (pMES_SaveTestRecord)GetProcAddress(m_hDLL, "MES_SaveTestRecord");
	m_drv.MES_NextSN = (pMES_NextSN)GetProcAddress(m_hDLL, "MES_NextSN");
	m_drv.FTP_ConnectFTPServer = (pFTP_ConnectFTPServer)GetProcAddress(m_hDLL, "FTP_ConnectFTPServer");
	m_drv.FTP_UpLoad_TestLogFile_To_FTPServer = (pFTP_UpLoad_TestLogFile_To_FTPServer)GetProcAddress(m_hDLL, "FTP_UpLoad_TestLogFile_To_FTPServer");
	m_drv.FTP_DisConnectFTPServer = (pFTP_DisConnectFTPServer)GetProcAddress(m_hDLL, "FTP_DisConnectFTPServer");

    if (!m_drv.IsValid())
    {
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

BOOL CMesDriver::MES_Login(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage)
{
    if (NULL != m_drv.MES_Login)
    {
#ifdef _DUMMY_TINNO_MES_
        return TRUE;
#else
        return m_drv.MES_Login(in_mesnote,in_username,in_userpwd,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
    }
    else
    {
        return FALSE;
    }
}
BOOL CMesDriver::MES_Login_out(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage)
{
	if (NULL != m_drv.MES_Login)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.MES_Login_out(in_mesnote,in_username,in_userpwd,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::MES_GetinfoSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SONO,char *out_ReturnMessage)
{
	if (NULL != m_drv.MES_Login)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.MES_GetinfoSN(in_mesnote,in_username,in_userpwd,in_PCBSN,in_MONO,out_IMEI_M,out_IMEI_S,out_SN ,out_MAC,out_BT,out_SpecialSN,out_OTPpwd,out_imeimo,out_RoutingStep,out_RoutingStep_Pre,out_RoutingStep_Next,out_MONOSMT,out_MONOPack,out_MONOAssy,out_SONO,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}
BOOL CMesDriver::MES_SaveTestRecord(char *in_mesnote,char *in_username,char *in_userpwd,char *in_SN,char *in_ShiftID,char *in_StartDateTime,char *in_EndDateTime,char *in_TestStatu,char *in_TestComment,char *in_Type,char *in_TestName,char *in_FixtureNo,char *in_RFCableNo,char *in_InstrumentNo,char *in_ToolVersion,char *in_platform,char *in_failCode,char *in_failCodeDesc,char *out_ReturnMessage)
{
	if (NULL != m_drv.MES_Login)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.MES_SaveTestRecord(in_mesnote,in_username,in_userpwd,in_SN,in_ShiftID,in_StartDateTime,in_EndDateTime,in_TestStatu,
			in_TestComment,in_Type,in_TestName,in_FixtureNo,in_RFCableNo,in_InstrumentNo,in_ToolVersion,in_platform,in_failCode,in_failCodeDesc,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}

BOOL CMesDriver::MES_NextSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotSN,char *in_ShiftID,char *out_ReturnMessage)
{
	if (NULL != m_drv.MES_NextSN)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.MES_NextSN(in_mesnote,in_username,in_userpwd,in_LotSN,in_ShiftID,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}

BOOL CMesDriver::FTP_ConnectFTPServer(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage)
{
	if (NULL != m_drv.MES_NextSN)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.FTP_ConnectFTPServer(in_FTP_IP,in_FTP_UserName,in_FTP_Password,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}

BOOL CMesDriver::FTP_UpLoad_TestLogFile_To_FTPServer(char* in_OrderID,char* in_LocalFilePathName, char* in_LogFileName, char* in_FTP_Directory, char* out_ReturnMessage)
{
	if (NULL != m_drv.MES_NextSN)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.FTP_UpLoad_TestLogFile_To_FTPServer(in_OrderID,in_LocalFilePathName,in_LogFileName,in_FTP_Directory,out_ReturnMessage)==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}

BOOL CMesDriver::FTP_DisConnectFTPServer()
{
	if (NULL != m_drv.MES_NextSN)
	{
#ifdef _DUMMY_TINNO_MES_
		return TRUE;
#else
		return m_drv.FTP_DisConnectFTPServer()==1 ? TRUE : FALSE;
#endif
	}
	else
	{
		return FALSE;
	}
}

#ifdef _DUMMY_TINNO_MES_
#pragma warning(default:4100)
#endif