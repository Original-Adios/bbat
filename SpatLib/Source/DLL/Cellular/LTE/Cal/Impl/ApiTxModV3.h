#pragma once
#include "ApiBase.h"
#include "IApiTxModV3.h"
#include <vector>

class CApiTxModV3 :
    public CApiBase,
    public IApiTxModV3
{
public:
    CApiTxModV3(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    virtual ~CApiTxModV3(void);

    virtual SPRESULT Run();
    virtual SPRESULT PreInit();
    virtual void ClearData();

private:
    SPRESULT GetMaxPointsNumber();
    void InitParam(int nBand, int nChannel, int nSection);

    SPRESULT InsertSection(BOOL* ret, int nBand, int nChannel, int nSection, BOOL bGap);
    SPRESULT InsertChannel(BOOL* ret, int nBand, int nChannel, int nSection);
    SPRESULT InsertTrigger(int nBand, int nChannel);

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
	
	uint32 m_unRangeCount;

    PC_MODEM_RF_V3_LTE_APC_REQ_CMD_T* m_pPhoneParam;
    RF_LTE_CAL_TX_V1_REQ_T* m_pTesterParam;
    RF_LTE_CAL_TX_V1_RLT_T* m_pTesterResult;
	PC_MODEM_RF_V3_LTE_APC_RSP_T*    m_pPhoneRet;
	vector<uint16> m_vecPdetRslt;

    std::vector<CALI_APC_ARFCN_T> m_arrPhoneChannel;
    std::vector<CALI_APC_APT_CONFIG_T> m_arrPhoneApt;

    std::vector<RF_LTE_CAL_TX_V1_REQ_CHANNEL_T> m_arrInsChannel;
    std::vector<RF_LTE_CAL_TX_V1_REQ_SECTION_T> m_arrInsSection;

    BOOL m_bResult;
    uint32 m_uRetryTime;

    uint32 m_uTxSize;
    uint32 m_uMaxSize;
	//MT8820
	BOOL m_bMT8820;
};
