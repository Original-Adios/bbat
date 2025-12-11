#pragma once
#include <Windows.h>
#include <string>
#include "drvdef.h"


#define ShareMemory_SLDriver    L"SLDriverObject"
//////////////////////////////////////////////////////////////////////////
class CSLDriver sealed
{
public:
    CSLDriver(void);
   ~CSLDriver(void);

    // Load or free SLNETDB.DLL 
    BOOL Startup(std::wstring& strDLL);
    void Cleanup(void);

    // Invoke interfaces of SLNETDB.DLL
    BOOL   SL_init(char *ip, int port = 3395, SL_Receive_callback proc1 = NULL, SL_Status_callback proc2 = NULL,int steptime = 500, int buf_send_Max = 300, int buf_recv_Max = 300, char *bufpath = NULL);
    BOOL   SL_quit(void);
    LPCSTR SL_GetLastError(void);
    BOOL   SL_ClearLastError(void);
    BOOL   SL_isConnected(void);
    BOOL   SL_AddnewPhone(const char* sn, int timeout = 5000);//timeout>0，同步模式，=0异步模式
    BOOL   SL_PutDatatoPhone(const char* name, bool success, int taketime, const char* head, const char* field, const char* values, const char* title = NULL, int index = 0, int tread = 0);
    BOOL   SL_ClrDatatoPhone(int tread = 0);
    BOOL   SL_ExtDatatoPhone(SL_Station Stationattr, const char* sn, \
        int success, int taketime, const char* head, \
        int timeout = 0, int tread = 0, char* decdatas = NULL);
    BOOL   SL_UserSoftver(const char* itemname_in, const char* linename, const char* softname, \
        const char* PCinfo, const char* lastconfigdate,\
        int *lineid = NULL, int *itemid = NULL, char* itemname = NULL,\
        int timeout = 5000\
        );


    SLNET_DRIVER_T& GetDrvObject(void) { return m_drv; };

private:
    SLNET_DRIVER_T m_drv;
    HMODULE m_hDLL;
    CHAR m_szErrMsg[512];
};
