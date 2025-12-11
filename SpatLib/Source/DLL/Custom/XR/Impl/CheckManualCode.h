#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>
#include "UnisocMesBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckManualCode : public CUnisocMesBase
{
    DECLARE_RUNTIME_CLASS(CCheckManualCode)
public:
    CCheckManualCode(void);
    virtual ~CCheckManualCode(void);

protected:
    virtual SPRESULT __PollAction (void);
    
};
