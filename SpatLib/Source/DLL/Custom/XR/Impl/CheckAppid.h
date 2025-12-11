#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>
#include "UnisocMesBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckAppid : public CUnisocMesBase
{
    DECLARE_RUNTIME_CLASS(CCheckAppid)
public:
    CCheckAppid(void);
    virtual ~CCheckAppid(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);

private:
	uint32 m_u32AppidBase;
};
