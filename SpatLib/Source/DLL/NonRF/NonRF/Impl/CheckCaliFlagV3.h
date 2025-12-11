#pragma once
#include "ImpBase.h"
#include "SpatBase.h"
#include "global_def.h"
#include "NrUtility.h"
#include "LteUtility.h"
using namespace std;

#define MAX_NV_BAND_NUMBER_MV3 23

#define V3_LTE_CALI_FLAG 0x01
#define V3_LTE_FT_FLAG 0x100
#define V3_LTE_ANT_FLAG 0x10000
#define V3_NR_CALI_FLAG 0xFFFF
#define V3_NR_FT_FLAG 0x00010000
#define V3_NR_ANT_FLAG 0x00020000

//////////////////////////////////////////////////////////////////////////
class CCheckCaliFlagV3 : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CCheckCaliFlagV3)
public:
	CCheckCaliFlagV3(void);
	virtual ~CCheckCaliFlagV3(void);

	typedef enum
	{
		FLAGMODE_GSM,
		FLAGMODE_TD,
		FLAGMODE_WCDMA,
		FLAGMODE_ANTENNA,
	}FLAG_MODE_E;

	typedef enum
	{
		PCS_CALI_AGC = 0,
		PCS_CALI_APC,
		DCS_CALI_AGC,
		DCS_CALI_APC,
		EGSM_CALI_AGC,
		EGSM_CALI_APC,
		GSM850_CALI_AGC,
		GSM850_CALI_APC,
		PCS_FINAL,
		DCS_FINAL,
		EGSM_FINAL,
		GSM850_FINAL,
		EDGE_CALI,
		EDGE_FINAL,
		TD_CALI_APC,
		TD_CALI_AGC,
		TD_FINAL,
		W_CALI_BANDI,
		W_CALI_BANDII,
		W_CALI_BANDIII,
		W_CALI_BANDIV,
		W_CALI_BANDV,
		W_CALI_BANDVI,
		W_CALI_BANDVIII,
		W_CALI_BANDXIX,
		WCDMA_FINAL,
		ANTENNA_FLAG,
		FLAG_INDEX_MAX
	}FLAG_Index;
	struct FlagInfo {
		FLAG_Index  flagIndex;
		FLAG_MODE_E flagMode;   // GSM/W/T
		LPCWSTR     NameW;
		LPCSTR      NameA;
		int         FlagMask;
		int         nBit;
	};

protected:
	virtual SPRESULT __InitAction();
	virtual BOOL LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

private:
	SPRESULT CheckCaliFlag();
	int* GetFlag(FLAG_MODE_E flagmode);
	virtual SPRESULT RunAfcCheckFlag(BOOL& bCheckAllPass);
	virtual SPRESULT RunLteCheckFlag(BOOL& bCheckAllPass);
	virtual SPRESULT RunNrCheckFlag(BOOL& bCheckAllPass);

private:
	static FlagInfo m_FlagInfo[FLAG_INDEX_MAX];
	int*  m_pCalFlag;
	int  m_nGSMFlag;
	int	 m_nTDFlag;
	int  m_nWCDMAflag;
	int	 m_nAntennaFlag;


public:
	BOOL m_bFlagAFC;	
	BOOL m_bFlagNRAuto;
	BOOL m_bFlagLTEAuto;

	std::vector<LPCWSTR> m_vecFlagLTECali;
	std::vector<LPCWSTR> m_vecFlagLTEFT;
	std::vector<LPCWSTR> m_vecFlagLTEAnt;
	std::vector<LPCWSTR> m_vecFlagNRCali;
	std::vector<LPCWSTR> m_vecFlagNRFT;
	std::vector<LPCWSTR> m_vecFlagNRAnt;
};
