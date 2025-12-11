#pragma once
#include "global_def.h"
#include "cellular_def.h"

#pragma pack(push, 1)

struct LTE_Loss_Data_Head
{
	uint16 usFreq;
	uint32 uArfcn;
	uint8 byBand;
	uint8 byPath;
	LTE_Loss_Data_Head()
	{
		Init();
	}
	void Init()
	{
		usFreq = 0;
		uArfcn = 0;
		byBand = 0;
		byPath = 0;
	}
};

struct LTE_Loss_Data_TX
{
    LTE_Loss_Data_Head Head;
    uint16 usWord;
    uint16 usPower;
	LTE_Loss_Data_TX()
	{
		usWord = 0;
		usPower = 0;
	}
	void Init()
	{
		Head.Init();
		usWord = 0;
		usPower = 0;
	}
};

struct LTE_Loss_Data_RX
{
    LTE_Loss_Data_Head Head;
    uint16 usRssi;
    uint16 usPower;
    uint8 byIndex;
	LTE_Loss_Data_RX()
	{
		usRssi = 0;
		usPower = 0;
		byIndex = 0;
	}
	void Init()
	{
		Head.Init();
		usRssi = 0;
		usPower = 0;
		byIndex = 0;
	}
};

struct LTE_Loss_Data
{
    LTE_ANT_E ANT;
    int nPath;
    LTE_Loss_Data_RX RxData;
    LTE_Loss_Data_TX TxData;
	LTE_Loss_Data()
	{
		ANT = LTE_ANT_MAIN;
		nPath = 1;
	}
	void Init()
	{
		ANT = LTE_ANT_MAIN;
		nPath = 1;
		RxData.Init();
		TxData.Init();
	}
};

struct LTE_Loss_Data_Head_V4
{
	uint16 usFreq;
	uint32 uChannel;
	uint8 byBand;
	uint8 byChannelId;
	uint8 byPath;
	LTE_Loss_Data_Head_V4()
	{
		Init();
	}
	void Init()
	{
		usFreq = 0;
		uChannel = 0;
		byBand = 0;
		byChannelId = 0;
		byPath = 0;
	}
};

struct LTE_Loss_Data_Head_V5
{
	uint16 usFreq;
	uint32 uChannel;
	uint8 byBand;
	uint32 PathId;
	uint8 PathGroupIndex;
	uint8 TriggerStream;
	uint8 byPath;
	LTE_Loss_Data_Head_V5()
	{
		Init();
	}
	void Init()
	{
		usFreq = 0;
		uChannel = 0;
		byBand = 0;
		PathId = 0;
		PathGroupIndex = 0;
		byPath = 0;
	}
};

struct LTE_Loss_Data_TX_V4
{
	LTE_Loss_Data_Head_V4 Head;
	uint16 usWord;
	uint16 usPower;
	LTE_Loss_Data_TX_V4()
	{
		usWord = 0;
		usPower = 0;
	}
	void Init()
	{
		Head.Init();
		usWord = 0;
		usPower = 0;
	}
};

struct LTE_Loss_Data_TX_V5
{
	LTE_Loss_Data_Head_V5 Head;
	uint16 usWord;
	uint16 usPower;
	uint16 usVoltage;
	LTE_Loss_Data_TX_V5()
	{
		usWord = 0;
		usPower = 0;
		usVoltage = 0;
	}
	void Init()
	{
		Head.Init();
		usWord = 0;
		usPower = 0;
		usVoltage = 0;
	}
};


struct LTE_Loss_Data_RX_V4
{
	LTE_Loss_Data_Head_V4 Head;
	uint16 usRssi;
	uint16 usPower;
	uint8 byIndex;
	LTE_Loss_Data_RX_V4()
	{
		usRssi = 0;
		usPower = 0;
		byIndex = 0;
	}
	void Init()
	{
		Head.Init();
		usRssi = 0;
		usPower = 0;
		byIndex = 0;
	}
};

struct LTE_Loss_Data_RX_V5
{
	LTE_Loss_Data_Head_V5 Head;
	uint16 usRssi;
	uint16 usPower;
	uint8 byIndex;
	LTE_Loss_Data_RX_V5()
	{
		usRssi = 0;
		usPower = 0;
		byIndex = 0;
	}
	void Init()
	{
		Head.Init();
		usRssi = 0;
		usPower = 0;
		byIndex = 0;
	}
};

struct LTE_Loss_Data_V4
{
	LTE_RF_ANTENNA_E ANT;
	int nPath;
	LTE_Loss_Data_RX_V4 RxData;
	LTE_Loss_Data_TX_V4 TxData;
	LTE_Loss_Data_V4()
	{
		ANT = LTE_RF_ANT_MAIN;
		nPath = 1;
	}
	void Init()
	{
		ANT = LTE_RF_ANT_MAIN;
		nPath = 1;
		RxData.Init();
		TxData.Init();
	}
};

struct LTE_Loss_Data_V5
{
	LTE_RF_ANTENNA_E ANT;
	int nPath;
	LTE_Loss_Data_RX_V5 RxData;
	LTE_Loss_Data_TX_V5 TxData;
	LTE_Loss_Data_V5()
	{
		ANT = LTE_RF_ANT_MAIN;
		nPath = 1;
	}
	void Init()
	{
		ANT = LTE_RF_ANT_MAIN;
		nPath = 1;
		RxData.Init();
		TxData.Init();
	}
};

struct NR_Loss_Data
{
	int nBand;
	uint32 uArfcn;
	double dFreq;
	BOOL bTx;
	int nParam;
	double dPower;
	RF_ANT_E RfAnt;
	int nAnt;
	double dResult;

	NR_Loss_Data()
	{
		memset(this, 0, sizeof NR_Loss_Data);
	}
};

#pragma pack(pop)