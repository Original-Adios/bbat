#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CWriteNetCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteNetCodes)
public:
    CWriteNetCodes(void);
    virtual ~CWriteNetCodes(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file


};
