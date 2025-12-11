#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckCPLC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCPLC)
public:
    CCheckCPLC(void);
    virtual ~CCheckCPLC(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strCPLC;
	BOOL m_bCompareCPLC;
};