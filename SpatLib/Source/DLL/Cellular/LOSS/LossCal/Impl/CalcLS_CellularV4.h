#pragma once
#include "CalcLS.h"

class CCalcLSCellularV4 : public CCalcLS
{
    DECLARE_RUNTIME_CLASS(CCalcLSCellularV4)
public:
    CCalcLSCellularV4(void);
    virtual ~CCalcLSCellularV4(void);

    virtual SPRESULT   __InitAction(void);
};
