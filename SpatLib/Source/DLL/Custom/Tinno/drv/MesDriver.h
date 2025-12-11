#pragma once
#include <Windows.h>
#include <string>
#include "drvdef.h"

#define ShareMemory_TinnoDriver    L"TinnoDriverObject"

//////////////////////////////////////////////////////////////////////////
class CMesDriver sealed
{
public:
    CMesDriver(void);
   ~CMesDriver(void);

    // Load or free SLNETDB.DLL 
    BOOL Startup(std::wstring& strDLL);
    void Cleanup(void);

    // Invoke interfaces of SLNETDB.DLL
	BOOL MES_Login(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
	BOOL MES_Login_out(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
	BOOL MES_GetinfoSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SONO,char *out_ReturnMessage);
	BOOL MES_SaveTestRecord(char *in_mesnote,char *in_username,char *in_userpwd,char *in_SN,char *in_ShiftID,char *in_StartDateTime,char *in_EndDateTime,char *in_TestStatu,char *in_TestComment,char *in_Type,char *in_TestName,char *in_FixtureNo,char *in_RFCableNo,char *in_InstrumentNo,char *in_ToolVersion,char *in_platform,char *in_failCode,char *in_failCodeDesc,char *out_ReturnMessage);
	BOOL MES_NextSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotSN,char *in_ShiftID,char *out_ReturnMessage);

	BOOL FTP_ConnectFTPServer(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage);
	BOOL FTP_UpLoad_TestLogFile_To_FTPServer(char* in_OrderID,char* in_LocalFilePathName, char* in_LogFileName, char* in_FTP_Directory, char* out_ReturnMessage);
	BOOL FTP_DisConnectFTPServer();

    TINNO_DRIVER_T& GetDrvObject(void) { return m_drv; };

private:
    TINNO_DRIVER_T m_drv;
    HMODULE m_hDLL;
    CHAR m_szErrMsg[512];
};
