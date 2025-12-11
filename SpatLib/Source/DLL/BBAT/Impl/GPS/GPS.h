#pragma once
#include "../ImpBase.h"

#define MAXGPSNUMBER 20
#define DELAY_BEFOREGPSSEARCH_MS 2000
//////////////////////////////////////////////////////////////////////////
class CGps : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CGps)
public:
	CGps(void);
	virtual ~CGps(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
	int m_nMinApNumber;
	int m_nMinRssiSpec;
	int m_nReadTimes;
	int m_nReadApCount;
	int m_nGpsOpen;
	int m_nGpsClose;
	int m_nGpsSearch;

	SPRESULT RunOpen();
	SPRESULT RunSearch();
	SPRESULT RunRead();
	SPRESULT RunClose();
	SPRESULT RunRssi(int nRetrytimes);
	SPRESULT ReadAction();

};