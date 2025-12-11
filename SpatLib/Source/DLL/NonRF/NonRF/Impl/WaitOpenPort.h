#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CWaitOpenPort : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWaitOpenPort)
public:
    CWaitOpenPort(void);
    virtual ~CWaitOpenPort(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    RM_MODE_ENUM    m_eMode;
    BOOL       m_bAutoSetup;
};
