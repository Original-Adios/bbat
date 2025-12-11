#pragma once

#include "IApiImp.h"
#include <vector>

using namespace std;

class IApiTxV3 : public IApiImp
{
public:
    struct SectionData
    {
        uint16 usApt;
        uint16 usPaMode;
        uint16 usBeginWord;
        int16  sWordOffset;
        double dEnp;
        uint16 usWordStep;
        uint16 usDone;
        vector<double> arrPower;
    };

    struct ChannelData
    {
        uint32 usArfcn;
        uint16 usPointNumber;
        vector<SectionData> arrSection;
    };

    struct BandData
    {
        int nIndicator;
        vector<ChannelData> arrChannel;
    };

public:
    vector<BandData>* m_parrBandData;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    uint16 m_Afc;

    SPRESULT (*DealwithData)(void*, BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    void* m_pCurrentCaller;
};
