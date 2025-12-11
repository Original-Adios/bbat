#pragma once
#include "ILossFunc.h"
#include "IWlanApi.h"
#include "IBTApi.h"
#include "GPSApiAT.h"
#include "ComDef.h"

typedef struct  _tag_WCN_MEASURE_T
{
	int16 nChannel;
	int8 nType;
	int8 nPath[WCN_ANT];
	double dExpPower[WCN_ANT];
	double dPower[WCN_ANT];
	double dOrigPower[WCN_ANT];
	_tag_WCN_MEASURE_T()
	{
		ZeroMemory(this, sizeof(_tag_WCN_MEASURE_T));
	}
}WCN_MEASURE_T;

class CWCNMeasurePower :
	public ILossFunc
{
public:
	CWCNMeasurePower(CImpBase *pSpat);
	~CWCNMeasurePower(void);

	virtual SPRESULT Init();
	virtual SPRESULT Release();

protected:
	SPRESULT WlanMeasurePower(WCN_MEASURE_T *pList, int nListCount);
	SPRESULT BTMeasurePower(WCN_MEASURE_T *pList, int nListCount);
	SPRESULT GetGPSCNR(GPS_BAND eBand, RF_ANT_E eAnt, double& dCNR, double dCellPower);
	SPRESULT GPS_ModeEnable(GPS_MODE eMode);

	void ConfigWlanParam(WCN_MEASURE_T *pParam, int nAntIndex);
	void ConfigBtParam(WCN_MEASURE_T *pParam, int nAntIndex);

	SPWI_WLAN_PARAM_TESTER  m_TesterParam;
	SPWI_RESULT_T m_TesterRlt;

	SPWI_BT_PARAM_TESTER m_BtTesterParam;


	ICWlanApi* m_pWlanApi;
	ICBTApi* m_pBtApi;
	CGPSApiAT* m_pGpsApi;

	IRFDevice*        m_pRFTester;
	static const double g_GpsFreq[GPS_MAX_BAND];
	static const char g_GpsBand[GPS_MAX_BAND][8];
};

