#pragma once
#include "ApiBase.h"
#include "IApiTxV2.h"
#include <vector>

class CApiTxV21 :
    public CApiBase,
    public IApiTxV2
{
public:
    CApiTxV21(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    virtual ~CApiTxV21(void);

    virtual SPRESULT Run();
    virtual SPRESULT PreInit();
    virtual void ClearData();

protected:
    SPRESULT GetMaxPointsNumber();
    virtual void InitParam(int nBand, int nChannel, int nSection);

    SPRESULT InsertSection(BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap);
    SPRESULT InsertChannel(BOOL* ret, int nBand, int nChannel, int nSection);
    SPRESULT InsertTrigger(int nBand, int nChannel);

    SPRESULT DoFunc();
    void GetData();
    SPRESULT SwitchCa();

protected:
    uint32 m_nStartBand;
    uint32 m_nStopBand;
    uint32 m_nStartChannel;
    uint32 m_nStopChannel;
    uint32 m_nStartSection;
    uint32 m_nStopSection;

    int m_nMaxPointNumber;
    int m_nPointNumber;
    BOOL m_bChannelGap;

    PC_LTE_FDT_TX_T* m_pPhoneParam;
    RF_LTE_CAL_TX_V1_REQ_T* m_pTesterParam;
    RF_LTE_CAL_TX_V1_RLT_T* m_pTesterResult;

    std::vector<RF_LTE_CAL_TX_V1_REQ_CHANNEL_T> m_arrInsChannel;
    std::vector<RF_LTE_CAL_TX_V1_REQ_SECTION_T> m_arrInsSection;

    LTE_CA_E* m_pCurrentCa;
    BOOL m_bResult;
    uint32 m_uRetryTime;
	//MT8820
	BOOL m_bMT8820;
};
