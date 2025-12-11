#pragma once
#include <vector>
#include "cellular_def.h"

using namespace std;

class IFileConfig
{
public:
    struct TxBandInfo
    {
        vector<uint32> arrChannel;
        vector<uint16> arrWord[MAX_LTE_CA];
        double dMaxPower[MAX_LTE_CA];
        double dMinPower[MAX_LTE_CA];
        //mgb
        double dSvchPower[2];
        int MinWord;
        int MaxWord;
        BOOL Enable_Sar = FALSE;
        BOOL Enable_PdtCheck = FALSE;
    };

    struct AgcSpec
    {
        double dMinLowerSpec;
        double dMinUpperSpec;
        double dMaxLowerSpec;
        double dMaxUpperSpec;
        double dCompensationLowerSpec;
        double dCompensationUpperSpec;
        double dCompensationWifiLowerSpec;
        double dCompensationWifiUpperSpec;
    };

	struct FdiqSpec
	{
		double			dFdiqLowLimit_P;
		double			dFdiqHighLimit_P;
		double			dFdiqLowLimit_G;
		double			dFdiqHighLimit_G;
		double			dFdiqLowLimit_K;
		double			dFdiqHighLimit_K;
		double			dFdiqLowLimit_F[9];
		double			dFdiqHighLimit_F[9];
	};

    struct RxBandInfo
    {
        vector<uint32> arrChannel;
        uint32 usRefChannel;
		uint32 usRefChannelIrr;
        vector<double> arrPower[MAX_LTE_ANT][MAX_LTE_CA];
        AgcSpec Spec[MAX_LTE_ANT][MAX_LTE_CA];
    };

    struct TxCommonInfo
    {
        double dScanTolerance;
        double dPowerSpec;
        BOOL   bMainEnable;
        BOOL   bDivEnable;
        BOOL   bPccEnable;
        BOOL   bSccEnable;
        BOOL   bPdtEnable;
		BOOL   bPdtPolyFitEnable;
        BOOL   bDcEnable;
        BOOL   bInternal;
        BOOL   bSmartSection;
        BOOL   bInternalSimulate;
        int    nCalcPower;
        BOOL   bCalcPower;
        int    nExPwrCount;
        uint32 unSmartSectionCount;
        double dSatPwrSpec;
        uint32 unPwrCtrWord;
        int    nTriggerTxWord;
        BOOL   bPdtSarEnable;
    };

    struct RxCommonInfo
    {
        uint16 usIrrWordIndex;
        int    nMaxRssi;
        int    nMinRssi;
        int    nScanTarget;
        BOOL   bMainEnable;
        BOOL   bDivEnable;
        BOOL   bPccEnable;
        BOOL   bSccEnable;
        BOOL   bIrrEnable;
        BOOL   bCheckLinearity;
        BOOL   bAccurate;
        BOOL   bRfRdMode;
        BOOL   bReadNvSpec;
        double dScanTolerance;
        double dSpecTolerance;
        double dMaxCellPower;
        double dMinCellPower;
        uint16 unRetryCount;
        BOOL	bAgcLinCheck;
        double dAgcLineSpecLow;
        double dAgcLineSpecHigh;
        double dAgcOverLappedSpecLow;
        double dAgcOverLappedSpecHigh;
        int    nGainDelta;
        int    nOverlapTarget;
        int    nOverlapdeltaLimit;
        BOOL    bRxJamSigScan;
        int    nTriggerTxWord;
		//mgb
		double dSpecTolCompCh;
		uint16 nCompChGainLow;
		uint16 nCompChGainMid;
		uint16 nCompChGainHigh;
		uint16 nBypassLnaGain;
		int    nBypassLimtLow;
		int    nBypassLimtHigh;
		int	   nAfcGainIndex;
		BOOL   bPowerDivider;
		//Fdiq
		FdiqSpec stFdiqSpec;
		BOOL	bAgcLna4;
		//Ca
		BOOL	bAgcCaEnable;
		BOOL	bAgcCaNonEn;
		double	dCaMin;
		double	dCaMax;
    };

    struct CommonInfo
    {
        BOOL bSaveLog;
		BOOL bSaveHdetNvmLog;
        BOOL bScan;
        BOOL bRx;
        BOOL bTx;
        int  nStartCellPower;
		//mgb
		BOOL	bIndoor;
		BOOL	bOutdoor;
    };

public:
    TxBandInfo   m_TxBandInfo[MAX_LTE_BAND];
    RxBandInfo   m_RxBandInfo[MAX_LTE_BAND];
    TxCommonInfo m_TxCommonInfo;
    RxCommonInfo m_RxCommonInfo;
    CommonInfo   m_CommonInfo;
    BOOL m_arrBandEnable[MAX_LTE_BAND];
    BOOL m_bTestBandFromNV[MAX_LTE_BAND];
    BOOL m_bAutoBand;
    RF_ANT_E m_PriAnt;
    RF_ANT_E m_DivAnt;
};
