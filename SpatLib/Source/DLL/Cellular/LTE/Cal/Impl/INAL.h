#pragma once

#include <vector>
#include "cellular_def.h"

using namespace std;

class INAL
{
public:
#pragma region config
    struct NvConfig_TxApt
    {
        int nApt;
        int nPaMode;
        int nIndex;
        vector<double> arrPower;

		NvConfig_TxApt(void)
		{
			nApt = 0;
			nPaMode = 0;
			nIndex = 0;
			arrPower.clear();
		}
    };

    struct NvConfig_TxSwitch
    {
        int nBeginIndex;
        int nEndIndex;
    };

    struct NvConfig_Tx
    {
        int nBand;
        int nIndicator;
        int nNvIndex;
        int MinWord;
        int MaxWord;
        vector<NvConfig_TxApt> arrApt;
        vector<NvConfig_TxSwitch> arrSwitch;
        vector<uint32> arrChannel;

		NvConfig_Tx(void)
		{
			nBand = 0;
			nIndicator = 0;
			nNvIndex = 0;
            MinWord = 0;
            MaxWord = 0;
			arrApt.clear();
			arrSwitch.clear();
			arrChannel.clear();
		}
    };

    struct NvConfig_Rx
    {
        int nBand;
        int nIndicator;
        int nNvIndex;
		uint16 nAgcLna0Cnt : 4;
		uint16 nAgcLna10Cnt:4;
		uint16 nAgcLna12Cnt:4;
		uint16 nAgcLna21Cnt:4;
        uint32 usRefChannel;
		uint32 usRefChannelIrr;
        vector<uint16> arrBeginIndex;
        vector<uint16> arrWord;
        vector<uint32> arrChannel;
        vector<uint32> arrWordIndex;

		NvConfig_Rx(void)
		{
			nBand = 0;
			nIndicator = 0;
			nNvIndex = 0;
			nAgcLna0Cnt = 0;
			nAgcLna10Cnt = 0;
			nAgcLna12Cnt = 0;
			nAgcLna21Cnt = 0;
			usRefChannel = 0;
			usRefChannelIrr = 0;
			arrBeginIndex.clear();
			arrWord.clear();
			arrChannel.clear();
			arrWordIndex.clear();
		}
    };
	//fdiq
	struct NvConfig_Fdiq
	{
		int nBand;
		int nIndicator;
		int nNvIndex;
		unsigned short nStepCount;
		unsigned int nArfcn;
		unsigned int nBandFlag;

		NvConfig_Fdiq(void)
		{
			nBand = 0;
			nIndicator = 0;
			nNvIndex = 0;
			nStepCount = 0;
			nArfcn = 0;
			nBandFlag = 0;
		}
	};

    struct NvConfig_PdtChannel
    {
        uint16 usRefIndex;
        uint32 usArfcn;
    };

	struct NvConfig_PdtPoint
	{
		int nApt;
		int nPaMode;
		int nIndex;
		double dExpPwr;
	};

    struct NvConfig_Pdt
    {
        vector<NvConfig_PdtChannel> arrChannel;
		vector<NvConfig_PdtPoint> arrPdtPwrPoint;
		LTE_POWER_CLASS PowerClass = LTE_POWER_CLASS3;
		double MaxPower;
    };

    struct NvConfig_AfcTcxo
    {
        uint16 Center;
        uint16 Slope;
    };

    struct NvConfig_AfcDcxo
    {
        uint16 CafcSlope;
        uint16 Cadc;
        uint16 Cafc;
    };

    struct NvConfig_Afc
    {
        uint16 Type;
        uint16 Freq;
        NvConfig_AfcTcxo Tcxo;
        NvConfig_AfcDcxo Dcxo;
    };

    struct NvConfig_PdtCommon
    {
		BOOL bNewSol;
        vector<double> arrDataPower;
        vector<double> arrResultPower;
		vector<double> arrDataPowerPolyFit;

		NvConfig_PdtCommon()
		{
			bNewSol = FALSE;
			arrDataPower.clear();
			arrResultPower.clear();
			arrDataPowerPolyFit.clear();
		}
    };

    struct NvConfig_TxDcCommon
    {
        BOOL bIEnable;
        BOOL bQEnable;
    };

    struct NvConfig_Irr
    {
        uint16 usRxWordIndex;
    };

    struct NvConfig_TxDc
    {
        int16 sIBegin;
        int16 sIEnd;
        int16 sQBegin;
        int16 sQEnd;
        uint8 RbCount;
        uint8 RbPosition;
        uint8 Indicator;
        uint16 usArfcn;
        uint16 usRefIndex;
        uint16 usAptIndex;
        uint16 uPowerIndex;
        uint16 usPaMode;
        int nNvIndex;
        int nBand;
        int nApt;

    };

