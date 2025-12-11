#pragma once
/*
    Implement the functions to read or write code

*/
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CUnisocMesHelper
{
public:
    CUnisocMesHelper(CImpBase*pImp);
    virtual ~CUnisocMesHelper(void);

	MES_RESULT UnisocMesActive();
	SPRESULT UnisocMesGetAssignedCodes(INPUT_CODES_T input_codes[],UINT32 uiSize);
	SPRESULT UnisocMesGetDeviceCodes(INPUT_CODES_T input_codes[]);

private:
	CImpBase*m_pImp;
};
