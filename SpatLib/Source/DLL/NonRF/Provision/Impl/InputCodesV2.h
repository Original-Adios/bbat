#pragma once
#include "ImpBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "GenCodes.h"

//////////////////////////////////////////////////////////////////////////

class CInputCodesV2 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CInputCodesV2)
public:
    CInputCodesV2(void);
    virtual ~CInputCodesV2(void);
protected:
	virtual void     __EnterAction(void);
	virtual void     __LeaveAction(void);

    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    void GetXMLConfig(INPUT_CODES_T* pInput, std::wstring strCodeName, BOOL bHex = FALSE);
	void SetJsConfig(BC_INDEX eIndex, Value* vArray, Document::AllocatorType& allocator);
    SPRESULT GetInputCode(BC_INDEX eIndex);
    SPRESULT InitGenCode(BC_INDEX index);
	SPRESULT Debug();

protected:
	INPUT_CODES_T  m_InputSN[BC_MAX_NUM];
	CGenCodes     m_GenCodes[BC_MAX_NUM];
    BOOL  m_bCheckXmode;

};
