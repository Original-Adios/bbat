#pragma once

#include "TDMeasurePower.h"
#include "ComDef.h"
#include <list>

class CTdGs : public CTDMeasurePower
{
public:
    CTdGs(CImpBase *pSpat);
    virtual ~CTdGs(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
	uint16 FindGainIndexFromNv(uint16 nChan);
	void ConfigChannel();

protected:
	TD_APC_NV m_tApcNv;
	MEASURE_T m_PointGroup[MAX_TD_LOSS_NUMBER];
	std::list<int> m_listChannel;
};