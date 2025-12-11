#pragma once
#include "CalcLS.h"

class CCalcLSCellularV5 : public CCalcLS
{
    DECLARE_RUNTIME_CLASS(CCalcLSCellularV5)
public:
    CCalcLSCellularV5(void);
    virtual ~CCalcLSCellularV5(void);

    virtual SPRESULT   __InitAction(void);
    virtual SPRESULT   __PollAction(void);
};
