#pragma once

#include "ISpLogExport.h"
#include "..\Lib\ICallHiMesDll.h"

#using "..\Lib\IntelligentPrecise.PhoneTest.Interface.dll"
using namespace IntelligentPrecise::PhoneTest::Interface;

class CXCallHiMesDll sealed
{
public:
	CXCallHiMesDll(ISpLog* pLogUtil);
	virtual ~CXCallHiMesDll(void);

	virtual MES_RESULT _HiAoiCheck(LPCSTR lpcSn);
	virtual MES_RESULT _HiAddTextTestInfo(LPCSTR lpcSn, LPCSTR lpcStationName, LPCSTR lpcMac, LPCSTR lpcFilePath, LPCSTR lpcFlag);
	virtual MES_RESULT _HiAddTextTestInfoCheck(LPCSTR lpcSn, LPCSTR lpcMac);
	virtual MES_RESULT _HiGetChipInfo(LPCSTR lpcSn);

//protected:
//		MesProt pHiMes;

private:
	ISpLog* m_pLogUtil;
	gcroot<MesProt^> m_pHiMes;
};

