#pragma once
#include "ApiBase.h"
#include <vector>
#include "global_def.h"
#include "uni_error_def.h"
#include "cellular_def.h"
#include "Nv.h"
#include "ModV3Nv.h"

using namespace std;

class CNvHelper:
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
	};
public:
    CNvHelper(LPCWSTR lpName, CFuncCenter* pFuncCenter ,BOOL bv2);
    ~CNvHelper(void);
	SPRESULT Load();
	SPRESULT LoadClcPorComp();
	SPRESULT PreInit();

	BOOL m_bV2;
    BOOL m_bSameDownloadChecksum;
	void ApplyChecksum();
	CNv* m_pNvV2;
	CModV3Nv* m_pNvV3;
	NvData_HelperInfo m_arrpDownload_HelperInfo[MAX_NV_BAND_NUMBER_MV3];
	std::vector<Band_Info> m_arrBandInfo;	
	int m_nBandCount;
	int m_nVersion;
};
