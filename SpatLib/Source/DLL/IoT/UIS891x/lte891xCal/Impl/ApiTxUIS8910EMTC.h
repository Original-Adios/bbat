#pragma once
#include "ApiBase.h"
#include "IApiTx.h"
#include <vector>

class CApiTxUIS8910EMTC :
    public CApiBase,
    public IApiTx
{
public:
    CApiTxUIS8910EMTC(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    CApiTxUIS8910EMTC(const CApiTxUIS8910EMTC&obj) :CApiTxUIS8910EMTC(obj.m_lpFuncName,obj.m_pFuncCenter) {}
    virtual ~CApiTxUIS8910EMTC(void);
    CApiTxUIS8910EMTC& operator=(const CApiTxUIS8910EMTC&)
    {
        return *this;
    }

    virtual SPRESULT Run();
    virtual SPRESULT PreInit();
    virtual void ClearData();

private:
    SPRESULT GetMaxPointsNumber();
    void InitParam(int nBand, int nChannel, int nSection);

    SPRESULT InsertSection(BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap);
    SPRESULT InsertChannel(BOOL* ret, int nBand, int nChannel, int nSection);
    SPRESULT InsertTrigger(int nBand, int nChannel);
	SPRESULT SetApcParam();

    SPRESULT DoFunc();
    void GetData();

private:
    uint32 m_nStartBand;
    uint32 m_nStopBand;
    uint32 m_nStartChannel;
    uint32 m_nStopChannel;
    uint32 m_nStartSection;
    uint32 m_nStopSection;

    int m_nMaxPointNumber;
    int m_nPointNumber;
    BOOL m_bChannelGap;

    PC_LTE_APC_UIS8910_T* m_pPhoneParam;
    RF_LTE_CAL_TX_V1_REQ_T* m_pTesterParam;
    RF_LTE_CAL_TX_V1_RLT_T* m_pTesterResult;

    std::vector<PC_LTE_APC_V3_CHANNEL_T> m_arrPhoneChannel;
    std::vector<PC_LTE_APC_V2_APT_T> m_arrPhoneApt;

    std::vector<RF_LTE_CAL_TX_V1_REQ_CHANNEL_T> m_arrInsChannel;
    std::vector<RF_LTE_CAL_TX_V1_REQ_SECTION_T> m_arrInsSection;

    BOOL m_bResult;
	BOOL m_bLinteger;
    uint32 m_uRetryTime;

    uint32 m_uTxSize;
    uint32 m_uMaxSize;
	//MT8820
	BOOL m_bMT8820;
};
