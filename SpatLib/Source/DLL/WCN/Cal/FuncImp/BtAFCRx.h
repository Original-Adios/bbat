#pragma once
#include "impbase.h"
#include "IBTApi.h"
#include "GPSApiAT.h"
//#include "IWlanApi.h"

#define ShareMemory_TSX_CONFIG  L"FF69F2C3-205D-4F91-89EE-1FB224D66C26_TSX_CONFIG"
#define     TF01        0x54463031

typedef struct _tagTSX_CAL_PARAM
{
	double dS1S2Delta;
	double dS2S3Delta;
	double dS2S3Delta_2;
	double dS3S4Delta;


	double dC1TsxLow;
	double dC1TsxUpp;
	double dC1OscLow;
	double dC1OscUpp;

	double dFerLimit;

	uint16 nStartDac;
	uint16 nEndDac;
	uint32 nStep2TimeOut;
	uint32 nStep3TimeOut;
	uint32 nStep4TimeOut;

	BT_RFPATH_ENUM ePath;
	STRUCT_INITIALIZE(_tagTSX_CAL_PARAM);
}TSX_CAL_PARAM_T;

typedef struct _tagSHARE_TSX_CONF
{
	CRYSTAL_TYPE eCrystal;
	TSX_DATA_V2 stTsxData[4];
	WCN_CAL_DATA_V1 stWcnCalData;
	BOOL bStep[5];
	TSX_CAL_PARAM_T stParam;
	STRUCT_INITIALIZE(_tagSHARE_TSX_CONF);
}SHARE_TSX_CONF_T;

class CBtAFCRx :
	public CImpBase
{
	DECLARE_RUNTIME_CLASS(CBtAFCRx)
public:
	CBtAFCRx(void);
	~CBtAFCRx(void);
protected:
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __FinalAction(void);

	SPRESULT AFC_Step0(SHARE_TSX_CONF_T &stTsxConf);
	SPRESULT AFC_Step1(SHARE_TSX_CONF_T &stTsxConf);
	SPRESULT AFC_Step2(SHARE_TSX_CONF_T &stTsxConf);
	SPRESULT AFC_Step3(SHARE_TSX_CONF_T &stTsxConf);
	SPRESULT AFC_Step4(SHARE_TSX_CONF_T &stTsxConf);

	SPRESULT GetTemp(double &dTsxTmp, double &dOscTmp);

	ICBTApi* m_pBtApi;
	CGPSApiAT* m_pGpsApi;
//	ICWlanApi* m_pWlanApi;

	TSX_CAL_PARAM_T m_stCalParam;
	BOOL m_bFinal;
	BOOL m_bStart;
	BOOL m_bTSX;
};

