#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>
#include "UnisocMesBase.h"


//////////////////////////////////////////////////////////////////////////
class CWriteSSNSN1 : public CUnisocMesBase
{
    DECLARE_RUNTIME_CLASS(CWriteSSNSN1)
public:
    CWriteSSNSN1(void);
    virtual ~CWriteSSNSN1(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);
};
