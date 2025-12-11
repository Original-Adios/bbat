#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckTP : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckTP)
public:
    CCheckTP(void);
    virtual ~CCheckTP(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strTP;
};