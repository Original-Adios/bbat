#pragma once
#include "WcnTestSysWearAT.h"

class CWcnTestSysWearATEx :
	public CWcnTestSysWearAT
{
	DECLARE_RUNTIME_CLASS(CWcnTestSysWearATEx)
public:
	CWcnTestSysWearATEx(void);
	virtual ~CWcnTestSysWearATEx(void);

protected:
	virtual BOOL WlanLoadXML();
	virtual SPRESULT ConfigWlanParam();
	virtual SPRESULT WifiAddCase(int nChanl, E_WLAN_RATE eRate, ANTENNA_ENUM chain, int cbw, int sbw, int nStartDelay, int nTxDelay, int nRxDelay, int nEndDelay);

	virtual SPRESULT ConfigBtParam();
	virtual SPRESULT BtAddCase(BT_TYPE eModeType, int nChan, BT_PACKET ePacketType, BT_PATTERN ePattern, int nStartDelay, int nTxDelay, int nRxDelay, int nEndDelay);
	virtual SPRESULT WaitForReconnect(int nTimeout);
};

