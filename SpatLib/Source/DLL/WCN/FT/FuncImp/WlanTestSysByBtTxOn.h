#pragma once

#include "../ImpBase.h"
#include "WlanAntSwitch.h"
#include "BTMeasBase.h"
#include "BTEnterMode.h"
#include "BTApiAT.h"
#include "BTRFPathSwitch.h"
#include "WlanApiAT.h"

using namespace std;

#define CHKRESULT_WITH_WIFI_BY_BTINF_POWER_OFF(statement)    {   								\
                                    SPRESULT __sprslt = (statement);				\
                                    if (SP_OK != __sprslt)							\
                                    {												\
                                        CHKRESULT(m_pBtApi->DUT_RFOn(BT_TX, false));\
                                        return __sprslt;							\
                                    }												\
                                }

#define CHKRESULT_WITH_NOTIFY_WIFI_BY_BTINF_POWER_OFF(statement, Itemname)    {   				\
                                    SPRESULT __sprslt = (statement);				\
                                    if (SP_OK != __sprslt)							\
                                    {												\
                                        CHKRESULT(m_pBtApi->DUT_RFOn(BT_TX, false));\
                                        _UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1); \
                                        return __sprslt;							\
                                    }												\
                                }

class CWlanTestSysByBtTxOn : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWlanTestSysByBtTxOn)
protected:
    CWlanTestSysByBtTxOn(void);//
    virtual ~CWlanTestSysByBtTxOn(void);

    virtual SPRESULT __InitAction(void);
    virtual SPRESULT __PollAction(void);
    virtual void     __LeaveAction(void);
    virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
    SPRESULT BT_Tx_On(void);
    SPRESULT BT_Tx_Off(void);
    SPRESULT WlanMeasPerform(void* vecParam);
    SPRESULT TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pDownlinkMeasParam, SPWI_RESULT_T* pRxReult);
    void ConfigTesterParam(SPWI_WLAN_PARAM_MEAS_GROUP* pMeasParam);
    SPRESULT BT_TxOn_TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pMeasParam, WlanMeasBandSpec* pSpec);
    SPRESULT BT_TxOff_TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pMeasParam, WlanMeasBandSpec* pSpec);

private:
    WLAN_PARAM_CONF m_stWlanParamImp;
    BTMeasParamChan btChanParam;
    SPWI_WLAN_PARAM_MEAS_GROUP     m_MeasParam;
    SPWI_RESULT_T       m_rstData;
    SPWI_WLAN_PARAM_TESTER  m_stTester;


    ICBTApi* m_pBtApi;
    CBTEnterMode* m_pBtEnterMode;
    CBTRFPathSwitch* m_pRfPathSwitch;
    CWlanEnterMode* m_pEnterMode;
    CWlanAntSwitch* m_pWlanAnt;
    ICWlanApi* m_pWlanApi;
    int MAX_MEAS_COUNT;
    int NonINFSleep;
};
