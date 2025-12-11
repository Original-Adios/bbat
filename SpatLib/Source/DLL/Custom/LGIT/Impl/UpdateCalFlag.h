#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUpdateCalFlag : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUpdateCalFlag)
public:
    CUpdateCalFlag(void);
    virtual ~CUpdateCalFlag(void);

protected:
    //virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
	SPTEST_RESULT_T m_result;
};