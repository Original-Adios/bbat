#pragma once
#include "WCNMeasurePower.h"
#include "ComDef.h"

class CWCNCLC :
	public CWCNMeasurePower
{
public:
	CWCNCLC(CImpBaseUIS8910 *pSpat);
	~CWCNCLC(void);
	virtual SPRESULT Run();
	virtual SPRESULT Init();
	virtual SPRESULT Release();
protected:
	SPRESULT RunWlan();
	SPRESULT RunBt();
	SPRESULT RunGps();
	WCN_MEASURE_T m_BtPointGroup[MAX_BT_LOSS_NUMBER];
	WCN_MEASURE_T m_WlanPointGroup[MAX_WLAN_LOSS_NUMBER];
};

