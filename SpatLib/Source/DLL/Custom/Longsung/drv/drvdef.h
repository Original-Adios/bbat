#pragma once
#include "SLNETDB.h"

//
typedef bool  (SLCALL *pSL_init)(char *ip, int port, SL_Receive_callback proc1, SL_Status_callback proc2, int steptime, int buf_send_Max, int buf_recv_Max, char *bufpath);
typedef bool  (SLCALL *pSL_quit)(void);

typedef bool  (SLCALL *pSL_GetLastError)(char *err);	
typedef bool  (SLCALL *pSL_ClearLastError)(void);
typedef bool  (SLCALL *pSL_isConnected)(void);

typedef bool  (SLCALL *pSL_AddnewPhone)(const char* sn, int timeout);//timeout>0，同步模式，=0异步模式

typedef bool  (SLCALL *pSL_PutDatatoPhone)(const char* name, bool success, int taketime, const char* head, const char* field, const char* values, const char* title, int index, int tread);
typedef bool  (SLCALL *pSL_ClrDatatoPhone)(int tread);
typedef bool  (SLCALL *pSL_ExtDatatoPhone)(SL_Station Stationattr, const char* sn, \
    int success, int taketime, const char* head, \
    int timeout, int tread, char* decdatas);

typedef bool  (SLCALL *pSL_UserSoftver)(const char* itemname_in, const char* linename, const char* softname, \
    const char* PCinfo, const char* lastconfigdate,\
    int *lineid, int *itemid, char* itemname,\
    int timeout\
    );


typedef struct _tagSLNET_DRIVER_T
{
    pSL_init    SL_init;
    pSL_quit    SL_quit;
    pSL_GetLastError    SL_GetLastError;
    pSL_ClearLastError  SL_ClearLastError;
    pSL_isConnected     SL_isConnected;
    pSL_AddnewPhone     SL_AddnewPhone;
    pSL_PutDatatoPhone  SL_PutDatatoPhone;
    pSL_ClrDatatoPhone  SL_ClrDatatoPhone;
    pSL_ExtDatatoPhone  SL_ExtDatatoPhone;
    pSL_UserSoftver     SL_UserSoftver;

    _tagSLNET_DRIVER_T(void) {
        Reset();
    };

    void Reset(void) {
        SL_init = NULL;
        SL_quit = NULL;
        SL_GetLastError = NULL;
        SL_ClearLastError = NULL;
        SL_isConnected = NULL;
        SL_AddnewPhone = NULL;
        SL_PutDatatoPhone = NULL;
        SL_ClrDatatoPhone = NULL;
        SL_ExtDatatoPhone = NULL;
        SL_UserSoftver = NULL;
    };

    BOOL IsValid(void) {
        return ( (NULL != SL_init) && (NULL != SL_quit) && (NULL != SL_GetLastError) && (NULL != SL_ClearLastError)
            && (NULL != SL_isConnected) && (NULL != SL_PutDatatoPhone) && (NULL != SL_ClrDatatoPhone) 
            && (NULL != SL_ExtDatatoPhone)  && (NULL != SL_AddnewPhone) && (NULL != SL_UserSoftver)
            );
    };

} SLNET_DRIVER_T;

