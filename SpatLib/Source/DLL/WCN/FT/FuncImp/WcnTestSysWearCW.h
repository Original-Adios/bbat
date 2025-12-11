#pragma once
#include "WcnTestSysWearAT.h"

class CWcnTestSysWearCW : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CWcnTestSysWearCW)
public:
	CWcnTestSysWearCW(void);
	virtual ~CWcnTestSysWearCW(void);

protected:
	virtual SPRESULT __InitAction(void);
	virtual SPRESULT __PollAction(void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
	virtual SPRESULT __FinalAction(void);

protected:
	virtual SPRESULT WlanLoadXmlFile(WLAN_PARAM_CONF& stWlanParamImp);
	virtual SPRESULT BTLoadXmlFile(vector<BTMeasParamBand>& VecBTParamBandImp);

	virtual SPRESULT ConfigWlanParam();
	virtual SPRESULT WifiClearCase();
	virtual SPRESULT WifiAddCase(WLAN_PARAM_CONF& stWlanParamImp, int nTxDelay, int nEndDelay);

	virtual SPRESULT ConfigBtParam();
	virtual SPRESULT BtClearCase();
	virtual SPRESULT BtAddCase(vector<BTMeasParamBand>& VecBTParamBandImp, int nTxDelay, int nEndDelay);
	
	virtual SPRESULT WaitForReconnect(int nTimeout);
	virtual SPRESULT DoList();
	virtual SPRESULT WcnRunCase();
	virtual SPRESULT DoWlanTest();
	virtual SPRESULT DoBtTest();

private:
	CHANNEL_ATTRIBUTE m_ca;
	BOOL m_bWlan;
	BOOL m_bBt;
	UINT32 m_nWlanTx_Delay;
	UINT32 m_nWlanEnd_Delay;

	UINT32 m_nBTTx_Delay;
	UINT32 m_nBTEnd_Delay;
	UINT32 m_nTriggerTimeOut;
	UINT32 m_nConnectTimeOut;
	WLAN_PARAM_CONF m_stWlanParamImp;
	BTMeasParam   m_stBTMeasParam;
	DWORD m_dwTime;
};

