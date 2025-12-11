#pragma once

#include "IApiImp.h"
#include <vector>

using namespace std;

class IApiRxModV3 : public IApiImp
{
public:
    struct PointData 
    {
        double dPower;
        double dRssi;
        double dGain;
        uint16 usGainIndex;
		BOOL bMeas;

		PointData(void)
		{
			dPower = 0.0;
			dRssi = 0.0;
			dGain = 0.0;
			usGainIndex = 0;
			bMeas = TRUE;
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
		int nBand;
        int nChannelId;
        uint16 nTriggerArfcn = 0;
        vector<ChannelData> arrChannel;

        uint32 nPathId;
        uint16 nPathGroupIndex;
        uint32 nTriggerPathId;
        uint16 nTriggerGroupIndex;
        int nTriggerWord;
        int nTriggerUpLinkStream;
        BandData()
        {
            ZeroMemory(this, sizeof(BandData));
        }
    };


public:
    vector<BandData>* m_parrBandData;

    RF_ANT_E m_Ant;
    RF_CHAIN_E m_RfChain;

    uint16 m_Afc;
	uint32 m_uMaxRetryCout;

    SPRESULT (*DealwithData)(void*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    void* m_pCurrentCaller;

	RF_BWIndex_E m_Bw;
	BOOL m_bVsBW_Enable;

	uint8 m_TxIns;
	uint8 m_RxIns;
	BOOL m_bBandAdaptation;
};
