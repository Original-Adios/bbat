#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CLoadPMICID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadPMICID)
public:
    CLoadPMICID(void);
    virtual ~CLoadPMICID(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strCode;
};
