#pragma once
#include "../ImpBase.h"

#define MAXWIFINUMBER 20
#define MAXWIFIRSSISPEC -20
#define DELAY_BEFOREWIFISEARCH_MS 2000

//////////////////////////////////////////////////////////////////////////
class CWifi : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWifi)
public:
	CWifi(void);
    virtual ~CWifi(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);


private:
	int m_nOpen;
	int m_nClose;
	int m_nRead;
	int m_nChannel;
	int m_nMinApNumber; 
	int m_nMinRssiSpec; 
	int m_nReadApCount;
	int m_nSearchTimes;

	SPRESULT RunOpen();
	SPRESULT RunSearch();
	SPRESULT RunRead();
	SPRESULT RunClose();
	SPRESULT RunReadRssi();
	SPRESULT RunChannel();	
	SPRESULT SearchAction();
};