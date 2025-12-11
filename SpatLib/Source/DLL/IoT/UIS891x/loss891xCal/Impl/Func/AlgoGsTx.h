#pragma once
#include "FuncBase.h"
#include "NalGs_UIS8910.h"
#include "FileConfig_UIS8910.h"
#include "IAlgo.h"
#include "IApiTx.h"

class CAlgoGsTx :
    public CFuncBase,
    public IAlgo
{
private:
    typedef CNalGs_UIS8910::Config Config;
    typedef CNalGs_UIS8910::Result Result;

    typedef IApiTx::BandData BandData;
    typedef IApiTx::ChannelData ChannelData;
    typedef IApiTx::SectionData SectionData;
    typedef IApiTx::TxSetPara TxSetPara;

public:
    CAlgoGsTx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsTx(void);

    virtual SPRESULT PreInit();
    virtual void SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca );
    virtual SPRESULT Init();
    virtual SPRESULT Run();

private:
    SPRESULT InitData();
    static SPRESULT GetResultStatic(void*, BOOL*, TxSetPara&, uint32);
    SPRESULT GetResult(BOOL*, TxSetPara&, uint32);

private:
    CNalGs_UIS8910* m_pNal;
    CFileConfig_UIS8910* m_pFileConfig;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    IApiTx* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;
};
