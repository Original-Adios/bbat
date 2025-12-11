#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
class CWriteKudaCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteKudaCodes)
public:
    CWriteKudaCodes(void);
    virtual ~CWriteKudaCodes(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);

};
