#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CNTC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CNTC)
public:
    CNTC(void);
    virtual ~CNTC(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    int m_nUpSpec = 0;
    int m_nDownSpec = 0;

	SPRESULT RunNTC();
    SPRESULT RunNTC_SUB();

    BOOL m_bNTC = FALSE;
    BOOL m_bNTC_Sub = FALSE;

};
