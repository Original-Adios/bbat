#pragma once
#include "ApiBase.h"
#include <vector>
#include "global_def.h"
#include "uni_error_def.h"
#include "cellular_def.h"
#include "ModV4Nv.h"

using namespace std;

class CNvHelperV4:
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
		int nCount2rx;
		int nCount4rx;
		int RxChannelId[MAX_LTE_RF_ANTENNA];
		int TxChannelId[MAX_LTE_RF_ANTENNA];
		bool Tx[MAX_LTE_RF_ANTENNA];
		bool Rx[MAX_LTE_RF_ANTENNA];
		int  TxAntIndx[MAX_LTE_RF_ANTENNA];
		int  RxAntIndx[MAX_LTE_RF_ANTENNA];
	};
public:
    CNvHelperV4(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNvHelperV4(void);
	SPRESULT Load();
	SPRESULT LoadClcPorComp();
	SPRESULT PreInit();

    BOOL m_bSameDownloadChecksum;

	CModV4Nv* m_pNvV4;
	NvData_HelperInfo m_arrpDownload_HelperInfo[MAX_NV_BAND_NUMBER_MV3];
	std::vector<Band_Info> m_arrBandInfo;	
	int m_nBandCount;
};
