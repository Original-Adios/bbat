#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class COpenPort : public CImpBase
{
    DECLARE_RUNTIME_CLASS(COpenPort)
public:
    COpenPort(void);
    virtual ~COpenPort(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    RM_MODE_ENUM    m_eMode;
    BOOL       m_bAutoSetup;
};
