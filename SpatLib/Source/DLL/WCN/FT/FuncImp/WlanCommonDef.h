#pragma once
#include <vector>
#include <map>
#include <list>
#include "wcn_def.h"

using namespace std;

typedef struct _tag_WLAN_CHANNEL_T
{
	int		nCenChan;
	int		nPriChan;
	void Init()
	{
		nCenChan = 1;
		nPriChan = 1;
	}
	_tag_WLAN_CHANNEL_T()
	{
		Init();
	}
}WLAN_CHANNEL_T;

struct WlanMeasParamGroup_Sub
{
	ANTENNA_ENUM eAnt;
	int nRfSwitchAnt1st;
	int nRfSwitchAnt2nd;
	int nCBWType;
	int nSBWType;
	int nChCode;
	int nHeLtfSize;
	int nGuardInterval;
	int nRuSize;
	E_WLAN_RATE eRate;
	double dRefLvl;
	BOOL	bSetPwrLvl;
	double dPwrLvl;
	double dVsgLvl;
	double dRxMinLvl;
	double dRxMaxLvl;
	double dRxProbeStep;
	DWORD  dwMask;
	int	nTotalPackets;
	int	nAvgCount;
	int nTxSleep;
	void Init()
	{
		eAnt = ANT_PRIMARY;
		nRfSwitchAnt1st = 0;
		nRfSwitchAnt2nd = 0;
		nCBWType = 0;
		nSBWType = 0;
		nChCode = 0;
		eRate = DSSS_1;
		dRefLvl = 20.0;
		bSetPwrLvl = FALSE;
		dPwrLvl = 0;
		dVsgLvl = -50.0;
		dRxMinLvl = -60.0;
		dRxMaxLvl = -20.0;
		dRxProbeStep = 1.0;
		dwMask = 0;
		nTotalPackets = 100;
		nAvgCount = 1;
		nTxSleep = 0;
		nHeLtfSize = 1;
		nGuardInterval = 0;
		nRuSize = 26;
	}
	WlanMeasParamGroup_Sub()
	{
		Init();
	}
};

typedef struct
{
	vector<WLAN_CHANNEL_T> vectChan;
	WlanMeasParamGroup_Sub stAlgoParamGroupSub;
	void Init()
	{
		vectChan.clear();
		stAlgoParamGroupSub.Init();
	}
} SPWI_WLAN_PARAM_GROUP;

typedef struct WLAN_LOSS_PARAM
{
	double	m_dPreLoss;
	double	m_Tolernece;
	double	m_dPreLoss_5G;
	double	m_Tolernece_5G;
	double m_TxLoss_Low;
	double m_TxLoss_Up;
	double m_RxLoss_Low;
	double m_RxLoss_Up;
	double m_Loss_Low;
	double m_Loss_Up;

	WLAN_LOSS_PARAM()
	{
		Init();
	}
	void Init()
	{

		m_dPreLoss = 0.0;
		m_Tolernece = 0.0;
		m_dPreLoss_5G = 0.0;
		m_Tolernece_5G = 0.0;
		m_TxLoss_Low = 0.0;
		m_TxLoss_Up = 0.0;
		m_RxLoss_Low = 0.0;
		m_RxLoss_Up = 0.0;
		m_Loss_Low = 0,0;
		m_Loss_Up = 0,0;
	}
};

typedef struct _tag_SPWI_WLAN_PARAM_MEAS_GROUP
{
	WIFI_PROTOCOL_ENUM eMode;
	WLAN_CHANNEL_T stChan;
	WlanMeasParamGroup_Sub stTesterParamGroupSub;
	void Init()
	{
		eMode = WIFI_802_11b;
		stChan.Init();
		stTesterParamGroupSub.Init();
	}
	_tag_SPWI_WLAN_PARAM_MEAS_GROUP()
	{
		Init();
	}
} SPWI_WLAN_PARAM_MEAS_GROUP;

//WCN Meas spec
typedef struct _tag_WlanMeasBandSpec
{
	DOUBLE_LIMIT	dTxp;
	DOUBLE_LIMIT	dTXPant2nd;

	DOUBLE_LIMIT	dTxpRate[MAX_WLAN_RATE];;
	DOUBLE_LIMIT	dTXPant2ndRate[MAX_WLAN_RATE];
	DOUBLE_LIMIT	dEvm[MAX_WLAN_RATE];
	DOUBLE_LIMIT    dFer;
	DOUBLE_LIMIT    dIQ;
	double			dPer;
	double			dLvlGap;
	double          dSpecMaskMargin[8];
	DOUBLE_LIMIT    dRssi;
	void Int()
	{
		dTxp.Init();
		dTXPant2nd.Init();
		for(int i=0; i<MAX_WLAN_RATE; i++)
		{
			dTxpRate[i].Init();
			dTXPant2ndRate[i].Init();
			dEvm[i].Init();
		}
		dFer.Init();
		dIQ.Init();
		dPer = 10.0;
		dLvlGap = 3.0;
		for(int i=0; i<8; i++)
		{
			dSpecMaskMargin[i] = 0.0;
		}
		dRssi.Init();
	}
	_tag_WlanMeasBandSpec()
	{
		Int();
	}
}WlanMeasBandSpec;

struct WlanMeasParamBand
{
	WLAN_BAND_ENUM eBand;
	WIFI_PROTOCOL_ENUM eMode;
	vector<SPWI_WLAN_PARAM_GROUP> vecConfParamGroup;
	WlanMeasBandSpec stSpec;
	double			m_dPreLoss;
	double			m_Tolernece;
	void Init()
	{
		eMode = WIFI_802_11b;
		eBand = WLAN_BAND_24G;
		vecConfParamGroup.clear();
		stSpec.Int();
		m_dPreLoss = 0.0;
		m_Tolernece = 0.0;
	}
	WlanMeasParamBand()
	{
		Init();
	}
};

typedef struct _WLAN_PARAM_CONF
{
	vector<WlanMeasParamBand>   VecWlanParamBandImp;
	void Init()
	{
		VecWlanParamBandImp.clear();
	}
}WLAN_PARAM_CONF;
