#pragma once
#include "SpatBase.h"
#include "wcdmaUtility.h"
#include "ScanConfig.h"
#include "ExtraLogFile.h"
#include "ComDef.h"
#include "ILossFunc_UIS8910.h"

//////////////////////////////////////////////////////////////////////////
class CImpBaseUIS8910 : public CSpatBase
{
public:
    CImpBaseUIS8910(void);
    virtual ~CImpBaseUIS8910(void);
    SPRESULT CheckLoss(double dLoss, string strMode);

    RF_CABLE_LOSS_EX m_lossVal;
    RF_CABLE_LOSS_EX m_lossOrg;
    GS_MODULE_GS10 m_gs;    

    BOOL	m_bWcdmaBandNum[BI_W_MAX_BAND];
    BOOL	m_bTdBandNum[BI_TD_MAX_BAND];
    BOOL	m_bGsmBandNum[BI_GSM_MAX_BAND];
    BOOL    m_bLteBandNum[MAX_LTE_BAND];
    BOOL    m_bWcdmaSelected;
    BOOL    m_bGsmSelected;
    BOOL    m_bTdSelected;
    BOOL    m_bLteSelected;
	BOOL	m_bDMRFTSelected;
	BOOL	m_bDMRCalSelected;
    RF_ANT_E m_eWcdmaBandAnt[BI_W_MAX_BAND];
    //GSM
    int     m_nGsmPcl[BI_GSM_MAX_BAND];
    //WCDMA 
    double  m_dWcdmaTargetPwr;
    double  m_dLossValLower;
    double  m_dLossValUpper;
    double  m_dLossValLower_bk;
    double  m_dLossValUpper_bk;
    std::vector<LOSS_LIMIT> m_vecLossLimit;
	//TD
	 double  m_dTdTargetPwr;

	 //DMR
	 int	m_nDmrFreqCnt;
	 double m_dDmrFreq[MAX_DMR_LOSS_NUMBER];
	 double m_dDmrTargetPwr;
	 int16	m_nDmrPcl;
	 
	 BOOL   m_bWcnAnt;
	 BOOL   m_bWlanBandNum[2][WIFI_MaxProtocol];//[Band24G&Band5G][11b/g/n/ac/a]
	 BOOL   m_bWlanAnt[2][MAX_RF_ANT];//[Band24G&Band5G][ANT]
	 BOOL   m_bBTBandNum[MAX_BT_TYPE];//[bdr/edr/ble]
	 BOOL   m_bBTAnt[MAX_RF_ANT];//[ANT]
	 BOOL   m_bGPSBandNum;

	 double m_dWlanRefLvl;
	 double m_dBtRefLvl;
	 BOOL   m_bWlanSelected;
	 BOOL   m_bBtSelected;
	 BOOL   m_bGpsSelected;
     int    m_Uetype;

    BOOL    m_bLossCheck;
    double  m_dLossCheckSpec;

    string m_strNegativeLossDetected;

    virtual void GetLossLimit(const double dFreq, double& lowerLimit, double& upperLimit);
    void LossLowerLimitSearch(int iStartIndex,const double& upperLimit, double& lowerLimit);
protected:
    /// Action Step
    virtual SPRESULT   __PollAction(void);
    virtual SPRESULT   __FinalAction(void);
    
    
    
    SPRESULT    UpgradeLoss();
    SPRESULT    LoadFromPhone();
    SPRESULT    SaveToPhone();
    
    SPRESULT ChangeMode(RM_MODE_ENUM eMode);
    void ZeroLossValue(RF_CABLE_LOSS_EX&loss);
    BOOL    CheckLossDelta( const char* czModeName, RF_CABLE_LOSS_UNIT_EX* pOldLoss, RF_CABLE_LOSS_UNIT_EX* pCurLoss );
    

    ILossFunc_UIS8910 *m_pGsmFunc;
    //ILossFunc_UIS8910 *m_pWcdmaFunc;
    ILossFunc_UIS8910 *m_pLteFunc;
    //ILossFunc_UIS8910 *m_pTdFunc;
	//ILossFunc_UIS8910 *m_pDMRCalFunc;
	//ILossFunc_UIS8910 *m_pDMRFtFunc;
	ILossFunc_UIS8910 *m_pWCNFunc;
private:
    /// first four bytes save the length of the golden sample information
    char    m_szGSBuf[MAX_GOLDEN_SAMPLE_SIZE];    // 4*1024(4K)
};
