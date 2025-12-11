#pragma once
#include "../ImpBase.h"

#define MAXBTNUMBER 20
#define MAXBTRSSISPEC -20
#define DELAY_BEFOREBTSEARCH_MS 2000
//////////////////////////////////////////////////////////////////////////
class CBT : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CBT)
public:
    CBT(void);
    virtual ~CBT(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
	int m_nMinApNumber;
	int m_nMinRssiSpec;
	int m_nSearchTimes;
	int m_nBtOpen;
	int m_nBtClose;
	int m_nBtSearch;
	int m_nReadApCount;

	SPRESULT RunRead();
	SPRESULT RunReadRssi();
	SPRESULT RunOpen();
	SPRESULT RunClose();
	SPRESULT RunSearch();
	SPRESULT SearchAction();
	
};