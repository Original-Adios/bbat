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
        uint16 usLnaStatus;
        PointData(void)
        {
            dPower = 0.0;
            dRssi = 0.0;
            dGain = 0.0;
            usGainIndex = 0;
            usLnaStatus = 0;
        }
    };

    struct ChannelData
    {
        uint32 usArfcn;
        int32 nIndicator;
        vector<PointData> arrPoint;
    };

    struct BandData
    {
        int nBand;
		int nTriggerWord;
		int nEnp;
        vector<ChannelData> arrChannel;
    };

    struct RxSetPara
    {
        uint32 nStartBand;
        uint32 nStartChannel;
        uint32 nStartGain;
        uint32 nStopBand;
        uint32 nStopChannel;
        uint32 nStopGain;
        RxSetPara()
        {
            nStartBand = 0;
            nStartChannel = 0;
            nStartGain = 0;
            nStopBand = 0;
            nStopChannel = 0;
            nStopGain = 0;
        }
    };
    struct AgcTrigger
    {
        uint32 nBand;
        uint32 arfcn;
        unsigned char indicator;
        bool bLteBand2WifiCal;
        AgcTrigger()
        {
            reset();
        }
        void reset()
        {
            nBand = 0;
            arfcn = 0;
            indicator = 0;
            bLteBand2WifiCal = false;
        }
    };
public:
    vector<BandData>* m_parrBandData;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    uint16 m_Afc;
    uint32 m_uMaxRetryCout;
    int m_DeltaTriggerLevl;
    int m_TriggerLevlNum;
    int m_RxBwConfig;

    int m_UeType; 

    SPRESULT (*DealwithData)(void*, RxSetPara&, uint32);
    void* m_pCurrentCaller;
    AgcTrigger* m_pAgcTrigger;
};
