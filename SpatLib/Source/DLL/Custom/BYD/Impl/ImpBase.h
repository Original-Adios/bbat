#pragma once
#include "SpatBase.h"
#include "../drv/MesDriver.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"

#define ShareMemory_BydNumber    L"ShareMemory_BydNumber"
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

	SPRESULT GetMesDriverFromShareMemory(void);
	SPRESULT SetMesDriverIntoShareMemory(void);

	INPUT_CODES_T m_InputSN[BC_MAX_NUM];

	SPRESULT MES_GetSN(char*pszSN,int iSize);

	BOOL GetInfoFromCustomData(char*pszCustomData,char*pszName,char*pszValue,UINT uSize);
	BOOL SetSfcKeyCollectData(char*pszNumbae,char*pszData,UINT uSize);

protected:
	CMesDriver* m_pMesDrv;
};
