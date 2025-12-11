#pragma once
#include <vector>
#include <map>
#include <list>
#include "wcn_def.h"

using namespace std;

struct BTMeasParamChan
{
	int nCh;
	int nTxLogicCh;
	BT_PACKET ePacketType;
	BT_PATTERN ePattern;
	double dRefLvl;
	double dVsgLvl;
	DWORD  dwMask;
	int	 nAvgCount;
	int	 nTotalPackets;
	BT_RFPATH_ENUM ePath;
	int nRfSwitchAnt1st;
	int nRfSwitchAnt2nd;
	int nPacketLen;
	int nNumAntID;
	int nCteType;
	int nCteLength;
	int nSlotDurations;
	void Init()
	{
		nCh = 0;
		nTxLogicCh = 0;
		ePacketType = BDR_DH1;
		ePattern = BDR_TX_0x00;
		dRefLvl = 10.0;
		dVsgLvl = -50.0;
		dwMask = 0;
		nAvgCount = 1;
		nTotalPackets = 100;
		ePath = ANT_SINGLE;
		nRfSwitchAnt1st = 0;
		nRfSwitchAnt2nd = 0;
		nPacketLen = 10;
		nNumAntID = 2;
		nCteType = 0;
		nCteLength = 0;
		nSlotDurations = 1;
	}
	BTMeasParamChan()
	{
		Init();
	}
};


struct HCIBTMeasParamChan
{
	BT_TYPE eMode;
	int nCh;
	int nTxLogicCh;
	BT_HCI_PACKET_TYPE ePacketType;
	BT_PATTERN ePattern;
	double dRefLvl;
	double dVsgLvl;
	DWORD  dwMask;
	int	 nAvgCount;
	int	 nTotalPackets;
	BT_RFPATH_ENUM ePath;
	int nRfSwitchAnt1st;
	int nRfSwitchAnt2nd;
	int nPacketLen;
	void Init()
	{
		eMode = BDR;
		nCh = 0;
		nTxLogicCh = 0;
		ePacketType = HCI_BDR_DH1;
		ePattern = BDR_TX_0x00;
		dRefLvl = 10.0;
		dVsgLvl = -50.0;
		dwMask = 0;
		nAvgCount = 1;
		nTotalPackets = 100;
		ePath = ANT_SINGLE;
		nRfSwitchAnt1st = 0;
		nRfSwitchAnt2nd = 0;
		nPacketLen = 10;
	}
	HCIBTMeasParamChan()
	{
		Init();
	}
};

//Bluetooth meas spec
//
struct BTBDR_MeasBandSpec
{
	DOUBLE_LIMIT	dTxp;
	DOUBLE_LIMIT	dTxpShared;
	DOUBLE_LIMIT	dTsbd;
	DOUBLE_LIMIT    dTsacp[12];
	DOUBLE_LIMIT    dModF1avg;
	DOUBLE_LIMIT    dModF2max;
	DOUBLE_LIMIT    dIcft;
	DOUBLE_LIMIT    dIcftMax;
	DOUBLE_LIMIT    dIcftMin;
	DOUBLE_LIMIT    dCfd;
	DOUBLE_LIMIT    dCfdMax;
	DOUBLE_LIMIT    dCfdMin;
	double          dModRatio;
	double			dBer;
	DOUBLE_LIMIT	dRssi;
	void Init()
	{
		dTxp.Init();
		dTxpShared.Init();
		dTsbd.Init();
		for(int i=0; i<12; i++)
		{
			dTsacp[i].Init();
		}
		dModF1avg.Init();
		dModF2max.Init();
		dIcft.Init();
		dIcftMax.Init();
		dIcftMin.Init();
		dCfd.Init();
		dCfdMax.Init();
		dCfdMin.Init();
		dModRatio = 3.0;
		dBer = 1.0;
		dRssi.Init();
	}
	BTBDR_MeasBandSpec()
	{
		Init();
	}
};

struct BTEDR_MeasBandSpec
{
	DOUBLE_LIMIT	dTxp;
	DOUBLE_LIMIT	dTxpShared;	
	DOUBLE_LIMIT	dRTxp;
	DOUBLE_LIMIT    dW0;
	DOUBLE_LIMIT    dWi;
	DOUBLE_LIMIT    dRmsEvm_2DHX;
	DOUBLE_LIMIT    dPeakEvm_2DHX;
	DOUBLE_LIMIT    dP99Evm_2DHX;
	DOUBLE_LIMIT    dRmsEvm_3DHX;
	DOUBLE_LIMIT    dPeakEvm_3DHX;
	DOUBLE_LIMIT    dP99Evm_3DHX;
	DOUBLE_LIMIT    dIbse[12];
	double          dBer;
	DOUBLE_LIMIT	dRssi;
	void Init()
	{
		dTxp.Init();
		dTxpShared.Init();
		dRTxp.Init();
		dW0.Init();
		dWi.Init();
		dRmsEvm_2DHX.Init();
		dPeakEvm_2DHX.Init();
		dRmsEvm_3DHX.Init();
		dPeakEvm_3DHX.Init();
		dP99Evm_2DHX.Init();
		dP99Evm_3DHX.Init();
		for(int i=0; i<12; i++)
		{
			dIbse[i].Init();
		}
		dBer = 1.0;
		dRssi.Init();
	}
	BTEDR_MeasBandSpec()
	{
		Init();
	}
};

