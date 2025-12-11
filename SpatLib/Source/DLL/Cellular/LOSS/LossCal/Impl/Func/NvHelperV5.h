#pragma once
#include "ApiBase.h"
#include <vector>
#include "global_def.h"
#include "uni_error_def.h"
#include "cellular_def.h"
#include "ModV5Nv.h"

using namespace std;

class CNvHelperV5:
	public CFuncBase
{
public:
	struct NvData_HelperInfo
	{
		uint16 Band;
		uint16 DivDisable;
		uint16 RxCa;
		uint16 TxCa;
	};

	struct Band_Info
	{
		LTE_BAND_E nBand;
		int nNumeral;
		int nIndicator;
		//int nCount2rx;
		//int nCount4rx;
		//int n2RxPathGroup;
		//int n2RxPath0Channelid;
		//int n4RxPath0Channelid;
		int nTxCount;
		uint32 nTxPathId;
		uint16 nTxPathGroupIndex;

		int nRxCount;
		uint32 nRxPathId[4];
		uint16 nRxPathGroupIndex[4];
	
		uint32 nTriggerPathId;
		uint8  nTriggerUpLinkStream;
		uint16 nTriggerGroupIndex;
		bool Tx[MAX_LTE_RF_ANTENNA];
		bool Rx[MAX_LTE_RF_ANTENNA];
	};
public:
    CNvHelperV5(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNvHelperV5(void);
	SPRESULT Load();
	SPRESULT LoadClcPorComp();
	SPRESULT ReadBandList();
	SPRESULT PreInit();

    BOOL m_bSameDownloadChecksum;

	CModV5Nv* m_pNvV5;
	NvData_HelperInfo m_arrpDownload_HelperInfo[MAX_NV_BAND_NUMBER_MV3];
	std::vector<Band_Info> m_arrBandInfo;	
	int m_nBandCount;
};
