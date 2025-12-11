#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CWriteCU : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteCU)

public:
    CWriteCU(void);
    virtual ~CWriteCU(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strCU;
};