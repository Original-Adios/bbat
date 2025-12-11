#pragma once
#include "ApiBase.h"
#include "IApiRxV3.h"
#include <vector>

using namespace std;
#pragma pack(push, 2)
class CApiRxV3 :
	public CApiBase,
	public IApiRxV3
{
public:
    CApiRxV3(LPCWSTR lpName, CFuncCenter* pFuncCenter);
	virtual ~CApiRxV3();
    virtual SPRESULT PreInit();
    virtual SPRESULT Run();
    virtual SPRESULT Init();
	virtual void ClearData();

protected:
    virtual SPRESULT DoFunc();
    virtual void InitParam();
    SPRESULT GetData();

    virtual SPRESULT InsertChannel(int nBand, int nChannel);
    virtual SPRESULT InsertGainIndex(int nBand, int nChannel, int nGain);
	SPRESULT SwitchCa();
	SPRESULT SetAgcParam();
    
protected:
#define GainIndexOffset 14
    int m_nStartBand;
    int m_nStopBand;
    int m_nStartChannel;
    int m_nStopChannel;
    int m_nStartGain;
    int m_nStopGain;

    int m_CurrentBand;
    int m_CurrentChannel;

    PC_LTE_AGC_V3_T* m_pPhoneParam;
	PC_LTE_AGC_V2_T* m_pPhoneParamV2;
    RF_LTE_CAL_RX_V1_REQ_T* m_pTesterParam;

    vector<PC_LTE_AGC_V3_CHANNEL_T> m_vecApiChannels;
	vector<PC_LTE_AGC_V2_CHANNEL_T> m_vecApiChannelsV2;
    vector<PC_LTE_AGC_V2_POINT_T> m_ApiPoints;
    vector<unsigned int> m_ApiRetRssi;

	vector<RF_LTE_CAL_RX_V1_REQ_CHANNEL_T> m_arrInsChannel;
	vector<double> m_arrPower;

	LTE_CA_E* m_pCurrentCa;  
	BOOL m_bMT8820;
	BOOL m_bLinteger;
};
#pragma pack(pop)  // push 4
