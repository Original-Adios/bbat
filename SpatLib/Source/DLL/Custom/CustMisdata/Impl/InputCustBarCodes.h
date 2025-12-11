#pragma once
#include "ImpBase.h"
#include "BarCodeUtility.h"
#include "GenCodes.h"

//////////////////////////////////////////////////////////////////////////

class CInputCustBarCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CInputCustBarCodes)
public:
    CInputCustBarCodes(void);
    virtual ~CInputCustBarCodes(void);
protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    void GetXMLConfig(INPUT_CODES_T* pInput, std::wstring strCodeName, BOOL bHex = FALSE);
	void SetJsConfig(BC_INDEX eIndex, Value* vArray, Document::AllocatorType& allocator);
    SPRESULT GetInputCode(BC_INDEX eIndex);
    SPRESULT InitGenCode(BC_INDEX index);
	BOOL SetCustomerXMLConfig(char *pInLabel);
protected:
	INPUT_CODES_T  m_InputSN[BC_MAX_CUSTMIZENUM];
	CGenCodes     m_GenCodes[BC_MAX_NUM];
    BOOL  m_bCheckXmode;

};
