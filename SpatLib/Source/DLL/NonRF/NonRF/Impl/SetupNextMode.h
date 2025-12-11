#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSetupNextMode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSetupNextMode)
public:
    CSetupNextMode(void);
    virtual ~CSetupNextMode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strMode;
};
