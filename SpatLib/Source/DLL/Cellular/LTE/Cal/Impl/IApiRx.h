#pragma once

#include "IApiImp.h"
#include <vector>

using namespace std;

class IApiRx : public IApiImp
{
public:
    struct PointData 
    {
        double dPower;
        double dRssi;
        double dGain;
        uint16 usGainIndex;
    };

    struct ChannelData
    {
        uint16 usArfcn;
        int32 nIndicator;
        vector<PointData> arrPoint;
    };

    struct BandData
    {
        vector<ChannelData> arrChannel;
    };

public:
    vector<BandData>* m_parrBandData;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    uint16 m_Afc;
	uint32 m_uMaxRetryCout;

    SPRESULT (*DealwithData)(void*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    void* m_pCurrentCaller;
};
