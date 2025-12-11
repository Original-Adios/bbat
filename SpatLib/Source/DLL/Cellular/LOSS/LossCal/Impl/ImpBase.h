#pragma once
#include "SpatBase.h"
#include "wcdmaUtility.h"
#include "ScanConfig.h"
#include "ExtraLogFile.h"
#include "ComDef.h"
#include "ILossFunc.h"
#include "Phonecommand.h"



typedef struct _tag_GPS_CONFIG_T
{
	BOOL   bCheck;
	RF_ANT_E   nGPSAnt;
	_tag_GPS_CONFIG_T()
	{
		memset(this,0,sizeof(*this));
	}
}GPS_CONFIG_T;
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
	CImpBase( void );
	virtual ~CImpBase( void );
	SPRESULT CheckLoss( double dLoss, string strMode );

	RF_CABLE_LOSS_EX  m_lossVal;
	GS_MODULE_GS10 m_gs;

	BOOL    m_bC2KBandNum[BI_C_MAX_BAND];
	BOOL    m_bWcdmaBandNum[BI_W_MAX_BAND];
	BOOL    m_bTdBandNum[BI_TD_MAX_BAND];
	BOOL    m_bGsmBandNum[BI_GSM_MAX_BAND];
	BOOL    m_bLteBandNum[MAX_LTE_BAND];
	BOOL    m_bWcdmaSelected;
	BOOL    m_bC2KSelected;
	BOOL    m_bGsmSelected;
	BOOL    m_bTdSelected;
	BOOL    m_bNrSelected;
	BOOL    m_bLteSelected;
	BOOL    m_bDMRFTSelected;
	BOOL    m_bDMRCalSelected;
	//RF_ANT_E m_eC2KBandAnt[BI_C_MAX_BAND];
	RF_ANT_E m_eWcdmaBandAnt[BI_W_MAX_BAND];
	//GSM
	int     m_nGsmPcl[BI_GSM_MAX_BAND];
	//WCDMA

	double  m_dWcdmaTargetPwr;
	double  m_dLossValUpper;
	double  m_dLossValLower;
	//C2K
	double  m_dC2KTargetPwrConf;
	//TD
	double  m_dTdTargetPwr;

	//DMR
	int    m_nDmrFreqCnt;
	double m_dDmrFreq[MAX_DMR_LOSS_NUMBER];
	double m_dDmrTargetPwr;
	int16  m_nDmrPcl;

	BOOL   m_bWcnAnt;
	BOOL   m_bWlanBandNum[2][WIFI_MaxProtocol];//[Band24G&Band5G][11b/g/n/ac/a]
	BOOL   m_bWlanAnt[2][MAX_RF_ANT];//[Band24G&Band5G][ANT]
	BOOL   m_bBTBandNum[MAX_BT_TYPE];//[bdr/edr/ble]
	BOOL   m_bBTAnt[MAX_RF_ANT];//[ANT]
	GPS_CONFIG_T m_Gps_Setting[GPS_MAX_BAND];

	double m_dWlanRefLvl;
	double m_dBtRefLvl;
	BOOL   m_bWlanSelected;
	BOOL   m_bBtSelected;
	BOOL   m_bGpsSelected;
	E_MODEM_VER m_eModemVer;
	double  m_dPreLoss;

	string m_strNegativeLossDetected;
	BOOL    m_bLossCheck;
	double  m_dLossCheckSpec;
	WCN_CHIPSET_TYPE_ENUM m_eCSType;

protected:
	/// Action Step
	virtual SPRESULT   __PollAction( void );
	virtual SPRESULT   __FinalAction( void );



	SPRESULT    UpgradeLoss();
	SPRESULT    LoadFromPhone();
	SPRESULT    SaveToPhone();


	SPRESULT ChangeMode( RM_MODE_ENUM eMode );
	void ZeroLossValue(RF_CABLE_LOSS_EX& loss );
	BOOL    CheckLossDelta( const char* czModeName, RF_CABLE_LOSS_UNIT_EX* pOldLoss, RF_CABLE_LOSS_UNIT_EX* pCurLoss );

	void GetLossFmtStr(SP_MODE_INFO eMode, std::wstring& strFmt, BOOL bExp, BOOL bTX);
	void CopyBufData( int nLen, GS_RAT rat, void* pData, uint32 &nTotalLen );

	ILossFunc* m_pGsmFunc;
	ILossFunc* m_pWcdmaFunc;
	ILossFunc* m_pLteFunc;
	ILossFunc* m_pNrFunc;
	ILossFunc* m_pTdFunc;
	ILossFunc* m_pC2KFunc;
	ILossFunc* m_pDMRCalFunc;
	ILossFunc* m_pDMRFtFunc;
	ILossFunc* m_pWCNFunc;

	RF_CABLE_LOSS_EX  m_lossOrg;

	std::vector<uint8> m_arrLteInfo;
	std::vector<uint8> m_arrNrInfo;

	/// first four bytes save the length of the golden sample information
	char    m_szGSBuf[MAX_GOLDEN_SAMPLE_CUSTOMER_SIZE];    // 4*1024(4K) extended to 64k for NR by jzj
private:
	//Get magic number
	SPRESULT GetMagicType( GS_DATA_HEADER &Header );
	SPRESULT LoadFromPhonePhaseCheckArea(GS_DATA_HEADER *pHeader);
	SPRESULT LoadFromPhoneCusArea(GS_DATA_HEADER *pHeader);
	//Save to phone
	SPRESULT SaveToPhaseCheckArea( GS_DATA_HEADER *pHeader, int nTotalLen );
	SPRESULT SaveToCustomerArea( GS_DATA_HEADER *pHeader, int nTotalLen );

	/// Compatible with different versions of golden sample data definition
	void LoadFromPhone_R6();
	void LoadFromPhone_R7();
	void ConvertRawDataToGS(int nStartAddr);
	void LoadFromPhone_R8(uint32 nTotalSize);
};
