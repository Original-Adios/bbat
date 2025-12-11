#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSetMode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSetMode)
public:
    CSetMode(void);
    virtual ~CSetMode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strMode;
};
