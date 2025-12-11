#pragma once
#include "ILossFunc_UIS8910.h"
#include "IWlanApi.h"
#include "IBTApi.h"
#include "GPSApiAT.h"

typedef struct
{
	int8 Cnt;
	int8 AgcIdx[10];
	int16 Power[10];
}WCN_PARA_T;

typedef struct  
{
	int16 nChannel;
	int8 nType;
	int8 nPath[MAX_RF_ANT];
	double dExpPower[MAX_RF_ANT];
	double dPower[MAX_RF_ANT];
	double dOrigPower[MAX_RF_ANT];
	WCN_PARA_T Para[MAX_RF_ANT];
	WCN_PARA_T OrgPara[MAX_RF_ANT];
}WCN_MEASURE_T;

class CWCNMeasurePower :
	public ILossFunc_UIS8910
{
public:
	CWCNMeasurePower(CImpBaseUIS8910 *pSpat);
	~CWCNMeasurePower(void);

	virtual SPRESULT Init();
	virtual SPRESULT Release();

protected:
	SPRESULT WlanMeasurePower(WCN_MEASURE_T *pList, int nListCount);
	SPRESULT BTMeasurePower(WCN_MEASURE_T *pList, int nListCount);
	SPRESULT GetGPSSNR(double& dSNR);

	void ConfigWlanParam(WCN_MEASURE_T *pParam, int nAntIndex);
	void ConfigBtParam(WCN_MEASURE_T *pParam, int nAntIndex);

	SPWI_WLAN_PARAM_TESTER  m_TesterParam;
	SPWI_RESULT_T m_TesterRlt;

	SPWI_BT_PARAM_TESTER m_BtTesterParam;
	SPBT_RESULT_T m_BtTesterRlt;



	ICWlanApi* m_pWlanApi;
	ICBTApi* m_pBtApi;
	CGPSApiAT* m_pGpsApi;

	IRFDevice*        m_pRFTester;
	SP_HANDLE         m_hDUT;
};