    struct NvConfig
    {
        vector<NvConfig_Tx> arrTx;
        vector<NvConfig_Rx> arrRx;
        vector<NvConfig_Pdt> arrPdt;
        vector<NvConfig_TxDc> arrTxDc;
		vector<NvConfig_Fdiq> arrRxFdiq;
    };
#pragma endregion

#pragma region result
    struct NvResult_TxPoint
    {
        uint16 usWord;
        uint16 usDone;
        int16  sDeltaWord;
        double dPower;
        double dTargetPower;
        uint16 usResult;
    };

    struct NvResult_TxSection
    {
        vector<NvResult_TxPoint> arrPoint;

		uint16 Apt;
		uint16 StartWord;
		vector<double> MeasurePower;
    };

    struct NvResult_TxChannel
    {
        vector<NvResult_TxSection> arrSection;
    };

    struct NvResult_Tx
    {
        vector<NvResult_TxChannel> arrChannel;
    };

    struct NvResult_RxCompPoint
    {
        double dRssi;
        double dGain;
        double dBaseGain;
        double dDeltaGain;
        uint16 usResult;
    };

    struct NvResult_RxCompChannel
    {
        vector<NvResult_RxCompPoint> arrPoint;
    };

    struct NvResult_RxRefPoint
    {
        double dRssi;
        double dGain;
        uint16 usResult;
    };
    struct NvResult_Rx
    {
        vector<NvResult_RxRefPoint> arrRefPoint;
        vector<NvResult_RxCompChannel> arrCompChannel;
    };

	struct NvResult_RxCa
	{
		double dCaLna[4];
	};

    struct NvResult_PdtPoint
    {
        double dLogValue;
        uint16 usResult;
        uint16 usLinearity;

		NvResult_PdtPoint()
		{
			dLogValue = 0.0;
			usResult = 0;
			usLinearity = 0;
		}
    };

    enum NvResult_PdtLinearity
    {
        PdtLinearity_TRUE,
        PdtLinearity_BEGIN_HALF,
        PdtLinearity_END_HALF,
        PdtLinearity_FALSE
    };

    struct NvResult_PdtRange
    {
        vector<NvResult_PdtPoint> arrPoint;
        NvResult_PdtLinearity Linearity;

		NvResult_PdtRange()
		{
			arrPoint.clear();
			Linearity = PdtLinearity_TRUE;
		}
    };

    struct NvResult_PdtChannel
    {
        vector<NvResult_PdtRange> arrRange;
        BOOL bExistLinearity;

		NvResult_PdtChannel()
		{
			arrRange.clear();
			bExistLinearity = FALSE;
		}
    };

    struct NvResult_Pdt
    {
        vector<NvResult_PdtChannel> arrChannel;

		NvResult_Pdt()
		{
			arrChannel.clear();
		}
    };

    struct NvResult_AfcTcxo
    {
        uint16 Center;
        uint16 Slope;
    };

    struct NvResult_AfcDcxo
    {
        uint16 CafcSlope;
        uint16 Cadc;
        uint16 Cafc;
    };

    struct NvResult_Afc
    {
        uint16 Freq;
        NvConfig_AfcTcxo Tcxo;
        NvConfig_AfcDcxo Dcxo;
    };

    struct NvResult_Irr
    {
        int16 sAlphaAmp;
        int16 sSlopPhase;
        int16 sShiftPahse;
    };

	//FDIQ
	struct NvResult_Fdiq
	{
		int16  P[3];      
		int16  J[3];     
		int16  K[3];
		int16  C[3][9];
	};

    struct NvResult_TxDc
    {
        int16 sIIndex;
        int16 sQIndex;
    };

    struct NvResult
    {
        vector<NvResult_Tx> arrTx;
        vector<NvResult_Rx> arrRx;
        vector<NvResult_Pdt> arrPdt;
        vector<NvResult_Irr> arrIrr;
        vector<NvResult_TxDc> arrTxDc;
		vector<NvResult_Fdiq> arrFdiq;
		vector<NvResult_RxCa> arrRxCa;
    };
#pragma endregion

public:
    NvConfig_Afc m_AfcConfig;
    NvResult_Afc m_AfcResult;

    NvConfig_PdtCommon m_PdtCommonConfig[MAX_LTE_BAND];
    NvConfig_Irr m_IrrConfig;
    NvConfig_TxDcCommon m_TxDcCommonConfig;

    NvConfig m_arrNvConfig[MAX_LTE_ANT][MAX_LTE_CA];
    NvResult m_arrNvResult[MAX_LTE_ANT][MAX_LTE_CA];
};
