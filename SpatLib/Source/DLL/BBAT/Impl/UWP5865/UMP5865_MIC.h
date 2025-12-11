#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUMP5865_MIC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUMP5865_MIC)
public:
    CUMP5865_MIC(void);
    virtual ~CUMP5865_MIC(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	


private:
	int m_iPeakUpSpec;
	int m_iPeakDownSpec;

	
};