#pragma once
#include "FuncBase.h"
#include "NalClc_UIS8910.h"
#include "FileConfig_UIS8910.h"
#include "IAlgo.h"
#include "IApiRx.h"

class CAlgoClcRx :
    public CFuncBase,
    public IAlgo
{
private:
    typedef CNalClc_UIS8910::Config Config;
    typedef CNalClc_UIS8910::Result Result;

    typedef IApiRx::BandData BandData;
    typedef IApiRx::ChannelData ChannelData;
    typedef IApiRx::PointData PointData;

public:
    CAlgoClcRx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcRx(void);

    virtual SPRESULT PreInit();
    virtual void SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca );
    virtual SPRESULT Init();
    virtual SPRESULT Run();
    SPRESULT Check();

private:
    SPRESULT InitData();
    SPRESULT AdjustLoss(BOOL &ret, uint32 uIndex);
    SPRESULT CheckLoss(uint32 uIndex);
    void SetLoss(double dLoss);

private:
    CNalClc_UIS8910* m_pNal;
    CFileConfig_UIS8910* m_pFileConfig;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    IApiRx* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

};