struct BTBLE_MeasBandSpec
{
	DOUBLE_LIMIT	dTxpAvg;
	DOUBLE_LIMIT	dTxpAvgShared;
	DOUBLE_LIMIT	dTxpPeak;
	DOUBLE_LIMIT    dIbse[12];
	DOUBLE_LIMIT    dF1Avg;
	double          dF2Max;
	double          dRatio;
	DOUBLE_LIMIT    dFnMax[RF_PHY_MAX];
	DOUBLE_LIMIT    dF0FnMax[RF_PHY_MAX];
	DOUBLE_LIMIT    dF1F0[RF_PHY_MAX];
	DOUBLE_LIMIT    dFnFn5[RF_PHY_MAX];

	DOUBLE_LIMIT    dIcftMax[RF_PHY_MAX];
	DOUBLE_LIMIT    dIcftMin[RF_PHY_MAX];
	DOUBLE_LIMIT    dCfdMax[RF_PHY_MAX];
	DOUBLE_LIMIT    dCfdMin[RF_PHY_MAX];
	double		    dBer;
	DOUBLE_LIMIT	dRssi;
	void Init()
	{
		dF2Max = 3.0;
		dRatio = 1.0;
		dBer = 1.0; 
		dTxpAvg.Init();
		dTxpAvgShared.Init();
		dTxpPeak.Init();
		for(int i=0; i<12; i++)
		{
			dIbse[i].Init();
		}
		dF1Avg.Init();
		for(int i=0; i<RF_PHY_MAX; i++)
		{
			dFnMax[i].Init();
			dF0FnMax[i].Init();
			dF1F0[i].Init();
			dFnFn5[i].Init();
			dIcftMax[i].Init();
			dIcftMin[i].Init();
			dCfdMax[i].Init();
			dCfdMin[i].Init();
		}
		dRssi.Init();
	}
	BTBLE_MeasBandSpec()
	{
		Init();
	}
};

struct BTMeasBandSpec
{
	BTBDR_MeasBandSpec stBdrSpec;
	BTEDR_MeasBandSpec stEdrSpec;
	BTBLE_MeasBandSpec stBleSpec;
	BTBLE_MeasBandSpec stBleExSpec;
	void Init()
	{
		stBdrSpec.Init();
		stEdrSpec.Init();
		stBleSpec.Init();
		stBleExSpec.Init();
	}
};

struct BTMeasParamBand
{
	BT_TYPE eMode;
	vector<BTMeasParamChan> vecBTFileParamChan;
	BTMeasBandSpec stSpec;
	void Init()
	{
		eMode = BDR;
		vecBTFileParamChan.clear();
		stSpec.Init();
	}
	BTMeasParamBand()
	{
		Init();
	}
};

struct HCIBTMeasParamBand
{
	//BT_TYPE eMode;
	vector<HCIBTMeasParamChan> vecBTFileParamChan;
	BTMeasBandSpec stSpec;
	void Init()
	{
		//eMode = BDR;
		vecBTFileParamChan.clear();
		stSpec.Init();
	}
	HCIBTMeasParamBand()
	{
		Init();
	}
};

struct BTMeasParam
{
	vector<BTMeasParamBand> stBTParamBand;
	void Init()
	{
		stBTParamBand.clear();
	}
};

struct BT_LOSS_PARAM
{
	double	m_dPreLoss;
	double	m_Tolernece;
	double m_TxLoss_Low;
	double m_TxLoss_Up;
	double m_RxLoss_Low;
	double m_RxLoss_Up;
	double m_Loss_Low;
	double m_Loss_Up;

	BT_LOSS_PARAM()
	{
		Init();
	}

	void Init()
	{

		m_dPreLoss = 0.0;
		m_Tolernece = 0.0;
		m_TxLoss_Low = 0.0;
		m_TxLoss_Up = 0.0;
		m_RxLoss_Low = 0.0;
		m_RxLoss_Up = 0.0;
		m_Loss_Low = 0.0;
		m_Loss_Up = 0.0;
	}
};