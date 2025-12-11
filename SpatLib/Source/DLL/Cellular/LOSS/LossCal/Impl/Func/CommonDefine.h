#pragma once
#include "global_def.h"

struct GsData
{
    int nBand;
    uint32 uArfcn;
    double dFreq;
    BOOL bTx;
    int nParam;
    double dPower;
    RF_ANT_E RfAnt;
    int nAnt;
    int nChannel;
    int PathGroup;
    double dResult;
    int nVoltage;

    GsData()
    {
        memset(this, 0, sizeof GsData);
    }
};