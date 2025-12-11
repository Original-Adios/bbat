#pragma once
#include "../ImpBase.h"

#define MAXMCUBTNUMBER 2
#define MAXBTRSSISPEC -20
#define DELAY_BEFOREBTSEARCH_MS 2000
//////////////////////////////////////////////////////////////////////////
class CMcuBT : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMcuBT)
public:
    CMcuBT(void);
    virtual ~CMcuBT(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
	int m_nMinRssiSpec;

	SPRESULT RunReadRssi();
	
};