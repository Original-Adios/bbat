#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_Start_New : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Start_New)
public:
    CMES_Start_New(void);
    virtual ~CMES_Start_New(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strProject;
};
