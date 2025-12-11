#pragma once
#include "MakeGS.h"

class CMakeGSCellularV5 :
    public CMakeGS
{
    DECLARE_RUNTIME_CLASS(CMakeGSCellularV5)
public:
    CMakeGSCellularV5(void);
    virtual ~CMakeGSCellularV5(void);
    virtual SPRESULT   __InitAction(void);
    virtual SPRESULT   __PollAction(void);
};
