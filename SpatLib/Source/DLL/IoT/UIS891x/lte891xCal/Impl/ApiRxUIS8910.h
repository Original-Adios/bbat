#pragma once
#include "ApiBase.h"
#include "IApiRx.h"
#include <vector>

using namespace std;
#pragma pack(push, 2)
class CApiRxUIS8910 :
	public CApiBase,
	public IApiRx
{
public:
    CApiRxUIS8910(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    CApiRxUIS8910(const CApiRxUIS8910&obj) :CApiRxUIS8910(obj.m_lpFuncName,obj.m_pFuncCenter) {}
	virtual ~CApiRxUIS8910();
    CApiRxUIS8910& operator=(const CApiRxUIS8910&)
    {
        return *this;
    }

    virtual SPRESULT PreInit();
    virtual SPRESULT Run();
    virtual SPRESULT Init();
	virtual void ClearData();

protected:
    virtual SPRESULT DoFunc();
    virtual void InitParam(int nBand, int nChannel, int nGain);
    SPRESULT GetData(uint32 nRetryCount);

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

    int m_nMaxChanNumber;
    int m_nMaxPointNumber;

    PC_LTE_AGC_UIS8910_T* m_pPhoneParam;
    RF_LTE_CAL_RX_V1_REQ_T* m_pTesterParam;

    vector<PC_LTE_AGC_UIS8910_CHANNEL_T> m_vecApiChannels;
    vector<PC_LTE_AGC_UIS8910_POINT_T> m_ApiPoints;
    vector<unsigned int> m_ApiRetRssi;

	vector<RF_LTE_CAL_RX_V1_REQ_CHANNEL_T> m_arrInsChannel;
	vector<double> m_arrPower;

	LTE_CA_E* m_pCurrentCa;  
	BOOL m_bMT8820;
	BOOL m_bLinteger;
};
#pragma pack(pop)  // push 4
