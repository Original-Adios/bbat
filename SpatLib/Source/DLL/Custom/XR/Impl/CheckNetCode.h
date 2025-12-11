#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>
#include "UnisocMesBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckNetCode : public CUnisocMesBase
{
    DECLARE_RUNTIME_CLASS(CCheckNetCode)
public:
    CCheckNetCode(void);
    virtual ~CCheckNetCode(void);

protected:
    virtual SPRESULT __PollAction (void);
    
};
