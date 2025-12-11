#pragma once
//#include <vector>
#include "BTMeasBase.h"

using namespace std;

class CBTMeasBLE : public CBTMeasBase
{
public:
	virtual ~CBTMeasBLE(void);
	CBTMeasBLE(CImpBase *pImpBase, BT_TYPE eType, ICBTApi *pBtApi);//

protected:
	// Measurement functions
	virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult, RFPATH_ENUM eRfPath = ANT_SINGLE);
	virtual BOOL MeasureUplink_CW( BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult, RFPATH_ENUM eRfPath = ANT_SINGLE);
	virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPWI_VALUES_T *pPER, RFPATH_ENUM eRfPath = ANT_SINGLE);

private:
    SPWI_BT_PARAM_TESTER *m_pstTester;
    SPWI_BT_PARAM        *m_pRfConf;
};
