#pragma once
#include "global_def.h"
#include "Global/uni_error_def.h"
#include <vector>
#include "CommonApiBase.h"
#include "CommonDefine.h"
#include "ImpBase.h"
#include "NrUtility.h"

using namespace std;

class CCommonAlgoGsBase
{
public:

    struct PointInfo
    {
        int Band;
        uint32 uArfcn;
        double dFreq;
        BOOL bTx;
        int nAnt;
        int nChannel;
        int PathGroup;
        RF_ANT_E RfAnt;
        int nVoltage;
        PointInfo()
        {
            ZeroMemory(this, sizeof(*this));
        }
    };

    typedef CCommonApiBase::PointData PointData;

private:
public:
    CCommonAlgoGsBase(CImpBase *pImp);
    virtual ~CCommonAlgoGsBase(void);

    virtual SPRESULT MakeGs() = 0;

protected:
    SPRESULT RunBandInfo();

public:
    int m_nTxAnt;
    int m_nRxAnt;
    CCommonApiBase *m_pApi;

    vector<PointInfo> m_arrBandInfo;
    vector<GsData> m_arrGsData;
    CImpBase *m_pImp;
    int m_nRxParam;

protected:
    //must be init
    int m_nTxParamStart;
    int m_nTxParamEnd;
    double m_dTxParamSlope;
    double m_dTxTarget;

    double m_dRxCellpowerStart;
    double m_dRxCellpowerEnd;
    
    double m_dRxTarget;

    double m_dTolerance;
    BOOL m_bAdjust;

    RF_CABLE_LOSS_UNIT_EX*m_pLoss;
private:
    SPRESULT GetTestData(int nIndex);
    SPRESULT RunTestData(int nIndex);
    SPRESULT ShowResult( int nIndex );
    BOOL AdjustResult(int nIndex);
    void GetGsData();

private:
    vector<PointData> m_arrPoint[MAX_RF_ANT];
};
