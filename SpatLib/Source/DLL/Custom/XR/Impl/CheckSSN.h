#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
class CCheckSSN : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckSSN)
public:
    CCheckSSN(void);
    virtual ~CCheckSSN(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);

};
