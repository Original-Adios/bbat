#pragma once
#include "../ImpBase.h"
#include "BTApiHCI.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//////////////////////////////////////////////////////////////////////////

class CHciBtCheckAddr : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CHciBtCheckAddr)
public:
    CHciBtCheckAddr(void);
    virtual ~CHciBtCheckAddr(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    SPRESULT CheckBTCodeFrTxt(const char* lpAddr);

    BOOL m_bCheckInfoFrTxt;
    INPUT_CODES_T m_InputSN[BC_MAX_NUM];
	std::wstring m_strTXTpath;

};
