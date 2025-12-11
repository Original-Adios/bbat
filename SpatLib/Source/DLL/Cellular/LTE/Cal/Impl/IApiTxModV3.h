#pragma once

#include "IApiImp.h"
#include <vector>

using namespace std;

class IApiTxModV3 : public IApiImp
{
public:
	struct PointData 
	{
		double dPower;
		double dLogValue[3];
		uint16 usValue[3];
	};

    struct SectionData
    {
        uint16 usApt;
        uint16 usPaMode;
        uint16 usBeginWord;
        int16  sWordOffset;
        double dEnp;
        uint16 usWordStep;
        uint16 usDone;
		uint16 usPdet;
        vector<PointData> arrPower;
    };

    struct ChannelData
    {
        uint32 usArfcn;
        uint16 usPointNumber;
        vector<SectionData> arrSection;
    };

    struct BandData
    {
		int nBand;
        int nIndicator;
        int nChannelId;
		uint32 nPathId;
        uint16 nPathGroupIndex;     //for v5
        vector<ChannelData> arrChannel;
        BandData()
        {
            ZeroMemory(this, sizeof(BandData));
        }
    };

public:
    vector<BandData>* m_parrBandData;

    RF_ANT_E m_Ant;
    RF_CHAIN_E m_RfChain;

    SPRESULT (*DealwithData)(void*, BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    void* m_pCurrentCaller;

	uint8 m_TxIns;
	uint8 m_RxIns;
	BOOL m_bBandAdaptation;
};
