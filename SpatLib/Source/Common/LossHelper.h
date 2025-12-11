#pragma once
#include "global_def.h"
//#include "global.h"
#include "cellular_def.h"
#include <deque>
#include "SpatBase.h"


using namespace std;

class CLossHelper
{
public:
    CLossHelper(void);
    virtual ~CLossHelper(void);

public:
    void ClearLoss();
    SPRESULT SetLoss(int nBand, uint32 uArfcn, double dFreq, RF_IO_E trx, double dLoss, RF_ANT_E ANT);
    SPRESULT AdjustLoss(double dTarget, double dValue, RF_IO_E trx, BOOL& bNeedRetry,
        double dTolernece, double m_dLossUpperLimit = 200);
    SPRESULT UpdateLoss(LOSS_MODE_E Mode);

private: 
    RF_CABLE_LOSS_POINT_EX* FindLoss(int nBand, uint32 uArfcn, double dFreq, RF_IO_E trx);
    SPRESULT ApplyLoss(RF_IO_E trx);

public:
    E_NETWORK_MODE m_NetMode = NM_INVALID;
    CSpatBase* m_pSpatbase = NULL;
	double m_dTolernece;
	double m_Loss;
private:
    deque<RF_CABLE_LOSS_POINT_EX> m_arrLoss;
    double m_dFreq;
    int m_nBand;
    uint32 m_uArfcn;
    RF_ANT_E m_Ant;
    vector<RF_ANT_E> m_arrAnt;

};

