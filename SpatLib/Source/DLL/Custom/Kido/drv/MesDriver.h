#pragma once
#include <Windows.h>
#include <string>
#include "drvdef.h"

#define ShareMemory_MesDriver    L"MesDriverObject"

//////////////////////////////////////////////////////////////////////////
class CMesDriver sealed
{
public:
    CMesDriver(void);
   ~CMesDriver(void);

    // Load or free CustomerSolutionMes.dll
    BOOL Startup(std::wstring& strDLL);
    void Cleanup(void);

    // Invoke interfaces CustomerSolutionMes.dll
	BOOL fnGetVersion(char *ver);
	BOOL fnMesStart(char *inMessage, char *retMessage, int iMesType);
	BOOL fnMesComplete(char *inMessage, int iTestRes, char *retMessage, int iMesType);
	BOOL fnGetCodeFromStore(const char* pMsg, const char* pStore, const unsigned int iLength, char* pRes);

	MES_DRIVER_T&GetDrvObject(void) { return m_drv; };

private:
    MES_DRIVER_T m_drv;
    HMODULE m_hDLL;
};
