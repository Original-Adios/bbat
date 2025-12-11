#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>
#include "UnisocMesBase.h"


//////////////////////////////////////////////////////////////////////////
class CWriteSSN : public CUnisocMesBase
{
    DECLARE_RUNTIME_CLASS(CWriteSSN)
public:
    CWriteSSN(void);
    virtual ~CWriteSSN(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);
};
