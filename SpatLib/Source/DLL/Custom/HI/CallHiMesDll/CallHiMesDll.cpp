#include "StdAfx.h"
#include "CallHiMesDll.h"
#include "XCallHiMesDll.h"



#define CXCallHiMesDllSafe(pHandle)    if (NULL == pHandle)   return MES_DISABLE_FAIL;

CCallHiMesDll::CCallHiMesDll(void)
{
}


CCallHiMesDll::~CCallHiMesDll(void)
{
}


HIMESDLL BOOL CreateMesDllObject(HANDLE pSpLog, HANDLE **pHandle)
{
	*pHandle = (HANDLE*)reinterpret_cast<HANDLE>(new CXCallHiMesDll((ISpLog*)pSpLog));
	CXCallHiMesDllSafe(pHandle);
	return TRUE;
}
HIMESDLL void ReleaseMesDllObject(HANDLE pHandle)
{
	if (pHandle)
	{
		delete pHandle;
	}
}

HIMESDLL MES_RESULT HiAoiCheck(HANDLE pHandle, LPCSTR lpcSn)
{
	CXCallHiMesDllSafe(pHandle);

	return ((CXCallHiMesDll*)pHandle)->_HiAoiCheck(lpcSn);
}

HIMESDLL MES_RESULT HiAddTextTestInfo(HANDLE pHandle, LPCSTR lpcSn, LPCSTR lpcStationName, LPCSTR lpcMac, LPCSTR lpcFilePath, LPCSTR lpcFlag)
{
	CXCallHiMesDllSafe(pHandle);
	return ((CXCallHiMesDll*)pHandle)->_HiAddTextTestInfo(lpcSn, lpcStationName, lpcMac, lpcFilePath, lpcFlag);
}

HIMESDLL MES_RESULT HiAddTextTestInfoCheck(HANDLE pHandle, LPCSTR lpcSn, LPCSTR lpcMac)
{
	CXCallHiMesDllSafe(pHandle);
	return ((CXCallHiMesDll*)pHandle)->_HiAddTextTestInfoCheck(lpcSn, lpcMac);
}

HIMESDLL MES_RESULT HiGetChipInfo(HANDLE pHandle, LPCSTR lpcSn)
{
	CXCallHiMesDllSafe(pHandle);
	return ((CXCallHiMesDll*)pHandle)->_HiGetChipInfo(lpcSn);
}