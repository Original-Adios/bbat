#pragma once
#include "TINNO_MES.h"

typedef int  (__stdcall*pMES_Login)(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
typedef int  (__stdcall*pMES_Login_out)(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
typedef int  (__stdcall*pMES_GetinfoSN)(char *in_mesnote,char *in_username,char *in_userpwd,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SONO,char *out_ReturnMessage);
typedef int  (__stdcall*pMES_SaveTestRecord)(char *in_mesnote,char *in_username,char *in_userpwd,char *in_SN,char *in_ShiftID,char *in_StartDateTime,char *in_EndDateTime,char *in_TestStatu,char *in_TestComment,char *in_Type,char *in_TestName,char *in_FixtureNo,char *in_RFCableNo,char *in_InstrumentNo,char *in_ToolVersion,char *in_platform,char *in_failCode,char *in_failCodeDesc,char *out_ReturnMessage);
typedef int  (__stdcall*pMES_NextSN)(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotSN,char *in_ShiftID,char *out_ReturnMessage);

typedef int  (__stdcall*pFTP_ConnectFTPServer)(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage);
typedef int  (__stdcall*pFTP_UpLoad_TestLogFile_To_FTPServer)(char* in_OrderID,char* in_LocalFilePathName, char* in_LogFileName, char* in_FTP_Directory, char* out_ReturnMessage);
typedef int  (__stdcall*pFTP_DisConnectFTPServer)();

typedef struct _tagTINNO_DRIVER_T
{
	pMES_Login MES_Login;
	pMES_Login_out MES_Login_out; 
	pMES_GetinfoSN MES_GetinfoSN;
	pMES_SaveTestRecord MES_SaveTestRecord;
	pMES_NextSN MES_NextSN;
	pFTP_ConnectFTPServer FTP_ConnectFTPServer;
	pFTP_UpLoad_TestLogFile_To_FTPServer FTP_UpLoad_TestLogFile_To_FTPServer;
	pFTP_DisConnectFTPServer FTP_DisConnectFTPServer; 

    _tagTINNO_DRIVER_T(void) {
        Reset();
    };

    void Reset(void) {
		MES_Login = NULL;
		MES_Login_out = NULL;
		MES_GetinfoSN = NULL;
		MES_SaveTestRecord = NULL;
		MES_NextSN = NULL;

		FTP_ConnectFTPServer = NULL;
		FTP_UpLoad_TestLogFile_To_FTPServer = NULL;
		FTP_DisConnectFTPServer = NULL;
    };

    BOOL IsValid(void) {
        return ( (NULL != MES_Login) && (NULL != MES_Login_out) && (NULL != MES_GetinfoSN) && (NULL != MES_SaveTestRecord) && (NULL != MES_NextSN)
		&& (NULL != FTP_ConnectFTPServer) && (NULL != FTP_UpLoad_TestLogFile_To_FTPServer) && (NULL != FTP_DisConnectFTPServer) );
    };

} TINNO_DRIVER_T;

