#pragma once
#include "../ImpBase.h"
#include "WlanCommonDef.h"
#include "BtCommonDef.h"
#include "CustomizedBase.h"

using namespace std;

class CWcnTestSysWear : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CWcnTestSysWear)
public:
	CWcnTestSysWear(void);//
    virtual ~CWcnTestSysWear(void);
	static unsigned _stdcall RunListThread(LPVOID pParam);
	SPRESULT RunList();

protected:
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
	virtual SPRESULT DoList();

	virtual BOOL WlanLoadXML();
	virtual BOOL BTLoadXML();
	SPRESULT DoBtTest();
	SPRESULT DoWlanTest();

	virtual SPRESULT ConfigWlanParam();
	virtual SPRESULT GetWlanRxResult();
	virtual SPRESULT ConfigBtParam();
	virtual SPRESULT GetBtRxResult();
	

	virtual SPRESULT WaitForReconnect(int nTimeout);
	

	int GetPacketType(BT_TYPE eMode, BT_PACKET ePacket);

	vector<SPWI_WLAN_PARAM_MEAS_GROUP>  m_vectWlanMeasParam;
	map<WIFI_PROTOCOL_ENUM, WlanMeasBandSpec> m_vectWlanSpec;

	vector<BTMeasParamBand>   m_VecBTParamBandImp;
	SPRESULT m_rltRunScrip;
	CHANNEL_ATTRIBUTE m_ca;
	DWORD m_dwTime;
	BOOL m_bWlan;
	BOOL m_bBt;
	int m_nTriggerTimeOut;
private:
	SPRESULT SendScrip();
	int m_nBeginTimeOut;
	int m_nTestTimeOut;
	int m_nConnectTimeOut;
	string m_strDeviceFilePath;
	HANDLE m_hThread;


	string m_strScripBuf;

	int m_nWlanEnterModeTime;
	int m_nWlanLeaveModeTime;
	int m_nBtEnterModeTime;
};
