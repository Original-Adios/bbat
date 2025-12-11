#pragma once
#include "ILossFunc.h"
typedef struct  _tag_MEASURE_T
{
	uint16 nChannel;
	uint16 nGainIndex;
	double dExpPower;
	double dPower;
	_tag_MEASURE_T()
	{
		ZeroMemory(this, sizeof(_tag_MEASURE_T));
	}
}MEASURE_T;

class CTDMeasurePower :
	public ILossFunc
{
public:
	CTDMeasurePower(CImpBase *pSpat);
	virtual ~CTDMeasurePower(void);
	SPRESULT MeasurePower(MEASURE_T *pList, int nListCount);
protected:
	PC_TD_FDT_TX m_tdTxDutParam;
	TD_FDT_TX_T m_tdTxInsParam;
	TD_FDT_RESULT m_tdInsResult;
};
