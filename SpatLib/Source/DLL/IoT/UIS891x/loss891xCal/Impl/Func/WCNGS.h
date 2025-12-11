#pragma once
#include "WCNMeasurePower.h"
#include "ComDef.h"
#include <vector>

class CWCNGS :
	public CWCNMeasurePower
{
public:
	CWCNGS(CImpBaseUIS8910 *pSpat);
	~CWCNGS(void);
	virtual SPRESULT Run();
	virtual SPRESULT Init();
	virtual SPRESULT Release();
protected:
	
	SPRESULT RunWlan();
	SPRESULT RunBt();
	SPRESULT RunGps();
	void ConfigBtChannel();
	void ConfigWlanChannel();
	std::vector<int> m_vecWlan[2];
	std::vector<int> m_vecBt;
	std::vector<int> m_vecBtBle;

	WCN_MEASURE_T m_BtPointGroup[MAX_BT_LOSS_NUMBER];
	WCN_MEASURE_T m_WlanPointGroup[MAX_WLAN_LOSS_NUMBER];

};

