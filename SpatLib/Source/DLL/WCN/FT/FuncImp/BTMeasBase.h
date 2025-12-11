#pragma once
#include <vector>
#include <assert.h>
//#include <map>
#include "IBTApi.h"
#include "../ImpBase.h"
#include "BtCommonDef.h"
#include "LossHelper.h"

#include "CustomizedBase.h"
using namespace std;

#define CHKRESULT_WITH_NOTIFY_WCN_ITEM(statement, Itemname)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
{                                                       \
	m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}	


#define CHKRESULT_WITH_BT_POWER_OFF(statement)    {   								\
									SPRESULT __sprslt = (statement);				\
									if (SP_OK != __sprslt)							\
									{												\
										m_pBTApi->DUT_RFOn(BT_TX, false);\
                                        m_pRfTesterBT->EndTest(0);       \
										return __sprslt;							\
									}												\
								}

#define CHKRESULT_WITH_NOTIFY_BT_POWER_OFF(statement, Itemname)    {   				\
									SPRESULT __sprslt = (statement);				\
									if (SP_OK != __sprslt)							\
									{												\
										m_pBTApi->DUT_RFOn(BT_TX, false);\
										m_pRfTesterBT->EndTest(0);       \
										m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1); \
										return __sprslt;							\
									}												\
								}
                                
#define CHKRESULT_WITH_BT_GEN_OFF(statement)    {   								\
									SPRESULT __sprslt = (statement);				\
									if (SP_OK != __sprslt)							\
									{												\
										m_pRfTesterBT->EndTest(0);       \
										return __sprslt;							\
									}												\
								}

#define CHKRESULT_WITH_NOTIFY_BT_RX_OFF(statement, Itemname)    {   				\
									SPRESULT __sprslt = (statement);				\
									if (SP_OK != __sprslt)							\
									{												\
										m_pBTApi->DUT_RFOn(BT_RX, false);\
										m_pRfTesterBT->EndTest(0);       \
										m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1); \
										return __sprslt;							\
									}												\
								}



class CBTMeasBase
{
public:
	virtual ~CBTMeasBase(void);
	CBTMeasBase(CImpBase *pImpBase, ICBTApi *pBtApi);//

public:
	virtual SPRESULT BTParamSet(BTMeasParamBand *pBTParamBand);
	virtual SPRESULT BTMeasPerform();
	virtual SPRESULT BTMeasLossPerform(BT_LOSS_PARAM BtLossParm);
protected:
    // Measurement functions
    virtual BOOL MeasureUplink(BTMeasParamChan *pUplinkMeasParam, SPBT_RESULT_T *pTestResult) = 0;
    virtual BOOL TestPER(BTMeasParamChan *pDownlinkMeasParam, SPBT_RESULT_T* pRxReult) = 0;

	void    ConfigTesterParam( BTMeasParamChan *pParam);
protected:
    SPRESULT TxSpecialPatternMeas(DWORD dwMask, SPWI_BT_PARAM_TESTER *pstRfTester, SPBT_RESULT_T *pTestResult);
	SPRESULT TxCWMeas(DWORD dwMask, SPWI_BT_PARAM_TESTER* pstRfTester, SPBT_RESULT_T* pTestResult);
	SPRESULT BDTxSpecialPatternMeas(DWORD dwMask, SPWI_BT_PARAM_TESTER* pstRfTester, SPBT_RESULT_T* pTestResult);
	
private:
    SPRESULT ShowMeasRst(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType = RF_PHY_1M);
    SPRESULT JudgeMeasRst(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType = RF_PHY_S8, double dBSLevel = -70);

    SPRESULT _JudgeBleRslt(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, BT_RFPATH_ENUM ePath, double dBSLevel);
	SPRESULT _JudgeBleExRslt(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType, double dBSLevel);
    SPRESULT _JudgeBdrRslt(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, BT_RFPATH_ENUM ePath, double dBSLevel);
    SPRESULT _JudgeEdrRslt(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType, double dBSLevel);

    SPRESULT _ShowMeasRstBdr(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);
    SPRESULT _ShowMeasRstEdr(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);
    SPRESULT _ShowMeasRstBle(DWORD  dwItemMask, SPBT_RESULT_T *pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);
	SPRESULT _ShowMeasRstBleEx(DWORD dwItemMask, SPBT_RESULT_T *pTestResult, int nChan,double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);

protected:
	BTMeasParamBand     *m_pBTParamBand;

	SPWI_BT_PARAM_TESTER m_stTester;
	SPBT_RESULT_T		 m_stRstData;


	ICBTApi				*m_pBTApi;
	IRFDevice			*m_pRfTesterBT;
    CImpBase            *m_pImpBase;
	CLossHelper m_LossHelp;
	BOOL bNeedRetry;
	//double m_dTolernece;
	//WLAN_LOSS_PARAM* m_WlanLossParam;
	//BT_LOSS_PARAM* m_BtLossParm;

public:
	CCustomized m_Customize;
};
