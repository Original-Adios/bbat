#pragma once

#include "IApiImp.h"
#include <vector>

using namespace std;

class IApiRxV3 : public IApiImp
{
public:
    struct PointData 
    {
        double dPower;
        double dRssi;
        double dGain;
        uint16 usGainIndex;
        uint16 usIndex;

		PointData(void)
		{
            ZeroMemory(this, sizeof(*this));
		}
    };

    struct ChannelData
    {
        uint32 usArfcn;
        int32 nIndicator;
        vector<PointData> arrPoint;

		ChannelData(void)
		{
			usArfcn = 0;
			nIndicator = 0;
			arrPoint.clear();
		}
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
