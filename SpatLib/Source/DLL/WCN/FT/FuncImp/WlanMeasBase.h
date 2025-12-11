#pragma once
#include "IWlanApi.h"
#include "../ImpBase.h"
#include "WlanCommonDef.h"
#include "LossHelper.h"
#include "CustomizedBase.h"

#define CHKRESULT_WITH_NOTIFY_WCN_ITEM(statement, Itemname)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
{                                                       \
	m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}

#define CHKRESULT_WITH_NOTIFY_WCN_ITEM_TX_OFF(statement, Itemname)  \
{										                            \
	SPRESULT __sprslt = (statement);                                \
	if (SP_OK != __sprslt)                                          \
{                                                                   \
	m_pWlanApi->DUT_TxOn(false)       ;                             \
	m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);          \
	return __sprslt;                                                \
}                                                                   \
}

#define CHKRESULT_WITH_NOTIFY_WCN_ITEM_RX_OFF(statement, Itemname)  \
{										                           \
	SPRESULT __sprslt = (statement);                               \
	if (SP_OK != __sprslt)                                           \
{                                                                 \
	m_pWlanApi->DUT_TxOn(false)       ;                \
	m_pRfTesterWlan->EndTest(0)       ;                \
	m_pImpBase->_UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}

class CWlanMeasBase
{
public:
	virtual ~CWlanMeasBase(void);
	CWlanMeasBase(CImpBase *pImpBase);//
	CWlanMeasBase() = default;

public:
	virtual SPRESULT WlanParamSet(WlanMeasParamBand *pWlanParamBand);
	virtual SPRESULT WlanLossParamSet(WLAN_LOSS_PARAM *wlanLossParam);
	virtual SPRESULT WlanMeasPerform();
	virtual SPRESULT WlanMeasLossPerform(WLAN_LOSS_PARAM wlanLossParam);

protected:
    // Measurement functions
    virtual SPRESULT MeasureUplink( SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam, SPWI_RESULT_T *pTestResult) = 0;
    virtual SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP *pDownlinkMeasParam, SPWI_RESULT_T *pRxReult) = 0;
protected:
	virtual SPRESULT ShowMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, int nChan, double dBSLevel, int nSbwType = 0, int nCbwType = 0, ANTENNA_ENUM eAnt = ANT_PRIMARY);
	virtual SPRESULT JudgeMeasRst(DWORD  dwItemMask, E_WLAN_RATE eRate, SPWI_RESULT_T *pTestResult, double dBSLevel, ANTENNA_ENUM eAnt = ANT_PRIMARY);

	void    ConfigTesterParam(SPWI_WLAN_PARAM_MEAS_GROUP *pUplinkMeasParam);

protected:
	unsigned long       m_nTxOnSleep;

	WlanMeasParamBand   *m_pWlanParamBand;
	

	ICWlanApi           *m_pWlanApi;
	IRFDevice           *m_pRfTesterWlan;
    CImpBase            *m_pImpBase;
protected:
    SPWI_WLAN_PARAM_MEAS_GROUP     m_MeasParam;
	SPWI_RESULT_T       m_rstData;
	SPWI_WLAN_PARAM_TESTER  m_stTester;
	CLossHelper m_LossHelp;
	BOOL bNeedRetry;
	//double m_dTolernece;
	//WLAN_LOSS_PARAM *m_WlanLossParam;
public:
	CCustomized m_Customize;
};
