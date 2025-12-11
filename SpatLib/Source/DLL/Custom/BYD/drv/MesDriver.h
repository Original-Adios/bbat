#pragma once
#include <Windows.h>
#include <string>
#include "drvdef.h"

#define ShareMemory_BydDriver    L"BydDriverObject"

//////////////////////////////////////////////////////////////////////////
class CMesDriver sealed
{
public:
    CMesDriver(void);
   ~CMesDriver(void);

    // Load or free MES2Interface.dll 
    BOOL Startup(std::wstring& strDLL);
    void Cleanup(void);

    // Invoke interfaces of MES2Interface.dll
	BOOL Start_New(char parSFC[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parRemark[], char *retMessage);
	BOOL Complete_New(char parSFC[], char parBoardCount[], char parQualityBatchNum[], char parWorkStation[], char parRemark[], char *retMessage);
	BOOL NcComplete_New(char parSFC[], char parNcType[], char parNcCode[], char parNcContext[], char parFailItem[], char parFailValue[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parNcPlace[], char parCreateUser[], char parOldStationName[], char parRemark[], char *retMessage);
	BOOL SfcKeyCollect_New(char parSFC[], char parData[], char *retMessage);
	BOOL GetNumberBySfc_New(char parSFC[], char parNumberStore[], char parModel[], char parModuleID[], char parCustomStatus[], char parRemark[], char *retNumber, char *retMessage);
	BOOL GetCustomDatabyShoporder_New(char *retCustomData, char *retMessage);

    BYD_DRIVER_T&GetDrvObject(void) { return m_drv; };

private:
    BYD_DRIVER_T m_drv;
    HMODULE m_hDLL;
    CHAR m_szErrMsg[512];
};
