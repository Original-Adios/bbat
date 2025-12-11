#pragma once
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "WriteCodes.h"
//////////////////////////////////////////////////////////////////////////
class CWriteSN_MGB_GSM : public CWriteCodes
{
    DECLARE_RUNTIME_CLASS(CWriteSN_MGB_GSM)
public:
    CWriteSN_MGB_GSM(void);
    virtual ~CWriteSN_MGB_GSM(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
	INPUT_CODES_T m_InputSN[BC_MAX_NUM];
};
