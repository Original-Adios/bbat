#pragma once
#include "MakeGS.h"

class CMakeGSCellularV4 :
    public CMakeGS
{
    DECLARE_RUNTIME_CLASS(CMakeGSCellularV4)
public:
    CMakeGSCellularV4(void);
    virtual ~CMakeGSCellularV4(void);
    virtual SPRESULT   __InitAction(void);
};
