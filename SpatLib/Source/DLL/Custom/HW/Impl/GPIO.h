#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CGPIO : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGPIO)
public:
    CGPIO(void);
    virtual ~CGPIO(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
