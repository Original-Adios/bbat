#pragma once
#include "wcntestsyswear.h"

typedef struct _tag_WIFI_CASE_RLT
{
	int nIndex;
	int nCh;
	int nCh2;//5G
	int nRate;
	int nRssi;
	int nGood;
	int nErr;
	int nFail;
	_tag_WIFI_CASE_RLT()
	{
		nIndex = 0;
		nCh = 0;
		nCh2 = 0;//5G
		nRate = 0;
		nRssi = 0;
		nGood = 0;
		nErr = 0;
		nFail = 0;
	}
}WIFI_CASE_RLT;

typedef struct _tag_BT_CASE_RLT
{
	int nIndex;
	int nErrBits;
	int nTotalBits;
	int nErrPackets;
	int nTotalPackets;
	int nRssi;
	_tag_BT_CASE_RLT()
	{
		nIndex = 0;
		nErrBits = 0;
		nTotalBits = 0;
		nErrPackets = 0;
		nTotalPackets = 0;
		nRssi = 0;
	}
}BT_CASE_RLT;

class CWcnTestSysWearAT :
	public CWcnTestSysWear
{
	DECLARE_RUNTIME_CLASS(CWcnTestSysWearAT)
public:
	CWcnTestSysWearAT(void);
	virtual ~CWcnTestSysWearAT(void);
	static unsigned _stdcall RunListThreadAT(LPVOID pParam);
	virtual SPRESULT RunList();

protected:
	virtual SPRESULT ConfigWlanParam();
	virtual SPRESULT GetWlanRxResult();
	virtual SPRESULT ConfigBtParam();
	virtual SPRESULT GetBtRxResult();
	virtual SPRESULT DoList();

//private:
	virtual SPRESULT WifiClearCase();
	virtual SPRESULT WifiAddCase(int nChan, E_WLAN_RATE eRate, int nTxDelay, int nRxDelay, int nEndDelay);
	virtual SPRESULT WifiGetRlt(WIFI_CASE_RLT *pRlt, int nSize, int& nCount);
	virtual SPRESULT BtClearCase();
	virtual SPRESULT BtAddCase(BT_TYPE eModeType, int nChan,  BT_PACKET ePacketType,  BT_PATTERN ePattern,int nTxDelay, int nRxDelay, int nEndDelay);
	virtual SPRESULT BtGetRlt(BT_CASE_RLT *pRlt,  int nSize, int& nCount);
	virtual SPRESULT WcnRunCase();

	int m_nBtChanCnt;

	HANDLE m_hThread;
};

