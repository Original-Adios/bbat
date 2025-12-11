#pragma once
#include "../ImpBase.h"
#include "LossHelper.h"
#include "GPSTestSys.h"
class CGPSTestLoss : public CGpsTestSys
{
	 DECLARE_RUNTIME_CLASS(CGPSTestLoss)
protected:
	CGPSTestLoss(void);
	virtual ~CGPSTestLoss(void);
	virtual SPRESULT __InitAction (void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __PollAction (void);
	virtual SPRESULT GPS_MeasureCW(GPSMeasParamChan* pGPSMeasParam);
	SPRESULT TestCNR(BOOL& bNeedRetry, GPSMeasParamChan* pGPSMeasParam, int& nCnr);

private:
	double m_dPreLoss;
	double m_RxLoss_Low;
	double m_RxLoss_Up;
	double m_Tolernece;
	CLossHelper m_LossHelp;
};
