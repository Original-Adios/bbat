#pragma once

#include "TDMeasurePower.h"
#include "ComDef.h"

class CTdClc : public CTDMeasurePower
{
public:
    CTdClc(CImpBase *pSpat);
    virtual ~CTdClc(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
protected:
	MEASURE_T m_PointGroup[MAX_TD_LOSS_NUMBER];
};