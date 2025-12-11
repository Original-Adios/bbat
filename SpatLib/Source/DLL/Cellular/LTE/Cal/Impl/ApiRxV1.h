#pragma once
#include "ApiBase.h"
#include "IApiRxV1.h"
#include <vector>

using namespace std;

class CApiRxV1 :
    public CApiBase,
    public IApiRxV1
{
public:
    CApiRxV1(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    virtual ~CApiRxV1(void);

    virtual SPRESULT Run();
    virtual SPRESULT PreInit();
    virtual void ClearData();

protected:
    virtual void InitParam(int nBand, int nChannel, int nSection);
    virtual SPRESULT GetData();
    SPRESULT DoFunc();

    SPRESULT InsertGainIndex(BOOL* ret, int nBand, int nChannel, int nGain);
    SPRESULT InsertPhoneChannel(BOOL* ret, int nBand, int nChannel);
    virtual SPRESULT InsertInsChannel(BOOL* ret, int nBand, int nChannel);
    SPRESULT SwitchCa();
	//mt8820
	SPRESULT ChangeParam(BOOL* ret, int nBand, int nChannel, int nGain);

protected:
#define GainIndexOffset 14

    uint32 m_nStartBand;
    uint32 m_nStopBand;
    uint32 m_nStartChannel;
    uint32 m_nStopChannel;
    uint32 m_nStartGain;
    uint32 m_nStopGain;

    PC_LTE_FDT_RX_T* m_pPhoneParam;
    PC_LTE_FDT_RX_RESULT_T* m_pPhoneRet;
    RF_LTE_CAL_RX_V1_REQ_T* m_pTesterParam;

    std::vector<RF_LTE_CAL_RX_V1_REQ_CHANNEL_T> m_arrInsChannel;
    std::vector<double> m_arrPower;

    LTE_CA_E* m_pCurrentCa;

	//MT8820
	BOOL m_bMT8820;
};
