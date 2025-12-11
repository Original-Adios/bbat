#pragma once
#include "ApiBase.h"
#include "IApiRxModV3.h"
#include <vector>

using namespace std;
#pragma pack(push, 2)
class CApiRxModV4 :
	public CApiBase,
	public IApiRxModV3
{
public:
    CApiRxModV4(LPCWSTR lpName, CFuncCenter* pFuncCenter);
	virtual ~CApiRxModV4();
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
    
protected:
    int m_nStartBand;
    int m_nStopBand;
    int m_nStartChannel;
    int m_nStopChannel;
    int m_nStartGain;
    int m_nStopGain;

    int m_CurrentBand;
    int m_CurrentChannel;

    PC_MODEM_RF_V4_LTE_AGC_PARAM* m_pPhoneParamV4;
    RF_LTE_CAL_RX_V1_REQ_T* m_pTesterParam;

    vector<PC_MODEM_RF_V4_LTE_AGC_CHANNEL_T> m_vecApiChannels;
    vector<PC_MODEM_RF_V3_LTE_AGC_GAIN_T> m_ApiPoints;

	struct PC_MODEM_RF_V3_LTE_AGC_RSSI_RECEIVE_T
	{
		int16 rssi:12;
		int16 reserved:4;
	};
    vector<PC_MODEM_RF_V3_LTE_AGC_RSSI_RECEIVE_T> m_ApiRetRssi;

	vector<RF_LTE_CAL_RX_V1_REQ_CHANNEL_T> m_arrInsChannel;
	vector<double> m_arrPower;

//	LTE_CA_E* m_pCurrentCa;  
	BOOL m_bMT8820;
//	BOOL m_bLinteger;
    int m_nChannelId;
};
#pragma pack(pop)  // push 4
