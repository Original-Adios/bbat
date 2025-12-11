#pragma once
#include "SpatBase.h"
#include "../drv/MesDriver.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include <shlwapi.h>
#include <atltime.h>
#include <comutil.h>
#include <ObjBase.h>
#include <Nb30.h>
#include <winsock2.h> 
#include <winver.h>
#include <regex>
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib,"Version.lib")

#pragma comment(lib, "TINNO_MES_Interface.lib")

enum
{
	TINNO_SUCC  =  1,
	TINNO_FAIL  = -1
};

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

	SPRESULT GetMesDriverFromShareMemory(void);
	SPRESULT SetMesDriverIntoShareMemory(void);
	SPRESULT Get_Host_PCName(char*pszName,int iSize);
	SPRESULT Get_Tool_Info(char*pszToolName,char*pszToolVersion);
	int StringToArray(int iCount,char cTok,char*pszData, char*pszArray,int iStrLen);
	void StringReplace(char*pszData);

	INPUT_CODES_T m_InputSN[BC_MAX_NUM];
protected:
	CMesDriver* m_pMesDrv;
};
